#include <stdio.h>
#include <stdint.h>

#include "bigint.h" 

BigInt::BigInt( uint32_t len)
{
	length = len;
	value = new uint32_t[len];
}

BigInt::BigInt( const BigInt &n )
{
	length = n.length;
	value = new uint32_t[length];
	
	for ( uint32_t i = 0; i < length; i++ )
		value[i] = n.value[i];
}

BigInt::~BigInt()
{
	delete[] value;
}

bool BigInt::SetBytes( uint8_t *pBytes, uint32_t byteLen )
{
	if ( !pBytes )
		return (false);

	if ( byteLen != (length*4) )
		return (false);

	memcpy( value, pBytes, byteLen );

	return (true);
}

bool BigInt::ReadBytes( uint8_t *pBytes, uint32_t byteLen )
{
	if ( !pBytes )
		return (false);
	
	if ( byteLen != (length*4) )
		return (false);

	memcpy( pBytes, value, byteLen );

	return (true);
}

void BigInt::operator = (const BigInt &n)
{
	uint32_t i;

	for (i = 0; i < length && i < n.length; i++)
		value[i] = n.value[i];

	for (; i < length; i++)
		value[i] = 0;

	for (; i < n.length; i++)
	{
		if (n.value[i] != 0)
			throw new BigIntOverflow( "operator= incoming value too large!" );
	}
}

void BigInt::operator = ( uint32_t n)
{
	uint32_t i;
	value[0] = n;

	for (i = 1; i < length; i++)
		value[i] = 0;
}

void BigInt::operator += (const BigInt &n)
{
	uint32_t i;
	uint64_t carry = 0;

	for (i = 0; i < length; i++)
	{
		uint64_t sum = (uint64_t)value[i] + (i < n.length ? (uint64_t)n.value[i] : 0) + carry;
		value[i] = (uint32_t)(sum & 0xFFFFFFFF);
		carry = sum >> 32;
	}

	if (carry != 0)
		throw new BigIntOverflow ( "operator+= overflow in resulting operation" );

	for (; i < n.length; i++)
	{
		if (n.value[i] != 0)
			throw new BigIntOverflow ( "operator+= overflow in resulting operation" );
	}
}

void BigInt::operator += ( uint32_t n)
{
	value[0] += n;
	if (value[0] < n)
	{
		uint32_t i;
		for (i = 1; i < length; i++)
		{
			if (++value[i] != 0)
				return;
		}
		throw new BigIntOverflow ( "operator+= overflowed in resulting operation" );
	}
}

void BigInt::operator -= (const BigInt &n)
{
	uint32_t i;
	uint64_t borrow = 0;

	for (i = 0; i < length; i++)
	{
		uint64_t subtrahend = (i < n.length ? (uint64_t)n.value[i] : 0) + borrow;
		borrow = (uint64_t)(value[i]) < subtrahend;
		value[i] -= (uint32_t)(subtrahend & 0xFFFFFFFF);
	}

	if (borrow != 0)
		throw new BigIntOverflow ( "operator-= Underflow in resulting operation" );

	for (; i < n.length; i++)
	{
		if (n.value[i] != 0)
			throw new BigIntOverflow ( "operator-= Underflow in resulting operation" );
	}
}

void BigInt::operator -= ( uint32_t n )
{
	if (value[0] >= n)
		value[0] -= n;
	else
	{
		value[0] -= n;
		for (unsigned i = 1; i < length; i++)
		{
			if ( --value[i] != 0xFFFFFFFF )
				return;
		}

		throw new BigIntOverflow( "operator-= Underflow in resulting operation" );
	}
}

void BigInt::operator *= (const BigInt &n)
{
	unsigned i;
	uint32_t *multiplier = new uint32_t[length];
	for (i = 0; i < length; i++)
	{
		multiplier[i] = value[i];
		value[i] = 0;
	}

	for (i = 0; i < length; i++)
	{
		uint32_t j;
		for (j = 0; j < n.length; j++)
		{
			uint64_t product = (uint64_t)multiplier[i] * (uint64_t)n.value[j];
			uint64_t k = i + j;

			while (product != 0)
			{
				if (k >= length)
					throw new BigIntOverflow ( "operator*= overflow in resulting operation" );

				product += value[k];
				value[k] = (uint32_t)(product & 0xFFFFFFFF);
				product >>= 32;
				k++;
			}
		}
	}

	delete[] multiplier;
}

void BigInt::operator *= ( uint32_t n)
{
	uint32_t i;
	uint64_t product = 0;

	for (i = 0; i < length; i++)
	{
		product += (uint64_t)n * (uint64_t)value[i];
		value[i] = (uint32_t)(product & 0xFFFFFFFF);
		product >>= 32;
	}

	if (product != 0)
		throw new BigIntOverflow ( "operator*= overflow in resulting operation" );
}

uint32_t BigInt::remainder( uint32_t n )
{
	uint32_t i = length;
	uint64_t rem = 0;

	while (i-- != 0)
	{
		uint64_t dividend = (uint64_t)rem << 32 | (uint64_t)value[i];

		value[i] = (uint32_t)((dividend / n) & 0xFFFFFFFF);

		rem = dividend % n;
	}

	return (uint32_t)(rem & 0xFFFFFFFF);
}

void BigInt::operator /= ( uint32_t n )
{
	(void)remainder(n);
}

void BigInt::operator %= ( uint32_t n )
{
	*this = remainder(n);
}

int32_t BigInt::Compare( const BigInt &n ) const
{
	uint32_t i;

	if (length > n.length)
	{
		for (i = length - 1; i >= n.length; i--)
		{
			if (value[i] != 0)
				return 1;
		}
	}
	else if (n.length > length)
	{
		for (i = n.length - 1; i >= length; i--)
		{
			if (n.value[i] != 0)
				return -1;
		}
	}
	else
		i = length - 1;

	while (true)
	{
		if (value[i] > n.value[i])
			return 1;
		if (value[i] < n.value[i])
			return -1;
		if (i == 0)
			return 0;
		i--;
	}
}

int32_t BigInt::Compare( uint32_t n ) const
{
	unsigned i;

	for (i = length - 1; i >= 1; i--)
	{
		if (value[i] != 0)
			return 1;
	}

	return value[0] > n ? 1 : value[0] < n ? -1 : 0;
}


bool BigInt::IsZero( void ) const
{
	uint32_t i;

	for (i = 0; i < length; i++)
	{
		if (value[i] != 0)
			return false;
	}

	return true;
}

char *BigInt::edit( char *s ) const
{
	BigInt n(*this);
	uint32_t i = 0, j = 0;

	do
	{
		s[i++] = n.remainder ( 10 ) + '0';
	} while ( !n.IsZero ( ) );

	s[i] = 0;

	for (j = 0; --i > j; j++)
	{
		char c = s[i];
		s[i] = s[j];
		s[j] = c;
	}

	return s;
}

bool BigInt::scan( const char *s )
{
	const char *t = s;
	bool found = false;

	while (*t == ' ' || *t == '\t')
		t++;

	*this = 0;

	while ('0' <= *t && *t <= '9')
	{
		found = true;
		*this *= 10;
		*this += (uint16_t)(*t++ - '0');
	}

	s = t;

	return found;
}

void BigInt::shift( uint32_t bit )
{
	for ( uint32_t i = 0; i < length; i++ )
	{
		uint64_t x = (uint64_t)value[i] << 1 | (uint64_t)bit;
		value[i] = (uint32_t)(x & 0xFFFFFFFF);
		bit = x >> 32;
	}

	if (bit != 0)
		throw new BigIntOverflow ( "shift: shifted too many bit positions, overflowed" );
}


void BigInt::Divide( const BigInt &dividend, const BigInt &divisor, BigInt &quotient, BigInt &remainder )
{
	if (divisor.IsZero())
		throw new BigIntOverflow ( "Operator Divide, Divide by zero" );

	remainder = 0;
	quotient = 0;
	uint32_t i = dividend.length;

	while (i-- != 0)
	{
		uint32_t bit = 32;
		while ( bit-- != 0 )
		{
			remainder.shift( dividend.value[i] >> bit & 1 );

			if (divisor <= remainder)
			{
				quotient.shift(1);
				// unsigned borrow = 0;
				// for (unsigned j = 0; j < divisor.length; j++) 
				// { 
				//   uint32_t subtrahend = divisor.value[j] + borrow; 
				//   borrow = remainder.value[j] < subtrahend;
				//   remainder.value[j] -= subtrahend;
				// } 
				remainder -= divisor;
			}
			else
				quotient.shift(false);
		}
	}
}

void BigInt::operator /= ( const BigInt &n )
{
	BigInt remainder(n.length);
	BigInt dividend(*this);
	Divide(dividend, n, *this, remainder);
}

void BigInt::operator %= ( const BigInt &n )
{
	BigInt quotient(length);
	BigInt dividend(*this);
	Divide(dividend, n, quotient, *this);
}

void BigInt::Power( const BigInt &base, const BigInt &exponent, const BigInt &modulus, BigInt &result )
{
	BigInt r(2 * base.length + 1);
	r = 1;
	bool one = true;
	uint32_t i = exponent.length;

	while (i-- != 0)
	{
		uint32_t bit = 1 << 31;

		do
		{
			if ( !one )
			{
				BigInt n(r);
				r *= n;
				r %= modulus;
			}

			if (exponent.value[i] & bit)
			{
				r *= base;
				r %= modulus;
				one = false;
			}

			bit >>= 1;
		} while (bit != 0);
	}
	result = r;
}


void BigInt::dump() const
{
	uint32_t i;

	for (i = 0; i < length; i++)
		printf(" %x", value[i]);

	putchar('\n');
}

