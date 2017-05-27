#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	int fd;
	unsigned char InBuffer[10000];
	unsigned char OutBuffer[10000];
	unsigned char SpecialKey;
	unsigned char CurKey;
	unsigned short CurData;
	int DataLen;
	int OutDataLen;
	int CurPos;

	//get the file data
	fd = open(argv[1], O_RDONLY);

	//read(fd, &CurPos, 2);

	//get the special key
	read(fd, &SpecialKey, 1);

	//get the rest
	DataLen = read(fd, InBuffer, sizeof(InBuffer));
	close(fd);

	//keep decompressing
	while(InBuffer[0] != SpecialKey)
	{
		//get our key
		OutDataLen = 0;
		CurKey = InBuffer[0];
		CurData = *(short *)&InBuffer[1];

		//printf("Key: %02x, Data: %04x\n", CurKey, CurData);

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

	//save the result chopping off the special char
	argv[1][strlen(argv[1]) - 1] = 'd';
	unlink(argv[1]);
	fd = open(argv[1], O_RDWR | O_CREAT, 0777);
	write(fd, &InBuffer[1], DataLen - 1);
	close(fd);
}
