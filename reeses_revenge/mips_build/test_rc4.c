#include "include/syscall.h"
#include "include/sha256.h"
#include "include/rc4_acc.h"

uint8 g_key[] = "Not a strong key";

#define RC4_BUFFER_SIZE		128

void main( void );

void _start( void )
{
	main();
}

size_t strlen( char *inStr )
{
        size_t pos = 0;

        for ( ;; )
        {
                if ( inStr[pos++] == '\0' )
                        return pos;
        }

        return 0;
}


int32 ReadDataUntil( uint8 *pBuffer, int32 maxLen )
{
	int pos;
	unsigned char curChar;

	for ( pos = 0; pos < maxLen; pos++ )
	{
		int bytesRead = read( 0, &curChar, 1 );

		if ( bytesRead == -1 || bytesRead == 0 )
			return -1;

		pBuffer[pos] = curChar;
	}

	return pos;
}

int writeAll( char *buffer, int len )
{
        int pos = 0;
        int bytesLeft = len;

        for ( pos = 0; pos < len; )
        {
                int bytesWritten = write( 1, (buffer+pos), bytesLeft );

                if ( bytesWritten <= 0 )
                        return -1;

                if ( bytesWritten > bytesLeft )
                        return -1;

                bytesLeft -= bytesWritten;
                pos+=bytesWritten;
        }

        return pos;
}

void main()
{
	uint8 rc4Buffer[RC4_BUFFER_SIZE];

	RC4Init( g_key, 16, 256 );

	int bytesRead;
	uint32 dataLen;
    
	for (;;)
	{
		bytesRead = ReadDataUntil( (uint8*)&dataLen, 4 );

		if ( bytesRead != 4 )
		{
			char szError[] = "Connection error\n";
			writeAll( szError, strlen( szError ) );
			exit(1);
		}

		if ( dataLen == 0 )
		{
			// Loop until they send 0
			break;
		}

		if ( dataLen > RC4_BUFFER_SIZE )
		{
			char szError[] = "Exceeded buffer\n";
			writeAll( szError, strlen( szError ) );
			exit(1);
		}

		bytesRead = ReadDataUntil( rc4Buffer, dataLen );

		if ( bytesRead != dataLen )
		{
			char szError[] = "Connectino error\n";
			writeAll( szError, strlen( szError ) );
			exit(1);
		}	

		RC4Encrypt( rc4Buffer, dataLen );

		// Send data
		writeAll( rc4Buffer, dataLen );
	}

	exit(0);
	return;
}
