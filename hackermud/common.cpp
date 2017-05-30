#include "common.h"
// Helper function to calculate the amount of CPU needed to move
uint16_t GetMovementCPUNeeded( CPlayer *pPlayer )
{
        uint16_t moveCPUNeeded = 0;

        moveCPUNeeded = (uint16_t)(pPlayer->GetMaxCPU() * MOVEMENT_CPU_THRESHOLD_PERCENTAGE);

        if ( moveCPUNeeded < MOVEMENT_CPU_THRESHOLD_MIN )
                moveCPUNeeded = MOVEMENT_CPU_THRESHOLD_MIN;

        return (moveCPUNeeded);
}

bool CanMoveCPUCheck( CPlayer *pPlayer )
{
        if ( !pPlayer )
                return (false);

        if ( pPlayer->GetCurCPU() < GetMovementCPUNeeded( pPlayer ) )
                return (false);
        else
                return (true);
}

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

std::string& ltrim(std::string& s) 
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
	std::ptr_fun<int, int>(std::isgraph)));
	
	return s;
}

std::string& rtrim(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(),
	std::ptr_fun<int, int>(std::isgraph)).base(), s.end());

	return s;
}

bool ReadInputLine( char *pszLine, uint32_t maxLen )
{
	uint32_t i = 0;
	for ( i = 0; i < maxLen; i++ )
	{
		if ( read( fileno(stdin), pszLine+i, 1 ) != 1 )
		{
			pszLine[i] = '\0';
			return (false);
		}

		if ( pszLine[i] == '\n' )
			break;
	}

	pszLine[i] = '\0';
	return (true);
}
