#include "syscall.h"
#include <linux/ptrace.h>

unsigned long main(int *Data)
{
	int pid;
	int ppid;
	int Result;

	//fork and let the child attach to the parent
	pid = my_syscall(__NR_fork, 0, 0, 0, 0, 0, 0);
	if(pid == 0)
	{
		//get parent pid and try to ptrace it
		ppid = my_syscall(__NR_getppid, 0, 0, 0, 0, 0, 0);
		if(my_syscall(__NR_ptrace, PTRACE_ATTACH, ppid, 0, 0, 0, 0) == 0)
		{
			my_syscall(__NR_waitpid, ppid, 0, 0, 0, 0, 0);
			my_syscall(__NR_ptrace, PTRACE_CONT, 0, 0, 0, 0, 0);
			my_syscall(__NR_ptrace, PTRACE_DETACH, ppid, 0, 0, 0, 0);
			Result = 0;
		}
		else
		{
			//failed, must be in a debugger
			Result = -1;
		}
		my_syscall(__NR_exit, Result, 0, 0, 0, 0, 0);
	}
	else
	{
		my_syscall(__NR_waitpid, pid, (int)Data, 0, 0, 0, 0);
	}

	return 4;
}
