#include "common.h"
#include "pow.h"
#include <fcntl.h>

CCommandHandler g_cmdHandler;

#if 0 // ENABLE_PROOF_OF_WORK
_POW::Equihash *g_pProofHash = NULL;
#endif

void sig_alarm_handler( int signum )
{
	printf( "%sTimeout on connection, closing.\n", FC_NORMAL_RED );
	
	exit( 1 );
}

uint8_t AsciiToHex( char c )
{
        if ( c >= '0' && c <= '9' )
                return (c - '0');
        else if ( c >= 'a' && c <= 'f' )
                return ((c - 'a') + 10);
        else if ( c >= 'A' && c <= 'F' )
                return ((c - 'A') + 10);
        else
                return 0;
}

uint8_t *ConvertHexStringToData( const char *pszStr, uint32_t &dataLen )
{
        uint8_t *pTemp = new uint8_t[(strlen(pszStr)/2)+1];

        uint32_t pos = 0;
        while ( *pszStr != '\0' && *pszStr != '\n' )
        {
                if ( pos%2 == 0 )
                        pTemp[pos/2] = AsciiToHex( *pszStr++ );
                else
                        pTemp[pos/2] = (pTemp[pos/2] << 4) | AsciiToHex( *pszStr++ );

                pos++;
        }

        if ( pos%2 == 1 )
        {
                // Invalid
                delete pTemp;
                return (NULL);
        }

        dataLen = (pos/2);

        return pTemp;
}

bool AuthenticateClient( TCPClientSocket *pClientSocket, string sUserName, uint8_t *passwordDigest )
{
	if ( !pClientSocket )
		return (false);

	// BUILD request
	uint8_t authRequest[4096];
	uint8_t authResponse[4096];
	uint32_t sendLength = 0;

	authRequest[0] = NWC_AUTH_REQUEST;
	authRequest[5] = sUserName.length();
	
	memcpy( authRequest+6, sUserName.c_str(), sUserName.length() );

	memcpy( authRequest+6+sUserName.length(), passwordDigest, 16 );

	sendLength = 6+sUserName.length()+16;

//#if ENABLE_PROOF_OF_WORK
#if 0
	// Send the Proof of Work inputs
	printf( "%sCalculating Proof of Work for login attempt.%s\n", FC_NORMAL_YELLOW, FC_BRIGHT_CYAN );
	_POW::Proof oSolution = g_pProofHash->FindProof();
	uint32_t nonce;
	uint32_t powInputs[16];

	uint32_t i = 0;
	for ( i = 0; i < oSolution.inputs.size(); i++ )
	{	
		if ( i >= 16 )
			break;

		powInputs[i] = oSolution.inputs[i];
	}

	for ( ; i < 16; i++ )
		powInputs[i] = 0;

	nonce = oSolution.nonce;

	*((uint32_t*)(authRequest+sendLength)) = nonce;
	sendLength += sizeof(nonce);
	
	memcpy( authRequest+sendLength, powInputs, 4*16 );
	sendLength += (4*16);
#endif

	// SEND
	*((uint32_t*)(authRequest+1)) = sendLength-5;

	// SEND request
	int32_t bytesSent = pClientSocket->SendData( authRequest, sendLength );

	return (bytesSent == (sendLength));
}

bool DoCreatePlayer( CServerConnection &oConnection, string sUserName, uint8_t *passwordDigest )
{
	struct timeval timeout;
	uint8_t cmdNumber;
	uint8_t *pCmdData;
	uint32_t cmdLen;

	TCPClientSocket *pClientSocket = (TCPClientSocket *)oConnection.GetSocket();
	
	if ( !pClientSocket )
		return (false);

	// BUILD request
	uint8_t authRequest[4096];
	uint8_t authResponse[4096];

	authRequest[0] = NWC_CREATE_PLAYER;
	authRequest[5] = sUserName.length();
	
	memcpy( authRequest+6, sUserName.c_str(), sUserName.length() );

	memcpy( authRequest+6+sUserName.length(), passwordDigest, 16 );

	// SEND
	*((uint32_t*)(authRequest+1)) = sUserName.length() + 16 + 1;

	// SEND request
	int32_t bytesSent = pClientSocket->SendData( authRequest, sUserName.length()+16+6 );

	if ( bytesSent != (sUserName.length()+16+6) )
		return (false);

	// Wait for authentication
	SocketSet oClientSocketSet;
	
	oClientSocketSet += (SocketBase *)oConnection.GetSocket();

	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	if ( select( (int)oClientSocketSet+1, oClientSocketSet, NULL, NULL, &timeout ) == 0 )
	{
		printf( "%sTimeout waiting for create player response.%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
		return (false);
	}
	
	// Read in data
	if ( !oConnection.ReadStream() )
	{
		printf( "%sFailed to receive create player response!%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
		return (false);
	}
				
	if ( oConnection.RecvCommand( cmdNumber, pCmdData, cmdLen ) )
	{
		uint32_t i;
		if ( cmdNumber != NWC_CREATE_PLAYER_RESPONSE || cmdLen != 1 )
		{
			printf( "%sFailed to receive create player response from server!%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
			return (false);
		}

		switch ( pCmdData[0] )
		{
		case NWC_CREATEPLAYER_PLAYER_EXISTS:
			printf( "%sPlayer already exists!\n", FC_NORMAL_RED );
			break;

		case NWC_CREATEPLAYER_INVALID_DATA:
			printf( "%sInvalid data when creating new player, try again!\n", FC_NORMAL_RED );
			break;

		case NWC_CREATEPLAYER_SUCCESS:
			printf( "%sNew player successfully created!\n", FC_BRIGHT_CYAN );
			break;

		default:
			return (false);
		}
        }
	else
	{
		printf( "%sFailed to create player response from server!%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
		return (false);
	}

	return (true);

}

bool DoLoginHash( CServerConnection &oConnection )
{
	struct timeval timeout;
	uint8_t cmdNumber;
	uint8_t *pCmdData;
	uint32_t cmdLen;
	
	// Wait for authentication
	SocketSet oClientSocketSet;
	
	oClientSocketSet += (SocketBase *)oConnection.GetSocket();

	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	if ( select( (int)oClientSocketSet+1, oClientSocketSet, NULL, NULL, &timeout ) == 0 )
	{
		printf( "%sTimeout waiting for challenge data.%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
		return (false);
	}
	
	// Read in data
	if ( !oConnection.ReadStream() )
	{
		printf( "%sFailed to receive challenge data from server!%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
		return (false);
	}
				
	if ( oConnection.RecvCommand( cmdNumber, pCmdData, cmdLen ) )
	{
		uint32_t i;
		if ( cmdNumber != NWC_AUTH_CHALLENGE || cmdLen != 36 )
		{
			printf( "%sFailed to receive challenge data from server!%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
			return (false);
		}

		if ( *((uint32_t*)pCmdData) != 32 )
		{
			printf( "%sFailed to receive challenge data from server!%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
			return (false);
		}

		pCmdData+=4;

		printf( "%sServer Login Challenge Data:\n", FC_BRIGHT_CYAN );
		for ( i = 0; i < 32; i++ )
			printf( "%02x", pCmdData[i] );

		printf( "\n\n%sEnter authentication token (hexadecimal string -- ex: a3bf..., 1024-bits):\n", FC_BRIGHT_CYAN );

		string sLoginToken;

		getline( cin, sLoginToken );

		if ( sLoginToken.length() != 256 )
		{
			printf( "%sLogin token hex string must be 256 characters in length!\n", FC_NORMAL_RED );
			return (false);
		}	

		uint8_t authCommandData[256];

		authCommandData[0] = NWC_AUTH_REQUEST;
		*((uint32_t*)(authCommandData+1)) = 128;

		// Convert...
		uint32_t cmdPos = 5;
		for ( i = 0; i < 256 && i < sLoginToken.length(); i += 2)
			authCommandData[cmdPos++] = (FromHexChar( sLoginToken[i+0] ) << 4) | FromHexChar( sLoginToken[i+1] );
	
		int32_t bytesSent = oConnection.GetSocket()->SendData( authCommandData, 128+5 );

		if ( bytesSent != 128+5 )
		{
			printf( "%sFailed to send auth command token.%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
			return (false);
		}
	}
	else
	{
		printf( "%sFailed to receive challenge data from server!%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
		return (false);
	}

	return (true);
}

bool DoLoginUsernamePassword( CServerConnection &oConnection )
{
	string sUserName;
	string sPassword;
	CMD5 oPasswordHash;

	printf( "%sPlease enter your name: ", FC_BRIGHT_CYAN );

	getline( cin, sUserName );

	if ( sUserName.length() < MIN_USERNAME_LENGTH )	
	{
		printf( "%sUsername too short.%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
		return (false);
	}

	if ( sUserName.length() > MAX_USERNAME_LENGTH )
	{
		printf( "%sUsername too long.%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
		return (false);
	}

	printf( "%sPlease enter password: ", FC_BRIGHT_CYAN );	

	getline( cin, sPassword );

	if ( sPassword.length() > MAX_PASSWORD_LENGTH )
	{
		printf( "%sPassword too long.%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
		return (false);
	}

	if ( sPassword.length() < MIN_PASSWORD_LENGTH )
	{
		printf( "%sPassword too short.%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
		return (false);
	}

	oPasswordHash.InitDigest();
	
	//oPasswordHash.UpdateDigest( (uint8_t*)sUserName.c_str(), strlen( sUserName.c_str() ) );
	oPasswordHash.UpdateDigest( (uint8_t*)sPassword.c_str(), sPassword.length() );

	uint8_t passwordDigest[16];

	oPasswordHash.GetDigest( passwordDigest, 16 );

	printf( "Logging in with password digest: " );
	for ( uint32_t i = 0; i < 16; i++ )
		printf( "%02x", passwordDigest[i] );
	printf( "\n" );

	// AUTHENTICATE
	if ( !AuthenticateClient( (TCPClientSocket *)oConnection.GetSocket(), sUserName, passwordDigest ) )
	{
		printf( "%sFailed to connect.%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
		return (false);
	}
	
	return (true);
}

bool CheckNewUser( CServerConnection &oConnection )
{
	string sSelection;
	string sNewUsername;
	string sPassword;
	CMD5 oPasswordHash;

	printf( "%sLogin with new user? (y/n) ", FC_BRIGHT_CYAN );

	getline( cin, sSelection );

	if ( sSelection[0] == 'y' || sSelection[0] == 'Y' )
	{
		printf( "%sEnter new (7-12 c): ", FC_BRIGHT_CYAN );

		getline( cin, sNewUsername );

		if ( sNewUsername.length() > MAX_USERNAME_LENGTH )
		{
			printf( "%sYour selected username is too long.%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
			return (false);
		}

		printf( "%sPlease enter password: ", FC_BRIGHT_CYAN );

		getline( cin, sPassword );

		if ( sPassword.length() > MAX_PASSWORD_LENGTH )
		{
			printf( "%sPassword too long.%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
			return (false);
		}

		if ( sPassword.length() < MIN_PASSWORD_LENGTH )
		{
			printf( "%sPassword too short.%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
			return (false);
		}

		oPasswordHash.InitDigest();
		
		//oPasswordHash.UpdateDigest( (uint8_t*)sUserName.c_str(), strlen( sUserName.c_str() ) );
		oPasswordHash.UpdateDigest( (uint8_t*)sPassword.c_str(), sPassword.length() );

		uint8_t passwordDigest[16];
		
		oPasswordHash.GetDigest( passwordDigest, 16 );

		return DoCreatePlayer( oConnection, sNewUsername, passwordDigest );
	}
	else
	{
		printf( "%sLogging in with existing user\n", FC_BRIGHT_CYAN );
		return (true);
	}

	return (true);
}

#if ENABLE_PROOF_OF_WORK
bool GetProofOfWork( CServerConnection &oConnection )
{
	struct timeval timeout;
	uint8_t cmdNumber;
	uint8_t *pCmdData;
	uint32_t cmdLen;
	
	// Wait for authentication
	SocketSet oClientSocketSet;
	
	oClientSocketSet += (SocketBase *)oConnection.GetSocket();

	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	if ( select( (int)oClientSocketSet+1, oClientSocketSet, NULL, NULL, &timeout ) == 0 )
	{
		printf( "%sTimeout waiting for proof of work data.%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
		return (false);
	}
	
	// Read in data
	if ( !oConnection.ReadStream() )
	{
		printf( "%sFailed to proof of work data from server!%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
		return (false);
	}
				
	if ( oConnection.RecvCommand( cmdNumber, pCmdData, cmdLen ) )
	{
		uint32_t i;
		if ( cmdNumber != NWC_AUTH_PROOF || cmdLen != 18 )
		{
			printf( "%sFailed to receive proof of work from server!%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
			return (false);
		}

		printf( "%sProof Challenge Data: ", FC_NORMAL_CYAN );
		for ( uint32_t i = 0; i < cmdLen; i++ )
			printf( "%02x", pCmdData[i] );
		printf( "\n" );

		// Send the Proof of Work inputs
		printf( "%sEnter Proof of Work Data: %s\n", FC_NORMAL_YELLOW, FC_BRIGHT_CYAN );

		char szLine[512];
	
		if ( fgets( szLine, 511, stdin ) == NULL )
                	return (false);

		uint32_t dataLen = 0;
		uint8_t *pHexData = ConvertHexStringToData( szLine, dataLen );

		if ( pHexData == NULL )
		{
			printf( "%sInvalid hex string!\n", FC_NORMAL_RED );
			return (false);
		}

		if ( dataLen != (17*4) )
		{
			printf( "%sInvalid hex string, wrong length!\n", FC_NORMAL_RED );
			return (false);
		}

		uint8_t proofSolutionRequest[256];
		uint32_t sendLength = 0;

		proofSolutionRequest[0] = NWC_PROOF_DATA;
		sendLength = 5;

		memcpy( proofSolutionRequest+sendLength, pHexData, dataLen );
		sendLength += dataLen;

		*((uint32_t*)(proofSolutionRequest+1)) = sendLength-5;
		
		TCPClientSocket *pClientSocket = (TCPClientSocket *)oConnection.GetSocket();

		if ( !pClientSocket )
			return (false);

		// SEND request
		int32_t bytesSent = pClientSocket->SendData( proofSolutionRequest, sendLength );

		return (bytesSent == sendLength);
	}
	else
	{
		printf( "%sReceive error from server getting proof of work!%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
		return (false);
	}

	return (true);
}
#endif

int main( void )
{
	struct timeval timeout;
	uint8_t cmdNumber;
	uint8_t *pCmdData;
	uint32_t cmdLen;

	setvbuf( stdout, NULL, _IONBF, 0 );

	int fd = open( "/home/flags/hackermud", O_RDONLY);

	printf( "%sWelcome to hackerMUD!\n", FC_NORMAL_CYAN );

#if ENABLE_SIG_ALARM_HANDLER
	signal( SIGALRM, sig_alarm_handler );
	alarm( MAX_IDLE_SECS );
#endif

	IPAddress oServerAddress;

	//oServerAddress.SetHostName( SERVER_IP_ADDRESS, false );

	TCPClientSocket oClientSocket( oServerAddress, SERVER_PORT );

	CServerConnection oConnection( &oClientSocket );

#if ENABLE_PROOF_OF_WORK
	if ( !GetProofOfWork( oConnection ) )
		return (0);
#endif

	if ( !CheckNewUser( oConnection ) )
		return (0);

#if ENABLE_LOGIN_HASH
	if ( !DoLoginHash( oConnection ) )
		return (0);
#else
	if ( !DoLoginUsernamePassword( oConnection ) )
		return (0);
#endif
	
	// Setup socket set (for select wrapper)
	SocketSet oClientSocketSet;

	oClientSocketSet += (SocketBase *)&oClientSocket;

	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	if ( select( (int)oClientSocketSet+1, oClientSocketSet, NULL, NULL, &timeout ) == 0 )
	{
		printf( "%sTimeout attempting to authenticate.%s\n", FC_NORMAL_RED, FC_NORMAL_WHITE );
		return (0);
	}

	uint32_t rxMissCounter = 0;
	uint32_t inputMissCounter = 0;

	timeout.tv_sec = 0;
	timeout.tv_usec = 125000;


	do
	{
		oClientSocketSet.Clear();	// Clear FDS
		oClientSocketSet += (SocketBase *)&oClientSocket;

		timeout.tv_sec = 0;
		timeout.tv_usec = 125000;
		if ( select( (int)oClientSocketSet+1, oClientSocketSet, NULL, NULL, &timeout ) == 0 )
		{
			rxMissCounter++;
		}
		else
		{
			// Read in data
			if ( !oConnection.ReadStream() )
				oConnection.Disconnect();

			for ( ;; )
			{
				bool bBadCommand = false;

				if ( oConnection.RecvCommand( cmdNumber, pCmdData, cmdLen ) )
				{
					rxMissCounter = 0;

					if ( !CNetworkCommands::DoNetworkCommand( &oConnection, cmdNumber, pCmdData, cmdLen ) )
						bBadCommand = true;
					
					if ( pCmdData )
						delete pCmdData;	
				}
				else
					break;

				if ( bBadCommand )
					break;
			}
		}

		if ( oConnection.IsDisconnected() )
			break;
		
		if ( oConnection.HasOutput() )
		{
			printf( "\n%s\n", oConnection.GetOutput().c_str() );
			oConnection.ClearOutput();

			DoPrompt( &oConnection );	
		}

		fd_set oStdinSet;
		FD_ZERO( &oStdinSet );
		FD_SET( STDIN_FILENO, &oStdinSet );

		timeout.tv_sec = 0;
		timeout.tv_usec = 125000;
		if ( select( STDIN_FILENO+1, &oStdinSet, NULL, NULL, &timeout ) == 0 )
		{
			inputMissCounter++;
		}
		else
		{
			inputMissCounter = 0;

			char szCommandLine[MAX_INPUT_LENGTH+1];

			if ( ReadInputLine( szCommandLine, MAX_INPUT_LENGTH ) )
			{
			
				if ( oConnection.GetPlayer() )
				{
					g_cmdHandler.DoCommand( &oConnection, szCommandLine );
				}
				else
					printf( "Not connected yet!\n" );
			}
			else
				break; // Lost connection

#if ENABLE_SIG_ALARM_HANDLER
			alarm( MAX_IDLE_SECS );
#endif
		}

		if ( inputMissCounter > NO_INPUT_DISCONNECT_COUNTER )
		{
			printf( "%sNo input received in awhile. Disconnecting.\n", FC_BRIGHT_YELLOW );
			break;	
		}

		if ( oConnection.HasOutput() )
		{
			printf( "\n%s\n", oConnection.GetOutput().c_str() );
			oConnection.ClearOutput();

			DoPrompt( &oConnection );	
		}

	} while ( !oConnection.IsDisconnected() ); 
}
