#include "syscall64.h"

static char IV[16] = "AAAAAAAAAAAAAAA";
static char DecryptKey[32] = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";

int main(int dummy0, int dummy1, int dummy2, int dummy3, int dummy4, int dummy5, char *Data)
{
	int fd;
	int Ret;

	*(long *)Data = 0;

	fd = my_syscall(__NR_open, "/dev/defcon_2017", 1, 0, 0, 0, 0);
	if(fd < 0)
		*(long *)Data = fd;
	else
	{
		Ret = my_syscall(__NR_write, fd, IV, 16, 0, 0, 0);
		if(Ret != 16)
			*(long *)Data = Ret;
		else
		{
			Ret = my_syscall(__NR_write, fd, DecryptKey, 32, 0, 0, 0);
			if(Ret != 32)
				*(long *)Data = Ret;
		}
	}

	my_syscall(__NR_close, fd, 0, 0, 0, 0, 0);
	return 8;
}
