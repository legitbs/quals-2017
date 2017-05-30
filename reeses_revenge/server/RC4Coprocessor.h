#ifndef __RC4_COPROCESSOR_H__
#define __RC4_COPROCESSOR_H__

class CCPU;

class CRC4Coprocessor
{
public:
	CRC4Coprocessor() {};
	~CRC4Coprocessor() { }

	void Emulate( CCPU *pCPU, uint32_t instr, uint32_t pc );

	void init_emulate( CCPU *pCPU, uint32_t instr, uint32_t pc );
	void encrypt_emulate( CCPU *pCPU, uint32_t instr, uint32_t pc );

	static uint16_t rs(const uint32_t i)		{ return (i >> 21) & 0x01f; }
    static uint16_t rt(const uint32_t i)		{ return (i >> 16) & 0x01f; }
	static uint16_t rd(const uint32_t i)		{ return (i >> 11) & 0x01f; }
    static uint16_t funct(const uint32_t i)		{ return (i & 0x03f); }

protected:
	void InitKSA( uint8_t *pKeyData, uint8_t keyLen, uint32_t dropAmount );

	void RC4Block( uint8_t *pBlock, uint8_t blockLen );

private:
	uint32_t	m_state[256];
};

#endif // _RC4_COPROCESSOR_H__
