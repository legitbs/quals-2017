#include "syscall64.h"

static char Size[5] = "\x11\x22\x33\x44";

int main(int dummy0, int dummy1, int dummy2, int dummy3, int dummy4, int dummy5, char *Data)
{
	int fd;
	int ReadLen = *(int *)Size;
	fd = my_syscall(__NR_open, "/dev/defcon_2017", 0, 0, 0, 0, 0);
	my_syscall(__NR_read, fd, Data, ReadLen, 0, 0, 0);
	my_syscall(__NR_close, fd, 0, 0, 0, 0, 0);
	return ReadLen;
}
