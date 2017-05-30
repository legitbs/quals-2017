#ifndef __BigInt_H__
#define __BigInt_H__

#include <stdint.h>
#include <string.h>
#include <stdio.h>

class BigIntOverflow
{
public:
	BigIntOverflow( const char *pszReason )
	{
		size_t strSize = strlen ( pszReason );
		m_pszReason = new char[strSize+1];

		memcpy( m_pszReason, pszReason, strSize );
		m_pszReason[strSize] = '\0';
	}

	~BigIntOverflow( )
	{
		if ( m_pszReason )
			delete[] m_pszReason;
	}

private:
	char *m_pszReason;
};

class BigInt
{
public:
	BigInt( uint32_t len );
	BigInt( const BigInt & );
	~BigInt( );
	
	bool IsZero( void ) const;
	int32_t Compare( const BigInt & ) const;
	int32_t Compare( uint32_t ) const;

	bool SetBytes( uint8_t *pBytes, uint32_t byteLen );
	bool ReadBytes( uint8_t *pBytes, uint32_t byteLen );

	void operator= ( const BigInt & );
	void operator= ( uint32_t );
	void operator+= ( const BigInt & );
	void operator+= ( uint32_t );
	void operator-= ( const BigInt & );
	void operator-= ( uint32_t );
	void operator*= ( const BigInt & );
	void operator*= ( uint32_t );
	void operator/= ( const BigInt & );
	void operator/= ( uint32_t );
	void operator%= ( const BigInt & );
	void operator%= ( uint32_t );

	static void Divide(const BigInt &, const BigInt &, BigInt &, BigInt &);

	char *edit(char *) const;
	bool scan(const char *);
	void dump() const;

	bool operator == (const BigInt &n) const 
	{ 
		return (Compare(n) == 0);
	}

	bool operator != (const BigInt &n) const
	{ 
		return Compare(n) != 0; 
	}

	bool operator >  (const BigInt &n) const 
	{ 
		return Compare(n) >  0; 
	}

	bool operator >= (const BigInt &n) const 
	{ 
		return Compare(n) >= 0; 
	}

	bool operator <  (const BigInt &n) const 
	{ 
		return Compare(n) <  0; 
	}

	bool operator <= (const BigInt &n) const 
	{ 
		return Compare(n) <= 0;
	}

	bool operator == ( uint32_t n)
	{
		return Compare(n) == 0; 
	}

	bool operator != ( uint32_t n) const
	{ 
		return Compare(n) != 0; 
	}

	bool operator >  ( uint32_t n) const
	{ 
		return Compare(n) >  0; 
	}

	bool operator >= ( uint32_t n) const
	{ 
		return Compare(n) >= 0; 
	}

	bool operator <  ( uint32_t n) const
	{ 
		return Compare(n) <  0; 
	}

	bool operator <= ( uint32_t n) const
	{ 
		return Compare(n) <= 0; 
	}

	static void Power(const BigInt &, const BigInt &, const BigInt &, BigInt &);

private:
	uint32_t remainder( uint32_t );
	void shift( uint32_t amount );
	uint32_t *value;
	uint32_t length;

};

#endif // __BigInt_H__
