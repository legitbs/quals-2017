#ifndef NEKISAHLOTH_LIB_ZLIB
#define NEKISAHLOTH_LIB_ZLIB

#include <zlib.h>
#include "OpenSSL.h"

class ZLib
{
	public:
		ZLib(OpenSSL *SSL, int MaxCompressSize, int MaxDecompressSize);
		~ZLib();

		virtual int ReadHeader();
		virtual int DecompressData(unsigned char **Buffer);
		virtual int CompressData(unsigned char *Buffer, int Len);
		virtual int WriteHeader();
		virtual int WriteData();
	private:
		int _MaxCompressSize;
		int _MaxDecompressSize;
		int _CompressSize;
		int _DecompressSize;
		OpenSSL *_SSL;
		unsigned char *_InBuffer;
		unsigned char *_OutBuffer;
};

#endif
