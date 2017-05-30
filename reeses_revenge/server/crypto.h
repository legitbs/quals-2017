// alsa-control-server
// shane tully (shane@shanetully.com)
// shanetully.com
// https://github.com/shanet/Alsa-Channel-Control

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <openssl/err.h>

#include <stdio.h>
#include <string>
#include <string.h>
#include <stdint.h>

// Key lengths should be in bits
#define DEFAULT_RSA_KEYLEN 2048

#define SUCCESS 0
#define FAILURE -1

#define KEY_PUBLIC	1
#define KEY_PRIVATE	2

#ifndef CRYPTO_H
#define CRYPTO_H

class Crypto {

public:
    Crypto();

    ~Crypto();

    int initNoGen();
    int genKeypair();

    int rsaSign(const unsigned char *msg, size_t msgLen, unsigned char *signature, uint32_t *signatureLen);

    int rsaVerify(const unsigned char *msg, size_t msgLen, unsigned char *signature, size_t signatureLen);

    int rsaVerifyRecover(const unsigned char *msg, size_t msgLen, unsigned char *signature, size_t signatureLen, unsigned char *digest, size_t *digestLen);

    int writeKeyToFile(FILE *fd, int key);

    int setPublicKey(unsigned char *pubKey, size_t publicKeyLen);

    int getPublicKey(unsigned char **pubKey, size_t *publicKeyLen);

    int setPrivateKey(unsigned char *privateKey, size_t privateKeyLen);

    int getPrivateKey( unsigned char **privateKey, size_t *privateKeyLen);

private:
    EVP_PKEY *localKeypair;

};

#endif
