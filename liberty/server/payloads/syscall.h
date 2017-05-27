#include <i386-linux-gnu/asm/unistd_32.h>

unsigned int my_syscall(int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp)
{
	int Result;

	__asm__("push %%ebx\n"
		"push %%edi\n"
		"push %%ebp\n"
		"mov %1, %%eax\n"
		"mov %2, %%ebx\n"
		"mov %3, %%ecx\n"
		"mov %4, %%edx\n"
		"mov %5, %%esi\n"
		"mov %6, %%edi\n"
		"mov %7, %%ebp\n"
		"int $0x80\n"
		"pop %%ebp\n"
		"pop %%edi\n"
		"pop %%ebx\n"
		: "=a" (Result)
		: "m" (eax), "m" (ebx), "m" (ecx), "m" (edx), "m" (esi), "m" (edi), "m" (ebp)
		: "ecx", "edx", "esi"
	);

	return Result;
}
