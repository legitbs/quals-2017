#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>

int main()
{
	int addr = syscall( SYS_mmap, 0x41410000, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);

	printf("%llx\n", addr);


	return 0;
}
