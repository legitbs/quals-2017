#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

typedef struct BestCharStruct
{
	unsigned char Char;
	unsigned int Count;
} BestCharStruct;

typedef struct OutDataStruct
{
	unsigned char *Data;
	unsigned int DataLen;
	unsigned int DataMaxLen;
} OutDataStruct;

void FreeOutData(OutDataStruct *OutData)
{
	if(!OutData->Data)
		free(OutData->Data);
	memset(OutData, 0, sizeof(OutDataStruct));
}

void AddToOutData(OutDataStruct *OutData, void *InData, unsigned int len)
{
	unsigned char *Data = (char *)InData;

	//add data to the buffer
	if(!OutData->Data)
	{
		OutData->Data = (char *)malloc(len + 1024);
		OutData->DataLen = 0;
		OutData->DataMaxLen = len + 1024;
	}
	else if((OutData->DataLen + len) >= OutData->DataMaxLen)
	{
		OutData->DataMaxLen = OutData->DataLen + len + 1024;
		OutData->Data = (char *)realloc(OutData->Data, OutData->DataMaxLen);
	}

	memcpy(&OutData->Data[OutData->DataLen], Data, len);
	OutData->DataLen += len;
}

BestCharStruct FindBestChar(unsigned char *data, unsigned int len, unsigned int SkipChar)
{
	unsigned int CharCount[256];
	unsigned int i;
	BestCharStruct CurBest;

	memset(CharCount, 0, sizeof(CharCount));

	//loop through all entries getting counts
	for(i = 0; i < len; i++)
		CharCount[data[i]]++;

	//find the lowest count
	CurBest.Char = 0;
	CurBest.Count = len;
	for(i = 0; i < 256; i++)
	{
		//if we have a skip char then skip it
		if((SkipChar < 256) && (i == SkipChar))
			continue;

		if(CharCount[i] < CurBest.Count)
		{
			CurBest.Char = i & 0xff;
			CurBest.Count = CharCount[i];
		}
	};
	return CurBest;
}

OutDataStruct Phase1(OutDataStruct *InData)
{
	unsigned char CompressKey;
	unsigned int i;
	unsigned int Count;
	OutDataStruct OutData;
	unsigned char *Data;
	unsigned int len;

	//start phase 1
	Data = InData->Data;
	len = InData->DataLen;
	CompressKey = FindBestChar(Data, len, 0x100).Char;

	memset(&OutData, 0, sizeof(OutData));
	AddToOutData(&OutData, &CompressKey, 1);

	i = 0;
	while(i < len)
	{
		//plenty of data to work on, if we find 4 matching entries in a row then we can compress
		//as our setup is CompressByte MatchCount OrigByte so we have at least 1 byte
		if((i < (len - 3)) && (Data[i] == Data[i+1]) && (Data[i] == Data[i+2]) && (Data[i] == Data[i+3]))
		{
			//find out how many we have, maximum being 258 as value of 1 translates to 4 bytes
			Count = 4;
			while(((i + Count) < len) && (Count < 258) && (Data[i] == Data[i+Count]))
				Count++;

			//output the special values
			Count -= 3;
			AddToOutData(&OutData, &CompressKey, 1);
			AddToOutData(&OutData, &Count, 1);
			AddToOutData(&OutData, &Data[i], 1);
			i += (Count + 2);
		}
		else if(Data[i] == CompressKey)
		{
			//found our compress key (highly unlikely), add an indicator so we decompress properly
			AddToOutData(&OutData, &CompressKey, 1);
			AddToOutData(&OutData, "\x00", 1);
		}
		else
			AddToOutData(&OutData, &Data[i], 1);

		i++;
	}

	return OutData;
}

char *memfind(char *InBuffer, unsigned long InBufLen, char *FindData, unsigned long FindLen)
{
	unsigned char *FindPos;
	unsigned long CurPos;

	//start searching for matches
	CurPos = 0;
	while(CurPos < InBufLen)
	{
		//quickly find the first byte we are looking for
		FindPos = memchr(&InBuffer[CurPos], FindData[0], InBufLen - CurPos);

		//if no match or not enough room then fail
		if((!FindPos) || (((unsigned long)FindPos + FindLen) > (unsigned long)&InBuffer[InBufLen]))
			break;

		//have a position, try it
		if(memcmp(FindPos, FindData, FindLen) == 0)
			return FindPos;

		//not there, try again
		CurPos = (unsigned long)FindPos - (unsigned long)InBuffer;
		CurPos++;
	};

	//failed
	return (char *)0;
}

OutDataStruct Phase2(OutDataStruct *InData)
{
	unsigned char CompressKey;
	OutDataStruct OutData;
	OutDataStruct CurData;
	OutDataStruct TempData;
	OutDataStruct DataList[256];
	unsigned int DataListLen;
	unsigned int i;
	unsigned int Pos;
	long x;
	unsigned int Found;
	unsigned char *DataPos;
	unsigned char *LastDataPos;

	memset(&OutData, 0, sizeof(OutData));
	memset(&CurData, 0, sizeof(CurData));
	memset(&TempData, 0, sizeof(TempData));
	DataListLen = 0;
	memset(DataList, 0, sizeof(DataList));

	//setup the initial buffer by prepending a null byte to it to indicate done
	AddToOutData(&OutData, "\x00", 1);
	AddToOutData(&OutData, InData->Data, InData->DataLen);

	//go find a compression key to use
	CompressKey = FindBestChar(OutData.Data, OutData.DataLen, 0x100).Char;
	printf("compress key: %02x\n", CompressKey);

	//create a copy of the data, replacing all entries of the compress key with compress key + 0xff
	for(i = 0; i < OutData.DataLen; i++)
	{
		AddToOutData(&CurData, &OutData.Data[i], 1);
		if(CurData.Data[i] == CompressKey)
			AddToOutData(&CurData, "\xff", 1);
	}

	//get rid of our temporary out data
	FreeOutData(&OutData);

	//start creating a new data entry
	AddToOutData(&OutData, &CompressKey, 1);

	//CurData contains the data to process on
	for(i = 255; i > 2; i--)
	{
		//go try to find a match
		Pos = 0;
		while((Pos + i) < CurData.DataLen)
		{
			//see if we can find a match in the buffer
			DataPos = memfind(&CurData.Data[Pos + i], CurData.DataLen - Pos - i, &CurData.Data[Pos], i);
			if(DataPos)
			{

				//add it to our list, find all entries and replace

				//if our data list matches our compress key then we need to insert a dummy entry
				if(DataListLen == CompressKey)
				{
					AddToOutData(&DataList[DataListLen], &CompressKey, 1);
					AddToOutData(&DataList[DataListLen], &CompressKey, 1);
					DataListLen++;
				}

				//add our copy of the data
				AddToOutData(&DataList[DataListLen], &CurData.Data[Pos], i);

				//do full replacements in the data
				FreeOutData(&TempData);
				AddToOutData(&TempData, CurData.Data, Pos);

				AddToOutData(&TempData, &CompressKey, 1);
				AddToOutData(&TempData, &DataListLen, 1);

				//DataPos is the 2nd position found, mark the end of the first finding so we can copy the data
				//inbetween when the loop starts
				LastDataPos = &CurData.Data[Pos + i];
				x = 0;
				while(DataPos)
				{
					//add everything between the new position and the last one
					AddToOutData(&TempData, LastDataPos, (unsigned long)DataPos - (unsigned long)LastDataPos);

					//add special marker
					AddToOutData(&TempData, &CompressKey, 1);
					AddToOutData(&TempData, &DataListLen, 1);

					//adjust last position to start copying from
					LastDataPos = DataPos + i;

					//calculate new amount of data to search in
					x = (long)&CurData.Data[CurData.DataLen] - (long)LastDataPos;
					if((unsigned long)LastDataPos >= (unsigned long)&CurData.Data[CurData.DataLen])
						break;

					//find new entry
					DataPos = memfind(LastDataPos, x, &CurData.Data[Pos], i);
				};

				//add rest of the data
				if((unsigned long)LastDataPos < (unsigned long)&CurData.Data[CurData.DataLen])
					AddToOutData(&TempData, LastDataPos, (unsigned long)&CurData.Data[CurData.DataLen] - (unsigned long)LastDataPos);

				DataListLen++;

				//if we hit 256 entries then bail
				if(DataListLen == 256)
					break;

				//swap buffers
				FreeOutData(&CurData);
				CurData = TempData;
				memset(&TempData, 0, sizeof(TempData));
			}
			else
				Pos++;

			//if we hit 256 entries then bail
			if(DataListLen == 256)
				break;
		}
	}

	//add all of the entries
	for(i = 0; i < DataListLen; i++)
	{
		AddToOutData(&OutData, &DataList[i].DataLen, 1);
		AddToOutData(&OutData, DataList[i].Data, DataList[i].DataLen);
	}

	printf("Key: %02x, %d entries\n", CompressKey, DataListLen);

	AddToOutData(&OutData, CurData.Data, CurData.DataLen);

	//all done
	return OutData;
}

OutDataStruct Phase3(OutDataStruct *InData)
{
	OutDataStruct OutData;
	OutDataStruct CurData;
	OutDataStruct SmallestData;
	BestCharStruct SpecialKey;
	BestCharStruct CompressKey;
	unsigned short BestMatchShort;
	unsigned int BestMatchCount;
	unsigned int WordCount[0x10000];
	unsigned int i;

	memset(&OutData, 0, sizeof(OutData));
	memset(&CurData, 0, sizeof(CurData));
	memset(&SmallestData, 0, sizeof(SmallestData));

	SpecialKey = FindBestChar(InData->Data, InData->DataLen, 0x100);
	if(SpecialKey.Count != 0)
	{
		puts("Failed to find valid char for phase 3\n");
		return OutData;
	}

	//add the special key in then the normal data
	AddToOutData(&CurData, &SpecialKey.Char, 1);
	AddToOutData(&CurData, InData->Data, InData->DataLen);

	//loop until we have a buffer that isn't smaller
	SmallestData.DataLen = CurData.DataLen + 1;
	while(CurData.DataLen < SmallestData.DataLen)
	{
		//setup smallest to be the current buffer and rest current
		FreeOutData(&SmallestData);
		SmallestData = CurData;
		memset(&CurData, 0, sizeof(CurData));

		//find a char to use
		CompressKey = FindBestChar(SmallestData.Data, SmallestData.DataLen, (unsigned int)SpecialKey.Char);

		//do histogram
		memset(WordCount, 0, sizeof(WordCount));
		for(i = 0; i < SmallestData.DataLen - 1; i++)
			WordCount[*(unsigned short *)&SmallestData.Data[i]]++;

		//go find the worst offender
		BestMatchShort = 0;
		BestMatchCount = 0;
		for(i = 0; i < 0x10000; i++)
		{
			if(WordCount[i] > BestMatchCount)
			{
				BestMatchShort = (unsigned short)i;
				BestMatchCount = WordCount[i];
			}
		}

		//if it is big enough then start replacing
		if(BestMatchCount >= 3)
		{
			//add the char and two bytes it replaces
			AddToOutData(&CurData, &CompressKey.Char, 1);
			AddToOutData(&CurData, &BestMatchShort, 2);

			//cycle through all data
			for(i = 0; i < SmallestData.DataLen; i++)
			{
				//if we found the compress key leave it alone
				if(SmallestData.Data[i] == CompressKey.Char)
				{
					AddToOutData(&CurData, &CompressKey.Char, 1);
					AddToOutData(&CurData, &SpecialKey.Char, 1);
				}
				else if((i < (SmallestData.DataLen - 2)) &&
					(*(unsigned short *)&SmallestData.Data[i] == BestMatchShort) &&
					(SmallestData.Data[i+2] != SpecialKey.Char))
				{
					//found an entry to replace
					AddToOutData(&CurData, &CompressKey, 1);
					i++;
				}
				else
					AddToOutData(&CurData, &SmallestData.Data[i], 1);
			}
		}
		else
			break;	//nothing valid to replace, stop looking
	}

	//add our special key to the beginning
	FreeOutData(&CurData);
	AddToOutData(&CurData, &SpecialKey.Char, 1);
	AddToOutData(&CurData, SmallestData.Data, SmallestData.DataLen);

	return CurData;
}

unsigned char * Compress(unsigned char *Data, unsigned int *len)
{
	OutDataStruct OutData;
	unsigned int PrevLen;

	memset(&OutData, 0, sizeof(OutData));

	OutData.Data = Data;
	OutData.DataLen = *len;
	PrevLen = *len;
	OutData = Phase1(&OutData);
	printf("Phase1: %d -> %d\n", PrevLen, OutData.DataLen);

	PrevLen = OutData.DataLen;
	OutData = Phase2(&OutData);
	printf("Phase2: %d -> %d\n", PrevLen, OutData.DataLen);


	PrevLen = OutData.DataLen;
	OutData = Phase3(&OutData);
	printf("Phase3: %d -> %d\n", PrevLen, OutData.DataLen);

	*len = OutData.DataLen;
	return OutData.Data;
}
/*
int main(int argc, char **argv)
{
	int fd;
	unsigned char *Buffer;
	OutDataStruct CompressBuffer;
	int FileLen;
	unsigned char NewName[256];

	//read in
	fd = open(argv[1], O_RDONLY);
	FileLen = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	Buffer = malloc(FileLen);
	read(fd, Buffer, FileLen);
	close(fd);

	//compress
	CompressBuffer = Compress(Buffer, FileLen);

	//write out
	sprintf(NewName, "%s.z", argv[1]);
	unlink(NewName);
	fd = creat(NewName, 0777);
	write(fd, &FileLen, 2);
	write(fd, CompressBuffer.Data, CompressBuffer.DataLen);
	close(fd);
}
*/
