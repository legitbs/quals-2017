#include "syscall64.h"

typedef struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET
    unsigned short   sin_port;     // e.g. htons(3490)
    unsigned int     sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
} sockaddr_in;

long my_strlen(char *str)
{
	int i;
	for(i = 0; str[i]; i++){}
	return i;
}

int main(int dummy0, int dummy1, int dummy2, int dummy3, int dummy4, int dummy5, char *Data)
{
	int RandVal;
	int Digits;
	char Buffer[256];
	char *CurPos;
	int fd;
	struct sockaddr_in addr;

	fd = my_syscall(__NR_open, "/dev/urandom", 0, 0, 0, 0, 0);
	my_syscall(__NR_read, fd, (long)&RandVal, 4, 0, 0, 0);

	memcpy(Buffer, "NICK _LEGITBS_", 14);
	CurPos = &Buffer[14];
	for(fd = 0; fd < 8; fd++)
	{
		CurPos[fd] = (RandVal >> (fd * 4)) & 0x0f;
		CurPos[fd] += 0x30;
		if(CurPos[fd] > 0x39)
			CurPos[fd] += 0x07;
	}
	memcpy(&CurPos[8], "\nUSER LegitBS 0 LegitBS LegitBS\n", 32);

	//setup the connection
	memset(&addr, 0, sizeof(addr));

	addr.sin_family = 2;
	addr.sin_port = 0x0b1a;
	addr.sin_addr = 0x04c65382;
	
	//go connect
	fd = my_syscall(__NR_socket, 2, 1, 6, 0, 0, 0);
	if(my_syscall(__NR_connect, fd, (long)&addr, sizeof(addr), 0, 0, 0))
	{
		*(long *)Data = -1;
		return 8;
	}

	my_syscall(__NR_sendto, fd, (long)Buffer, my_strlen(Buffer), 0, 0, 0);

	//continue reading until we hit "PING"
	while(1)
	{
		//read a character, if not P then read until new line
		my_syscall(__NR_recvfrom, fd, (long)Buffer, 1, 0, 0, 0);

		if(Buffer[0] == 'P')
		{
			my_syscall(__NR_recvfrom, fd, (long)Buffer, 4, 0, 0, 0);
			if((Buffer[0] == 'I') && (Buffer[1] == 'N') && (Buffer[2] == 'G') && (Buffer[3] == ' '))
			{

				//get the pong value
				memset(Buffer, 0, sizeof(Buffer));
				for(Digits = 0; ; Digits++)
				{
					my_syscall(__NR_recvfrom, fd, (long)&Buffer[Digits], 1, 0, 0, 0);
					if(Buffer[Digits]==0x0a)
						break;
				}
				break;
			}
		}
		
		//find newline
		while(1)
		{
			my_syscall(__NR_recvfrom, fd, (long)Buffer, 1, 0, 0, 0);
			if(Buffer[0] == '\n')
				break;			
		}
	}

	//got ping, send pong
	my_syscall(__NR_sendto, fd, "PONG ", 5, 0x8000, 0, 0);
	my_syscall(__NR_sendto, fd, (long)Buffer, my_strlen(Buffer), 0, 0, 0);

	//connect to the channel
	my_syscall(__NR_sendto, fd, "JOIN #defconctf\nPRIVMSG #defconctf :I love the LegitBS CTF and trust them with my box!\nQUIT\n", 92, 0, 0, 0);

	*(long *)Data = addr.sin_addr;
	return 8;
}

