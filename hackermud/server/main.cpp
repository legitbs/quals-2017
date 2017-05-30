#include "common.h"

// The timestamp for when the server will reboot (0 means never reboot)
uint32_t g_serverRebootTimestamp = 0;

MersenneRNG g_oRNG;

void DoAuthFail( CServerConnection *pCur )
{
	pCur->Disconnect();
}

void AcceptNewConnection( TCPServerSocket *pControlSocket, NUtil::LIST_PTR( CServerConnection ) pConnectionList )
{
	try
	{
		if ( pControlSocket == NULL || pConnectionList == NULL )
			return;

		TCPSocket *pNewSocket = pControlSocket->AcceptClient();

		if ( pNewSocket )
		{
			CServerConnection *pNewConnection = new CServerConnection( pNewSocket );
#if ENABLE_LOGIN_HASH
			uint8_t loginChallenge[32];
			ReadDevURandom( loginChallenge, 32 );

			// Set the challenge data for the response token
			pNewConnection->SetLoginChallengeData( loginChallenge, 32 );

			// Send them the challenge data (they will need to reply with it)
			NWCSendAuthChallenge( pNewConnection, loginChallenge, 32 );	
#else

#if ENABLE_PROOF_OF_WORK
			uint8_t proofSeed[16];
			ReadDevURandom( proofSeed, 16 );

			// Set Proof Seed for connection
			printf( "Proof seed: " );
			for ( uint32_t i = 0; i < 16; i++ )
				printf( "%02x", proofSeed[i] );
			printf( "\n" );

			pNewConnection->SetProofSeed( proofSeed, 16 );

			// Send Proof Auth Request
			NWCSendAuthProof( pNewConnection, proofSeed, 16 );
#endif

#endif
			// Add new connection
			pConnectionList->InsertHead( pNewConnection );

			printf( "NEW CON\n" );
		}

	}	
	catch ( SocketException e )
	{
		printf( "[ACCEPT] Exception: %s\n", (const char*)e );
	}
} 

void HandlePlayerData( CServerConnection *pCur, NUtil::LIST_PTR( CServerConnection ) pConnectionList )
{
	if ( !pCur->ReadStream() )
	{
		pCur->Disconnect();
		return;
	}

	pCur->ResetInactivityTimer();

	for (;;)
	{
		uint8_t cmdNumber;
		uint8_t *pCmdData;
		uint32_t cmdLen;

		if ( pCur->RecvCommand( cmdNumber, pCmdData, cmdLen ) )
		{
			if ( !CNetworkCommands::DoNetworkCommand( pCur, pConnectionList, cmdNumber, pCmdData, cmdLen ) )
			{
				pCur->Disconnect();
				printf( "Command from player failed.\n" );
			}

			if ( pCmdData )
				delete pCmdData;
		}
		else
			break;
	}
}

void HandleDisconnects( NUtil::LIST_PTR( CServerConnection ) pConnectionList )
{
	CServerConnection *pCur;
	CServerConnection *pNext;

	for ( pCur = pConnectionList->Head(); pCur; pCur = pNext )
	{
		pNext = pConnectionList->Next( pCur );

		pCur->UpdateInactivityTimer();

		if ( pCur->GetInactivityTimer() > MAX_INACTIVITY_TICK_COUNT )
		{
			string sInfoMessage = "Kicked by server for being inactive";

			if ( pCur->GetPlayer() )
				NWCPlayerInfoMessage( pCur->GetPlayer(), sInfoMessage );

			pCur->Disconnect();
		}

		if ( pCur->IsDisconnected() )
		{
			if ( pCur->GetPlayer() )
			{
				printf( "[%s] disconnected.\n", pCur->GetPlayer()->GetName().c_str() );

				if ( pCur->GetPlayer()->GetRoom() )
					NWCPlayerLeavesFromRoom( pCur, NWC_PLAYER_LEAVES_LOGOUT, 0 );
				
				// Drop any items on exit
				pCur->GetPlayer()->DoExit();
			}
			else
				printf( "LOGIN disconnected\n" );

			try
			{
				delete pCur;
			}
			catch ( SocketException e )
			{
				printf( "[CLOSE] Exception: %s\n", (const char*)e );
			}
		}
	}	
}

void InitRandom( void )
{
	uint32_t randomSeed;

	randomSeed = (uint32_t)time(NULL);

	printf( "Seeded random number generator with: %u\n", randomSeed );
	g_oRNG.Seed( randomSeed );
}

int main( int argc, char *argv[] )
{
	uint32_t tickCount = 0;

	// Don't buffer output for server logging
	setvbuf( stdout, NULL, _IONBF, 0 );
	
	// Setup signal handlers...
	signal( SIGPIPE, SIG_IGN );

	// Print banner and start loading server objects	
	printf( "Starting hackermud server (start time: %u).\n", (uint32_t)time(NULL) );

	printf( "Initializing random number generator.\n" );
	InitRandom();

	printf( "Initializing world data.\n" );
	InitWorld();

	if ( argc > 1 )
	{
		uint32_t rebootInSeconds = atoi(argv[1]);

		if ( rebootInSeconds < 60 )
			rebootInSeconds = 60;

		g_serverRebootTimestamp = time(NULL) + rebootInSeconds;

		printf( "Hacker server set to automatically reboot in %d seconds.\n", rebootInSeconds );
	}

	struct timeval null_timeout;
	null_timeout.tv_sec = 0;
	null_timeout.tv_usec = 0;

	struct timeval last_time;

	// Get the last time for the main game loop
	gettimeofday( &last_time, NULL );

	try
	{
		TCPServerSocket oServerControlSocket( SERVER_IP_PORT, SERVER_BACKLOG_COUNT );
	

		NUtil::LIST_DECLARE( CServerConnection, m_connectionLink ) oConnectionList;

		bool bRunning = true;
		do
		{
			for ( uint8_t iCount = 0; iCount < 50; iCount++ )
			{
				// Accept up to 50 connections in one game tick
				SocketSet oServerSocketSet;
				oServerSocketSet += (SocketBase *)&oServerControlSocket;

				// Check for connections
				if ( select( oServerSocketSet+1, oServerSocketSet, NULL, NULL, &null_timeout ) != 0 )
				{
					// Accept new connections
					AcceptNewConnection( &oServerControlSocket, &oConnectionList );
				}
				else
					break; 	// No new connections
			}

			SocketSet oClientSocketSet;
			oClientSocketSet.Clear();

			for ( CServerConnection *pCur = (CServerConnection *)oConnectionList.Head(); pCur; pCur = (CServerConnection *)oConnectionList.Next( pCur ) )
			{
				oClientSocketSet += (SocketBase *)pCur->GetSocket();
			}
	
			if ( select( oClientSocketSet+1, oClientSocketSet, NULL, NULL, &null_timeout ) != 0 )
			{
				for ( CServerConnection *pCur = (CServerConnection *)oConnectionList.Head(); pCur; pCur = (CServerConnection *)oConnectionList.Next( pCur ) )
				{
					// Ignore disconnected sockets
					if ( pCur->IsDisconnected() )
						continue;

					if ( oClientSocketSet.IsMember( (SocketBase *)pCur->GetSocket() ) )
					{
						HandlePlayerData( pCur, &oConnectionList );
					}
				}
			}

			// Update events
			UpdateEvent( tickCount++ );

			// Handle disconnects
			HandleDisconnects( &oConnectionList );

			// Wait game tick to align with game loop
			{
				struct timeval now_time;

				gettimeofday( &now_time, NULL );

				int32_t delta_usec = (last_time.tv_usec - now_time.tv_usec) + 1000000 / SERVER_TICK_PER_SECOND;
				int32_t delta_sec = (last_time.tv_sec - now_time.tv_sec);

				while ( delta_usec < 0 )
				{
					delta_usec += 1000000;
					delta_sec--;
				}

				while ( delta_usec >= 1000000 )
				{
					delta_usec -= 1000000;
					delta_sec++;
				}

				if ( delta_sec > 0 || (delta_sec == 0 && delta_usec > 0) )
				{
					struct timeval stall_time;
		
					stall_time.tv_usec = delta_usec;
					stall_time.tv_sec = delta_sec;

					if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 )
					{
						printf( "Critical error -- select failed during game loop stall.\n" );
						exit(-1);
					}

				}
			}

			// Reset last time for synchronizing server delay
			gettimeofday( &last_time, NULL );

		} while ( bRunning );
	
	} 
	catch ( SocketException e )
	{
		printf( "Socket Exception: %s\n", (const char*)e );
		exit(-1);
	}

	// Destroy world
	DestroyWorld();

	// Shutdown
	printf( "Stopping hackermud server.\n" );
}
