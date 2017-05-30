// Derived from:
//  Simple RSA Implementation for demonstrating a timing attack.
//  * Do not use this in production! *
//  Created by Arve Nygård on 05/05/15.
//  Copyright (c) 2015 Arve Nygård. All rights reserved.
//
#include "common.h"

#include "ttmath/ttmath.h"

extern SimplePerformanceCounter g_simpleCounter;

bool g_bInSquare = false;

bool RSA::LoadPrivateKey( const char *pszFilename, RSANum *pPrivateExponent, RSANum *pModulus )
{
        FILE *pFile;
        char szLine[2048];

        if ( !(pFile = fopen ( pszFilename, "r" )) )
        {
                LogDebug ( LOG_PRIORITY_HIGH, "Unable to open private key, aborting.\n" );
                return (false);
        }

        bool bFoundModulus = false;
        bool bFoundExponent = false;
        while ( !feof ( pFile ) )
        {
                if ( fgets ( szLine, 2048, pFile ) == NULL )
                        break;

                if ( strlen ( szLine ) > 16 )
                {
                        bool bColonFound = false;
                        uint32_t colon_position = 0;

                        for ( colon_position = 0; colon_position < strlen ( szLine ); colon_position++ )
                        {
                                if ( szLine[colon_position] == ':' )
                                {
                                        bColonFound = true;
                                        break;
                                }
                        }

                        if ( !bColonFound )
                                continue;

                        if ( strncmp ( szLine, "modulus", colon_position ) == 0 )
                        {
                                pModulus->FromString ( szLine + colon_position + 1 );
                                bFoundModulus = true;
                        }

                        if ( strncmp ( szLine, "privateExponent", colon_position ) == 0 )
                        {
                                pPrivateExponent->FromString ( szLine + colon_position + 1 );
                                bFoundExponent = true;
                        }
                }
        }

        fclose ( pFile );

        if ( bFoundExponent && bFoundModulus )
                return (true);
        else
                return (false);
}

bool RSA::SetBytes( RSANum &number, uint8_t *pBytes, uint32_t byteLen )
{
	if ( !pBytes )
		return (false);

	if ( byteLen != (RSA_KEY_BITS/8) )
		return (false);

	memcpy( ((uint8_t*)number.table), pBytes, byteLen );

	return (true);
}

bool RSA::ReadBytes( RSANum &number, uint8_t *pBytes, uint32_t byteLen )
{
	if ( !pBytes )
		return (false);

	if ( byteLen != (RSA_KEY_BITS/8) )
		return (false);

	memcpy( pBytes, ((uint8_t*)number.table), byteLen );

	return (true);
}

uint32_t RSA::GetNumBits( const RSANum &number )
{
	ttmath::Big<32,32> k;

	k.Log( number, 2 );

	k = ttmath::Floor(k);

	return k.ToInt() + 1;
}

bool RSA::EncryptDecrypt( RSANum &result, const RSANum &input, const RSANum &exponent, const RSANum &modulus )
{
	result = RSA::ModExp( input, exponent, modulus );

	return (true); 
}

void RSA::nPrime( const RSANum n, RSANum &r, RSANum &nPrime )
{
	r = 2;

	//r.Pow( GetNumBits(n) );
	r.Pow( RSA_KEY_BITS );

	RSANum rInverse = ModInverse( r, n );

	nPrime = (r*rInverse - 1) / n;
}

RSANum RSA::MontgomeryProduct( const RSANum &a, const RSANum &b, const RSANum &nprime, const RSANum &r, const RSANum &n )
{
	RSANum t = a * b;
	RSANum m = t * nprime % r;
	RSANum u = (t + m*n) / r;

	//g_simpleCounter.AddTime( 1 );

	if ( u >= n )
	{
		//if ( g_bInSquare )
		g_simpleCounter.AddTime( 200 );
		return (u- n);
	}
	else 
		return (u);
}

RSANum RSA::ModExp( const RSANum &M, const RSANum &d, const RSANum &n )
{
	if ( n%2 != 1 ) 
	{
		std::string sOutput;
		n.ToString( sOutput );

		printf( "n = %s\n", sOutput.c_str() );
		throw RSAException( "Invalid modulus -- must be odd" );
	}

	RSANum r;
	RSANum nprime;

	nPrime( n, r, nprime );

	RSANum M_bar = (M * r) % n;
	RSANum x_bar = (r % n);
	
	//for ( long k = (GetNumBits(d)-1); k >= 0 ; k--) 
	for ( long k = (RSA_KEY_BITS-1); k >= 0 ; k--) 
	{
		g_bInSquare = false;
		x_bar = MontgomeryProduct( x_bar, x_bar, nprime, r, n );

		if ( d.GetBit( k ) )
		{
			g_bInSquare = true;
			x_bar = MontgomeryProduct( M_bar, x_bar, nprime, r, n );
		}
	}

	return MontgomeryProduct(x_bar, 1, nprime, r, n);
}

        
RSANum RSA::ModInverse( RSANum a, RSANum b )
{
	RSANum b0 = b, t, q;
	RSANum x0 = 0, x1 = 1;

	if (b == 1) return 1;
    while (a > 1) {
        q = a / b;
        t = b, b = a % b; a = t;
        t = x0, x0 = x1 - q * x0, x1 = t;
    }
    if (x1 < 0) x1 += b0;
    return x1;
#if 0
	if ( b == 1 )
		return 1;

	while ( a > 1 ) 
	{
		q = a / b;

		t = b;
		b = a % b; 
		a = t;

		t = x0;

		x0 = x1 - q * x0;
		x1 = t;
	}

	if ( x1 < 0 ) 
		x1 += b0;

	return x1;
#endif
}
