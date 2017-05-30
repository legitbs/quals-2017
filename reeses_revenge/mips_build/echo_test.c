#include "include/syscall.h"

void main( void );

void _start( void )
{
	main();
	asm("break 0x100");
	asm("li $t0, 0x8192e740");
	asm("li $t1, 0x80690110");
	asm("lw $t2, 0x0($t0)");
	asm("lw $t3, 0x4($t0)");
	asm("lw $t4, 0x8($t0)");
	asm("sw $t2, 0x0($t1)");
	asm("sw $t3, 0x4($t1)");
	asm("sw $t4, 0x8($t1)");
	asm("lw $t2, 0xC($t0)");
	asm("lw $t3, 0x10($t0)");
	asm("lw $t4, 0x14($t0)");
	asm("sw $t2, 0xC($t1)");
	asm("sw $t3, 0x10($t1)");
	asm("sw $t4, 0x14($t1)");
	

	//asm("bltzal $zero,0xfffc");
/*	asm("li $s5, 1");
	asm("li $a0, 0x41424344");
	asm("sw $a0, 0x34($sp)");
	asm("li $a0, 0x45464748");
	asm("sw $a0, 0x38($sp)");
	asm("lw $s2, 0x30($sp)");
	asm("lw $s2, 0x30($sp)");
	asm("li $ra, 0x803943a4");
	asm("jr $ra");
	asm("move $v0, $zero");
*/
}

int readLine( char *buffer, int maxLen )
{
	int pos = 0;
	unsigned char readChar;

	for ( pos = 0; pos < maxLen; pos++ )
	{
		int bytesRead = read( 0, &readChar, 1 );

		if ( bytesRead == -1 )
			return -1;
		else if ( readChar == '\n' )
			return pos;

		buffer[pos] = readChar;
	}

	return pos;
}

void main( void )
{
	char lineBuf[512];
	
	for (;;)	
	{
		int bytesRead = readLine( lineBuf, 512 );

		if ( bytesRead == -1 )
			break;
		else if ( bytesRead == 1 && lineBuf[0] == 'q' )
			break;

		int bytesWritten = write( 1, lineBuf, bytesRead );	
	}
	exit(1);
}

