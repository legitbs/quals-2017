#include <fcntl.h>
#include "syscall.h"

long main(char *Data)
{
	long fd;
	int Read;
	char *ReadLen = "abcd";
	int CurLen;
	fd = my_syscall(__NR_open, (int)"/proc/bus/pci/devices", O_RDONLY, 0, 0, 0, 0);
	if(fd < 0)
	{
		*(int *)Data = 0xffffffff;
		return 4;
	}

	CurLen = *(int *)ReadLen;
	while(CurLen)
	{
		if(CurLen > 4096)
			Read = 4096;
		else
			Read = CurLen;
		Read = my_syscall(__NR_read, fd, (int)Data, Read, 0, 0, 0);
		CurLen -= Read;
	}

	Read = 4096;
	Read = my_syscall(__NR_read, fd, (int)Data, Read, 0, 0, 0);
	my_syscall(__NR_close, fd, 0, 0, 0, 0, 0);
	return Read;
}
