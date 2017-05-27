#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct EntryDataStruct
{
	int Len;
	char Data[256];
} EntryDataStruct;

int main(int argc, char **argv)
{
	int fd;
	unsigned char InBuffer[10000];
	unsigned char OutBuffer[10000];
	unsigned char CurKey;
	int EntryCount;
	int DataLen;
	int OutDataLen;
	int CurPos;
	int CurEntry;
	int EntryLen;
	EntryDataStruct EntryData[256];

	//get the file data
	fd = open(argv[1], O_RDONLY);
	DataLen = read(fd, InBuffer, sizeof(InBuffer));
	close(fd);

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

		printf("%d total entries\n", EntryCount);

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

	//save the result chopping off the special char
	argv[1][strlen(argv[1]) - 1] = 'e';
	unlink(argv[1]);
	fd = open(argv[1], O_RDWR | O_CREAT, 0777);
	write(fd, &InBuffer[1], DataLen - 1);
	close(fd);
}
