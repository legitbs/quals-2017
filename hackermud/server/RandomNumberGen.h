#ifndef __RANDOM_NUMBER_GEN_H__
#define __RANDOM_NUMBER_GEN_H__

#include <stdint.h>

class CRNGCommon
{
public:

	typedef enum
	{
		RNG_OUT_OF_MEMORY = 0,
		RNG_NO_DATA = 1,
		RNG_NEED_SEED = 2,
		RNG_ERROR = 3,
		RNG_DATA_INVALID = 4,
		RNG_OK = 5
	} RNG_STATUS;

private:
};

class RandomNumberGen
{
public:
	RandomNumberGen() {};
	~RandomNumberGen() {};

	virtual CRNGCommon::RNG_STATUS GetState( void* pData, uint32_t dataLen ) = 0;
	virtual CRNGCommon::RNG_STATUS SaveState( void** pData, uint32_t &dataLen ) = 0;

	virtual CRNGCommon::RNG_STATUS Seed( uint32_t seed ) = 0;

	virtual uint32_t GetU32( void ) = 0;
	virtual uint32_t GetRange( uint32_t start, uint32_t end ) = 0;
	virtual uint8_t GetU8( void ) = 0;
	virtual uint16_t GetU16( void ) = 0;
	virtual double GetReal( void ) = 0;

private:
};

#endif // __RANDOM_NUMBER_GEN_H__
