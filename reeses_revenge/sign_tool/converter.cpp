#include "converter.h"
#include "meb_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>

typedef 	uint32_t	Elf32_Addr;
typedef 	uint16_t	Elf32_Half;
typedef		uint32_t	Elf32_Off;
typedef		int32_t	Elf32_Sword;
typedef		uint32_t	Elf32_Word;

// ELF Machine for MIPS R3000
#define EM_R3000		8

// offsets within e_ident
#define EI_MAG0			0		// File identification
#define EI_MAG1			1		// File identification
#define EI_MAG2			2		// File identification
#define EI_MAG3			3		// File identification
#define EI_CLASS		4		// File class
#define EI_DATA			5		// Data encoding
#define EI_VERSION		6		// File version
#define EI_PAD			7		// Start of padding bytes
#define EI_NIDENT		16		// Size of e_ident[]

// elf file type flags
#define ET_NONE			0		// No file type
#define ET_REL          1		//	Relocatable file
#define ET_EXEC			2		// Executable file
#define ET_DYN          3		// Shared object file
#define ET_CORE         4		// Core file
#define ET_LOPROC       0xff00	// Processor-specific
#define ET_HIPROC       0xffff	// Processor-specific
    
// ELF format
#define ELFCLASSNONE    0		// Invalid class
#define ELFCLASS32      1		// 32-bit objects
#define ELFCLASS64      2		// 64-bit objects
    
// encoding
#define ELFDATANONE     0		// Invalid data encoding
#define ELFDATA2LSB     1		// See below
#define ELFDATA2MSB     2		// See below

typedef struct
{
	uint8_t e_ident[EI_NIDENT];
	Elf32_Half e_type;
    Elf32_Half e_machine;
	Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off  e_phoff;
    Elf32_Off  e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;

#define SHT_NULL		0
#define SHT_PROGBITS	1
#define SHT_SYMTAB		2
#define SHT_STRTAB		3
#define SHT_RELA		4
#define SHT_HASH		5
#define SHT_DYNAMIC		6
#define SHT_NOTE		7
#define SHT_NOBITS		8
#define SHT_REL			9
#define SHT_SHLIB		10
#define SHT_DYNSYM		11
#define SHT_LOPROC		0x70000000
#define SHT_HIPROC		0x7fffffff
#define SHT_LOUSER		0x80000000
#define SHT_HIUSER		0x8fffffff

typedef struct 
{
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off  sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32_Shdr; 

typedef struct 
{
    Elf32_Word p_type;
    Elf32_Off  p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;

#define STT_NOTYPE		0
#define STT_OBJECT		1
#define STT_FUNC		2
#define STT_SECTION		3
#define STT_FILE		4
#define STT_LOPROC		13
#define STT_HIPROC		15

#define ELF32_ST_BIND(i)	((i)>>4)
#define ELF32_ST_TYPE(i)	((i)&0xf)
#define ELF32_ST_INFO(b,t)	(((b)<<4)+((t)&0xf))

typedef struct
{
	Elf32_Word		st_name;
	Elf32_Addr		st_value;
	Elf32_Word		st_size;
	uint8_t			st_info;
	uint8_t			st_other;
	Elf32_Half		st_shndx;
} Elf32_Sym;

char *GetStringFromStrTab( uint8_t *pELFBase, uint32_t elfSize, uint32_t baseOffset, uint32_t index )
{
	if ( (baseOffset + index) >= elfSize )
		return (NULL);

	return (char *)(pELFBase + baseOffset + index);
}

bool convert_elf_to_meb( uint8_t *pELFData, uint32_t elfSize, uint8_t **pOutData, uint32_t &outDataSize, std::string &sErrorText, Crypto &oCrypto )
{
	// Check size
	if ( elfSize < sizeof(Elf32_Ehdr) )
	{
		sErrorText = "Invalid file format";
		return false;
	}

	// Read header
	Elf32_Ehdr elfHeader;

	memcpy( (void *)&elfHeader, pELFData, sizeof(Elf32_Ehdr) );

	// Check header.
	if ( elfHeader.e_ident[EI_MAG0] != 0x7f || elfHeader.e_ident[EI_MAG1] != 'E' ||
		 elfHeader.e_ident[EI_MAG2] != 'L' || elfHeader.e_ident[EI_MAG3] != 'F' )
	{
		sErrorText = "Invalid file format. Bad Header.";
		return false;
	}

	if ( elfHeader.e_ident[EI_CLASS] != ELFCLASS32 )
	{
		sErrorText = "Invalid file format. Must be 32-bit ELF class.";
		return false;
	}

	if ( elfHeader.e_ident[EI_DATA] != ELFDATA2LSB )
	{
		sErrorText = "Invalid file format. Must be little endian ELF.";
		return false;
	}

	if ( elfHeader.e_type != ET_EXEC )
	{
		sErrorText = "Invalid file format. Must be executable ELF.";
		return false;
	}

	if ( elfHeader.e_machine != EM_R3000 )
	{
		sErrorText = "Invalid file format. Must be a MIPS R3000 machine.";
		return false;
	}

	// ****************************************************** //
	// LOAD SECTION HEADERS
	// ****************************************************** //
	Elf32_Shdr *elfSHdr = new Elf32_Shdr[elfHeader.e_shnum];

	uint32_t offsetSHdr = elfHeader.e_shoff;
	
	if ( elfSize < (offsetSHdr + sizeof(Elf32_Shdr) * elfHeader.e_shnum ) )
	{
		sErrorText = "Invalid file format. Too small.";
		return false;
	}

	memcpy( elfSHdr, (pELFData + offsetSHdr), (sizeof(Elf32_Shdr) * elfHeader.e_shnum) );

	// Get string table offset!
	uint32_t secHeaderStringTableOffset = elfSHdr[elfHeader.e_shstrndx].sh_offset;

	if ( secHeaderStringTableOffset >= elfSize )
	{
		sErrorText = "Invalid file format. Too small.";
		return false;
	}

	bool bFoundErrnoLocation = false;
	bool bFoundGPLocation = false;
	bool bFoundEndLocation = false;
	bool bFoundSymTabLocation = false;
	bool bFoundStrTable = false;

	uint32_t iSymTabIdx = 0;
	uint32_t errnoLocationValue = 0;
	uint32_t gpRegisterValue = 0;
	uint32_t endAddressValue = 0;
	uint32_t stringTableIdx = 0;

	for ( uint32_t iSec = 0; iSec < elfHeader.e_shnum; iSec++ )
	{
		printf( "Section %d: name %s\n", iSec, GetStringFromStrTab( pELFData, elfSize, secHeaderStringTableOffset, elfSHdr[iSec].sh_name ) );

		if ( elfSHdr[iSec].sh_type == SHT_SYMTAB )
		{
			bFoundSymTabLocation = true;
			iSymTabIdx = iSec;
		}

		if ( elfSHdr[iSec].sh_type == SHT_STRTAB && iSec != elfHeader.e_shstrndx )
		{
			bFoundStrTable = true;
			stringTableIdx = iSec;
		}
	}

	if ( !bFoundStrTable )
	{
		sErrorText = "Invalid file format. Missing string table.";
		return false;
	}

	if ( bFoundSymTabLocation )
	{
		// Read in the symbol table...
		if ( elfSHdr[iSymTabIdx].sh_offset >= elfSize )
		{
			sErrorText = "Invalid file format. Too small.";
			return false;
		}

		uint32_t symTabCount = elfSHdr[iSymTabIdx].sh_size / elfSHdr[iSymTabIdx].sh_entsize;

		if ( elfSHdr[iSymTabIdx].sh_entsize != sizeof(Elf32_Sym) )
		{
			sErrorText = "Invalid file format. Symbol table entry size is not equal to internal struct.";
			return false;
		}

		Elf32_Sym *elfSymTab = new Elf32_Sym[symTabCount];

		memcpy( elfSymTab, (pELFData+elfSHdr[iSymTabIdx].sh_offset), elfSHdr[iSymTabIdx].sh_size );

		for ( uint32_t i = 0; i < symTabCount; i++ )
		{
			char *pszSymbolName = GetStringFromStrTab( pELFData, elfSize, elfSHdr[stringTableIdx].sh_offset, elfSymTab[i].st_name );
			if ( strcmp( pszSymbolName, "__errno_location" ) == 0 )
			{
				bFoundErrnoLocation = true;
				errnoLocationValue = elfSymTab[i].st_value;
			}
			else if ( strcmp( pszSymbolName, "_gp" ) == 0 )
			{
				bFoundGPLocation = true;
				gpRegisterValue = elfSymTab[i].st_value;
			}
			else if ( strcmp( pszSymbolName, "_end" ) == 0 )
			{
				bFoundEndLocation = true;
				endAddressValue = elfSymTab[i].st_value;
			}
		}
	}

	if ( !bFoundEndLocation )
	{
		sErrorText = "Invalid file format. Missing _end symbol in symtable.";
		return false;
	}

	if ( !bFoundGPLocation )
	{
		sErrorText = "Invalid file format. Missing _gp symbol in symtable.";
		return false;
	}

	if ( !bFoundErrnoLocation )
	{
		sErrorText = "Invalid file format. Missing __errno_location symbol in symtable.";
		return false;
	}

	// ****************************************************** //
	// LOAD PROGRAM HEADERS
	// ****************************************************** //
	Elf32_Phdr *elfPHdr = new Elf32_Phdr[elfHeader.e_phnum];

	uint32_t offsetPHdr = elfHeader.e_phoff;

	if ( elfSize < (offsetPHdr + sizeof(Elf32_Phdr) * elfHeader.e_phnum) )
	{
		sErrorText = "Invalid file format. Too small.";
		return false;
	}

	// Read the program headers
	memcpy( elfPHdr, (pELFData+offsetPHdr), (sizeof(Elf32_Phdr) * elfHeader.e_phnum) );

	// Generate MEB File
	CMebFile oFile;

	oFile.AddHeader( CMebFile::CURRENT_VERSION, 0, errnoLocationValue );

	for ( uint32_t i = 0; i < elfHeader.e_phnum; i++ )
	{
		oFile.AddLSec( elfPHdr[i].p_vaddr, elfPHdr[i].p_memsz, elfPHdr[i].p_type, (pELFData+elfPHdr[i].p_offset), elfPHdr[i].p_filesz );
	}

	oFile.AddRSec( CMebFile::REGISTER_SP, 0x430000 ); // We default to using memory range 0x400000 - 0x430000 or 192K (where .text starts at the bottom, stack is at the top, and the data section is in between)
	oFile.AddRSec( CMebFile::REGISTER_PC, elfHeader.e_entry );
	oFile.AddRSec( CMebFile::REGISTER_GP, gpRegisterValue );

	oFile.AddSignature( oCrypto );

	uint8_t *pData;
	uint32_t dataSize;
	
	if ( !oFile.GenerateFile( &pData, dataSize ) )
	{
		delete pData;
		pOutData = NULL;
		outDataSize = 0;
		return false;
	}

	(*pOutData) = pData;
	outDataSize = dataSize;

	return (true);
}
