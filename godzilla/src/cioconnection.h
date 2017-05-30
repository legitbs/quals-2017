#ifndef __CIO_CONNECTION_H__
#define __CIO_CONNECTION_H__

class CIOConnection
{
public:
	CIOConnection( int32_t sendFD, int32_t readFD ) : m_sendFD( sendFD ), m_readFD( readFD ) { };

	int32_t ReadData( uint8_t *pBuffer, uint32_t maxLen );
	int32_t WriteData( uint8_t *pBuffer, uint32_t maxLen );

private:
	int32_t	m_sendFD;
	int32_t m_readFD;
};

#endif // __CIO_CONNECTION_H__

