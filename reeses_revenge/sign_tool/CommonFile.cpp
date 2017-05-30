//     ,---,.  .--.--.       ,---,.          ____                  ,--,                  ___                       
//   ,'  .' | /  /    '.   ,'  .' |        ,'  , `.              ,--.'|                ,--.'|_                     
// ,---.'   ||  :  /`. / ,---.'   |     ,-+-,.' _ |         ,--, |  | :                |  | :,'   ,---.    __  ,-. 
// |   |   .';  |  |--`  |   |   .'  ,-+-. ;   , ||       ,'_ /| :  : '                :  : ' :  '   ,'\ ,' ,'/ /| 
// :   :  |-,|  :  ;_    :   :  |-, ,--.'|'   |  ||  .--. |  | : |  ' |     ,--.--.  .;__,'  /  /   /   |'  | |' | 
// :   |  ;/| \  \    `. :   |  ;/||   |  ,', |  |,,'_ /| :  . | '  | |    /       \ |  |   |  .   ; ,. :|  |   ,' 
// |   :   .'  `----.   \|   :   .'|   | /  | |--' |  ' | |  . . |  | :   .--.  .-. |:__,'| :  '   | |: :'  :  /   
// |   |  |-,  __ \  \  ||   |  |-,|   : |  | ,    |  | ' |  | | '  : |__  \__\/: . .  '  : |__'   | .; :|  | '    
// '   :  ;/| /  /`--'  /'   :  ;/||   : |  |/     :  | : ;  ; | |  | '.'| ," .--.; |  |  | '.'|   :    |;  : |    
// |   |    \'--'.     / |   |    \|   | |`-'      '  :  `--'   \;  :    ;/  /  ,.  |  ;  :    ;\   \  / |  , ;    
// |   :   .'  `--'---'  |   :   .'|   ;/          :  ,      .-./|  ,   /;  :   .'   \ |  ,   /  `----'   ---'     
// |   | ,'              |   | ,'  '---'            `--`----'     ---`-' |  ,     .-./  ---`-'                     
// `----'                `----'                                           `--`---'                                 
//                                                                                                                        
// Copyright 2010 The Praemittias Group, Inc.
// http://www.praemittias.com/
// All Rights Reserved.
// This software is sold subject to a license. Please refer to the
// contract with The Praemittias Group, Inc., for an explanation of
// the restricted rights associated with this code.
//
// FILE: CommonFile.cpp  
//
// DESCRIPTION: 
// Common file I/O used throughout the project
//*************************************************************************

#include "CommonFile.h"

#include <stdio.h>
#include <stdlib.h>

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

	// Close file
	fclose( pFp );
	
	// Save the read len
	readLen = size;

	// Return ok 
	return (CFileCommon::FC_OK);
}

CFileCommon::FC_STATUS CFileWriter::WriteFile( const char *pszFileName, void* pData, uint32_t writeLen )
{
	FILE *pFp;

	if ( pData == NULL )
		return (CFileCommon::FC_ERROR);

	if ( !(pFp = fopen( pszFileName, "wb" )) )
		return (CFileCommon::FC_CANNOT_OPEN);

	if ( fwrite( pData, 1, writeLen, pFp ) != writeLen )
		return (CFileCommon::FC_ERROR);
	
	// Close file
	fclose( pFp );

	// Return ok 
	return (CFileCommon::FC_OK);
}