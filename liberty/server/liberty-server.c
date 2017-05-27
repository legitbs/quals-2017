#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>

typedef void (*sighandler_t)(int);
#define AlarmTime 30

typedef struct _sAesData
{
	char	*in_block;
	char	*out_block;
	char	*expanded_key;		
	char	*iv;					// for CBC mode
	long	num_blocks;
} sAesData;

void iEncExpandKey256(char *, char *);
void iEnc256_CBC(sAesData *);
void iDecExpandKey256(char *, char *);
void iDec256_CBC(sAesData *);

int EncryptSendAndCheckBuffer(char *Payload, int OrigPayloadLen, int MaxLen, char *MatchResult, int MatchLen);
unsigned char * Compress(unsigned char *Data, unsigned int *len);

char EncIV[16];
char DecIV[16];
char EncKey[32];
char DecKey[32];
char EncExpandKey[256];
char DecExpandKey[256];
int RandomFD;

int ConnectionStatus;
#define CONNECTION_64BIT	0x01
#define CONNECTION_AES		0x02

#define MAX_PAYLOAD_SIZE 4096

static void alarmHandle(int sig)
{
	write(1, "Alarmed\n", 8);
	_exit(0);
}

int ReadResponse(char *Response, int MaxLen)
{
	int Len;
	int TotalLen;
	int ReadLen;

	//get how many bytes to read
	Len = 0;
	ReadLen = read(0, &Len, 4);
	if((ReadLen < 4) || (Len < 0) || (Len > MaxLen))
		return 0;

	//loop for all the data
	TotalLen = Len;
	while(Len)
	{
		ReadLen = read(0, Response, Len);
		if(ReadLen <= 0)
			return 0;

		Response += ReadLen;
		Len -= ReadLen;
	};

	return TotalLen;
}

int SendMessage(char *Payload, int MaxLen, char *Message, int Failed)
{
	int Len;
	int fd;
	int FileLen;
	int MsgLen;

	//get the file data, will be under 4k but stick with our maximum buffer size to the client
	if(ConnectionStatus & CONNECTION_64BIT)
	{
		//64bit mode
		if(Failed)
			fd = open("payloads/64/payload-fail.bin", O_RDONLY);
		else
			fd = open("payloads/64/payload-msg.bin", O_RDONLY);
	}
	else
	{
		//32bit mode
		if(Failed)
			fd = open("payloads/payload-fail.bin", O_RDONLY);
		else
			fd = open("payloads/payload-msg.bin", O_RDONLY);
	}
	if(fd < 0)
	{
		printf("Error opening message payload file\n");
		return 0;
	}

	//get the binary to send
	FileLen = read(fd, Payload, MaxLen);
	close(fd);

	//remove the A\x00 at the end of the payload
	FileLen -= 2;

	//send the message payload
	MsgLen = strlen(Message);
	Len = MsgLen + FileLen + 2;	//newline and null byte

	//make sure we aren't going to send too much
	if(Len > MaxLen)
	{
		printf("SendMessage error: length too large: %d bytes\n%s\n", Len, Message);
		return 0;
	}

	//copy our string in
	memcpy(&Payload[FileLen], Message, MsgLen);
	memcpy(&Payload[FileLen + MsgLen], "\n\x00", 2);

	//if we need to encrypt then do so
	if(ConnectionStatus & CONNECTION_AES)
		return EncryptSendAndCheckBuffer(Payload, Len, MaxLen, "\xff", 1);

	write(1, &Len, 4);
	write(1, Payload, Len);

	//now read the data back and validate
	MsgLen = ReadResponse(Payload, MaxLen);
	if((MsgLen == 1) && (Payload[0] == '\xff'))
		return 1;

	return 0;
}

int ReadAESResponse(char *Payload, int Len)
{
	char OutBuffer[MAX_PAYLOAD_SIZE];
	sAesData aesData;
	int PayloadLen;

	PayloadLen = ReadResponse(Payload, Len);
	if(PayloadLen == 0)
		return -1;

	//if not encrypted then just return
	if(!(ConnectionStatus & CONNECTION_AES))
		return PayloadLen;

	//if not a block multiple or length is too large then fail
	if((PayloadLen & 0x0F) || (PayloadLen > MAX_PAYLOAD_SIZE))
		return 0;

	//decrypt
	aesData.in_block = Payload;
	aesData.out_block = OutBuffer;
	aesData.expanded_key = DecExpandKey;
	aesData.num_blocks = PayloadLen >> 4;
	aesData.iv = DecIV;
	
	//setup the key and decrypt
	iDecExpandKey256(DecKey, DecExpandKey);
	iDec256_CBC(&aesData);

	//now get the original length
	PayloadLen = *(unsigned short *)&OutBuffer[0];
	if(PayloadLen > MAX_PAYLOAD_SIZE)
		return 0;

	//copy off the original packet
	memcpy(Payload, &OutBuffer[2+32], PayloadLen);

	//copy off the new decrypt key
	memcpy(DecKey, &OutBuffer[2], 32);

	return PayloadLen;
}

int InitAES(char *Payload, int MaxLen)
{
	int fd;
	int PayloadLen;
	int i;
	int FindCount;

	//open and send it
	fd = open("payloads/64/aes/payload-aes.bin", O_RDONLY);
	PayloadLen = read(fd, Payload, MaxLen);
	close(fd);

	//get the random fd
	RandomFD = open("/dev/urandom", O_RDONLY);

	//find the encrypt IV and replace it, remember our local copies are swapped
	//we know the keys are near the end of the payload so start near the end
	FindCount = 0;
	for(i = PayloadLen - (16+16+32+32+256+256+20); i < PayloadLen; i++)
	{
		if(memcmp(&Payload[i], "AAAAA", 5) == 0)
		{
			//fill it with random data, just a buffer
			read(RandomFD, &Payload[i], 256);
			i+= 256 - 1;
			FindCount++;
		}
		else if(memcmp(&Payload[i], "BBBBB", 5) == 0)
		{
			//fill it with random data, just a buffer
			read(RandomFD, &Payload[i], 256);
			i+= 256 - 1;
			FindCount++;
		}
		else if(memcmp(&Payload[i], "ENCIV", 5) == 0)
		{
			//found it, get our copy and fill it
			read(RandomFD, DecIV, sizeof(DecIV));
			memcpy(&Payload[i], DecIV, sizeof(DecIV));
			i += sizeof(DecIV) - 1;
			FindCount++;
		}
		else if(memcmp(&Payload[i], "DECIV", 5) == 0)
		{
			//found it, get our copy and fill it
			read(RandomFD, EncIV, sizeof(EncIV));
			memcpy(&Payload[i], EncIV, sizeof(EncIV));
			i += sizeof(EncIV) - 1;
			FindCount++;
		}
		else if(memcmp(&Payload[i], "ENCKEY", 6) == 0)
		{
			//found it, get our copy and fill it
			read(RandomFD, DecKey, sizeof(DecKey));
			memcpy(&Payload[i], DecKey, sizeof(DecKey));
			i += sizeof(DecKey) - 1;
			FindCount++;
		}
		else if(memcmp(&Payload[i], "DECKEY", 6) == 0)
		{
			//found it, get our copy and fill it
			read(RandomFD, EncKey, sizeof(EncKey));
			memcpy(&Payload[i], EncKey, sizeof(EncKey));
			i += sizeof(EncKey) - 1;
			FindCount++;
		}
	}

	if(FindCount != 6)
	{
		SendMessage(Payload, MAX_PAYLOAD_SIZE, "AES Payload issue", 1);
		return -1;
	}

	//send the payload and get the result
	write(1, &PayloadLen, 4);
	write(1, Payload, PayloadLen);

	//get an aes response
	ConnectionStatus |= CONNECTION_AES;
	PayloadLen = ReadAESResponse(Payload, PayloadLen);
	if(PayloadLen == 0)
		return 0;

	//check for errors
	if((PayloadLen == 1) && (Payload[0] == '\xff'))
	{
		ConnectionStatus &= ~CONNECTION_AES;
		SendMessage(Payload, MAX_PAYLOAD_SIZE, "Error accessing /dev/urandom", 1);
		return 0;
	}
	if((PayloadLen == 1) && (Payload[0] == '\xfe'))
	{
		ConnectionStatus &= ~CONNECTION_AES;
		SendMessage(Payload, MAX_PAYLOAD_SIZE, "Error allocating memory", 1);
		return 0;
	}
	else if((PayloadLen == 5) && (memcmp(Payload, "!AES!", 5) == 0))
	{
		SendMessage(Payload, MAX_PAYLOAD_SIZE, "Encryption Enabled", 0);
		return PayloadLen;
	}

	//failed
	return 0;

}

int EncryptSendAndCheckBuffer(char *Payload, int OrigPayloadLen, int MaxLen, char *MatchResult, int MatchLen)
{
	int PayloadLen;
	char TempBuffer[MAX_PAYLOAD_SIZE];
	char OutBuffer[MAX_PAYLOAD_SIZE];
	char NewKey[32];
	sAesData aesData;

	//if not encrypted then just return
	if(!(ConnectionStatus & CONNECTION_AES))
	{
		memcpy(OutBuffer, Payload, OrigPayloadLen);
		PayloadLen = OrigPayloadLen;
	}
	else
	{
		//adjust our payload length
		PayloadLen = OrigPayloadLen + 2 + sizeof(NewKey);
		if(PayloadLen & 0xf)
			PayloadLen = ((PayloadLen >> 4) + 1) << 4;

		//if the payload is too big then fail
		if(PayloadLen > MAX_PAYLOAD_SIZE)
		{
			SendMessage(Payload, MaxLen, "Payload too big to encrypt", 1);
			return 0;
		}

		//setup our length
		*(unsigned short *)&TempBuffer[0] = OrigPayloadLen;

		//get the new key and put it into the buffer
		read(RandomFD, NewKey, sizeof(NewKey));
		memcpy(&TempBuffer[2], NewKey, sizeof(NewKey));

		//copy the original buffer into our temporary
		memcpy(&TempBuffer[2+sizeof(NewKey)], Payload, OrigPayloadLen);

		//setup the encrypt info
		aesData.in_block = TempBuffer;
		aesData.out_block = OutBuffer;
		aesData.expanded_key = EncExpandKey;
		aesData.num_blocks = PayloadLen >> 4;
		aesData.iv = EncIV;

		//setup the key and encrypt
		iEncExpandKey256(EncKey, EncExpandKey);
		iEnc256_CBC(&aesData);

		//copy off the new encrypt key
		memcpy(EncKey, NewKey, sizeof(NewKey));
	}

	//send the payload and get the result
	write(1, &PayloadLen, 4);
	write(1, OutBuffer, PayloadLen);

	//decrypt the result
	PayloadLen = ReadAESResponse(Payload, MaxLen);
	if(PayloadLen == 0)
		return 0;

	//validate
	if(MatchLen && ((PayloadLen != MatchLen) || (memcmp(Payload, MatchResult, MatchLen) != 0)))
		return -1;

	return PayloadLen;
}

int SendAndCheckEncryptedPayload(char *Payload, int MaxLen, char *PayloadName, char *MatchResult, int MatchLen)
{
	int fd;
	int PayloadLen;
	char FullPayloadName[200];

	//get the payload name
	sprintf(FullPayloadName, "payloads/64/payload-%s.bin", PayloadName);

	//open and send it
	fd = open(FullPayloadName, O_RDONLY);
	PayloadLen = read(fd, Payload, MaxLen);
	close(fd);
	return EncryptSendAndCheckBuffer(Payload, PayloadLen, MaxLen, MatchResult, MatchLen);
}
int SendAndCheckPayload(char *Payload, int MaxLen, char *PayloadName, char *MatchResult, int MatchLen)
{
	int fd;
	int PayloadLen;
	char FullPayloadName[200];

	//get the payload name
	sprintf(FullPayloadName, "payloads/payload-%s.bin", PayloadName);

	//open and send it
	fd = open(FullPayloadName, O_RDONLY);
	PayloadLen = read(fd, Payload, MaxLen);
	close(fd);

	write(1, &PayloadLen, 4);
	write(1, Payload, PayloadLen);
	PayloadLen = ReadResponse(Payload, MaxLen);
	if(PayloadLen == 0)
		return -1;

	//validate
	if(MatchLen && ((PayloadLen != MatchLen) || (memcmp(Payload, MatchResult, MatchLen) != 0)))
		return 0;

	return PayloadLen;

}

int TestComms(char *Payload, int MaxLen)
{
	//payload #1
	char *Payload1Msg = "This is a test";
	if(SendAndCheckPayload(Payload, MaxLen, "test", Payload1Msg, strlen(Payload1Msg)) <= 0)
	{
		SendMessage(Payload, MaxLen, "Failed test #1", 1);
		return 0;
	}

	return 1;
}

int AttemptPTrace(char *Payload, int MaxLen)
{
	//see if we are being debugged
	if(SendAndCheckPayload(Payload, MaxLen, "ptrace", "\x00\x00\x00\x00", 4) <= 0)
	{
		SendMessage(Payload, MaxLen, "Please don't debug me", 1);
		return 0;
	}

	return 1;
}

typedef struct PCIVendorDeviceStruct
{
	unsigned short VendorID;
	unsigned short DeviceID;
} PCIVendorDeviceStruct;

int CheckPCI(char *Payload, int MaxLen)
{
	//keep reading data until we get all of the pci or 16k, whichever is first
	char *PCIData;
	char ConvertBuffer[5];
	int PCILen;
	int ReadLen;
	int fd;
	int PayloadLen;
	unsigned short DeviceID, VendorID;
	int MaxPCIDataLen;

	PCIVendorDeviceStruct BadPCI[] = {
		{0x80ee, 0},		//VirtualBox
		{0x15ad, 0},		//VMWare
		{0x5333, 0x8811},	//S3 Trio32, used by VirtualPC
		{0x1011, 0x0009},	//DECchip networking by VirtualPC
		{0x1af4, 0},		//qemu, Azue, Google
		{0xfffe, 0},		//Old VMWare ID
		{0x1ab8, 0},		//Parallels
		{0x5853, 0},		//Citrix Virtualization
		{0x1414, 0},		//Azure
		{0, 0}
	};

	//open and send it
	fd = open("payloads/payload-pci.bin", O_RDONLY);
	PayloadLen = read(fd, Payload, MaxLen);
	close(fd);

	//get as much as we can
	PCILen = 0;
	MaxPCIDataLen = 16*1024;
	PCIData = malloc(MaxPCIDataLen);
	while(ReadLen)
	{
		//modify the payload with the current offset to start at
		memcpy(&Payload[PayloadLen - 5], &PCILen, 4);
		ReadLen = 4096;

		//send payload
		write(1, &PayloadLen, 4);
		write(1, Payload, PayloadLen);

		//get the file data
		ReadLen = ReadResponse(&PCIData[PCILen], ReadLen);
		if(ReadLen == 0)
			break;

		PCILen += ReadLen;
		if(ReadLen != 4096)
			break;

		//if we are close to our full buffer then realloc it
		if((PCILen + 4096) >= MaxPCIDataLen)
		{
			MaxPCIDataLen += (16*1024);
			if(MaxPCIDataLen >= (1024*1024*4))
			{
				SendMessage(Payload, MaxLen, "Stop screwing with me", 1);
				return 0;
			}
			PCIData = realloc(PCIData, MaxPCIDataLen);
		}
	};

	//start validating the file data
	if((PCILen < 253) || (strchr(PCIData, '\n') == 0))
	{
		SendMessage(Payload, MaxLen, "I just want to look at your pci devices", 1);
		return 0;
	}

	//start parsing through the fields
	ReadLen = 0;
	ConvertBuffer[4] = 0;
	while(ReadLen < PCILen)
	{
		//the data has a specific format, grab the vendor and device
		memcpy(ConvertBuffer, &PCIData[ReadLen + 5], 4);
		VendorID = strtoul(ConvertBuffer, 0, 16);
		memcpy(ConvertBuffer, &PCIData[ReadLen + 9], 4);
		DeviceID = strtoul(ConvertBuffer, 0, 16);

		//start checking
		for(fd = 0; BadPCI[fd].VendorID; fd++)
		{
			//if we have a vendor and 0 or vendor and device, fail
			if((BadPCI[fd].VendorID == VendorID) && ((BadPCI[fd].DeviceID == DeviceID) || (BadPCI[fd].DeviceID == 0)))
			{
				free(PCIData);
				SendMessage(Payload, MaxLen, "I see you trying to use virtualization", 1);
				return 0;
			}
		}

		//Vendor and Device are good, next entry
		ReadLen += 253;
		while((ReadLen < PCILen) && PCIData[ReadLen] && (PCIData[ReadLen] != '\n'))
			ReadLen++;

		//We should have found a newline, otherwise complain
		if(ReadLen >= PCILen)
		{
			free(PCIData);
			SendMessage(Payload, MaxLen, "Why are you cutting lines off early", 1);
			return 0;
		}

		//if we found a null then end
		if(!PCIData[ReadLen])
			break;

		//we should be on a newline, advance and continue
		ReadLen++;
	};

	free(PCIData);
	return 1;
}

int SixtyFourBitBaby(char *Payload, int MaxLen)
{
	//let's try 64bit mode
	if(SendAndCheckPayload(Payload, MaxLen, "64", "\x64\x64\x64\x64\x64\x64\x64\x64", 8) <= 0)
	{
		SendMessage(Payload, MaxLen, "Your system must be old", 1);
		return 0;
	}

	ConnectionStatus |= CONNECTION_64BIT;
	SendMessage(Payload, MaxLen, "64-bit Enabled", 0);
	return 1;
}

int CheckCPUID(char *Payload, int MaxLen)
{
	int RetLen;
	int *Values;

	//go pull the CPUID then do some checks on the data
	RetLen = SendAndCheckPayload(Payload, MaxLen, "cpuid", 0, 0);
	if(RetLen != 0x18)
	{
		SendMessage(Payload, MaxLen, "I just want to see your processor info", 1);
		return 0;
	}

	//parse and check the data
	Values = (int *)Payload;

	//Values[0] == EBX of ID 7
	//Values[1] == ECX of ID 7
	//Values[2] == EDX of ID 7
	//Values[3] == ECX of ID 1
	//Values[4] == EDX of ID 1

	//see if the trap flag is set, if so then virtualized
	//if the hypervisor flag is set (id 1, ecx, bit 31), then virtualized
	if(Values[3] & 0x80000000)
	{
		SendMessage(Payload, MaxLen, "I found you virtualizing", 1);
		return 0;
	}

	//check if they support aes (id 1, ecx, bit 25) and sse4.2 (id 1, ecx, bit 20)
	if((Values[3] & 0x02100000) != 0x02100000)
	{
		SendMessage(Payload, MaxLen, "Awe, but I want to encrypt stuff", 1);
		return 0;
	}

	//now, check the reserved bits to make sure someone isn't just giving us all bits filled
	if((Values[0] & 0x00000040) || (Values[1] & 0xbfbfbfe0) || (Values[2] & 0xfffffff3) ||
		(Values[4] & 0x00100400) || (Values[3] & 0x00010000))
	{
		SendMessage(Payload, MaxLen, "Your processor is way too new, I got extra bits", 1);
		return 0;
	}

	return 1;
}

int CheckUID(char *Payload, int MaxLen)
{
	int Ret;

	//check for UID and GID of 0
	Ret = SendAndCheckEncryptedPayload(Payload, MaxLen, "uid", "\x00\x00\x00\x00\x00\x00\x00\x00", 8);
	if(Ret == 0)
		return 0;

	if(Ret < 0)
	{
		SendMessage(Payload, MaxLen, "I prefer root", 1);
		return 0;
	}

	SendMessage(Payload, MAX_PAYLOAD_SIZE, "Root obtained", 0);
	return 1;
}

int CheckSSH(char *Payload, int MaxLen)
{
	int Ret;

	//look for /root/.ssh, /home/*/.ssh
	//if we can't read home folders then someone lied to us about root access
	//if no ssh is found then fail
	Ret = SendAndCheckEncryptedPayload(Payload, MaxLen, "ssh", "SSH Found", 9);
	if(Ret == 0)
		return 0;

	if(Ret < 0)
	{
		if(*(long *)Payload == -13)
			SendMessage(Payload, MaxLen, "You lied to me about root", 1);
		else
			SendMessage(Payload, MaxLen, "No remote ssh access found", 1);
		return 0;
	}

	return 1;
}

int RunCommand(char *Payload, int MaxLen, char *Commands[])
{
	int fd;
	int PayloadLen;
	int i;
	int CmdLen;

	//open and send it
	fd = open("payloads/64/payload-runcmd.bin", O_RDONLY);
	PayloadLen = read(fd, Payload, MaxLen);
	close(fd);

	//start writing the commands to the end of the payload
	PayloadLen -= 5;
	for(i = 0; Commands[i]; i++)
	{
		//+1 for null byte
		CmdLen = strlen(Commands[i]) + 1;
		if((CmdLen + PayloadLen) > MaxLen)
		{
			SendMessage(Payload, MaxLen, "Payload too big, tell the organizers", 1);
			return 0;
		}

		//copy in the command plus the null byte
		memcpy(&Payload[PayloadLen], Commands[i], CmdLen);
		PayloadLen += CmdLen;
	}

	//make sure we terminate the end with another null byte
	Payload[PayloadLen] = 0;
	PayloadLen++;

	return EncryptSendAndCheckBuffer(Payload, PayloadLen, MaxLen, 0, 0);
}

int CheckPython(char *Payload, int MaxLen)
{
	//check python, this only works on 3.7
	int RetLen;
	char *Commands[] = {"/usr/bin/env","python","-c","exec(\"import string\\ntry:\\n print(int(string.Formatter().format(format_string='{a}',a=0)))\\nexcept:\\n print(1)\")", 0};

	RetLen = RunCommand(Payload, MaxLen, Commands);
	if(RetLen == 0)
		return 0;

	if((RetLen >= 1) && (Payload[0] == '1'))
	{
		SendMessage(Payload, MaxLen, "Python version is good", 0);
		return 1;
	}

	SendMessage(Payload, MaxLen, "Python version is too old", 1);
	return 0;
}

int CheckPackages(char *Payload, int MaxLen)
{
	//check and see if packages are installed
	char *Packages[] = {"nasm", "yasm", "ssh", "git", "gitk", "gcc", "wireshark", "libav-tools", "gpgv",
			"make", "cmake", "binwalk", "arj", "gcc-multilib", "build-essential", "flex",
			"libelf-dev", "binutils-dev", "libdwarf-dev", "libncurses5", "libncurses5-dev",
			"libssl-dev",
			0};
	char *Commands[] = {"/bin/bash", "-c", 0, 0};
	char CurCommand[256];
	int i;
	int RetLen;

	SendMessage(Payload, MaxLen, "Checking packages", 0);

	//loop through all packages testing them
	for(i = 0; Packages[i]; i++)
	{
		alarm(AlarmTime);
		sprintf(CurCommand, "dpkg -s %s | grep Status:", Packages[i]);
		Commands[2] = CurCommand;

		RetLen = RunCommand(Payload, MaxLen, Commands);
		if(RetLen == 0)
			return 0;

		if((RetLen != 29) || (memcmp(Payload, "Status: install ok installed\n", RetLen) != 0))
		{
			SendMessage(Payload, MaxLen, "Unable to locate a package", 1);
			return 0;
		}
	}

	return 1;
}

int CheckForDisassembler(char *Payload, int MaxLen)
{
	//check python, this only works on 3.7
	int RetLen;
	char *IDACommands[] = {"/bin/bash","-c", "ls /home/*/ida*/ida.key /opt/ida*/ida.key 2>/dev/null", 0};
	char *BinaryNinjaCommands[] = {"/bin/bash","-c", "ls /home/*/binaryninja/binaryninja 2>/dev/null", 0};

	SendMessage(Payload, MaxLen, "Checking for disassembler", 0);

	RetLen = RunCommand(Payload, MaxLen, IDACommands);
	if(RetLen == 0)
		return 0;

	if((RetLen >= 9) && (strstr(Payload, "/ida.key\n") != 0))
		return 1;

	//check for binary ninja
	RetLen = RunCommand(Payload, MaxLen, BinaryNinjaCommands);
	if(RetLen == 0)
		return 0;

	if((RetLen >= 8) && (strstr(Payload, "binaryninja\n") != 0))
		return 1;

	//fail
	SendMessage(Payload, MaxLen, "This does not look like a hacker's box", 1);
	return 0;
}

int ConfirmActual411Kernel(char *Payload, int MaxLen)
{
	int RetLen;
	char *CPUCommands[] = {"/bin/ls", "-1", "/sys/devices/system/cpu/cpu0/cache/index0", 0};
	char *MMCommands[] = {"/bin/ls", "-1", "/sys/kernel/mm/transparent_hugepage", 0};
	char *Newline;
	char *CurPos;
	int Found;

	//let's really confirm 4.11.0 kernel
	RetLen = RunCommand(Payload, MaxLen, CPUCommands);
	if(RetLen == 0)
		return 0;

	//cycle through looking for "id"
	//make sure it is null terminated
	Payload[RetLen] = 0;

	//process through the results looking for our entries
	Found = 0;
	CurPos = Payload;
	while(CurPos && *CurPos)
	{
		//find the newline, make it null if it exists
		Newline = strchr(CurPos, '\n');
		if(Newline)
		{
			*Newline = 0;
			Newline++;
		}

		//now check the entry
		if(strcasecmp("id", CurPos) == 0)
		{
			Found = 1;
			break;
		}

		//advance our pointers
		CurPos = Newline;
	}

	//if we found the id then check mm
	if(Found)
	{
		//check memory, we still call this so they don't know what we are looking for
		RetLen = RunCommand(Payload, MaxLen, MMCommands);
		if(RetLen == 0)
			return 0;

		//cycle through looking for "hpage_pmd_size"
		//make sure it is null terminated
		Payload[RetLen] = 0;

		//process through the results looking for our entries
		Found = 0;
		CurPos = Payload;
		while(CurPos && *CurPos)
		{
			//find the newline, make it null if it exists
			Newline = strchr(CurPos, '\n');
			if(Newline)
			{
				*Newline = 0;
				Newline++;
			}

			//now check the entry
			if(strcasecmp("hpage_pmd_size", CurPos) == 0)
			{
				Found = 1;
				break;
			}

			//advance our pointers
			CurPos = Newline;
		}
	}

	//if not there then fail it
	if(!Found)
	{
		SendMessage(Payload, MaxLen, "This does not look like a 4.11.0 kernel", 1);
		return 0;
	}

	//All good
	return 1;
}

int WriteFileData(char *Payload, int MaxLen, char *OutFilename, char *InData, int InDataLen, unsigned long OrigFileSize)
{
	int fd;
	long PayloadLen;
	int SizeOffset;
	int FilenameOffset;
	int FileDataOffset;
	char *RemoveTmpFolder[] = {"/bin/bash","-c", "umount /tmp/lightning_defcon_2017 2>/dev/null ; rm -r /tmp/lightning_defcon_2017", 0};

	//open the decompression data
	fd = open("payloads/64/payload-decompress.bin", O_RDONLY);
	PayloadLen = read(fd, Payload, MaxLen);
	close(fd);

	//find the /tmp value
	for(FilenameOffset = PayloadLen - 0x40; FilenameOffset < PayloadLen; FilenameOffset++)
	{
		if(*(unsigned int*)&Payload[FilenameOffset] == 0x706d742f)
			break;
	}

	//filename is 16 bytes, data is after it
	SizeOffset = FilenameOffset + 0x28;
	FileDataOffset = SizeOffset + 2;

	//fill in the name
	memcpy(&Payload[FilenameOffset], OutFilename, strlen(OutFilename) + 1);

	//copy in the input data
	if((InDataLen + PayloadLen - 8) > MaxLen)
	{
		RunCommand(Payload, MaxLen, RemoveTmpFolder);
		SendMessage(Payload, MaxLen, "Decompress payload too big, tell the organizers", 1);
		return 0;
	}

	//copy in the compressed data
	memcpy(&Payload[FileDataOffset], InData, InDataLen);

	//fill in the amount of data
	*(unsigned short *)&Payload[SizeOffset] = InDataLen;

	//adjust the payload size + 2 bytes for size value
	PayloadLen = (PayloadLen - 8) + InDataLen + 2;

	//send it
	PayloadLen = EncryptSendAndCheckBuffer(Payload, PayloadLen, MaxLen, (char *)&OrigFileSize, 8);
	if(PayloadLen == 0)
		return 0;

	if(PayloadLen == -1)
	{
		RunCommand(Payload, MaxLen, RemoveTmpFolder);
		SendMessage(Payload, MaxLen, "Error writing object file", 1);
		return 0;
	}

	return 1;
}

int WriteFile(char *Payload, int MaxLen, char *OutFilename, char *InFilename)
{
	int fd;
	long InDataLen;
	long OrigFileSize;
	char Buffer[8192];
	char *RemoveTmpFolder[] = {"/bin/rm","-r","/tmp/lightning_defcon_2017", 0};

	//open the input file
	fd = open(InFilename, O_RDONLY);
	if(fd <= 0)
	{
		RunCommand(Payload, MaxLen, RemoveTmpFolder);
		SendMessage(Payload, MaxLen, "Error reading decompression file, tell the organizers", 1);
		return 0;
	}

	//get the file length, first 2 bytes is the original file size
	InDataLen = lseek(fd, 0, SEEK_END);
	InDataLen -= 2;

	lseek(fd, 0, SEEK_SET);
	if(InDataLen > sizeof(Buffer))
	{
		RunCommand(Payload, MaxLen, RemoveTmpFolder);
		SendMessage(Payload, MaxLen, "Decompress payload too big, tell the organizers", 1);
		return 0;
	}

	//get original file size
	OrigFileSize = 0;
	read(fd, &OrigFileSize, 2);

	//read in the compressed data
	read(fd, Buffer, InDataLen);
	close(fd);

	//send it
	return WriteFileData(Payload, MaxLen, OutFilename, Buffer, InDataLen, OrigFileSize);
}

int CheckLegitBSRepo(char *Payload, int MaxLen)
{
	int RetLen;
	int i;
	char *Commands[] = {"/bin/bash","-c","ls ~/legitbs_ctf2017_withlovefromlightning -A1F | grep /", 0};
	char *Folders[] = {"website/", "ctf-registrar/", "finals-2014/", "quals-2016/", "cgc-docs/", "quals-2015/", "scorebot/", "quals-2014/", "medianoche/", "fritas/", "choripan/", "quals-2013/", "finals-2013/", 0};
	long FoundCount;
	char *Newline;
	char *CurPos;

	SendMessage(Payload, MaxLen, "Checking for LegitBS files", 0);

	RetLen = RunCommand(Payload, MaxLen, Commands);
	if((RetLen <= 0) || (RetLen >= MaxLen))
		return 0;

	//make sure it is null terminated
	Payload[RetLen] = 0;

	//process through the results looking for our entries
	FoundCount = 0;
	CurPos = Payload;
	while(CurPos && *CurPos)
	{
		//find the newline, make it null if it exists
		Newline = strchr(CurPos, '\n');
		if(Newline)
		{
			*Newline = 0;
			Newline++;
		}

		//now find the entry
		for(i = 0; Folders[i]; i++)
		{
			//if found then count it and stop looking
			if(strcasecmp(Folders[i], CurPos) == 0)
			{
				FoundCount |= (1 << i);
				break;
			}
		}

		//advance our pointers
		CurPos = Newline;
	}

	if(FoundCount != ((1 << ((sizeof(Folders) / sizeof(char *)) - 1)) - 1))
	{
		for(i = 0; Folders[i]; i++)
		{
			if((FoundCount & (1 << i)) == 0)
			{
				char Msg[200];
				sprintf(Msg, "Unable to find LegitBS public repo %s", Folders[i]);
				SendMessage(Payload, MaxLen, Msg, 1);
				return 0;
			}
		}
	}

	return 1;
}

int CheckKernelVersion(char *Payload, int MaxLen)
{
	int RetLen;
	char *Commands[] = {"/bin/uname","-r", 0};

	SendMessage(Payload, MaxLen, "Checking kernel", 0);

	//get the kernel version
	RetLen = RunCommand(Payload, MaxLen, Commands);
	if(RetLen <= 0)
		return 0;

	//we want 4.11.x
	if(memcmp(Payload, "4.11.0-rc5", 10) == 0)
	{
		SendMessage(Payload, MaxLen, "Thank you for running the latest, let's have some fun", 0);
		return 1;
	}
	else if(memcmp(Payload, "4.11.", 5) == 0)
	{
		SendMessage(Payload, MaxLen, "Oh soo close", 1);
		return 0;
	}
	else if(memcmp(Payload, "4.10.", 5) == 0)
	{
		SendMessage(Payload, MaxLen, "Stable? heh", 1);
		return 0;
	}
	else if(memcmp(Payload, "4.9.", 4) == 0)
	{
		SendMessage(Payload, MaxLen, "And you thought 4.9 was good enough", 1);
		return 0;
	}
	else if(memcmp(Payload, "4.", 2) == 0)
	{
		SendMessage(Payload, MaxLen, "I want a newer 4.x kernel", 1);
		return 0;
	}
	else if(memcmp(Payload, "3.", 2) == 0)
	{
		SendMessage(Payload, MaxLen, "Your kernel is too old", 1);
		return 0;
	}

	SendMessage(Payload, MaxLen, "Unable to determine kernel version", 1);
	return 0;
}

typedef struct FlagFileStruct
{
	char Key[32];
	char IV[16];
	char Flag[128];
	unsigned int ReadLen;
	char *FileData;
	int FileLen;
	int OrigFileLen;
} FlagFileStruct;

FlagFileStruct *GenerateFlagFile(char *Payload, int MaxLen)
{
	int fd;
	char TempFlag[128];
	FlagFileStruct *FlagData;
	char ExpandedKey[256];
	int i;
	sAesData aesData;
	char IV[16];
	char *FileData;

	//generate a flag file
	FlagData = malloc(sizeof(FlagFileStruct));
	memset(FlagData, 0, sizeof(FlagFileStruct));

	//first, grab our flag then get a random key and iv to encrypt it with
	fd = open("/dev/urandom", O_RDONLY);
	read(fd, FlagData->Key, 32);
	read(fd, FlagData->IV, 16);
	memcpy(IV, FlagData->IV, sizeof(IV));
	read(fd, &FlagData->ReadLen, 4);

	//make sure we are between 10 and 128
	FlagData->ReadLen %= (sizeof(FlagData->Flag) - 10);
	FlagData->ReadLen += 10;
	close(fd);

	//grab our flag
	fd = open("flag", O_RDONLY);
	if(fd <= 0)
	{
		SendMessage(Payload, MaxLen, "Error opening flag file, tell organizers", 1);
		return 0;
	}

	memset(TempFlag, 0, sizeof(TempFlag));
	read(fd, TempFlag, sizeof(TempFlag));
	close(fd);

	//setup the encrypt info
	aesData.in_block = TempFlag;
	aesData.out_block = FlagData->Flag;
	aesData.expanded_key = ExpandedKey;
	aesData.num_blocks = sizeof(TempFlag) >> 4;
	aesData.iv = IV;

	//setup the key and encrypt
	iEncExpandKey256(FlagData->Key, ExpandedKey);
	iEnc256_CBC(&aesData);

	//now open up the object file and modify it as needed
	fd = open("kernel/flagdata.o.z", O_RDONLY);
	if(fd <= 0)
	{
		SendMessage(Payload, MaxLen, "Error opening flag object file, tell organizers", 1);
		return 0;
	}

	//read the whole file in
	FlagData->FileLen = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	FileData = (char *)malloc(FlagData->FileLen);
	read(fd, FileData, FlagData->FileLen);
	close(fd);

	//cycle through it looking for the amount of data to read and change it
	for(i = 0; i <FlagData->FileLen; i++)
	{
		if(*(unsigned int*)&FileData[i] == 0x11223344)
		{
			//found it, replace it
			*(unsigned int *)&FileData[i] = FlagData->ReadLen;
			break;
		}
	}

	//find our A's and replace them
	for(i = 0; i < (FlagData->FileLen - 16); i++)
	{
		if(memcmp(&FileData[i], "AAAAAAAAAAAAAAAA", 16) == 0)
		{
			//found them, drop in the flag
			memcpy(&FileData[i], FlagData->Flag, sizeof(FlagData->Flag));
			break;
		}
	}

	//now compress the data
	FlagData->OrigFileLen = FlagData->FileLen;
	FlagData->FileData = (char *)Compress((unsigned char *)FileData, (unsigned int *)&FlagData->FileLen);
	free(FileData);

	//return our buffer of data
	return FlagData;
}

int WriteKernelModule(char *Payload, int MaxLen)
{
	FlagFileStruct *FlagData;
	char *CompileKernelCommands[] = {"/bin/bash","-c","cd /tmp/lightning_defcon_2017 ; PATH=/bin:/usr/bin make > /dev/null 2>&1; ls defcon_2017.ko", 0};
	char *CreateTmpFolder[] = {"/bin/bash","-c", "umount /tmp/lightning_defcon_2017  > /dev/null 2>&1 ; rm -r /tmp/lightning_defcon_2017  > /dev/null 2>&1 ; mkdir /tmp/lightning_defcon_2017 ; chmod 700 /tmp/lightning_defcon_2017 ; mount -t ramfs -o size=10m,uid=0,gid=0,mode=0700 ramfs /tmp/lightning_defcon_2017 > /dev/null 2>&1 ; mount | grep lightning_defcon_2017", 0};
	char *RemoveTmpFolder[] = {"/bin/bash","-c", "umount /tmp/lightning_defcon_2017 2>/dev/null ; rm -r /tmp/lightning_defcon_2017", 0};
	char *InstallKernel[] = {"/bin/bash","-c","rmmod defcon_2017 > /dev/null 2>&1 ; insmod /tmp/lightning_defcon_2017/defcon_2017.ko > /dev/null 2>&1 ; umount /tmp/lightning_defcon_2017 > /dev/null 2>&1 ; rm -r /tmp/lightning_defcon_2017 2>/dev/null ; lsmod | grep defcon_2017", 0};
	int RetLen;
	char TempResult[200];
	int fd;
	int Count;
	int ToUpload[5];
	unsigned char RandVal;
	char *RamFSText = "ramfs on /tmp/lightning_defcon_2017 type ramfs";
	char *DecryptXorKey = "DEFCON CTF \x20\x17 Lightning Was Here";

	//create the temporary folder
	RetLen = RunCommand(Payload, MaxLen, CreateTmpFolder);
	if(RetLen == 0)
		return 0;

	//see if it mounted, if not fail
	if(memcmp(Payload, RamFSText, strlen(RamFSText)) != 0)
	{
		RunCommand(Payload, MaxLen, RemoveTmpFolder);
		SendMessage(Payload, MaxLen, "Error setting up for kernel module", 1);
		return 0;
	}

	FlagData = GenerateFlagFile(Payload, MaxLen);
	if(!FlagData)
		return 0;

	SendMessage(Payload, MaxLen, "Uploading kernel object", 0);

	fd = open("/dev/urandom", O_RDONLY);

	for(Count = 0; Count < 5; Count++)
		ToUpload[Count] = Count;

	//upload the files in random orders
	for(Count = 5; Count > 0; Count--)
	{
		read(fd, &RandVal, 1);
		RandVal %= Count;

		alarm(AlarmTime);

		//write one of the randomly selected files
		switch(ToUpload[RandVal])
		{
			case 0:
				if(!WriteFileData(Payload, MaxLen, "/tmp/lightning_defcon_2017/a.o_shipped", FlagData->FileData, FlagData->FileLen, FlagData->OrigFileLen))
					return 0;
				break;

			case 1:
				if(!WriteFile(Payload, MaxLen, "/tmp/lightning_defcon_2017/b.o_shipped", "kernel/readwrite.o.a"))
					return 0;
				break;

			case 2:
				if(!WriteFile(Payload, MaxLen, "/tmp/lightning_defcon_2017/c.o_shipped", "kernel/iaesx64.o.a"))
					return 0;
				break;

			case 3:
				if(!WriteFile(Payload, MaxLen, "/tmp/lightning_defcon_2017/d.o_shipped", "kernel/defcon_2017_dev.o.a"))
					return 0;
				break;

			case 4:
				if(!WriteFile(Payload, MaxLen, "/tmp/lightning_defcon_2017/Makefile", "kernel/Makefile.a"))
					return 0;
				break;
		};

		//replace the random entry with the last entry in the list as next cycle we will look 1 shorter
		ToUpload[RandVal] = ToUpload[Count - 1];
	}
	close(fd);

	//compile the kernel
	RetLen = RunCommand(Payload, MaxLen, CompileKernelCommands);
	if(RetLen == 0)
		return 0;

	//see if the file exists
	if((RetLen != 15) || (memcmp(Payload, "defcon_2017.ko\n", 15) != 0))
	{
		RunCommand(Payload, MaxLen, RemoveTmpFolder);
		SendMessage(Payload, MaxLen, "Failed to compile the kernel", 1);
		return 0;
	}

	//install the kernel and look for it
	RetLen = RunCommand(Payload, MaxLen, InstallKernel);
	if(RetLen == 0)
		return 0;

	//copy the result
	memcpy(TempResult, Payload, sizeof(TempResult));

	//remove the temp folder for fun
	RunCommand(Payload, MaxLen, RemoveTmpFolder);

	//if enough data then check it
	if((RetLen < 32) || (memcmp(TempResult, "defcon_2017 ", 12) != 0))
	{
		SendMessage(Payload, MaxLen, "Failed to install kernel module", 1);
		return 0;
	}

	SendMessage(Payload, MaxLen, "Checking kernel module", 0);

	//read a certain number of bytes from the kernel, it should match our key
	//open and send it
	fd = open("payloads/64/payload-readdev_defcon2017.bin", O_RDONLY);
	RetLen = read(fd, Payload, MaxLen);
	close(fd);

	//add in the length to read
	*(int *)&Payload[RetLen-5] = FlagData->ReadLen;

	//send it and check
	RetLen = EncryptSendAndCheckBuffer(Payload, RetLen, MaxLen, FlagData->Flag, FlagData->ReadLen);
	if(RetLen == 0)
		return 0;

	if(RetLen < 0)
	{
		SendMessage(Payload, MaxLen, "Failed to validate kernel module", 1);
		return 0;
	}

	SendMessage(Payload, MaxLen, "Writing to kernel module", 0);

	//validated, give them the decryption key and iv
	fd = open("payloads/64/payload-writedev_defcon2017.bin", O_RDONLY);
	RetLen = read(fd, Payload, MaxLen);
	close(fd);

	//munge the key for decryption
	for(Count = 0; Count < 32; Count++)
		FlagData->Key[Count] ^= DecryptXorKey[Count];

	//add in the data to write
	memcpy(&Payload[RetLen-16], FlagData->IV, 16);
	memcpy(&Payload[RetLen-16-32], FlagData->Key, 32);

	//send it and check
	RetLen = EncryptSendAndCheckBuffer(Payload, RetLen, MaxLen, "\x00\x00\x00\x00\x00\x00\x00\x00", 8);
	if(RetLen == 0)
		return 0;

	if(RetLen < 0)
	{
		SendMessage(Payload, MaxLen, "Failed to write to kernel module", 1);
		return 0;
	}
	
	return 1;
}

int main(int argc, char **argv)
{
	char *Payload = malloc(MAX_PAYLOAD_SIZE);
	char *RevSlash;

	//make sure we stop if they try to just hang the connection open
        signal(SIGALRM, (sighandler_t)alarmHandle);
        alarm(AlarmTime);

	//changes as we change our connection reqs
	ConnectionStatus = 0;

	//make sure we are in our folder of the binary incase our working path didn't get setup properly
	readlink("/proc/self/exe", Payload, MAX_PAYLOAD_SIZE);
	RevSlash = strrchr(Payload, '/');
	*RevSlash = 0;
	chdir(Payload);

	//send our message and start testing
	if(SendMessage(Payload, MAX_PAYLOAD_SIZE, "Welcome to Liberty\nYou must pass all checks to get the key.\nGood luck", 0) == 0)
		return 0;

        alarm(AlarmTime);

	//start the tests
	if(!TestComms(Payload, MAX_PAYLOAD_SIZE))
		return 0;

        alarm(AlarmTime);

	//check for debugging
	if(!AttemptPTrace(Payload, MAX_PAYLOAD_SIZE))
		return 0;

        alarm(AlarmTime);

	//go process on pci devices, it will report various errors
	if(!CheckPCI(Payload, MAX_PAYLOAD_SIZE))
		return 0;

        alarm(AlarmTime);

	//check cpuid
	if(!CheckCPUID(Payload, MAX_PAYLOAD_SIZE))
		return 0;

        alarm(AlarmTime);

	//jump to 64bit mode
	if(!SixtyFourBitBaby(Payload, MAX_PAYLOAD_SIZE))
		return 0;

        alarm(AlarmTime);

	//start encryption
	if(!InitAES(Payload, MAX_PAYLOAD_SIZE))
		return 0;

        alarm(AlarmTime);

	if(!CheckUID(Payload, MAX_PAYLOAD_SIZE))
		return 0;

        alarm(AlarmTime);

	if(!CheckSSH(Payload, MAX_PAYLOAD_SIZE))
		return 0;

        alarm(AlarmTime);

	if(!CheckPython(Payload, MAX_PAYLOAD_SIZE))
		return 0;

        alarm(AlarmTime);

	if(!CheckKernelVersion(Payload, MAX_PAYLOAD_SIZE))
		return 0;

        alarm(AlarmTime);

	if(!CheckLegitBSRepo(Payload, MAX_PAYLOAD_SIZE))
		return 0;

        alarm(AlarmTime);

	if(!CheckForDisassembler(Payload, MAX_PAYLOAD_SIZE))
		return 0;

        alarm(AlarmTime);

	if(!CheckPackages(Payload, MAX_PAYLOAD_SIZE))
		return 0;

        alarm(AlarmTime);

	if(!ConfirmActual411Kernel(Payload, MAX_PAYLOAD_SIZE))
		return 0;

        alarm(AlarmTime);

	if(!WriteKernelModule(Payload, MAX_PAYLOAD_SIZE))
		return 0;

	SendMessage(Payload, MAX_PAYLOAD_SIZE, "All done, you sure are trusting. Access /dev/defcon_2017 for the flag", 1);
	return 0;
}
