#ifndef __MEB_FILE_H__
#define __MEB_FILE_H__

#include <stdint.h>
#include <vector>
#include "crypto.h"

class CMebFile
{
public:
	CMebFile() { m_pCrypto = NULL; }

	const static uint8_t REGISTER_SP = 29;
	const static uint8_t REGISTER_GP = 28;
	const static uint8_t REGISTER_PC = 32;

	const static uint32_t CURRENT_VERSION = 1;

	const static uint8_t MEBHDR_MAGIC0	= 0x41;
	const static uint8_t MEBHDR_MAGIC1  = 0x4A;
	const static uint8_t MEBHDR_MAGIC2  = 0x5C;
	const static uint8_t MEBHDR_MAGIC3  = 0x62;

	const static uint8_t LSECTYPE_TEXT = 0;		// Text section (executable code)
	const static uint8_t LSECTYPE_DATA = 1;		// Data section (.sdata, .data, bss, heap and stack)

	bool AddHeader( uint32_t version, uint32_t info, uint32_t errno_addr );
	bool AddLSec( uint32_t startAddr, uint32_t memSize, uint8_t sectionType, uint8_t *pData, uint32_t dataSize );
	bool AddRSec( uint8_t regNum, uint32_t regValue );

	bool AddSignature( Crypto &oCrypto );

	bool GenerateFile( uint8_t **pOutData, uint32_t &outSize );

private:
	// Header
	typedef struct
	{
		uint8_t		magic[4];
		uint32_t	version;
		uint32_t	info;
		uint32_t	lsec_offset;
		uint32_t	rsec_offset;
		uint16_t	lsec_num;
		uint16_t	rsec_num;
		uint32_t	sig_offset;
		uint32_t	errno_addr;
	} Meb32_Hdr;

	// Loadable Sections
	typedef struct
	{
		uint32_t	startAddr;
		uint32_t	memSize;
		uint32_t	fileSize;
		uint32_t	fileOffset;
		uint32_t	sectionType;
	} Meb32_LSec;

	// Register Sections (defines register starting values)
	typedef struct
	{
		uint8_t		regNum;
		uint32_t	regValue;
	} Meb32_RSec;

	// Signature Section (allows code signing of this blob)
	typedef struct
	{
		uint8_t signed_hash[256];		// RSA private key signed SHA-256 hash with PKCS#1

		// TODO: ADD MORE HERE
	} Meb32_Sig;

private:
	Meb32_Hdr	m_oHeader;
	Meb32_Sig	m_oSignature;
	std::vector<Meb32_LSec> m_oLSecs;
	std::vector<Meb32_RSec> m_oRSecs;
	std::vector<uint8_t *> m_oLSecData;

	Crypto *m_pCrypto;
};

#endif // __MEB_FILE_H__
