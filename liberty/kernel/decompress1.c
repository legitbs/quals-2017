#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	int fd;
	unsigned char InBuffer[10000];
	unsigned char OutBuffer[10000];
	unsigned char CurKey;
	unsigned char DupeChar;
	int DataLen;
	int OutDataLen;
	int CurPos;
	int EntryLen;

	//get the file data
	fd = open(argv[1], O_RDONLY);
	DataLen = read(fd, InBuffer, sizeof(InBuffer));
	close(fd);

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

	//save the result chopping off the special char
	argv[1][strlen(argv[1]) - 1] = 'f';
	unlink(argv[1]);
	fd = open(argv[1], O_RDWR | O_CREAT, 0777);
	write(fd, OutBuffer, OutDataLen);
	close(fd);
}
