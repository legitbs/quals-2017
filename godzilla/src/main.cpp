#include "common.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define USE_RSA_LIBRARY


#define KEY_SIZE	(24)	// 24 * 32 = 768 bits

void sig_alarm_handler( int signum )
{
        LogDebug( LOG_PRIORITY_LOW, "Connection timed out, closing!\n" );

        exit( 1 );
}

#ifdef USE_RSA_LIBRARY
void EncryptDecrypt ( RSANum &output, const RSANum &input, const RSANum &exponent, const RSANum &modulus )
#else
void EncryptDecrypt ( BigInt &output, const BigInt &input, const BigInt &exponent, const BigInt &modulus )
#endif
{
#ifdef USE_RSA_LIBRARY
	RSA::EncryptDecrypt( output, input, exponent, modulus );	
#else
	BigInt::Power ( input, exponent, modulus, output );
#endif
}

#ifdef USE_RSA_LIBRARY
RSANum *g_pPrivateExponent = NULL;
RSANum *g_pModulus = NULL;
#else
BigInt *g_pPrivateExponent = NULL;
BigInt *g_pModulus = NULL;
#endif

typedef struct RESPONSE_HEADER
{
	uint32_t requestTime;
	uint32_t processTime;
	uint32_t totalTime;
} __attribute__((packed)) tResponseHeader;

FastCodeTimer g_requestTimer;
LinuxPerformanceCounter g_processTimer;
SimplePerformanceCounter g_simpleCounter;
//FastCodeTimer g_processTimer;
FastCodeTimer g_totalTimer;

#if 0
void TestCrypto ( void )
{
	char output[2048];
	BigInt input ( KEY_SIZE );
	BigInt cipher ( KEY_SIZE );
	BigInt plain ( KEY_SIZE );

	BigInt modulus ( KEY_SIZE );
	BigInt exponent ( KEY_SIZE );
	BigInt private_exponent ( KEY_SIZE );

	modulus.scan ( "1003103838556651507628555636330026033778617920156717988356542246694938166737814566792763905093451568623751209393228473104621241127455927948500155303095577513801000908445368656518814002954652859078574695890342113223231421454500402449" );
	exponent = 65537;
	private_exponent.scan ( "392367165134195361369272681650368759264459318907143531616033088088022642237573834928857022858394967143290999783258469020147556675528861538147848511732866777271686517796157098707401444299783684621760881754370753860601693598162218193" );

	input.scan( "2398239582895892389442893489" );

	EncryptDecrypt ( cipher, input, exponent, modulus );

	cipher.edit ( output );
	printf ( "Cipher Data: %s\n", output );

	EncryptDecrypt ( plain, cipher, private_exponent, modulus );

	plain.edit ( output );
	printf ( "Plain Data: %s\n", output );

}
#endif

bool ReadPrivateKey ( const char *pszFilename, BigInt &modulus, BigInt &privateExponent )
{
	FILE *pFile;
	char szLine[2048];

	if ( !(pFile = fopen ( pszFilename, "r" )) )
	{
		LogDebug ( LOG_PRIORITY_HIGH, "Unable to open private key, aborting.\n" );
		return (false);
	}

	bool bFoundModulus = false;
	bool bFoundExponent = false;
	while ( !feof ( pFile ) )
	{
		if ( fgets ( szLine, 2048, pFile ) == NULL )
			break;
		
		if ( strlen ( szLine ) > 16 )
		{
			bool bColonFound = false;
			uint32_t colon_position = 0;

			for ( colon_position = 0; colon_position < strlen ( szLine ); colon_position++ )
			{
				if ( szLine[colon_position] == ':' )
				{
					bColonFound = true;
					break;
				}
			}

			if ( !bColonFound )
				continue;

			if ( strncmp ( szLine, "modulus", colon_position ) == 0 )
			{
				if ( modulus.scan ( szLine + colon_position + 1 ) )
					bFoundModulus = true;
			}

			if ( strncmp ( szLine, "privateExponent", colon_position ) == 0 )
			{
				if ( privateExponent.scan ( szLine + colon_position + 1 ) )
					bFoundExponent = true;
			}
		}
	}

	fclose ( pFile );

	if ( bFoundExponent && bFoundModulus )
		return (true);
	else
		return (false);
}

uint8_t *BuildServerCommand( uint8_t *responseKey, CStreamData *pCommand, uint32_t &outLength, bool bUseTheirKey, uint8_t *pTheirData )
{
	uint32_t newCommandLength = sizeof(uint8_t) + pCommand->GetLength() + sizeof(tServerRequestEncryptedHeader);

	uint8_t *pNewCommandData = new uint8_t[newCommandLength];

	if ( !pNewCommandData )
		return (NULL);

	tServerRequestEncryptedHeader *pEncryptedHeader = (tServerRequestEncryptedHeader *)(pNewCommandData);

	if ( bUseTheirKey )
	{
		memcpy( pEncryptedHeader, pTheirData, sizeof(tServerRequestEncryptedHeader) );
	}
	else
	{
		ReadDevURandom( pEncryptedHeader->responseKey, 16 );

		memcpy( pEncryptedHeader->responseKey, responseKey, 16 );
		ReadDevURandom( pEncryptedHeader->forwardKey, 16 );

		ReadDevURandom( pEncryptedHeader->padding, 40 );
		
		pEncryptedHeader->zero1 = 0;
		pEncryptedHeader->zero2 = 0;

		pEncryptedHeader->magic1 = ENCRYPT_MAGIC1;
		pEncryptedHeader->magic2 = ENCRYPT_MAGIC2;
		pEncryptedHeader->magic3 = ENCRYPT_MAGIC3;
		pEncryptedHeader->magic4 = ENCRYPT_MAGIC4;
	}


	// Set command length
	pNewCommandData[sizeof(tServerRequestEncryptedHeader)] = pCommand->GetLength();

	// Encrypted forward data
	pCommand->Peak( pNewCommandData+sizeof(tServerRequestEncryptedHeader)+sizeof(uint8_t), pCommand->GetLength() );

	EncryptRequestData( (uint32_t*)pEncryptedHeader->forwardKey, pNewCommandData+sizeof(tServerRequestEncryptedHeader)+sizeof(uint8_t), pCommand->GetLength() );	

	// Encrypt with private key the server command header	
#ifdef USE_RSA_LIBRARY
	RSANum input = 0;
	RSANum cipher;
	RSANum exponent;
#else
	BigInt input ( KEY_SIZE );
	BigInt cipher ( KEY_SIZE );

	BigInt exponent ( KEY_SIZE );
#endif

	exponent = 65537;


	if ( (KEY_SIZE*4) != sizeof(tServerRequestEncryptedHeader) )
	{
		LogDebug( LOG_PRIORITY_HIGH, "Invalid size in encrypted header\n" );
		return (NULL);
	}

#ifdef USE_RSA_LIBRARY
	if ( !RSA::SetBytes( input, pNewCommandData, (KEY_SIZE*4) ) )
		LogDebug( LOG_PRIORITY_LOW, "Failed to set input bytes" );
	
#else
	if ( !input.SetBytes( pNewCommandData, (KEY_SIZE*4) ) )
		LogDebug( LOG_PRIORITY_LOW, "Failed to set input bytes" );
#endif

	if ( input >= (*g_pModulus) )
	{
		LogDebug( LOG_PRIORITY_HIGH, "Invalid input used, greater than modulus!\n" );
		return (NULL);
	}

	// Start process timer
	//g_processTimer.StartTimer();
	g_simpleCounter.StartTimer();
	EncryptDecrypt ( cipher, input, (*g_pPrivateExponent), (*g_pModulus) );

	// End process timer
	//g_processTimer.EndTimer();

#ifdef NO_LOGGING

#else

#ifdef USE_RSA_LIBRARY

#else
	char output[2048];
	cipher.edit ( output );
	
	LogDebug( LOG_PRIORITY_LOW, "Cipher Data: %s\n", output );

	input.edit( output );
	LogDebug( LOG_PRIORITY_LOW, "Input Data: %s\n", output );
#endif

#endif

#ifdef USE_RSA_LIBRARY
	RSA::ReadBytes( cipher, pNewCommandData, (KEY_SIZE*4) );
#else
	cipher.ReadBytes( pNewCommandData, (KEY_SIZE*4) );
#endif

#ifdef NO_LOGGING

#else
	for ( uint32_t i = 0; i < newCommandLength; i++ )
		LogDebug( LOG_PRIORITY_LOW, "pNewCommandData[%d] = %02x\n", i, pNewCommandData[i] );
#endif

	outLength = newCommandLength;

	return pNewCommandData;
}

bool SendServerCommand( uint32_t cmdID, uint8_t decryptKey[16], uint8_t *pData, uint32_t dataLen, CStreamData *pServerResponse )
{
	int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	struct sockaddr_in server_addr;

	bzero( &server_addr, sizeof(server_addr) );
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(REMOTE_SERVER_PORT);

	inet_pton( AF_INET, "127.0.0.1", &(server_addr.sin_addr) );

	LogDebug( LOG_PRIORITY_LOW, "Client: Waiting to connect to server\n", dataLen );
	if ( connect( sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr) ) < 0 )
	{
		LogDebug( LOG_PRIORITY_HIGH, "Failed to connect to remote server!\n" );
		return (false);
	}

	CIOConnection oServerConnection( sockfd, sockfd );

	LogDebug( LOG_PRIORITY_LOW, "Client: Sending %d bytes to server\n", dataLen+4 );

	if ( oServerConnection.WriteData( (uint8_t*)&cmdID, sizeof(cmdID) ) != sizeof(cmdID) )
	{
		LogDebug( LOG_PRIORITY_HIGH, "Failed to send cmdID!\n" );
		return (false);
	}

	if ( oServerConnection.WriteData( pData, dataLen ) != dataLen )
	{
		LogDebug( LOG_PRIORITY_HIGH, "Failed to send data!\n" );
		return (false);
	}

	uint32_t responseID;
	uint8_t responseLen;

	if ( oServerConnection.ReadData( (uint8_t*)&responseID, sizeof(responseID)) != sizeof(responseID) )
	{
		LogDebug( LOG_PRIORITY_HIGH, "Failed to receive data!\n" );
		return (false);
	}

	if ( responseID != cmdID )
	{
		LogDebug( LOG_PRIORITY_HIGH, "Response ID did not match CMD ID\n" );
		return (false);
	}

	if ( oServerConnection.ReadData( (uint8_t*)&responseLen, sizeof(responseLen)) != sizeof(responseLen) )
	{
		LogDebug( LOG_PRIORITY_HIGH, "Failed to receive data!\n" );
		return (false);
	}

	if ( responseLen > MAX_COMMUNICATION_LENGTH )
	{
		LogDebug( LOG_PRIORITY_HIGH, "Failed to parse response data!\n" );
		return (false);
	}

	uint8_t *pResponseData = new uint8_t[responseLen];

	if ( oServerConnection.ReadData( pResponseData, responseLen ) != responseLen )
	{
		LogDebug( LOG_PRIORITY_HIGH, "Failed to receive data!\n" );
		return (false);
	}

	LogDebug( LOG_PRIORITY_LOW, "Response Data CMD ID = %08x LENGTH = %d\n", responseID, responseLen );

	// Close connection
	close( sockfd );

	// Decrypt
	DecryptRequestData( (uint32_t*)decryptKey, pResponseData, responseLen );

	pServerResponse->Write( (uint8_t*)&responseID, sizeof(responseID) );
	pServerResponse->Write( (uint8_t*)&responseLen, sizeof(responseLen) );
	pServerResponse->Write( pResponseData, responseLen );

	return (true);	
}
			
uint8_t *BuildCommandResponse( uint32_t cmdID, uint8_t *pResponseData, uint32_t responseLength, uint32_t &outLength )
{
	uint8_t *pCommandResponse = new uint8_t[sizeof(tResponseHeader)+responseLength+sizeof(uint16_t)];
	outLength = sizeof(tResponseHeader)+responseLength+sizeof(uint16_t);

	tResponseHeader *pResponseHeader = (tResponseHeader *)pCommandResponse;

	pResponseHeader->requestTime = (uint32_t)(g_requestTimer.GetTime() >> 6);	
	pResponseHeader->processTime = (uint32_t)(g_simpleCounter.GetTime()); // g_processTimer.GetTime());
	pResponseHeader->totalTime = (uint32_t)(g_totalTimer.GetTime() >> 6);

	LogDebug( LOG_PRIORITY_LOW, "requestTime = %u, processTime = %u, totalTime = %u, responseLength = %u", pResponseHeader->requestTime, pResponseHeader->processTime, pResponseHeader->totalTime, responseLength );

	*((uint16_t*)(pCommandResponse+sizeof(tResponseHeader))) = responseLength;

	memcpy( pCommandResponse+sizeof(tResponseHeader)+sizeof(uint16_t), pResponseData, responseLength );

	return (pCommandResponse);
}

bool SendErrorResponse( CIOConnection &oConnection, uint32_t cmdID )
{
	uint8_t responseData[2];

	responseData[0] = 0xFF;
	responseData[1] = 0;
 
	uint32_t outLength;
	uint8_t *pCommandResponse = BuildCommandResponse( cmdID, responseData, 2, outLength );

	if ( pCommandResponse && outLength > 0 )
	{
		// Send data
		if ( oConnection.WriteData( pCommandResponse, outLength ) != outLength )
			return (false);
	}
	else
		return (false);

	return (true);
}

bool MainReceiver( void )
{
	uint8_t rxData[4096];
	bool bDone = false;
	CIOConnection oConnection( 1, 2 );

	uint8_t cmdNumber;
	uint8_t cmdLength;
	uint32_t cmdID;


	while ( !bDone )
	{
		bool bUseTheirKeyData = false;
		uint8_t theirKeyData[sizeof(tServerRequestEncryptedHeader)];
		LogDebug( LOG_PRIORITY_LOW, "Their key data is %d size in length", sizeof(tServerRequestEncryptedHeader) );

		if ( oConnection.ReadData( (uint8_t*)&cmdID, sizeof(cmdID) ) != sizeof(cmdID) )
		{
			// Command ID
			return (false);
		}
		
		// Start Timers
		g_totalTimer.StartTimer();

		if ( oConnection.ReadData( (uint8_t*)&cmdNumber, sizeof(cmdNumber) ) != sizeof(cmdNumber) )
		{
			// Command failed
			return (false);
		}

		if ( cmdNumber & 0x80 )
		{
			LogDebug( LOG_PRIORITY_LOW, "Reading key data" );
			// Let them specify the encrypted data	
			if ( oConnection.ReadData( theirKeyData, sizeof(theirKeyData) ) != sizeof(theirKeyData) )
				return (false);

			bUseTheirKeyData = true;
		}

		if ( oConnection.ReadData( (uint8_t*)&cmdLength, sizeof(cmdLength) ) != sizeof(cmdLength) )
		{
			LogDebug( LOG_PRIORITY_LOW, "Connection read failed on receiving command length" );
			// Command failed
			return (false);
		}

		uint8_t *pCommandBuffer = new uint8_t[cmdLength];

		if ( oConnection.ReadData( pCommandBuffer, cmdLength ) != cmdLength )
		{
			LogDebug( LOG_PRIORITY_LOW, "Connection read failed on receiving command buffer" );
			delete pCommandBuffer;

			// Failed to read command data
			return (false);
		}



		// Process command
		CStreamData oCommandResponse;
		if ( !ProcessCommand( (cmdNumber & 0x7F), pCommandBuffer, cmdLength, bDone, &oCommandResponse ) )
		{
			LogDebug( LOG_PRIORITY_LOW, "Failed to process command" );
			delete pCommandBuffer;

			return (false);
		}
		

		// Free original command buffer
		delete pCommandBuffer;

		// Start request timer
		g_requestTimer.StartTimer();

		uint32_t outLength;
		
		uint8_t responseKey[16];
		uint8_t *pServerCommandData = BuildServerCommand( responseKey, &oCommandResponse, outLength, bUseTheirKeyData, theirKeyData );

		// End total timer
		g_totalTimer.EndTimer();

		if ( pServerCommandData == NULL )
		{
			LogDebug( LOG_PRIORITY_LOW, "Failed to build server command" );


			if ( !SendErrorResponse( oConnection, cmdID ) )
				return (false);

			continue;
		}

		if ( outLength > MAX_COMMUNICATION_LENGTH )
		{
			LogDebug( LOG_PRIORITY_LOW, "Out length too large\n" );
			
			if ( !SendErrorResponse( oConnection, cmdID ) )
				return (false);

			continue;
		}
		

		// Send server command
		CStreamData oServerResponse;
		if ( !SendServerCommand( cmdID, responseKey, pServerCommandData, outLength, &oServerResponse ) )
		{
			LogDebug( LOG_PRIORITY_LOW, "Send server command failed" );
			
			if ( !SendErrorResponse( oConnection, cmdID ) )
				return (false);

			continue;
		}

		// End request timer
		g_requestTimer.EndTimer();

		// Zero key after use
		bzero( responseKey, 16 );

		// Delete server command data -- no longer needed
		delete pServerCommandData;

		// Check for a server response -- before we build a command response message!
		if ( oServerResponse.GetLength() > 0 )
		{
			uint8_t *pResponseData = oServerResponse.GetCopyData();
		
			if ( !pResponseData )
				continue;
	

			// Build response 
			uint8_t *pCommandResponse = BuildCommandResponse( cmdID, pResponseData, oServerResponse.GetLength(), outLength );

			delete pResponseData;

			if ( pCommandResponse && outLength > 0 )
			{
				// Send data
				if ( oConnection.WriteData( pCommandResponse, outLength ) != outLength )
					return (false);
			}

			if ( pCommandResponse )
				delete pCommandResponse;
		}
		else
		{
			if ( !SendErrorResponse( oConnection, cmdID ) )
				return (false);
		}	
	}

	return (bDone);
}

int main ( void )
{
	// XINETD service
        setvbuf( stdout, NULL, _IONBF, 0 );

	OpenDebugLog( DEBUG_LOG_FILENAME );

	LogDebug( LOG_PRIORITY_LOW, "Service started:\n" );

#ifdef USE_RSA_LIBRARY
	g_pPrivateExponent = new RSANum;
	g_pModulus = new RSANum;

	if ( !RSA::LoadPrivateKey( PRIVATE_KEY_FILENAME, g_pPrivateExponent, g_pModulus ) )
	{
		LogDebug( LOG_PRIORITY_HIGH, "Failed to read key file\n" );
		return (-1);
	}

	std::string sOutput;
	g_pModulus->ToString( sOutput );

	LogDebug( LOG_PRIORITY_LOW, "Modulus: %s\n", sOutput.c_str() );
#else
	g_pPrivateExponent = new BigInt( KEY_SIZE );
	g_pModulus = new BigInt( KEY_SIZE );

	if ( !ReadPrivateKey( PRIVATE_KEY_FILENAME, *g_pModulus, *g_pPrivateExponent ) )
	{
		LogDebug( LOG_PRIORITY_HIGH, "Failed to read key file\n" );
		return (-1);
	}
#endif

	MainReceiver();

	CloseDebugLog();

	delete g_pPrivateExponent;
	delete g_pModulus;

	//TestCrypto ( );
#if 0
	char output[512];

	BigInt A ( 32 );
	BigInt B ( 32 );
	BigInt C ( 32 );
	BigInt N ( 32 );
	BigInt R ( 32 );

	A.scan( "3123456789" );
	B.scan( "3123456789" );

	N.scan ( "73123456789" );

	C = A;

	C.edit ( output );
	printf ( "C: %s\n", output );

	C += B;

	C.dump ( );

	A.edit ( output );
	printf ( "A: %s\n", output );

	B.edit ( output );
	printf ( "B: %s\n", output );

	C.edit ( output );
	printf ( "Output is: %s\n", output );

	// Do Power
	printf ( "Power function:\n" );

	// C /= 17;
	BigInt::Power ( C, A, N, R );

	N /= A;

	N.edit ( output );
	printf ( "N: %s\n", output );

	R.edit ( output );
	printf ( "Output is: %s\n", output );
#endif

	CloseDebugLog();
	return 0;
}
