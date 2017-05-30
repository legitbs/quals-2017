#include "common.h"

void EncryptRequestData( uint32_t cipherKey[4], uint8_t *pData, uint8_t dataLen )
{
        uint32_t cipherState[2];

	cipherState[0] = 0;
	cipherState[1] = 0;

        for ( uint32_t i = 0; i < dataLen; i++ )
        {
                if ( (i % 8) == 0 )
                        RunCipher( cipherState, cipherKey );

                pData[i] ^= ((uint8_t*)cipherState)[i%8];
        }
}


void DecryptRequestData( uint32_t cipherKey[4], uint8_t *pData, uint8_t dataLen )
{
        uint32_t cipherState[2];

	cipherState[0] = 0;
	cipherState[1] = 0;

        for ( uint32_t i = 0; i < dataLen; i++ )
        {
                if ( (i % 8) == 0 )
                        RunCipher( cipherState, cipherKey );

                pData[i] ^= ((uint8_t*)cipherState)[i%8];
        }
}

void RunCipher( uint32_t v[2], uint32_t key[4] )
{
        uint32_t v0=v[0];
        uint32_t v1=v[1];
        uint32_t delta = 0x83E778B9;
        uint32_t sum = (delta*16);

        for ( uint32_t i = 0; i < 16; i++ )
        {
                v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
                sum += delta;
                v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
        }

        v[0] = v0;
        v[1] = v1;
}
