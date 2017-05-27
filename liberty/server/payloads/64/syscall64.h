#include <x86_64-linux-gnu/asm/unistd_64.h>

unsigned long my_syscall(long rax, long rdi, long rsi, long rdx, long r10, long r8, long r9)
{
	unsigned long Result;

	__asm__("mov %1, %%rax\n"
		"mov %2, %%rdi\n"
		"mov %3, %%rsi\n"
		"mov %4, %%rdx\n"
		"mov %5, %%r10\n"
		"mov %6, %%r8\n"
		"mov %7, %%r9\n"
		"syscall\n"
		: "=a" (Result)
		: "m" (rax), "m" (rdi), "m" (rsi), "m" (rdx), "m" (r10), "m" (r8), "m" (r9)
		: "rdi", "rsi", "rdx", "r10", "r8", "r9"
	);

	return Result;
}
