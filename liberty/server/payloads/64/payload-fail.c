#include "syscall64.h"

int my_strlen(char *str)
{
	int count = 0;
	while(*str)
	{
		str++;
		count++;
	};
	
	return count;
}

void main(int dummy0, int dummy1, int dummy2, int dummy3, int dummy4, int dummy5, char *Data)
{
	char *Msg = "A";
	long MsgLen = my_strlen(Msg);
	my_syscall(__NR_write, 1, (long)Msg, MsgLen, 0, 0, 0);
	my_syscall(__NR_exit, 0, 0, 0, 0, 0, 0);
}
