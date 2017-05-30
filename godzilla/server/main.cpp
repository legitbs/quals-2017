#include "common.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define KEY_SIZE	(24)	// 24 * 32 = 768 bits

void sig_alarm_handler( int signum )
{
        LogDebug( LOG_PRIORITY_LOW, "Connection timed out, closing!\n" );

        exit( 1 );
}

void EncryptDecrypt ( BigInt &output, const BigInt &input, const BigInt &exponent, const BigInt &modulus )
{
	BigInt::Power ( input, exponent, modulus, output );
}

BigInt *g_pModulus = NULL;

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

bool ReadPublicKey ( BigInt &modulus )
{
	modulus.scan ( "1003103838556651507628555636330026033778617920156717988356542246694938166737814566792763905093451568623751209393228473104621241127455927948500155303095577513801000908445368656518814002954652859078574695890342113223231421454500402449" );

	return (true);
}
		
bool DecryptCommand( tServerRequestEncryptedHeader *pKeyHeader, uint8_t *pData, uint8_t cmdLength )
{
	char output_str[2048];
	BigInt input( KEY_SIZE );
	BigInt exponent( KEY_SIZE );
	BigInt output( KEY_SIZE );

	input.SetBytes( (uint8_t*)pKeyHeader, KEY_SIZE*4 );

        input.edit( output_str );
        LogDebug( LOG_PRIORITY_LOW, "Input Data: %s\n", output_str );

	if ( input >= *g_pModulus )
	{
		LogDebug( LOG_PRIORITY_LOW, "Input key invalid!\n" );
		return (false);
	}

	exponent = 65537;

	EncryptDecrypt( output, input, exponent, (*g_pModulus) );

	output.ReadBytes( (uint8_t*)pKeyHeader, KEY_SIZE*4 );

        output.edit ( output_str );

        LogDebug( LOG_PRIORITY_LOW, "Decrypted Data: %s\n", output_str );


	// Check zeros
	if ( pKeyHeader->zero1 != 0 || pKeyHeader->zero2 != 0 )
	{
		LogDebug( LOG_PRIORITY_LOW, "Invalid key header, missing zeros\n" );
		return (false);
	}

	// Check magics
	if ( pKeyHeader->magic1 != ENCRYPT_MAGIC1 || pKeyHeader->magic2 != ENCRYPT_MAGIC2 || pKeyHeader->magic3 != ENCRYPT_MAGIC3 || pKeyHeader->magic4 != ENCRYPT_MAGIC4 )
	{
		LogDebug( LOG_PRIORITY_LOW, "Invalid key header, missing magic\n" );
		return (false);
	}

	for ( uint32_t i = 0; i < cmdLength; i++ )
	{
		LogDebug( LOG_PRIORITY_LOW, "DecryptCommand::pData[%d] = %02x\n", i, pData[i] );
	}

	LogDebug( LOG_PRIORITY_LOW, "Decrypting with forward key (length=%d)\n", cmdLength );
	DecryptRequestData( (uint32_t*)pKeyHeader->forwardKey, pData, cmdLength );

	for ( uint32_t i = 0; i < cmdLength; i++ )
	{
		LogDebug( LOG_PRIORITY_LOW, "DecryptCommand::pData[%d] = %02x\n", i, pData[i] );
	}
	return (true);
}

uint8_t *BuildResponseCommand( tServerRequestEncryptedHeader *pKeyHeader, uint32_t cmdID, CStreamData *pData, uint32_t &outLength )
{
	if ( !pData )
		return (false);

	uint8_t *pResponse = new uint8_t[sizeof(cmdID) + sizeof(uint8_t) + pData->GetLength()];

	if ( !pResponse )
		return (false);

	// Append header -- this will be unecrypted (4-byte CMD ID + 1-byte LENGTH)
	*((uint32_t*)pResponse) = cmdID;
	pResponse[4] = pData->GetLength();

	uint32_t responsePos = sizeof(cmdID)+sizeof(uint8_t);

	pData->Read( pResponse+responsePos, pData->GetLength(), true );
	outLength = sizeof(cmdID)+pData->GetLength()+sizeof(uint8_t);

	// Encrypt the response data 
	EncryptRequestData( (uint32_t*)pKeyHeader->responseKey, pResponse+responsePos, pData->GetLength() );

	return (pResponse);
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

		if ( oConnection.ReadData( (uint8_t*)&cmdID, sizeof(cmdID) ) != sizeof(cmdID) )
		{
			LogDebug( LOG_PRIORITY_LOW, "Connection read failed on cmd ID" );
			// Command ID
			return (false);
		}

		LogDebug( LOG_PRIORITY_LOW, "Command ID: %08x\n", cmdID );
		if ( oConnection.ReadData( theirKeyData, sizeof(theirKeyData) ) != sizeof(theirKeyData) )
		{
			LogDebug( LOG_PRIORITY_HIGH, "Connection read failed on receiving key header" );
			return (false);
		}

		if ( oConnection.ReadData( (uint8_t*)&cmdLength, sizeof(cmdLength) ) != sizeof(cmdLength) )
		{
			LogDebug( LOG_PRIORITY_LOW, "Connection read failed on receiving command length" );
			// Command failed
			return (false);
		}

		uint8_t *pCommandBuffer = new uint8_t[cmdLength];

		LogDebug( LOG_PRIORITY_LOW, "Reading command (length=%d)\n", cmdLength );
		if ( oConnection.ReadData( pCommandBuffer, cmdLength ) != cmdLength )
		{
			LogDebug( LOG_PRIORITY_LOW, "Connection read failed on receiving command buffer" );
			delete pCommandBuffer;

			// Failed to read command data
			return (false);
		}

		// Decrypt Command
		if ( !DecryptCommand( (tServerRequestEncryptedHeader *)theirKeyData, pCommandBuffer, cmdLength ) )
		{
			LogDebug( LOG_PRIORITY_LOW, "Failed to decrypt command" );
			delete pCommandBuffer;

			return (false);
		}	

		if ( cmdLength < 2 )
		{
			LogDebug( LOG_PRIORITY_LOW, "Command length insufficient (%d)\n", cmdLength );
			delete pCommandBuffer;

			return (false);
		}

		cmdNumber = pCommandBuffer[0];
		uint8_t innerLength = pCommandBuffer[1];
		

		// Process command
		CStreamData oCommandResponse;
		if ( !ProcessCommand( (cmdNumber & 0x7F), pCommandBuffer+2, innerLength, bDone, &oCommandResponse ) )
		{
			LogDebug( LOG_PRIORITY_LOW, "Failed to process command" );
			delete pCommandBuffer;

			return (false);
		}

		// Free original command buffer
		delete pCommandBuffer;

		uint32_t outLength;
		
		uint8_t *pCommandResponse = BuildResponseCommand( (tServerRequestEncryptedHeader *)theirKeyData, cmdID, &oCommandResponse, outLength );

		if ( pCommandResponse == NULL )
		{
			LogDebug( LOG_PRIORITY_LOW, "Failed to build response command" );
			continue;
		}

		if ( outLength > MAX_COMMUNICATION_LENGTH )
		{
			LogDebug( LOG_PRIORITY_LOW, "Failed to build response command, outLength too long (%d)\n", outLength );
			continue;
		}

		LogDebug( LOG_PRIORITY_LOW, "Server response sent %d bytes\n", outLength );

		// Check for a server response -- before we build a command response message!
		if ( oConnection.WriteData( pCommandResponse, outLength ) != outLength )
			return (false);

		if ( pCommandResponse )
			delete pCommandResponse;
	}

	return (bDone);
}

int main ( void )
{
	// XINETD service
        setvbuf( stdout, NULL, _IONBF, 0 );

	OpenDebugLog( DEBUG_LOG_FILENAME );

	LogDebug( LOG_PRIORITY_LOW, "Server started:\n" );

	g_pModulus = new BigInt( KEY_SIZE );

	if ( !ReadPublicKey( *g_pModulus ) )
	{
		LogDebug( LOG_PRIORITY_HIGH, "Failed to read public key\n" );
		return (-1);
	}

	MainReceiver();

	CloseDebugLog();

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

	return 0;
}
