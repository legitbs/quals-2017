#ifndef __CIPHER_H__
#define __CIPHER_H__

#include <stdint.h>

void EncryptRequestData( uint32_t cipherKey[4], uint8_t *pData, uint8_t dataLen );
void DecryptRequestData( uint32_t cipherKey[4], uint8_t *pData, uint8_t dataLen );
void RunCipher( uint32_t v[2], uint32_t key[4] );

#endif // __CIPHER_H__
