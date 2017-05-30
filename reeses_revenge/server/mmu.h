#ifndef __MMU_H__
#define __MMU_H__

// Memory access types
// MEMACC_INSTFETCH		-- Memory access because of an instruction fetch
// MEMACC_DATALOAD		-- Memory access because of a load instruction
// MEMACC_DATASTORE		-- Memory access due to a store instruction
// MEMACC_ANY			-- Catch all for exception handling
#define MEMACC_INSTFETCH	0
#define MEMACC_DATALOAD		1
#define MEMACC_DATASTORE	2
#define MEMACC_ANY			3

#define PAGETABLE_SIZE		4096

class CCPU;

#define PAGE_TYPE_ALLOC		(1)
#define PAGE_TYPE_MMAP		(2)

typedef struct
{
	uint8_t		*pageData;
	uint8_t		pageType;
} tMMUPage;

#define MMU_BASE_ADDRESS	(0x400000)
#define MMU_MAX_ADDRESS		(0x1000000)

#define PAGE_COUNT_MAX		((MMU_MAX_ADDRESS-MMU_BASE_ADDRESS)/PAGETABLE_SIZE)

class CMMU
{
public:
	CMMU();
	~CMMU();

	// This simple MMU only supports a contiguous region of pages!
	bool Setup( uint32_t baseAddress, uint32_t endAddress );
	bool AddMMAPRegion( uint32_t startAddress, uint32_t length );

	// Now copy in page data (note setup should have been called first!!)
	bool WriteMemory( uint32_t address, uint8_t *pData, uint32_t length );

	bool ReadMemory( uint32_t address, uint8_t *pData, uint32_t length  );

	uint32_t fetchWord( uint32_t addr, uint8_t memaccType, CCPU *pCPU );
	uint16_t fetchHalfword( uint32_t addr, uint8_t memaccType, CCPU *pCPU );
	uint8_t fetchByte( uint32_t addr, uint8_t memaccType, CCPU *pCPU );

	void storeWord( uint32_t addr, uint32_t value, CCPU *pCPU );
	void storeHalfword( uint32_t addr, uint16_t value, CCPU *pCPU );
	void storeByte( uint32_t addr, uint8_t value, CCPU *pCPU );

private:
	uint32_t	m_baseAddress;
	tMMUPage	m_pages[PAGE_COUNT_MAX];
};

#endif // __MMU_H__
