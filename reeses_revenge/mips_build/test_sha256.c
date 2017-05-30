#include "include/syscall.h"
#include "include/sha256.h"
#include "include/rc4_acc.h"

void main( void );

void _start( void )
{
	main();
}


void start_sha( sha256_context *pCtx )
{
	// Init
	sha256_starts( pCtx );
}

void update_sha( sha256_context *pCtx, uint8 *msg, uint32 msgLen )
{
	// Update
	sha256_update( pCtx, (uint8 *) msg, msgLen );
}

void finish_sha( sha256_context *pCtx, char *pShaSum )
{
	// Finish
	sha256_finish( pCtx, pShaSum );
}

int readLine( char *buffer, int maxLen )
{
        int pos = 0;
        unsigned char readChar;

        for ( pos = 0; pos < maxLen; pos++ )
        {
                int bytesRead = read( 0, &readChar, 1 );

                if ( bytesRead == -1 )
                        return -1;
                else if ( readChar == '\n' )
                        return pos;

                buffer[pos] = readChar;
        }

        return pos;
}

char get_hex( uint8 value )
{
	if ( value < 10 )
	{
		return '0' + value;
	}
	else if ( value < 16 )
	{
		return 'a' + (value-10);
	}
	else
		return 'Y';
}

int get_sha_string( char *buffer, char *sha256sum )
{	
	int idx;
	uint8 value;
	uint32 pos = 0;

	for ( idx = 0; idx < 32; idx++ )
	{
		value = (sha256sum[idx] >> 4) & 0xF;	

		buffer[pos++] = get_hex( value );

		value = (sha256sum[idx] & 0x0F);
	
		buffer[pos++] = get_hex( value );	
	}

	return pos;
}


void main()
{
	sha256_context ctx;
	unsigned char sha256sum[32];
	char lineBuf[512];
	char sha_string[128]; 
	int writeLen;

	// Start sha
	start_sha( &ctx );

	for (;;)
	{
		int bytesRead = readLine( lineBuf, 512 );
		
		if ( bytesRead == -1 )
		{
			break;
		}
		if ( bytesRead == 7 && lineBuf[0] == '>' && lineBuf[1] == '>' && lineBuf[2] == 'E' && lineBuf[3] == 'N' && lineBuf[4] == 'D' && lineBuf[5] == '>' && lineBuf[6] == '>' )
		{
			finish_sha( &ctx, sha256sum );
			writeLen = get_sha_string( sha_string, sha256sum );

			sha_string[writeLen] = '\n';
			writeLen++;

			write( 1, sha_string, writeLen );

			// Done
			break;
		}	

		// Update sha
		update_sha( &ctx, lineBuf, bytesRead );
	}	

	exit(1);
}
