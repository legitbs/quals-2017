#include "syscall64.h"

static char InData[] = "AAAAAAA";
static char Filename[] = "/tmp/lightning_defcon_2017/aaaaaaaa";

typedef struct EntryDataStruct
{
	int Len;
	char Data[256];
} EntryDataStruct;

/*
int ConvertNumber(unsigned long Number, char *Str)
{
	int i;
	int Mask;

	//start printing values
	for(i = 15; i >= 0; i--)
	{
		Mask = (Number >> (4*i)) & 0x0f;
		Str[15-i] = Mask + 0x30;
		if(Str[15-i] > 0x39)
			Str[15-i] += 0x07;
	}
	Str[16] = ' ';
	Str[17] = 0;
	return 17;
}
*/
int main(int dummy0, int dummy1, int dummy2, int dummy3, int dummy4, int dummy5, char *Data)
{
	unsigned char InBuffer[10000];
	unsigned char OutBuffer[10000];
	unsigned char SpecialKey;
	unsigned char CurKey;
	unsigned short CurData;
	int DataLen;
	int OutDataLen;
	int CurPos;
	int EntryLen;
	int EntryCount;
	int CurEntry;
	unsigned char DupeChar;
	int fd;
	EntryDataStruct EntryData[256];

/*
	char SizeStr[20];
	int SizeStrLen;
*/
//////////////////
//Layer 3
//////////////////
	DataLen = *(unsigned short *)&InData[0];
	DataLen--;
	SpecialKey = InData[2];
	memcpy(InBuffer, &InData[3], DataLen);
/*
	SizeStrLen = ConvertNumber(DataLen, SizeStr);
	my_syscall(__NR_write, 1, SizeStr, SizeStrLen, 0, 0, 0);
*/
	//keep decompressing
	while(InBuffer[0] != SpecialKey)
	{
		//get our key
		OutDataLen = 0;
		CurKey = InBuffer[0];
		CurData = *(short *)&InBuffer[1];

		//decompress
		for(CurPos = 3; CurPos < DataLen; CurPos++)
		{
			//if found see if we have the special key, if so then just the byte
			//otherwise output what we replaced
			if(InBuffer[CurPos] == CurKey)
			{
				if(InBuffer[CurPos+1] == SpecialKey)
				{
					OutBuffer[OutDataLen] = InBuffer[CurPos];
					CurPos++;
				}
				else
				{
					*(unsigned short *)&OutBuffer[OutDataLen] = CurData;
					OutDataLen++;
				}
			}
			else
				OutBuffer[OutDataLen] = InBuffer[CurPos];	//no match, keep going

			OutDataLen++;
		}

		//copy our output to the input
		memcpy(InBuffer, OutBuffer, OutDataLen);
		DataLen = OutDataLen;
	};

	//need to chop off the special char char, moving earlier into the buffer so we are ok
	memcpy(InBuffer, &OutBuffer[1], DataLen - 1);
	DataLen--;
/*
	SizeStrLen = ConvertNumber(DataLen, SizeStr);
	my_syscall(__NR_write, 1, SizeStr, SizeStrLen, 0, 0, 0);
	my_syscall(__NR_write, 1, "\n", 1, 0, 0, 0);
*/
//////////////////
//Layer 2
//////////////////

	//keep decompressing
	while(InBuffer[0])
	{
		//get the key and num entries
		CurKey = InBuffer[0];

		//get all entries
		for(CurPos = 1, EntryCount = 0; InBuffer[CurPos]; EntryCount++)
		{
			EntryData[EntryCount].Len = InBuffer[CurPos];
			memcpy(EntryData[EntryCount].Data, &InBuffer[CurPos+1], EntryData[EntryCount].Len);
			CurPos += (EntryData[EntryCount].Len + 1);
		}

		//copy over the rest of the data to decode
		DataLen -= CurPos;
		memcpy(OutBuffer, &InBuffer[CurPos], DataLen);
		memcpy(InBuffer, OutBuffer, DataLen);

		//go get all of the entries and decode them
		for(CurEntry = EntryCount - 1; CurEntry >= 0; CurEntry--)
		{
			//start decoding
			OutDataLen = 0;
			for(CurPos = 0; CurPos < DataLen; CurPos++)
			{
				//if found see if we have 0xff, if so then just the byte
				//otherwise output what we replaced
				if(InBuffer[CurPos] == CurKey)
				{
					//we do not adjust our spot and rewrite it as we found a key byte
					if(InBuffer[CurPos+1] == 0xff)
					{
						OutBuffer[OutDataLen] = InBuffer[CurPos];
						CurPos++;
					}
					else if(InBuffer[CurPos+1] == CurEntry)
					{
						memcpy(&OutBuffer[OutDataLen], EntryData[CurEntry].Data, EntryData[CurEntry].Len);
						OutDataLen += (EntryData[CurEntry].Len-1);
						CurPos++;
					}
					else
						OutBuffer[OutDataLen] = InBuffer[CurPos];
				}
				else
					OutBuffer[OutDataLen] = InBuffer[CurPos];	//no match, keep going

				OutDataLen++;
			}

			//copy our output to the input
			memcpy(InBuffer, OutBuffer, OutDataLen);
			DataLen = OutDataLen;
		}
	};

	//need to chop off the 1st char char, moving earlier into the buffer so we are ok
	memcpy(InBuffer, &OutBuffer[1], DataLen - 1);
	DataLen--;

//////////////////
//Layer 1
//////////////////

	//get the byte that we use to indicate values to dupe
	CurKey = InBuffer[0];

	//go through and create the output
	OutDataLen = 0;
	for(CurPos = 1; CurPos < DataLen; CurPos++)
	{
		//if the special key then the 2nd byte is length, 3rd byte is dupe char
		if(InBuffer[CurPos] == CurKey)
		{
			//if the length is 0 then just output our byte as it was previously used
			EntryLen = InBuffer[CurPos+1];
			if(EntryLen == 0)
				OutBuffer[OutDataLen] = InBuffer[CurPos];
			else
			{
				//get our dupe char and adjust length. 1-255 -> 4->259
				DupeChar = InBuffer[CurPos+2];
				CurPos++;
				EntryLen += 3;
				memset(&OutBuffer[OutDataLen], DupeChar, EntryLen);
				OutDataLen += (EntryLen - 1);
			}

			CurPos++;
		}
		else
			OutBuffer[OutDataLen] = InBuffer[CurPos];
		OutDataLen++;
	}

	fd = my_syscall(__NR_creat, Filename, 0777, 0, 0, 0, 0);
	if(fd <= 0)
	{
		*(long *)Data = -1;
		return 8;
	}

	my_syscall(__NR_write, fd, OutBuffer, OutDataLen, 0, 0, 0);
	my_syscall(__NR_close, fd, 0, 0, 0, 0, 0);

	*(unsigned long *)Data = OutDataLen;
	return 8;
}
