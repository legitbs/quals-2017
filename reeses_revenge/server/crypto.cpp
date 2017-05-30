// alsa-control-server
// shane tully (shane@shanetully.com)
// shanetully.com
// https://github.com/shanet/Alsa-Channel-Control

#include "crypto.h"

using namespace std;

Crypto::Crypto() 
{
    localKeypair  = NULL;

    initNoGen();
}


Crypto::~Crypto() 
{
	EVP_PKEY_free(localKeypair);
}


int Crypto::rsaSign(const unsigned char *msg, size_t msgLen, unsigned char *signature, uint32_t *signatureLen) 
{
	EVP_MD_CTX *mdctx;
	EVP_PKEY *key;

	if ( msg == NULL || msgLen == 0 || signature == NULL || signatureLen == NULL )
		return FAILURE;

	// Set key
    key = localKeypair;
	
	// Create Digest
	if((mdctx = EVP_MD_CTX_create()) == NULL)
		return FAILURE;

	// Setup Digest as SHA256
	if(1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL))
		return FAILURE;

	// Setup signer
    if ( !EVP_SignInit_ex(mdctx, EVP_sha256(), NULL) )
		return FAILURE;

	// Sign message
	if ( !EVP_SignUpdate(mdctx, msg, msgLen) )
		return FAILURE;

	// Get signature data
	if ( !EVP_SignFinal(mdctx, signature, signatureLen, key ) )
		return FAILURE; 

	EVP_MD_CTX_cleanup(mdctx);
	EVP_MD_CTX_destroy(mdctx);

    return (int)(*signatureLen);
}

int Crypto::rsaVerifyRecover(const unsigned char *msg, size_t msgLen, unsigned char *signature, size_t signatureLen, unsigned char *digest, size_t *digestLen) 
{
	return FAILURE;
}

int Crypto::rsaVerify(const unsigned char *msg, size_t msgLen, unsigned char *signature, size_t signatureLen) 
{
	EVP_MD_CTX *mdctx;
    EVP_PKEY *key;

    key = localKeypair;

	// Create Digest
	if((mdctx = EVP_MD_CTX_create()) == NULL)
		return FAILURE;

	// Setup Digest as SHA256
	if(1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL))
		return FAILURE;

	// Setup signer
    if ( !EVP_VerifyInit_ex(mdctx, EVP_sha256(), NULL) )
		return FAILURE;

	// Sign message
	if ( !EVP_VerifyUpdate(mdctx, msg, msgLen) )
		return FAILURE;

	// Get signature data
	if ( EVP_VerifyFinal(mdctx, signature, signatureLen, key ) != 1 )
		return FAILURE; 

	EVP_MD_CTX_cleanup(mdctx);
	EVP_MD_CTX_destroy(mdctx);
    
	return (int)SUCCESS;
}


int Crypto::writeKeyToFile(FILE *fd, int key) 
{
	if(key == KEY_PRIVATE) 
	{
        	if(!PEM_write_PrivateKey(fd, localKeypair, NULL, NULL, 0, 0, NULL))
            		return FAILURE;
	} 
	else if(key == KEY_PUBLIC) 
	{
        	if(!PEM_write_PUBKEY(fd, localKeypair))
            		return FAILURE;
	}

	return SUCCESS;
}

int Crypto::setPublicKey(unsigned char *pubKey, size_t publicKeyLen)
{
	BIO *bio = BIO_new(BIO_s_mem());

	if (BIO_write(bio, pubKey, publicKeyLen) != (int)publicKeyLen)
		return FAILURE;

	RSA *_pubKey = (RSA*)malloc(sizeof(RSA));
	if ( _pubKey == NULL ) return FAILURE;

	PEM_read_bio_PUBKEY(bio, &localKeypair, NULL, NULL);

	BIO_free_all(bio);

	return SUCCESS;
}

int Crypto::getPublicKey(unsigned char **pubKey, size_t *publicKeyLen) 
{
    BIO *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(bio, localKeypair);

    int pubKeyLen = BIO_pending(bio);
    *pubKey = (unsigned char*)malloc(pubKeyLen);
    if(pubKey == NULL) return FAILURE;

    BIO_read(bio, *pubKey, pubKeyLen);

    // Insert the null terminator
    (*pubKey)[pubKeyLen-1] = '\0';

    (*publicKeyLen) = pubKeyLen;

    BIO_free_all(bio);

    return pubKeyLen;
}

int Crypto::setPrivateKey(unsigned char *privateKey, size_t privateKeyLen)
{
	BIO *bio = BIO_new(BIO_s_mem());

	if (BIO_write(bio, privateKey, privateKeyLen) != (int)privateKeyLen)
		return FAILURE;

	RSA *_privKey = (RSA*)malloc(sizeof(RSA));
	if ( _privKey == NULL ) return FAILURE;

	PEM_read_bio_PrivateKey(bio, &localKeypair, NULL, NULL);

	BIO_free_all(bio);

	return SUCCESS;
}

int Crypto::getPrivateKey(unsigned char **priKey, size_t *privateKeyLen) 
{
    BIO *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PrivateKey(bio, localKeypair, NULL, NULL, 0, NULL, NULL);

    int priKeyLen = BIO_pending(bio);
    *priKey = (unsigned char*)malloc(priKeyLen + 1);
    if(priKey == NULL) return FAILURE;

    BIO_read(bio, *priKey, priKeyLen);

    // Insert the null terminator
    (*priKey)[priKeyLen] = '\0';

    (*privateKeyLen) = priKeyLen;

    BIO_free_all(bio);

    return priKeyLen;
}

int Crypto::initNoGen()
{
    // Init RSA
    localKeypair = EVP_PKEY_new();
}

int Crypto::genKeypair() 
{

    EVP_PKEY_free(localKeypair);
    localKeypair = NULL;

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);

    if(EVP_PKEY_keygen_init(ctx) <= 0) 
        return FAILURE;
    
    if(EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, (int)DEFAULT_RSA_KEYLEN) <= 0) 
        return FAILURE;

    if(EVP_PKEY_keygen(ctx, &localKeypair) <= 0) 
        return FAILURE;

	BIO *outbio = NULL;
	outbio  = BIO_new(BIO_s_file());
	outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

    BIO_printf( outbio, "\nPublic:\n\n" );
    if ( EVP_PKEY_print_public(outbio, localKeypair, 2, NULL ) )
	
    BIO_printf( outbio, "\nPrivate:\n\n" );
    if ( EVP_PKEY_print_private(outbio, localKeypair, 2, NULL ) )

    BIO_free_all(outbio);

    EVP_PKEY_CTX_free(ctx);

    return SUCCESS;
}

