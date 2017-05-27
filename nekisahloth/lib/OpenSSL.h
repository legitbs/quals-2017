#ifndef NEKISAHLOTH_LIB_OPENSSL
#define NEKISAHLOTH_LIB_OPENSSL

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

class OpenSSL
{
	public:
		OpenSSL(char *ServerCert, char *PrivateKey);
		~OpenSSL();
		virtual int Read(char *Buffer, int ReadLen);
		virtual int Readline(char *Buffer, int MaxLen);
		virtual int Write(char *Buffer, int WriteLen);
		virtual int WriteString(char *Buffer);
		virtual int WriteString(const char *Buffer);
		virtual int Connect(int InFD, int OutFD);
		virtual int Disconnect();

	private:
		SSL *_SSL;
		SSL_CTX *_CTX;
};

#endif
