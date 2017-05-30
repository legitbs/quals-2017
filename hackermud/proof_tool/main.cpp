#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pow.h"

_POW::Equihash *g_pProofHash = NULL;

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

void CalculateProofOfWork( void )
{
	// Send the Proof of Work inputs
	printf( "Calculating Proof of Work.\n" );
	_POW::Proof oSolution = g_pProofHash->FindProof();
	
	uint8_t responseData[17*4];
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

	*((uint32_t*)responseData) = nonce;	
	memcpy( responseData+4, powInputs, 4*16 );

	printf( "Proof complete. Response data: " );
	for ( uint32_t i = 0; i < (4*17); i++ )
		printf( "%02x", responseData[i] );

	printf( "\n" );
}

void GetProofOfWork( void  )
{
	char szLine[1024];
	printf( "Enter proof input: " );

	if ( fgets( szLine, 512, stdin ) == NULL )
		return;

	uint32_t dataLen = 0;
	uint8_t *pHexData = ConvertHexStringToData( szLine, dataLen );

	if ( pHexData == NULL )
	{
		printf( "Invalid hex string!\n" );
		return;
	}

	if ( dataLen != 18 )
	{
		printf( "Invalid hex string, wrong length!\n" );
		return;
	}
	
	uint8_t POW_K = pHexData[0];
	uint8_t POW_N = pHexData[1];

	uint32_t seed[4];
	seed[0] = *((uint32_t*)(pHexData+2));
	seed[1] = *((uint32_t*)(pHexData+6));
	seed[2] = *((uint32_t*)(pHexData+10));
	seed[3] = *((uint32_t*)(pHexData+14));
		
	_POW::Seed hashSeed( 0 );
	hashSeed.SetSeed( seed );

	g_pProofHash = new _POW::Equihash( POW_N, POW_K, hashSeed );
		
	if ( !g_pProofHash )
	{
		printf( "Failed to allocate Proof of Work function!\n" );
		return;
	}
}

int main( void )
{
	setvbuf( stdout, NULL, _IONBF, 0 );

	GetProofOfWork( );

	CalculateProofOfWork( );
}
