#ifndef __COMMON_FILE_H__
#define __COMMON_FILE_H__

class CFileCommon
{
public:

	typedef enum
	{
		FC_FILE_NOT_FOUND = 1,
		FC_CANNOT_OPEN = 2,
		FC_ERROR = 3,
		FC_OUT_OF_MEMORY = 4,
		FC_OK = 0
	} FC_STATUS;

private:
};

class CFileReader
{
public:
	static CFileCommon::FC_STATUS ReadFile( const char *pszFileName, void** pData, uint32_t &readLen );

private:
};

class CFileWriter
{
public:
	static CFileCommon::FC_STATUS WriteFile( const char *pszFileName, void *pData, uint32_t writeLen );
};

#endif // __COMMON_FILE_H__
