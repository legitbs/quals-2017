#include "syscall64.h"

int main(int dummy0, int dummy1, int dummy2, int dummy3, int dummy4, int dummy5, char *Data)
{
	int UID;
	int GID;

	UID = my_syscall(__NR_getuid, 0, 0, 0, 0, 0, 0);
	GID = my_syscall(__NR_getgid, 0, 0, 0, 0, 0, 0);

	*(unsigned int*)&Data[0] = UID;
	*(unsigned int*)&Data[4] = GID;
	return 8;
}

