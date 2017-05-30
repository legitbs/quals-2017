#include "include/syscall.h"
#include "include/sha256.h"
#include "include/rc4_acc.h"

#define EI 13  /* typically 10..13 */
#define EJ  5  /* typically 4..5 */
#define P   1  /* If match length <= P then output one character */
#define N (1 << EI)  /* buffer size */
#define F ((1 << EJ) + P)  /* lookahead buffer size */

#define MAX_COMPRESSBUF_SIZE	(2048*8)
#define MAX_UNCOMPRESSBUF_SIZE	(4096*24)

#define EOF		(-1)


int bit_buffer = 0, bit_mask = 128;
unsigned long codecount = 0, textcount = 0;
unsigned char buffer[N * 2];

typedef struct
{
	uint32 	inBytePos;
	uint32 	outBytePos;
	uint32 	maxInBytePos;
	uint8 	compressBuf[MAX_COMPRESSBUF_SIZE];
	uint8 	uncompressBuf[MAX_UNCOMPRESSBUF_SIZE];
} COMPRESS_INFO;

COMPRESS_INFO g_compressData;

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


int readDataUntil( char *buffer, int maxLen )
{
        int pos = 0;
        unsigned char readChar;

        for ( pos = 0; pos < maxLen; pos++ )
        {
                int bytesRead = read( 0, &readChar, 1 );

                if ( bytesRead == -1 || bytesRead == 0 )
                        return -1;

                buffer[pos] = readChar;
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



void error(void)
{
	char szError[] = "ERR: Good not output\n";
	writeAll( szError, strlen( szError ) );  
	exit(1);
}

void writeByte( uint8 val )
{
	g_compressData.uncompressBuf[g_compressData.outBytePos++] = val;
}

int readByte( void )
{
	int val;

	if ( g_compressData.inBytePos > g_compressData.maxInBytePos )
		return -1;
	
	val = g_compressData.compressBuf[g_compressData.inBytePos++];

	return val;
}

void putbit1(void)
{
	bit_buffer |= bit_mask;
	if ((bit_mask >>= 1) == 0) 
	{
		writeByte( bit_buffer );

        	bit_buffer = 0;  bit_mask = 128;  codecount++;
	}
}

void putbit0(void)
{
	if ((bit_mask >>= 1) == 0) 
	{
		writeByte( bit_buffer );

        	bit_buffer = 0;  bit_mask = 128;  codecount++;
	}
}

void flush_bit_buffer(void)
{
	if (bit_mask != 128) 
	{
        
		writeByte( bit_buffer );
        	
		codecount++;
	}
}

void output1(int c)
{
	int mask;
    
	putbit1();
	mask = 256;
	while (mask >>= 1) 
	{
        	if (c & mask) 
			putbit1();

		else putbit0();
	}
}

void output2(int x, int y)
{
	int mask;
    
	putbit0();
	mask = N;

	while (mask >>= 1) 
	{
        	if (x & mask) 
			putbit1();
        	else 
			putbit0();
	}

	mask = (1 << EJ);
	while (mask >>= 1) 
	{
        	if (y & mask) 
			putbit1();
        	else 
			putbit0();
	} 
}

int getbit(int n) /* get n bits */
{
	int i, x;
	static int buf, mask = 0;
    
	x = 0;
	for (i = 0; i < n; i++) 
	{
		if (mask == 0) 
		{
			//if ((buf = fgetc(infile)) == EOF) 
			//		return EOF;
			if ( (buf = readByte()) == EOF )
				return EOF;

			mask = 128;
        	}
        	x <<= 1;

        	if (buf & mask) 
			x++;
        	
		mask >>= 1;
	}
	return x;
}

int32 DoCompress( uint32 inBufSize )
{
    int i, j, f1, x, y, r, s, bufferend, c;
	
    // Setup
    g_compressData.inBytePos = 0;
    g_compressData.outBytePos = 0;


    for (i = 0; i < N - F; i++) 
	buffer[i] = ' ';
    
    for (i = N - F; i < N * 2; i++) 
    {
        if ( (c = readByte()) == EOF ) 
		break;

        buffer[i] = c;  
	textcount++;
    }

    bufferend = i;  r = N - F;  s = 0;

    while (r < bufferend) 
    {
        f1 = (F <= bufferend - r) ? F : bufferend - r;

        x = 0;  y = 1;  
	c = buffer[r];

        for (i = r - 1; i >= s; i--)
            if (buffer[i] == c) 
	    {
                for (j = 1; j < f1; j++)
                    if (buffer[i + j] != buffer[r + j]) 
			break;
                
		if (j > y) 
		{
                    x = i;  y = j;
                }
            }

        if (y <= P) 
		output1(c);
        else 
		output2(x & (N - 1), y - 2);

        r += y;  s += y;

        if (r >= N * 2 - F) 
	{
            for (i = 0; i < N; i++) 
		buffer[i] = buffer[i + N];
            
	    bufferend -= N;  r -= N;  s -= N;
            while (bufferend < N * 2) 
            {
                if ((c = readByte()) == EOF) 
			break;

                buffer[bufferend++] = c;  textcount++;
            }
        }
    }
    flush_bit_buffer();
}

int32 DoDecompress( uint32 inBufSize )
{
	int i, j, k, r, c;

	// Setup
	g_compressData.inBytePos = 0;
	g_compressData.outBytePos = 0;

	// Setup max
	g_compressData.maxInBytePos = inBufSize;
    
	for (i = 0; i < N - F; i++) buffer[i] = ' ';
	r = N - F;
	
	while ((c = getbit(1)) != EOF) 
	{
        	if (c) 
		{
			if ((c = getbit(8)) == EOF) 
				break;
			
			writeByte( c );
			buffer[r++] = c;  r &= (N - 1);
        	} 
		else 
		{
			if ((i = getbit(EI)) == EOF) 
				break;

			if ((j = getbit(EJ)) == EOF) 
				break;
            
			for (k = 0; k <= j + 1; k++) 
			{
				c = buffer[(i + k) & (N - 1)];
	
				writeByte( c );

				buffer[r++] = c;  r &= (N - 1);
            		}
        	}
    	}

	return g_compressData.outBytePos;
}

void main()
{
	int writeLen;
	int bytesRead;	
	uint8_t action;
	uint32 compressDataLen;

	for (;;)
	{
		bytesRead = readDataUntil( &action, 1 );

		if ( bytesRead != 1 )
		{
			char szError[] = "ERR: Bad connection\n";
			writeAll( szError, strlen( szError ) );
			exit(1);
		}

		// Read in the length of the compressed buffer
		bytesRead = readDataUntil( &compressDataLen, 4 );

		// Now read in the data (constrain it to the uncompressbuf_size)
		if ( compressDataLen > MAX_COMPRESSBUF_SIZE || bytesRead != 4 )
		{
			char szError[] = "ERR: Too much\n";
			writeAll( szError, strlen( szError ) );
			exit(1);
		}

		// Check for exit condition
		if ( compressDataLen == 0 )
			break;


		// Now read in the data
		bytesRead = readDataUntil( &g_compressData.compressBuf, compressDataLen );

		if ( bytesRead != compressDataLen )
		{
			char szError[] = "ERR: Bad connection\n";
			writeAll( szError, strlen( szError ) );
			exit(1);
		}

		// Perform decompression
		int uncompressSize;

		if ( action == 0 )
		{
			uncompressSize = DoCompress( compressDataLen );
		}
		else if ( action == 1 )
		{
			uncompressSize = DoDecompress( compressDataLen );
		}
		else
		{
			char szError[] = "ERR: Unknown action\n";
			writeAll( szError, strlen( szError ) );
			exit(1);
		}
		

		if ( uncompressSize == -1 )
		{
			char szError[] = "ERR: Failed making data bigger\n";
			writeAll( szError, strlen( szError ) );
			exit(1);
		}

		// Now write out the data
		int bytesWritten = writeAll( &uncompressSize, 4 ); 	

		if ( bytesWritten != 4 )
		{
			char szError[] = "ERR: You have poor connection\n";
			writeAll( szError, strlen( szError ) );
			exit(1);
		}

		bytesWritten = writeAll( g_compressData.uncompressBuf, uncompressSize );

		if ( bytesWritten != uncompressSize )
		{
			char szError[] = "ERR: Your connection is terrible\n";
			writeAll( szError, strlen( szError ) );
			exit(1);
		}
	}

	exit(0);
}
