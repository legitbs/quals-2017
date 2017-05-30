#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "CommonFile.h"
#include "converter.h"
#include "crypto.h"

void TestCrypto( void )
{
	Crypto oCrypto;

	oCrypto.genKeypair();
	
	char msg[] = "This is the test message!!!!";
	size_t msgLen = strlen(msg);

	unsigned char signature1[4096];
	unsigned char signature2[4096];
	uint32_t signature1Len = 4096;
	uint32_t signature2Len = 4096;

	oCrypto.rsaSign( (const unsigned char *)msg, msgLen, signature1, &signature1Len );

	printf( "Signed... signature length is: %d\n", signature1Len );

	unsigned char *publicKey = NULL;
	size_t pubkeyLen;
	size_t privKeyLen;

	oCrypto.getPublicKey( &publicKey, &pubkeyLen );

	printf( "Public Key %s\n", publicKey );	

	unsigned char *privateKey = NULL;
	
	oCrypto.getPrivateKey( &privateKey, &privKeyLen );

	printf( "Private Key %s\n", privateKey );

	CFileWriter::WriteFile( "pub.key", publicKey, pubkeyLen );
	CFileWriter::WriteFile( "priv.key", privateKey, privKeyLen );

	Crypto oNewCrypto;

	//oNewCrypto.setPublicKey( publicKey, pubkeyLen );
	oNewCrypto.setPrivateKey( privateKey, privKeyLen );

	oNewCrypto.rsaSign( (const unsigned char *)msg, msgLen, signature2, &signature2Len );

	printf( "Signed... signature length is: %d\n", signature2Len );

	if ( memcmp( signature1, signature2, signature1Len ) == 0 )
	{
		printf( "Signatures match.\n" );
	}

	//msg[0] = 0xA;
	if ( oCrypto.rsaVerify( (const unsigned char *)msg, msgLen, signature2, signature2Len ) != SUCCESS )
	{
		printf ("Message signature check failed.\n" );
	}
	else
	{
		printf ("Message signature matches.\n" );
	}
}

int main( int argc, char **argv )
{
//	TestCrypto();
//	return 0;

	if ( argc != 4 )
	{
		printf( "Usage: %s <input ELF file> <output MEPS .meb file> <private key file>\n", argv[0] );
		exit(1);
	}

	void *pELFData = NULL;
	uint32_t elfSize = 0;

	if ( CFileReader::ReadFile( argv[1], &pELFData, elfSize ) != CFileCommon::FC_OK )
	{
		printf( "Error opening input ELF File %s\n", argv[1] );
		exit(1);
	}

        void *pPrivateKey = NULL;
	uint32_t privateKeySize = 0;

	if ( CFileReader::ReadFile( argv[3], &pPrivateKey, privateKeySize ) != CFileCommon::FC_OK )
	{
		printf( "Error open private key file %s\n", argv[3] );
		exit(1);
	}

	Crypto oCrypto;

	if ( oCrypto.setPrivateKey( (unsigned char *)pPrivateKey, privateKeySize ) != SUCCESS )
	{
		printf( "Failed to set private key.\n" );
		exit(1);
	}

	uint8_t *pOutData;
	uint32_t outDataSize = 0;

	std::string sErrorText;
	if ( !convert_elf_to_meb( (uint8_t*)pELFData, elfSize, &pOutData, outDataSize, sErrorText, oCrypto ) )
	{
		printf( "Error generating file; %s\n", sErrorText.c_str() );
		exit(1);
	}

	if ( CFileWriter::WriteFile( argv[2], pOutData, outDataSize ) != CFileCommon::FC_OK )
	{
		printf( "Error writing output file.\n" );
		exit(1);
	}

	printf( "Meb file %s created.\n", argv[2] );
	exit(0);
}
