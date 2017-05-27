#include <iostream>
#include <unistd.h>
#include "OpenSSL.h"
#include <openssl/bn.h>
#include <fstream>

OpenSSL::OpenSSL(char *ServerCert, char *PrivateKey)
{
	SSL_load_error_strings();
	SSL_library_init();
	//OpenSSL_add_all_algorithms();

	this->_CTX = SSL_CTX_new( SSLv23_server_method());
	SSL_CTX_set_options(this->_CTX, SSL_OP_SINGLE_DH_USE);
	SSL_CTX_set_options(this->_CTX, SSL_OP_NO_SSLv3);
	SSL_CTX_set_options(this->_CTX, SSL_OP_NO_TLSv1);
	SSL_CTX_set_options(this->_CTX, SSL_OP_NO_TLSv1_1);
	SSL_CTX_set_verify(this->_CTX, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, 0);
	if(SSL_CTX_use_certificate_file(this->_CTX, ServerCert, SSL_FILETYPE_PEM) <= 0)
	{
		std::cout << "Failed to setup server cert\n";
		_exit(0);
	}

	if(SSL_CTX_use_PrivateKey_file(this->_CTX, PrivateKey, SSL_FILETYPE_PEM) <= 0)
	{
		std::cout << "Failed to setup private cert\n";
		_exit(0);
	}

	if(SSL_CTX_load_verify_locations(this->_CTX, ServerCert, 0) <= 0)
	{
		std::cout << "Failed to setup verify location\n";
		_exit(0);
	}

	if(!SSL_CTX_check_private_key(this->_CTX))
	{
		std::cout << "Private key does not match cert\n";
		_exit(0);
	}

	this->_SSL = SSL_new(this->_CTX);
}

OpenSSL::~OpenSSL()
{
	//free the ssl setup
	SSL_free (this->_SSL);
	SSL_CTX_free (this->_CTX);
}

int OpenSSL::Connect(int InFD, int OutFD)
{
	X509 *client_cert;
	char *str;
	ASN1_INTEGER *serial;
	BIGNUM *bn;
	BIGNUM *bn2;

	//connect to the socket and validate the client
	SSL_set_rfd(this->_SSL, InFD);
	SSL_set_wfd(this->_SSL, OutFD);
	if(SSL_accept(this->_SSL) < 0)
		_exit(0);

	//check the client cert
	client_cert = SSL_get_peer_certificate(this->_SSL);
	if(!client_cert)
		_exit(0);

	//make sure issuer is proper
	str = X509_NAME_oneline (X509_get_issuer_name(client_cert), 0, 0);
	if((str == NULL) || (strcmp(str, "/C=US/ST=DEF CON/O=Legitimate Business Syndicate") != 0))
		_exit(0);

	//make sure subject is proper
	str = X509_NAME_oneline (X509_get_subject_name(client_cert), 0, 0);
	if((str == NULL) || (strcmp(str, "/C=US/ST=DEF CON/O=Legitimate Business Syndicate/OU=DEF CON CTF QUALS 2017") != 0))
		_exit(0);

	//verify the serial number is 51015
	serial = X509_get_serialNumber(client_cert);
	if(!serial)
		_exit(0);

	bn = ASN1_INTEGER_to_BN(serial, NULL);
	bn2 = 0;
	BN_dec2bn(&bn2, "51015");
	if(BN_ucmp(bn, bn2))
		_exit(0);

	//cleanup and return
	OPENSSL_free(str);
	X509_free(client_cert);
	return 1;
}

int OpenSSL::Disconnect()
{
	//SSL_shutdown(this->_SSL);
	return 0;
}

int OpenSSL::Readline(char *Buffer, int MaxLen)
{
	long i;

	//read a line of data in
	for(i = 0; i < (MaxLen - 1); i++)
	{
		if(SSL_read(this->_SSL, &Buffer[i], 1) <= 0)
			return 0;
		if((Buffer[i] == '\n') || (Buffer[i] == '\x00'))
		{
			Buffer[i] = 0;
			return i-1;
		}
	}

	//make sure it is null terminated
	Buffer[MaxLen - 1] = 0;
	return i;
}

int OpenSSL::Read(char *Buffer, int BytesToRead)
{
	return SSL_read(this->_SSL, Buffer, BytesToRead);
}

int OpenSSL::WriteString(char *Buffer)
{
	int CurLen;
	int WriteLen;
	int BufferLen;

	//write out all the data
	CurLen = 0;
	BufferLen = strlen(Buffer);
	while(CurLen < BufferLen)
	{
		WriteLen = this->Write(&Buffer[CurLen], BufferLen - CurLen);
		if(WriteLen <= 0)
			return 0;

		CurLen += WriteLen;
	};

	return CurLen;
}

int OpenSSL::WriteString(const char *Buffer)
{
	return this->WriteString((char *)Buffer);
}

int OpenSSL::Write(char *Buffer, int BytesToWrite)
{
	return SSL_write(this->_SSL, Buffer, BytesToWrite);
}

