int main(char *Data)
{
	//save regs, get some cpuid's then return
	__asm__("push %%ebx\n"
		"mov $7, %%eax\n"
		"cpuid\n"
		"mov %%ebx, %0\n"
		"mov %%ecx, %1\n"
		"mov %%edx, %2\n"
		"xor %%eax, %%eax\n"
		"inc %%eax\n"
		"cpuid\n"
		"mov %%ecx, %3\n"
		"mov %%edx, %4\n"
		"pop %%ebx\n"
	: "=m" (Data[0x00]), "=m" (Data[0x04]), "=m" (Data[0x08]), "=m" (Data[0x0c]), "=m" (Data[0x10])
	:
	: "eax", "ecx", "edx"
	);

	return 0x18;
}
