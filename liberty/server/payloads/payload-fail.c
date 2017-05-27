#include "syscall.h"

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

void main(char *Data)
{
	char *Msg = "A";
	long MsgLen = my_strlen(Msg);
	my_syscall(__NR_write, 1, (int)Msg, MsgLen, 0, 0, 0);
	my_syscall(__NR_exit, 0, 0, 0, 0, 0, 0);
}
