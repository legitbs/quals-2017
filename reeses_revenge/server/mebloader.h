#ifndef __MEBLOADER_H__
#define __MEBLOADER_H__

class CMebLoader
{
public:
	CMebLoader() { }
	~CMebLoader() { }

	bool LoadFile( uint8_t *pData, uint32_t dataSize, std::string &sErrorText );
	bool Run();

private:
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
	CSysCallHandler m_oSysCallHandler;
	CMMU	m_oMMU;
	CCPU	m_oCPU;
};

#endif // __MEBLOADER_H__
