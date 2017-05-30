#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "CommonFile.h"

CFileCommon::FC_STATUS CFileReader::ReadFile( const char *pszFileName, void** pData, uint32_t &readLen )
{
	FILE *pFp;

	if ( pData == NULL )
		return (CFileCommon::FC_ERROR);

	if ( !(pFp = fopen( pszFileName, "rb" )) )
		return (CFileCommon::FC_FILE_NOT_FOUND);

	uint32_t size;
	uint32_t start;
	uint32_t end;

	// Read in the file length
	fseek( pFp, 0, SEEK_SET );

	start = ftell( pFp );

	fseek( pFp, 0, SEEK_END );

	end = ftell ( pFp );

	size = (end - start);

	// Allocate buffer
	(*pData) = (void *)new unsigned char[ size ];

	if ( (*pData) == NULL )
		return (CFileCommon::FC_OUT_OF_MEMORY);

	// Return to beginning
	fseek( pFp, 0, SEEK_SET );

	// Read into memory
	fread( (*pData), 1, size, pFp );
	
	// Save the read len
	readLen = size;

	// Return ok 
	return (CFileCommon::FC_OK);
}

CFileCommon::FC_STATUS CFileWriter::WriteFile( const char *pszFileName, void* pData, uint32_t writeLen )
{
	return (CFileCommon::FC_OK);
}
