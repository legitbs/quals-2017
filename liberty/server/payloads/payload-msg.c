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

int main(char *Data)
{
	char *Msg = "A";
	long MsgLen = my_strlen(Msg);
	my_syscall(__NR_write, 1, (int)Msg, MsgLen, 0, 0, 0);
	*Data = 0xFF;
	return 1;
}
