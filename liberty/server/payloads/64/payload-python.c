#include "syscall64.h"

int main(int dummy0, int dummy1, int dummy2, int dummy3, int dummy4, int dummy5, char *Data)
{
	//run a command
	char *Command[] = {"/usr/bin/env","python","-c","exec(\"import string\\ntry:\\n print(int(string.Formatter().format(format_string='{a}',a=0)))\\nexcept:\\n print(1)\")", 0};
	int link[2];
	long pid;
	long Offset;

	//fix all string offsets
	__asm__("call getrip\n"
		"getrip:\n"
		"pop %0\n"
		: "=r" (Offset)
		:
		:
		);

	Offset &= 0xfffffffffffff000;
	for(pid = 0; Command[pid] != 0; pid++)
		Command[pid] += Offset;

	//create a pipe to share with the fork
	if(my_syscall(__NR_pipe2, link, 0, 0, 0, 0, 0) < 0)
	{
		*Data = 0xff;
		return 1;
	}

	//fork off and run the command
	pid = my_syscall(__NR_fork, 0, 0, 0, 0, 0, 0);
	if(pid == 0)
	{
		my_syscall(__NR_dup2, link[1], 1, 0, 0, 0, 0);
		my_syscall(__NR_close, link[0], 0, 0, 0, 0, 0);
		my_syscall(__NR_close, link[1], 0, 0, 0, 0, 0);
		my_syscall(__NR_execve, Command[0], Command, 0, 0, 0, 0);
	}
	else
	{
		//get the output
		my_syscall(__NR_close, link[1], 0, 0, 0, 0, 0);
		Offset = my_syscall(__NR_read, link[0], Data, 4060, 0, 0, 0);
		my_syscall(__NR_wait4, -1, &pid, 0, 0, 0, 0);
	}

	return Offset;
}
