#include "../syscall64.h"

#define write(fd, Msg, MsgLen) my_syscall(__NR_write, 1, (long)Msg, MsgLen, 0, 0, 0)

typedef struct _sAesData
{
	char	*in_block;
	char	*out_block;
	char	*expanded_key;		
	char	*iv;					// for CBC mode
	long	num_blocks;
} sAesData;

static char EncIV[16] = "ENCIV          ";
static char DecIV[16] = "DECIV          ";
static char EncKey[32] = "ENCKEY                         ";
static char DecKey[32] = "DECKEY                         ";
static int RandomFD = 0x11223344;
static char EncExpandKey[256] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
static char DecExpandKey[256] = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";

static int Encrypt_Send(int dummy0, int dummy1, int dummy2, int dummy3, int dummy4, int dummy5, unsigned int fd, char *Buffer, unsigned int Len, unsigned int Flags)
{
	char OutBuffer[4096];
	char TempBuffer[4096];
	char NewKey[32];
	sAesData aesData;

	//if the length is 4 then it is the size header, ignore it as we need to send our own size header
	if(Len == 4)
		return Len;

	//copy the original length to the first 2 bytes
	*(unsigned short *)(&TempBuffer[0]) = Len;

	//add in the 2 bytes for length and 32 bytes for the new key
	Len += 2 + sizeof(NewKey);

	//make sure length if a multiple of 16
	if(Len & 0x0f)
		Len = ((Len >> 4) + 1) << 4;

	//if too big then fail
	if(Len > 4096)
	{
		write(1, "Invalid send length\n", 20);
		my_syscall(__NR_exit, 0, 0, 0, 0, 0, 0);
	}

	//get the new key
	my_syscall(__NR_read, RandomFD, NewKey, 32, 0, 0, 0);

	//copy it into the buffer
	memcpy(&TempBuffer[2], NewKey, sizeof(NewKey));

	//copy the data to the buffer
	memcpy(&TempBuffer[2+sizeof(NewKey)], Buffer, (Len - 2 - sizeof(NewKey)));

	aesData.in_block = TempBuffer;
	aesData.out_block = OutBuffer;
	aesData.expanded_key = EncExpandKey;
	aesData.num_blocks = Len >> 4;
	aesData.iv = EncIV;

	//setup the key and encrypt
	iEncExpandKey256(EncKey, EncExpandKey);
	iEnc256_CBC(&aesData);

	//put our new key in place
	memcpy(EncKey, NewKey, sizeof(NewKey));

	//send the data
	my_syscall(__NR_sendto, fd, (long)&Len, 4, 0x8000, 0, 0);
	return my_syscall(__NR_sendto, fd, (long)OutBuffer, Len, Flags, 0, 0);
}

static int Encrypt_Recv(int dummy0, int dummy1, int dummy2, int dummy3, int dummy4, int dummy5, unsigned int fd, char *Buffer, unsigned int Len, unsigned int Flags)
{
	char InBuffer[4096];
	char OutBuffer[4096];
	int RecvLen;
	sAesData aesData;
	int CurLen;

	//if the length is 4 then it is the size header, just receive it
	if(Len == 4)
	{
		my_syscall(__NR_recvfrom, fd, Buffer, Len, Flags, 0, 0);
		return Len;
	}

	//receive data, keep receiving until we either fail or get all of the length requested
	CurLen = 0;
	while(CurLen < Len)
	{
		RecvLen = my_syscall(__NR_recvfrom, fd, (long)&InBuffer[CurLen], Len - CurLen, Flags, 0, 0);
		if(RecvLen <= 0)
			return RecvLen;

		//move along
		CurLen += RecvLen;
	};

	//lets go decrypt our buffer, our recieved length should be a multiple of 16
	//if not then we have an issue, return 0 bytes
	if(Len & 0x0f)
		return 0;

	//decrypt
	aesData.in_block = InBuffer;
	aesData.out_block = OutBuffer;
	aesData.expanded_key = DecExpandKey;
	aesData.num_blocks = Len >> 4;
	aesData.iv = DecIV;

	//setup the key and decrypt
	iDecExpandKey256(DecKey, DecExpandKey);
	iDec256_CBC(&aesData);

	//get the original length
	RecvLen = *(unsigned short *)&OutBuffer[0];

	//copy off what will be the new encrypt key
	memcpy(DecKey, &OutBuffer[2], 32);

	//copy to the proper buffer and return
	memcpy(Buffer, &OutBuffer[2+32], RecvLen);

	//return the original length although it is a lie to keep the check code happy
	return Len;
}

int main(int dummy0, int dummy1, int dummy2, int dummy3, int dummy4, int dummy5, char *Data)
{
	unsigned long NewLoc;
	unsigned long CurLoc;

	//open up /dev/urandom
	RandomFD = my_syscall(__NR_open, (long)"/dev/urandom", 0, 0, 0, 0, 0);
	if(RandomFD < 0)
	{
		//incidate /dev/urandom failed
		Data[0] = 0xff;
		return 1;
	}

	//unless we execve a new 64bit binary we can't remove the 32bit thread flag in the
	//kernel. Wish I could allocate higher memory
	//note, setting the MAP_FIXED bit doesn't do it, get out of memory error when giving an address

	//allocate a new memory location
	NewLoc = my_syscall(__NR_mmap, 0, 4096, 7, 0x22, 0, 0);
	if(NewLoc & 0xfff)
	{
		//error allocating memory
		Data[0] = 0xfe;
		return 1;
	}

	//get our current location
	__asm__("call MainJmp\n"
		"MainJmp:\n"
		"pop %0\n"
		"andq $0xfffffffffffff000, %0\n"
		: "=r" (CurLoc)
		:
		:
	);

	//copy 4k of data, don't care how big we really are
	memcpy(NewLoc, CurLoc, 4096);

	//calculate our new position and jump to it
	__asm__("addq $MainNewJmp, %0\n"
		"jmp %0\n"
		"MainNewJmp:\n"
		:
		: "r" (NewLoc)
		: "rax"
	);

	//get location of the main binary
	__asm__("movq 8(%%rbp), %0\n"
		"andq $0xfffffffffffff000, %0\n"
		: "=r" (CurLoc)
		:
		:
	);

	//setup our pointers
	*(unsigned long *)(CurLoc + SEND) = &Encrypt_Send;
	*(unsigned long *)(CurLoc + RECV) = &Encrypt_Recv;

	//all good
	memcpy(Data, "!AES!", 5);
	return 5;
}
