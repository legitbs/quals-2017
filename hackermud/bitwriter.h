#ifndef __BIT_WRITER_H__
#define __BIT_WRITER_H__

#include "common.h"

class CBitWriter
{
public:
	CBitWriter( uint8_t *pDest, uint32_t m_maxLen );

	void WriteBit( uint8_t bit );
	void WriteBits( uint32_t bits, uint8_t bitCount );
	void WriteByte( uint8_t byte );

	uint32_t Flush( void );
	
private:
	uint8_t *m_pDest;
	uint8_t m_bitBuffer;
	uint32_t m_curPos;
	uint8_t m_mask;
	uint32_t m_maxLen;
};

#endif // __BIT_WRITER_H__
