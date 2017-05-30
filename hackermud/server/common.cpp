#include "common.h"

vector<string> SplitString( string sInStr, char delim )
{
	vector<string> result;

	const char *str = sInStr.c_str();

	do
	{
		const char *begin = str;

		while ( *str != delim && *str )
			str++;

		result.push_back( string( begin, str ) );
	} while ( *str++ != '\0' );

	return result;
}

bool CheckPlayerName( const char *pszPlayerName )
{
	if ( strlen( pszPlayerName ) > MAX_PLAYER_NAME )
		return false;

	while ( *pszPlayerName != '\0' )
	{
		if ( (*pszPlayerName >= 'a' && *pszPlayerName <= 'z') ||
		     (*pszPlayerName >= 'A' && *pszPlayerName <= 'Z') ||
		     (*pszPlayerName >= '0' && *pszPlayerName <= '9')  )
			;
		else
			return (false); 
		
		pszPlayerName++;	
	}

	return (true);
}

void ReadDevURandom( uint8_t *pDest, uint32_t destLen )
{
	FILE *pFile;

        pFile = fopen( "/dev/urandom", "r" );

        if ( !pFile )
        {
                printf( "Couldn't open /dev/urandom\n" );

                for ( uint8_t i = 0; i < destLen; i++ )
                        pDest[i] = (uint8_t)rand();

                return;
        }

        if ( fread( pDest, sizeof(uint8_t), destLen, pFile ) != destLen )
        {
                printf( "Couldn't get enough entropy, seed used\n" );

                for ( uint8_t i = 0; i < destLen; i++ )
                        pDest[i] = (uint8_t)rand();

                return;
        }

        fclose( pFile );

        return;
}



void SanitizeString( char *pData, uint32_t dataLen )
{
	if ( !pData || dataLen == 0 )
		return;

	for ( uint32_t pos = 0; pos < dataLen; pos++ )
	{
		if ( pData[pos] == '\0' )
			break;

		if ( (pData[pos] >= 'a' && pData[pos] <= 'z') ||
		     (pData[pos] >= 'A' && pData[pos] <= 'Z') ||
		     (pData[pos] >= '0' && pData[pos] <= '9') ||
		     pData[pos] == '!' || pData[pos] == ',' ||
		     pData[pos] == '_' || pData[pos] == ' ' ||
		     pData[pos] == ':' || pData[pos] == ';' ||
		     pData[pos] == ')' || pData[pos] == '(' )
			;
		else
			pData[pos] = '_';
	}
}
