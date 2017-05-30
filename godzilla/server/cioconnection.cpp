#include "common.h"	

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
	
int32_t CIOConnection::WriteData( uint8_t *pBuffer, uint32_t sendLength )
{
	uint32_t totalBytesSent = 0;

	while ( totalBytesSent < sendLength )
	{
		uint32_t bytesRemaining = (sendLength-totalBytesSent);
		int32_t bytesSent = write( m_sendFD, pBuffer+totalBytesSent, bytesRemaining );

		if ( bytesSent <= 0 )
			return -1;

		totalBytesSent += bytesSent;
	}

	return (totalBytesSent);
}

int32_t CIOConnection::ReadData( uint8_t *pBuffer, uint32_t readLength )
{
	uint32_t totalBytesRead = 0;

	while ( totalBytesRead < readLength )
	{
		uint32_t bytesRemaining = (readLength-totalBytesRead);
		int32_t bytesRead = read( m_readFD, pBuffer+totalBytesRead, bytesRemaining );

		if ( bytesRead <= 0 )
			return -1;

		totalBytesRead += bytesRead;
	}

	return (totalBytesRead);
}
