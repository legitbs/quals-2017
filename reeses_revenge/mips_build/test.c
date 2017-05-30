#include "include/syscall.h"
#include "include/sha256.h"
#include "include/rc4_acc.h"

void main( void );

void _start( void )
{
	main();
}

unsigned int g_mytest = 0;

unsigned char g_heap_data[4096*24];
unsigned char g_heap_data2[4096];

unsigned char *g_pHeapCur;
unsigned char *g_pHeapCur2;

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

static char *msg[] = 
{
    "abc",
    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
   '\0' 
};

static char *val[] =
{
    "ba7816bf8f01cfea414140de5dae2223" \
    "b00361a396177a9cb410ff61f20015ad",
    "248d6a61d20638b8e5c026930c3e6039" \
    "a33ce45964ff2167f6ecedd419db06c1",
    "cdc76e5c9914fb9281a1c7e284d73e67" \
    "f1809a48a497200e046d39ccc7112cd0"
};


void test_sha( void )
{
	sha256_context ctx;
	unsigned char sha256sum[32];

	sha256_starts( &ctx );
                
	sha256_update( &ctx, (uint8 *) msg,
                               strlen( msg ) );

	sha256_finish( &ctx, sha256sum );
}

void main()
{
    g_mytest++;
    g_pHeapCur = g_heap_data;
    g_pHeapCur2 = g_heap_data2;
    unsigned int i, x;

    unsigned char dest[20];

    unsigned int bytesRead = read( 0, &dest, 10 );

    unsigned int bytesWrite = write( 1, &dest, bytesRead );

    unsigned char rc4Key[64];

    for ( i = 0; i < 32; i++ )
        rc4Key[i] = i;

    RC4Init( rc4Key, 32, 256 );

    RC4Encrypt( rc4Key, 32 );

    x = 0;
    for ( i = 0; i < 4000000000; i++ )
    {
    x++;
    }
    
    exit(1);
    return;
}
