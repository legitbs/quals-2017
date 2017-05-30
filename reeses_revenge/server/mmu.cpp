#include "common.h"

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>


CMMU::CMMU() : m_baseAddress(0)
{
	for ( uint32_t i = 0; i < PAGE_COUNT_MAX; i++ )
		m_pages[i].pageData = NULL;

}

CMMU::~CMMU()
{
	for ( uint32_t i = 0; i < PAGE_COUNT_MAX; i++ )
	{
		if ( m_pages[i].pageData )
		{
			if ( m_pages[i].pageType == PAGE_TYPE_ALLOC )
				free( m_pages[i].pageData );
			else if ( m_pages[i].pageType == PAGE_TYPE_MMAP )
				munmap( m_pages[i].pageData, PAGETABLE_SIZE );

		}
	}
}

bool CMMU::AddMMAPRegion( uint32_t startAddress, uint32_t length )
{
	if ( startAddress >= MMU_MAX_ADDRESS )
		return (false);

	if ( (startAddress+length) > MMU_MAX_ADDRESS )
		return (false);

	if ( startAddress < MMU_BASE_ADDRESS )
		return (false);

	uint32_t memRegionSize = length;
	uint32_t pageCount = 0;

	if ( memRegionSize % PAGETABLE_SIZE != 0 )
	{
		pageCount = (memRegionSize/PAGETABLE_SIZE)+1;	
	}
	else
		pageCount = (memRegionSize/PAGETABLE_SIZE);

	uint32_t startPage = ((startAddress-MMU_BASE_ADDRESS)/PAGETABLE_SIZE);
	// Set to zeros as this will allow the bss to be zero'd automatically
	for ( uint32_t i = startPage; i < (startPage+pageCount); i++ )
	{
		if ( m_pages[i].pageData )
			return (false);

		void *mmapAddress = (void *)(0x0000400000000000 | ((uint64_t)prng()<<12));

		void *pPage = mmap( mmapAddress, PAGETABLE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0 );

		if ( pPage == NULL )
			return (false);

		m_pages[i].pageData = (uint8_t*)pPage;
		m_pages[i].pageType = PAGE_TYPE_MMAP;
		
		memset( m_pages[i].pageData, 0, PAGETABLE_SIZE );
#if MMU_DEBUG
		printf( "MMAP PAGE(%2d): 0x%p\n", i, m_pages[i].pageData );
#endif
	}

	return (true);
}

bool CMMU::Setup( uint32_t startAddress, uint32_t endAddress )
{
	// Find size
	if ( startAddress >= endAddress )
		return false;

	if ( startAddress >= MMU_MAX_ADDRESS )
		return (false);

	if ( endAddress > MMU_MAX_ADDRESS )
		return (false);

	if ( startAddress < MMU_BASE_ADDRESS )
		return (false);

	uint32_t memRegionSize = (endAddress - startAddress);
	uint32_t pageCount = 0;

	if ( memRegionSize % PAGETABLE_SIZE != 0 )
	{
		pageCount = (memRegionSize/PAGETABLE_SIZE)+1;	
	}
	else
		pageCount = (memRegionSize/PAGETABLE_SIZE);

	// Set to zeros as this will allow the bss to be zero'd automatically
	for ( uint32_t i = ((startAddress-MMU_BASE_ADDRESS)/PAGETABLE_SIZE); i < pageCount; i++ )
	{
		m_pages[i].pageData = (uint8_t *)malloc( PAGETABLE_SIZE );
		m_pages[i].pageType = PAGE_TYPE_ALLOC;

		memset( m_pages[i].pageData, 0, PAGETABLE_SIZE );
#if MMU_DEBUG
		printf( "PAGE(%2d): 0x%p\n", i, m_pages[i].pageData );
#endif
	}

	// Now set base address
	m_baseAddress = startAddress;

	// Successfully setup
	return (true);
}

bool CMMU::WriteMemory( uint32_t address, uint8_t *pData, uint32_t length )
{
	if ( length == 0 || address < m_baseAddress )
		return false;

	uint32_t offsetAddress = (address - m_baseAddress);

	uint32_t pageTableIdx = offsetAddress / PAGETABLE_SIZE;
	if ( pageTableIdx > PAGE_COUNT_MAX )
	{
		return false;
	}

	uint32_t pageTableOffs = address % PAGETABLE_SIZE;

	// Calculate remaining bytes in page
	uint32_t bytesRemaining = PAGETABLE_SIZE - pageTableOffs;
		
	uint32_t bytesToWrite = length < bytesRemaining ? length : bytesRemaining;

	if ( m_pages[pageTableIdx].pageData == NULL )
		return (false);

	memcpy( m_pages[pageTableIdx].pageData + pageTableOffs, pData, bytesToWrite );

	if (bytesToWrite == length)
	{
		return true;
	}
	else
	{
		// Calculate the number of bytes to write from the next page
		uint32_t bytesLeftOver = length - bytesToWrite;

		uint8_t* pBufPtr = (uint8_t*)pData + bytesToWrite;

		return WriteMemory(address + bytesToWrite, pBufPtr, bytesLeftOver );
	}
}

bool CMMU::ReadMemory( uint32_t address, uint8_t *pData, uint32_t length )
{
	if ( m_pages == NULL || length == 0 || address < m_baseAddress )
		return false;

	uint32_t offsetAddress = (address - m_baseAddress);

	uint32_t pageTableIdx = offsetAddress / PAGETABLE_SIZE;
	if ( pageTableIdx > PAGE_COUNT_MAX )
	{
		return false;
	}

	uint32_t pageTableOffs = address % PAGETABLE_SIZE;

	// Calculate remaining bytes in page
	uint32_t bytesRemaining = PAGETABLE_SIZE - pageTableOffs;

	uint32_t bytesToRead = length < bytesRemaining ? length : bytesRemaining;
	
	if ( m_pages[pageTableIdx].pageData == NULL )
		return (false);

	memcpy( pData, m_pages[pageTableIdx].pageData + pageTableOffs, bytesToRead );

	if (bytesToRead == length)
	{
		return true;
	}
	else
	{
		// Calculate the number of bytes to read from the next page
		uint32_t bytesLeftOver = length - bytesToRead;

		uint8_t* pBufPtr = (uint8_t *)pData + bytesToRead;

		return ReadMemory( address + bytesToRead, pBufPtr, bytesLeftOver );
	}
}

uint32_t CMMU::fetchWord( uint32_t addr, uint8_t memaccType, CCPU *pCPU )
{
	uint32_t word;
	if ( !ReadMemory( addr, (uint8_t*)&word, sizeof(uint32_t) ) )
	{
		pCPU->doException( CCPU::EXCEPTION_AdEL, memaccType );
	}

	return (word);
}

uint16_t CMMU::fetchHalfword( uint32_t addr, uint8_t memaccType, CCPU *pCPU )
{
	uint16_t halfword;
	if ( !ReadMemory( addr, (uint8_t*)&halfword, sizeof(uint16_t) ) )
	{
		pCPU->doException( CCPU::EXCEPTION_AdEL, memaccType );
	}

	return (halfword);
}

uint8_t CMMU::fetchByte( uint32_t addr, uint8_t memaccType, CCPU *pCPU )
{
	uint8_t byte;
	if ( !ReadMemory( addr, (uint8_t*)&byte, sizeof(uint8_t) ) )
	{
		pCPU->doException( CCPU::EXCEPTION_AdEL, memaccType );
	}

	return (byte);
}

void CMMU::storeWord( uint32_t addr, uint32_t value, CCPU *pCPU )
{
	if ( !WriteMemory( addr, (uint8_t *)&value, sizeof(value) ) )
	{
		pCPU->doException( CCPU::EXCEPTION_AdES, MEMACC_DATASTORE );
	}
	return;
}

void CMMU::storeHalfword( uint32_t addr, uint16_t value, CCPU *pCPU )
{
	if ( !WriteMemory( addr, (uint8_t *)&value, sizeof(value) ) )
	{
		pCPU->doException( CCPU::EXCEPTION_AdES, MEMACC_DATASTORE );
	}
	return;
}

void CMMU::storeByte( uint32_t addr, uint8_t value, CCPU *pCPU )
{
	if ( !WriteMemory( addr, (uint8_t *)&value, sizeof(value) ) )
	{
		pCPU->doException( CCPU::EXCEPTION_AdES, MEMACC_DATASTORE );
	}
	return;
}
