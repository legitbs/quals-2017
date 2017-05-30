#ifndef _SHA256_H
#define _SHA256_H

#ifndef uint8
typedef unsigned char uint8;
#endif

#ifndef uint32
typedef unsigned long int uint32;
#endif

#ifndef int8
typedef char int8;
#endif

#ifndef int32
typedef long int int32;
#endif

typedef struct
{
    uint32 total[2];
    uint32 state[8];
    uint8 buffer[64];
}
sha256_context;

void sha256_starts( sha256_context *ctx );
void sha256_update( sha256_context *ctx, uint8 *input, uint32 length );
void sha256_finish( sha256_context *ctx, uint8 digest[32] );

#endif /* sha256.h */
