#include "common.h"

#include <time.h>

#define SYSCALL_EXIT		0x4000

#define MAX_READ_BUF	0xF00
#define MAX_WRITE_BUF	0xF00

#define MMAP_MAX_LENGTH	(0x4000)
#define MMAP_TABLE_SIZE	(8)

#define R	32
#define M1	3
#define M2	24
#define M3	10

uint32_t state[R];
uint32_t state_n;

typedef struct MMAP_TABLE_ENTRY
{
	void *pData;
	uint32_t mmapAddress;
	uint32_t mmapLength;
} tMMAPTableEntry;

tMMAPTableEntry g_oMMAPTable[MMAP_TABLE_SIZE];

uint32_t g_mmapTableCount = 0;


typedef void (CSysCallHandler::*tSyscallFptr)( CCPU *pCPU );

void CSysCallHandler::DoSyscall( CCPU *pCPU )
{
	// Table of emulation functions.
	static const tSyscallFptr syscallJumpTable[] = 
	{
        	&CSysCallHandler::DoNothing,	// 0
		&CSysCallHandler::DoExit,		// 1 - EXIT
		&CSysCallHandler::DoNothing,	// 2 - FORK
		&CSysCallHandler::DoRead,		// 3 - READ
		&CSysCallHandler::DoWrite,		// 4 - WRITE
		&CSysCallHandler::DoRand,		// 5
		&CSysCallHandler::DoNothing,	// 6
		&CSysCallHandler::DoNothing,	// 7
		&CSysCallHandler::DoNothing,	// 8
		&CSysCallHandler::DoNothing,	// 9
		&CSysCallHandler::DoNothing,	// 10
		&CSysCallHandler::DoNothing,	// 11
		&CSysCallHandler::DoNothing,	// 12
		&CSysCallHandler::DoTime,		// 13 - TIME
		&CSysCallHandler::DoMMAP,	// 14 - MMAP
	};

	// Get syscall number
	int32_t syscall_num = pCPU->m_regs[CCPU::REG_V0];

	syscall_num -= 4000;

	if ( syscall_num < 15 )
	{
		// Clear error
		pCPU->m_regs[CCPU::REG_A3] = 0;

		(this->*syscallJumpTable[syscall_num])( pCPU );
	}
}

void CSysCallHandler::DoNothing( CCPU *pCPU )
{
	// Do NOTHING
	pCPU->m_regs[CCPU::REG_V0] = 0;
}

void CSysCallHandler::DoExit( CCPU *pCPU )
{
	close(0);
	close(1);
	// Exit
	exit(1);
}

void CSysCallHandler::DoRead( CCPU *pCPU )
{
	// Read from current socket.

	if ( pCPU->m_regs[CCPU::REG_A0] != 0 )
		return;
	else
	{
		unsigned char tempReadBuf[MAX_READ_BUF];

		uint32_t address = pCPU->m_regs[CCPU::REG_A1];
		uint32_t readLen = pCPU->m_regs[CCPU::REG_A2];


		// OLD BUG (from original reeses service) allowed an overflow here of the tempReadBuf
		if ( readLen >= MAX_READ_BUF )
			readLen = MAX_READ_BUF;

		int32_t readSize;

		if ( readLen > 0 )
		{
			readSize = read( 0, tempReadBuf, readLen );

			if ( readSize == -1 )
			{
				pCPU->m_regs[CCPU::REG_A3] = -1;
				pCPU->m_regs[CCPU::REG_V0] = errno;
				return;
			}
			else if ( readSize == 0 )
			{
				// Do nothing
			}
			else if ( !pCPU->m_pMMU->WriteMemory( address, tempReadBuf, readSize ) )
			{
				pCPU->doException( CCPU::EXCEPTION_AdES );
				return;
			}
		}
		else
			readSize = 0;

		// Set return value
		pCPU->m_regs[CCPU::REG_V0] = readSize;
	}
}

void CSysCallHandler::DoWrite( CCPU *pCPU )
{
	if ( pCPU->m_regs[CCPU::REG_A0] != 1 )
		return;
	else
	{
		unsigned char tempWriteBuf[MAX_WRITE_BUF];

		uint32_t address = pCPU->m_regs[CCPU::REG_A1];
		uint32_t writeLen = pCPU->m_regs[CCPU::REG_A2];

		// OLD BUG (from original reeses service) allowed an overflow here of the tempWriteBuf
		if ( writeLen >= MAX_WRITE_BUF )
			writeLen = MAX_WRITE_BUF;

		int32_t writeSize;

		if ( writeLen > 0 )
		{
			if ( !pCPU->m_pMMU->ReadMemory( address, tempWriteBuf, writeLen ) )
			{
				pCPU->doException( CCPU::EXCEPTION_AdEL );
				return;
			}

			writeSize = write( 1, tempWriteBuf, writeLen );

			if ( writeSize == -1 )
			{
				pCPU->m_regs[CCPU::REG_A3] = -1;
				pCPU->m_regs[CCPU::REG_V0] = errno;
				return;
			}	
		}
		else
			writeSize = 0;

		// Set return value
		pCPU->m_regs[CCPU::REG_V0] = writeSize;
	}
}

void CSysCallHandler::DoTime( CCPU *pCPU )
{
	// Set time
	pCPU->m_regs[CCPU::REG_V0] = (uint32_t)time(NULL);
}

void CSysCallHandler::DoMMAP( CCPU *pCPU )
{
	uint32_t mmapAddress = pCPU->m_regs[CCPU::REG_A0];
	uint32_t mmapLength = pCPU->m_regs[CCPU::REG_A1];
	uint32_t mmapProt = pCPU->m_regs[CCPU::REG_A2];

	if ( mmapLength > MMAP_MAX_LENGTH )
	{
		// Fail mmap too large
		pCPU->m_regs[CCPU::REG_V0] = -1;

		return;
	}
	else
	{
		// Check MMAP address table
		if ( g_mmapTableCount >= MMAP_TABLE_SIZE )
		{
			// Fail mmap table exceeded
			pCPU->m_regs[CCPU::REG_V0] = -1;

			return;
		}

		if ( (mmapAddress & 0xFFF) != 0 )
		{
			// Fail mmap not page aligned
			pCPU->m_regs[CCPU::REG_V0] = -1;

			return;
		}

		uint64_t mmapTestEnd = ((uint64_t)mmapAddress + (uint64_t)mmapLength);
		
		if ( mmapTestEnd > 0x1000000 )
		{
			// Fail mmap -- cannot allocate past 0xF0000000
			pCPU->m_regs[CCPU::REG_V0] = -1;

			return;
		}

		if ( mmapAddress < 0x430000 )
		{
			// Fail mmap -- cannot allocate past 0xF0000000
			pCPU->m_regs[CCPU::REG_V0] = -1;

			return;
		}
			
		uint32_t mmapStartAddress = mmapAddress;
		uint32_t mmapEndAddress = mmapAddress+mmapLength;
			
		if ( mmapStartAddress >= 0x400000 && mmapStartAddress < 0x430000 )
		{
			// Fail mmap -- start is in between beginning of code section
			pCPU->m_regs[CCPU::REG_V0] = -1;

			return;
		}

		if ( mmapEndAddress <= 0x430000 && mmapEndAddress > 0x400000 )
		{
			// Fail mmap -- end is in between code section
			pCPU->m_regs[CCPU::REG_V0] = -1;

			return;
		}
			
		if ( mmapStartAddress < 0x400000 && mmapEndAddress > 0x430000 )
		{
			// Overlap
			// Fail mmap -- end is in between code section
			pCPU->m_regs[CCPU::REG_V0] = -1;

			return;
		}
		
		for ( uint32_t idx = 0; idx < g_mmapTableCount; idx++ )
		{
			mmapStartAddress = mmapAddress;
			mmapEndAddress = mmapAddress+mmapLength;

			uint32_t compareStartAddress = g_oMMAPTable[idx].mmapAddress;
			uint32_t compareEndAddress = compareStartAddress + g_oMMAPTable[idx].mmapLength;

			if ( compareStartAddress >= mmapStartAddress && compareStartAddress < mmapEndAddress )
			{
				// Fail mmap -- start is in between beginning of code section
				pCPU->m_regs[CCPU::REG_V0] = -1;

				return;
			}

			if ( compareEndAddress > mmapStartAddress && compareEndAddress <= mmapEndAddress )
			{
				// Fail mmap -- end is in between code section
				pCPU->m_regs[CCPU::REG_V0] = -1;

				return;
			}

			if ( compareStartAddress < mmapStartAddress && compareEndAddress > mmapEndAddress )
			{
				// Overlap
				// Fail mmap -- end is in between code section
				pCPU->m_regs[CCPU::REG_V0] = -1;

				return;
			}
		}

		if( !pCPU->GetMMU()->AddMMAPRegion( mmapAddress, mmapLength ) )
		{
			pCPU->m_regs[CCPU::REG_V0] = -1;

			return;
		}
	
		g_oMMAPTable[g_mmapTableCount].mmapAddress = mmapAddress;
		g_oMMAPTable[g_mmapTableCount].mmapLength = mmapLength;

		g_mmapTableCount++;
				
		pCPU->m_regs[CCPU::REG_V0] = mmapAddress;
	}
}

void CSysCallHandler::DoRand( CCPU *pCPU )
{
	// Set rand
	pCPU->m_regs[CCPU::REG_V0] = prng();  
}

void seed_prng( uint32_t value )
{
	uint32_t i;

	// CLear initial state
	bzero( (void *)state, R*sizeof(uint32_t) );

	state_n = 0;

	// Only use a maximum of 32 uint32_t's to seed state
	state[0] = value;

	for ( i = 1; i < R; i++ )
	{
		uint32_t state_value = state[(i-1)&0x1f];

		// Mix in some of the previous state, the current iteration, and multiply by a mersenne prime		
		state[i] = (uint32_t)((state_value ^ (state_value >> 30) + i) * 524287);
	}
}

uint32_t prng( void )
{
	// Get new random
	uint32_t v0 = state[ state_n ];
	uint32_t vM1 = state[ (state_n + M1) & 0x1f ];
	uint32_t vM2 = state[ (state_n + M2) & 0x1f ];
	uint32_t vM3 = state[ (state_n + M3) & 0x1f ];
		
	uint32_t z0 = state[ (state_n+31) & 0x1f ];
	uint32_t z1 = v0 ^ (vM1 ^ (vM1 >> 8));
	uint32_t z2 = (vM2 ^ (vM2 << 19)) ^ (vM3 ^ (vM3 << 14));
	
	uint32_t newV1 = z1 ^ z2;
	uint32_t newV0 = (z0 ^ (z0 << 11)) ^ (z1 ^ (z1 << 7)) ^ (z2 ^ (z2 << 13));

	state[ state_n ] = newV1;
	state[ (state_n+31) & 0x1f ] = newV0;

	state_n = (state_n + 31) & 0x1f;

	return newV0;
}
