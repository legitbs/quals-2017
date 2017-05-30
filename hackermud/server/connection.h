#ifndef __SERVER_CONNECTION_H__
#define __SERVER_CONNECTION_H__

#include "common.h"

class CServerConnection
{
public:
	typedef enum
	{
		CONNECTION_WAIT_PROOF,
		CONNECTION_WAIT_AUTH,
		CONNECTION_PLAYER,
		CONNECTION_DISCONNECT
	} eConnectionState;

public:
	CServerConnection( TCPSocket *pSocket );
	~CServerConnection( );

	eConnectionState GetConnectionState( void ) { return m_connectionState; };
	CPlayer *GetPlayer( void ) { return m_pPlayer; };
	TCPSocket *GetSocket( void ) { return m_pSocket; };

	void AcceptProof( void )
	{
		m_connectionState = CONNECTION_WAIT_AUTH;
	}

	void Authenticate( CPlayer *pPlayer );

	void Disconnect( bool bFastDisconnect = false );

	bool IsDisconnected( void ) const { return m_connectionState == CONNECTION_DISCONNECT; };

	bool SendCommand( uint8_t cmdNumber, CStreamData *pData );
	bool RecvCommand( uint8_t &cmdNumber, uint8_t *&pData, uint32_t &dataLen );

	bool ReadStream( void );

	bool SetLoginChallengeData( uint8_t *pData, uint32_t dataLen ) 
	{ 
		if ( dataLen != 32 || !pData )
			return false;
		
		memcpy( m_loginChallengeData, pData, 32 ); 
	
		return (true);
	};

	bool GetLoginChallengeData( uint8_t *pData, uint32_t dataLen )
	{
		if ( dataLen != 32 || !pData )
			return (false);

		memcpy( pData, m_loginChallengeData, 32 );

		return (true);	
	}

	void ResetInactivityTimer( void )
	{
		m_inactivityTimer = 0;
	}

	uint32_t GetInactivityTimer( void )
	{
		return (m_inactivityTimer);
	}

	void UpdateInactivityTimer( void )
	{
		m_inactivityTimer++;
	}

	bool DidPassProof( void )
	{
		return (m_bPassProof);
	}

	void SetDidPassProof( void )
	{
		m_bPassProof = true;
	}

	bool SetProofSeed( uint8_t *pProofData, uint8_t proofLength )
	{
		if ( proofLength != 16 )
			return (false);

		memcpy( m_proofSeed, pProofData, 16 );
	}

	bool GetProofSeed( uint8_t *pDest, uint8_t destLen )
	{
		if ( destLen < 16 )
			return (false);

		memcpy( pDest, m_proofSeed, 16 );
	}

private:	
	CPlayer *m_pPlayer;
	eConnectionState m_connectionState;	
	TCPSocket *m_pSocket;

	CStreamData m_rxStream;

	uint8_t m_loginChallengeData[32];
	uint8_t m_proofSeed[16];
	bool m_bPassProof;

	uint32_t m_inactivityTimer; // Inactive (no commands) timer

public:
	NUtil::LIST_LINK( CServerConnection ) m_connectionLink;
};

#endif // __SERVER_CONNECTION_H__
