#include "ZLib.h"
#include <sstream>
#include <bsd/bsd.h>
#include <fstream>

ZLib::ZLib(OpenSSL *SSL, int MaxCompressSize, int MaxDecompressSize)
{
	this->_CompressSize = 0;
	this->_DecompressSize = 0;
	this->_InBuffer = 0;
	this->_OutBuffer = 0;
	this->_SSL = SSL;
	this->_MaxCompressSize = MaxCompressSize;
	this->_MaxDecompressSize = MaxDecompressSize;
}

ZLib::~ZLib()
{
	//if we have a buffer then free it
	if(this->_InBuffer)
		free(this->_InBuffer);

	if(this->_OutBuffer)
		free(this->_OutBuffer);
}

int ZLib::ReadHeader()
{
	char SizeStr[256];
	const char *ErrStr;

	//get the input size
	this->_SSL->Readline(SizeStr, 10);
	this->_CompressSize = strtonum(SizeStr, 0, this->_MaxCompressSize, &ErrStr);
	if(ErrStr != 0)
	{
		this->_CompressSize = 0;
		this->_DecompressSize = 0;
		return 0;
	}

	//get the output size
	this->_SSL->Readline(SizeStr, 10);
	this->_DecompressSize = strtonum(SizeStr, 0, this->_MaxDecompressSize, &ErrStr);
	if(ErrStr != 0)
	{
		this->_CompressSize = 0;
		this->_DecompressSize = 0;
		return 0;
	}

	//adjust the decompress size slightly for padding just in-case
	this->_DecompressSize += 32;
	if(this->_DecompressSize > this->_MaxDecompressSize)
	{
		this->_CompressSize = 0;
		this->_DecompressSize = 0;
		return 0;
	}

	return 1;
}

int ZLib::DecompressData(unsigned char **Buffer)
{
	z_stream strm;
	int ret;
	int CurSize;
	int ReadAmount;

	//make sure the pointer is 0
	*Buffer = 0;

	//if no sizes then fail
	if(!this->_CompressSize && !this->_DecompressSize)
		return 0;

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = this->_CompressSize;
	strm.next_in = Z_NULL;
	ret = inflateInit(&strm);
	if (ret != Z_OK)
		return ret;

	//allocate a buffer for the output
	if(this->_InBuffer)
		free(this->_InBuffer);
	this->_InBuffer = (unsigned char *)malloc(this->_CompressSize);

	if(this->_OutBuffer)
		free(this->_OutBuffer);
	this->_OutBuffer = (unsigned char *)malloc(this->_DecompressSize);

	//setup the output area
	strm.next_in = this->_InBuffer;
	strm.avail_out = this->_DecompressSize;
	strm.next_out = this->_OutBuffer;

	//read in all of our data
	CurSize = 0;
	while(CurSize < this->_CompressSize)
	{
		//read data
		ReadAmount = this->_SSL->Read((char *)&this->_InBuffer[CurSize], this->_CompressSize - CurSize);
		if(ReadAmount <= 0)
			return 0;

		//move forward
		CurSize += ReadAmount;
	}

	//have all the data, decompress it
	ret = inflate(&strm, Z_NO_FLUSH);
	if(ret != Z_STREAM_END)
	{
		inflateEnd(&strm);
		return 0;
	}

	//give them the data pointer
	*Buffer = this->_OutBuffer;
	this->_DecompressSize -= strm.avail_out;
	inflateEnd(&strm);

	//make sure it is within reason due to our padding
	if(this->_DecompressSize > this->_MaxDecompressSize)
		return 0;
/*
	char OutBuffer[100];
	std::ofstream f;
	sprintf(OutBuffer, "ZLIB SSL: %lx, InBuffer: %lx, Diff: %lx\n", (unsigned long)this->_SSL, (unsigned long)this->_InBuffer, (unsigned long)this->_InBuffer - (unsigned long)this->_SSL);
	f.open("log.txt", std::ios::app);
	f << OutBuffer;
	f.close();
*/
//	free(this->_InBuffer);
//	this->_InBuffer = 0;
	return this->_DecompressSize;
}

int ZLib::CompressData(unsigned char *Buffer, int Len)
{
	z_stream strm;
	int ret;

	///we never use this, it's just around to make it appear to be a fully functional c++ class
	//that might have been ripped off from somewhere

	//if the input is too large then fail
	if(Len > this->_MaxDecompressSize)
		return 0;

	/* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = Len;
	strm.next_in = Buffer;
	ret = deflateInit(&strm, 9);
	if (ret != Z_OK)
		return ret;

	//allocate a buffer for the output
	if(this->_OutBuffer)
		free(this->_OutBuffer);
	this->_OutBuffer = (unsigned char *)malloc(this->_MaxCompressSize);

	//setup the output area
	strm.avail_out = this->_MaxCompressSize;
	strm.next_out = this->_OutBuffer;

	//have all the data, compress it
	ret = deflate(&strm, Z_FINISH);
	if((ret == Z_STREAM_ERROR) || (strm.avail_in != 0))
	{
		deflateEnd(&strm);
		return 0;
	}

	//store how much we compressed
	this->_CompressSize = this->_MaxCompressSize - strm.avail_out;
	deflateEnd(&strm);
	return this->_CompressSize;
}

//dummpy functions, makes it appear to be a fully functional c++ object taken from someplace

int ZLib::WriteHeader()
{
	int NumLen;
	char NumStr[256];
	std::stringstream ss;

	//send compressed size
	ss << this->_CompressSize;
	NumLen = strlen(ss.str().c_str());
	memcpy(NumStr, ss.str().c_str(), NumLen);
	NumStr[NumLen] = '\n';
	if(this->_SSL->Write(NumStr, NumLen + 1) <= 0)
		return 0;

	//send decompress size
	ss << this->_DecompressSize;
	NumLen = strlen(ss.str().c_str());
	memcpy(NumStr, ss.str().c_str(), NumLen);
	NumStr[NumLen] = '\n';
	if(this->_SSL->Write(NumStr, NumLen + 1) <= 0)
		return 0;

	return 1;
}

int ZLib::WriteData()
{
	int CurLen;
	int WriteLen;

	//if we don't have a buffer then fail
	if(!this->_OutBuffer)
		return 0;

	//write out all the data
	CurLen = 0;
	while(CurLen < this->_CompressSize)
	{
		WriteLen = this->_SSL->Write((char *)&this->_OutBuffer[CurLen], this->_CompressSize - CurLen);
		if(WriteLen <= 0)
			return 0;

		CurLen += WriteLen;
	};

	return CurLen;
}

