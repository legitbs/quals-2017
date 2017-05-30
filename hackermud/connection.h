#ifndef __SERVER_CONNECTION_H__
#define __SERVER_CONNECTION_H__

#include "common.h"

class CServerConnection
{
public:
	typedef enum
	{
		CONNECTION_WAIT_AUTH,
		CONNECTION_PLAYER,
		CONNECTION_DISCONNECT
	} eConnectionState;

public:
	CServerConnection( TCPClientSocket *pSocket );
	~CServerConnection( );

	eConnectionState GetConnectionState( void ) { return m_connectionState; };
	CPlayer *GetPlayer( void )
	{
		if ( IsDisconnected() )
			return (NULL);
		else
			return &m_oPlayer; 
	}

	CPlayer *GetPlayerForAuthentication( void )
	{
		return (&m_oPlayer);
	}

	TCPSocket *GetSocket( void ) { return m_pSocket; };

	void Authenticate( );

	void Disconnect( void );

	bool IsDisconnected( void ) const { return m_connectionState == CONNECTION_DISCONNECT; };

	bool SendCommand( uint8_t cmdNumber, CStreamData *pData );
	bool RecvCommand( uint8_t &cmdNumber, uint8_t *&pData, uint32_t &dataLen );

	bool ReadStream( void );

	void WriteOutput( char *pszString )
	{
		m_bOutputAvailable = true;
		m_sOutputString += pszString;
	}

	bool HasOutput( void ) { return (m_bOutputAvailable); };
	std::string &GetOutput( void ) { return m_sOutputString; };
	void ClearOutput( void ) 
	{ 
		m_bOutputAvailable = false; 

		m_sOutputString = "";
	}

private:	
	CPlayer m_oPlayer;
	eConnectionState m_connectionState;	
	TCPClientSocket *m_pSocket;

	bool m_bOutputAvailable;
	string m_sOutputString;

	CStreamData m_rxStream;
};

#endif // __SERVER_CONNECTION_H__
