#include "common.h"

CBitWriter::CBitWriter( uint8_t *pDest, uint32_t maxLen )
{
	m_pDest = pDest;
	m_bitBuffer = 0;
	m_curPos = 0;
	m_mask = 0;
	m_maxLen = maxLen;
}

void CBitWriter::WriteBit( uint8_t bit )
{
	WriteBits( bit, 1 );	
}

void CBitWriter::WriteBits( uint32_t bit, uint8_t bitCount )
{
	for ( uint32_t i = 0; i < bitCount; i++ )
	{
		m_bitBuffer <<= 1;

		if ( bit & (1<<((bitCount-1)-i)) )
			m_bitBuffer++;

		m_mask++;

		if ( m_mask == 8 )
		{
			// Check for overflow
			// BUG:: This check is not here in the client and will allow a possible overflow
#ifdef SERVER
			if ( m_curPos >= m_maxLen )
				return;
#endif		
			// FLUSH
			m_pDest[m_curPos++] = m_bitBuffer;
			
			m_bitBuffer = 0;
			m_mask = 0;
		}
	}	
}

void CBitWriter::WriteByte( uint8_t byte )
{
	WriteBits( byte, 8 );
}

uint32_t CBitWriter::Flush( void )
{
	if ( m_mask == 0 )
		return m_curPos; // Flushed already

	while ( m_mask < 8 )
	{
		m_mask++;
		m_bitBuffer <<= 1;
	}

	// Check for overflow
	// BUG:: This check is not here in the client and will allow a possible overflow
#ifdef SERVER
	if ( m_curPos >= m_maxLen )
		return m_curPos;
#endif

	// FLUSH
	m_pDest[m_curPos] = m_bitBuffer;
	m_curPos++;

	m_mask = 0;
	m_bitBuffer = 0;

	return (m_curPos);
}
