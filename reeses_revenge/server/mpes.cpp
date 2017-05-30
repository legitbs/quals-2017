#include "common.h"
#include "CommonFile.h"

#define SERVER		1


#include <stdio.h>
#include <stdlib.h>

#if SERVER

#define MAX_FILE_SIZE		100000
#define MAX_IDLE_SECS		60

uint32_t ReadStdin( uint8_t *pBuf, uint32_t bufLen )
{
	uint32_t pos = 0;
	for ( pos = 0; pos < bufLen; pos++ )
	{
		int32_t readAmount = read( 0, pBuf+pos, 1 );
		
		if ( readAmount != 1 )
		{
			return 0;
		}
	}

	return pos;
}

void sig_alarm_handler( int signum )
{
	// Shutdown
	exit(1);
}

int main( void )
{
	// Setup alarm timeout
	signal( SIGALRM, sig_alarm_handler );
	alarm( MAX_IDLE_SECS );
	
	// Disable buffer on stdout
	setvbuf(stdout, NULL, _IONBF, 0);

	uint32_t dataSize = 0;

	// Read file length
	uint32_t readLen = ReadStdin( (uint8_t*)&dataSize, 4 );

	// Seed random number generator
	unsigned int time_val = time(NULL);

	seed_prng( time_val ^ (unsigned int)(((uint64_t)&printf >> 12) & 0xFFFFFFFF) );

	if ( readLen != 4 )
	{
		printf( "ERROR: Failed read length\n" );
		return 0;
	}

	if ( dataSize > MAX_FILE_SIZE )
	{
		printf( "ERROR: File too large\n" );
		return 0;
	}

	uint8_t *pFileData = new uint8_t[dataSize];

	if ( pFileData == NULL )
	{
		printf( "ERROR: Out of memory\n" );
		return 0;
	}

	uint32_t fileReadLen = ReadStdin( pFileData, dataSize );

	if ( fileReadLen != dataSize )
	{
		printf( "ERROR: Failed read data\n" );
		return 0;
	}

	// NOW RUN
	CMebLoader oMeb;

	std::string sErrorText;
	
	if ( !oMeb.LoadFile( pFileData, dataSize, sErrorText ) )
	{
		printf( "ERROR: %s\n", sErrorText.c_str() );
		return 0;
	}

	oMeb.Run();

	return 0;
}

#else

int main( int argc, char **argv )
{
	if ( argc != 2 )
	{
		printf( "Usage: mpes <file name of binary to run>\n" );
		return 0;
	}

	// Seed random number generator
	unsigned int time_val = time(NULL);

	seed_prng( time_val ^ (unsigned int)(((uint64_t)&printf >> 12) & 0xFFFFFFFF) );

	void *pData;
	uint32_t dataSize;
	
	if ( (CFileReader::ReadFile( argv[1], &pData, dataSize ) != CFileCommon::FC_OK) )
	{
		printf( "Error reading file %s.\n\r", argv[1] );
		return 0;
	}

	CMebLoader oMeb;

	std::string sErrorText;
	
	if ( !oMeb.LoadFile( (uint8_t*)pData, dataSize, sErrorText ) )
	{
		printf( "Error load file: %s\n", sErrorText.c_str() );
		return 0;
	}

	oMeb.Run();

	return 0;
}

#endif 
