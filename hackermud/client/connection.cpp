#include "common.h"

CServerConnection::CServerConnection( TCPClientSocket *pSocket )
	: m_pSocket( pSocket ), m_connectionState( CONNECTION_WAIT_AUTH )
{

}

CServerConnection::~CServerConnection(  )
{

}

void CServerConnection::Authenticate( void )
{
	m_connectionState = CONNECTION_PLAYER;
}

void CServerConnection::Disconnect( void )
{
	m_pSocket = NULL;
	m_connectionState = CONNECTION_DISCONNECT;
}

bool CServerConnection::SendCommand( uint8_t cmdNumber, CStreamData *pData )
{
	if ( !m_pSocket )
		return (false);

	if ( !pData )
		return (false);

	uint32_t dataLen = pData->GetReadAvailable();

	uint8_t *pSocketData = new uint8_t[dataLen+5];
	pSocketData[0] = cmdNumber;

	*((uint32_t*)(pSocketData+1)) = dataLen;

	try
	{
		pData->Read( pSocketData+5, dataLen );

		int32_t bytesTx = m_pSocket->SendData( pSocketData, dataLen+5 );	
		if ( bytesTx != dataLen+5 )
		{
			printf( "Bytes tx did not match data written in CServerConnection::SendData\n" );
			return (false);
		}
	}
	catch ( CStreamException e )
	{
		printf( "Stream exception on server connection write: %s\n", e.GetReason().c_str() );

		return (false);
	}
	catch ( SocketException e )
	{
		printf( "Socket exception on server connection write: %s\n", (const char *)e );
		return (false);
	}

	return (true);
}

bool CServerConnection::RecvCommand( uint8_t &readCmdNumber, uint8_t *&pData, uint32_t &dataLen )
{
	if ( !m_pSocket )
		return (false);

	uint8_t cmdNumber;
	uint32_t cmdLength;

	uint8_t cmdData[5];

	if ( m_rxStream.GetReadAvailable() < 5 )
		return (false); // No data to read

	if ( m_rxStream.Peak( cmdData, 5 ) == 5 )
	{
		// Check if the whole command is present
		cmdNumber = cmdData[0];
		cmdLength = *((uint32_t*)(cmdData+1));

		if ( m_rxStream.GetReadAvailable() >= cmdLength+5 )
		{
			pData = new uint8_t[cmdLength];
		
			m_rxStream.SkipForward( 5 );	
			m_rxStream.Read( pData, cmdLength );

			readCmdNumber = cmdNumber;
			dataLen = cmdLength;

			return (true);
		}
		else
			return (false);
	}
	else
		return (false);

}

bool CServerConnection::ReadStream( void )
{
	// Read in data chunks at a time
	uint8_t dataChunk[4096];

	try
	{
		int bytesRx = m_pSocket->RecvData( dataChunk, 4096 );

		m_rxStream.Write( dataChunk, bytesRx );
	}
	catch ( CStreamException e )
	{
		printf( "Stream exception on server connection read: %s\n", e.GetReason().c_str() );
		return (false);	
	}
	catch ( SocketException e )
	{
		printf( "Socket exception on server connection read: %s\n", (const char *)e );
		return (false);
	}

	return (true);	
}
