#include "syscall64.h"

int main(int dummy0, int dummy1, int dummy2, int dummy3, int dummy4, int dummy5, char *Data)
{
	//run a command
	char *InCmd = "AAAA";
	char *Command[100];
	int link[2];
	long pid;
	char *Offset;
	int Entry;

	for(Entry = 0, Offset = InCmd; *Offset != 0; Entry++)
	{
		//set the entry
		Command[Entry] = Offset;

		//find the null byte
		while(*Offset)
			Offset++;

		//increment past the null
		Offset++;
	}
	Command[Entry] = 0;

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
		my_syscall(__NR_close, 2, 0, 0, 0, 0, 0);
		my_syscall(__NR_execve, Command[0], Command, 0, 0, 0, 0);
	}
	else
	{
		//get the output
		my_syscall(__NR_close, link[1], 0, 0, 0, 0, 0);
		my_syscall(__NR_wait4, -1, &pid, 0, 0, 0, 0);
		Offset = my_syscall(__NR_read, link[0], Data, 4060, 0, 0, 0);
	}

	//if no data then fail
	if(Offset <= 0)
	{
		Offset = 8;
		*(unsigned long *)Data = 0;
	}
	return Offset;
}
