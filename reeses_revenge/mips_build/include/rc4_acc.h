
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

void RC4Init( uint8_t *pKey, uint8_t keySize, uint32_t dropAmount )
{
	{
	register unsigned long __a0 asm("$4") = (unsigned long)pKey;
	register unsigned long __a1 asm("$5") = (unsigned long)keySize;
	register unsigned long __a2 asm("$6") = (unsigned long)dropAmount;

	__asm__ volatile(
	".set   noreorder\n\t"
	".long %0\n\t"
	".set reorder"
	:
	: "i"(0x48853000), "r" (__a0), "r" (__a1), "r" (__a2)
	: "memory"
	);
	}
}

void RC4Encrypt( uint8_t *pBlock, uint32_t blockSize )
{
	{
	register unsigned long __a0 asm("$4") = (unsigned long)pBlock;
	register unsigned long __a1 asm("$5") = (unsigned long)blockSize;

	__asm__ volatile(
	".set   noreorder\n\t"
	".long %0\n\t"
	".set reorder"
	:
	:"i"(0x48852001), "r" (__a0), "r" (__a1)
	: "memory"
	);
	}
}
