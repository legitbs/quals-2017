#ifndef __RSA_H__
#define __RSA_H__

#define RSA_KEY_BITS	(768)

#include "ttmath/ttmath.h"

// Make sure we can hold the maximum number size
typedef ttmath::UInt<((RSA_KEY_BITS/64)*2)+1> RSANum;

class RSAException
{
public:
        RSAException( const char *pszReason )
        {
                size_t strSize = strlen ( pszReason );
                m_pszReason = new char[strSize+1];

                memcpy( m_pszReason, pszReason, strSize );
                m_pszReason[strSize] = '\0';
        }

        ~RSAException( )
        {
                if ( m_pszReason )
                        delete[] m_pszReason;
        }

private:
        char *m_pszReason;
};

class RSA
{
public:
	static bool SetBytes( RSANum &number, uint8_t *pBytes, uint32_t byteLen );
	static bool ReadBytes( RSANum &number, uint8_t *pBytes, uint32_t byteLen );

	static bool LoadPrivateKey( const char *pszFilename, RSANum *pPrivateExponent, RSANum *pModulus );

	static bool EncryptDecrypt( RSANum &result, const RSANum &input, const RSANum &exponent, const RSANum &modulus );	

	static uint32_t GetNumBits( const RSANum &number ); 

private:
	static void nPrime( const RSANum n, RSANum &r, RSANum &nPrime );
	
	static RSANum MontgomeryProduct( const RSANum &a, const RSANum &b, const RSANum &nprime, const RSANum &r, const RSANum &n );

	static RSANum ModExp( const RSANum &M, const RSANum &d, const RSANum &n );
	
	static RSANum ModInverse( RSANum a, RSANum b );
};

#endif // __RSA_H__
