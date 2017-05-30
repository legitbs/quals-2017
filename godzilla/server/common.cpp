#include "common.h"

uint32_t g_logPriority = DEFAULT_LOG_PRIORITY;
FILE *g_pDebugLog = NULL;

void ReadDevURandom( uint8_t *pDest, uint32_t destLen )
{
        FILE *pFile;

        if ( destLen < 32 )
                return;

        pFile = fopen( "/dev/urandom", "r" );

        if ( !pFile )
        {
		LogDebug( LOG_PRIORITY_HIGH, "Couldn't open /dev/urandom\n" );

                for ( uint8_t i = 0; i < 32; i++ )
                        pDest[i] = (uint8_t)rand();

                return;
        }

        if ( fread( pDest, sizeof(uint8_t), destLen, pFile ) != destLen )
        {
		LogDebug( LOG_PRIORITY_HIGH, "Couldn't get enough entropy, seed used\n" );

                for ( uint8_t i = 0; i < 32; i++ )
                        pDest[i] = (uint8_t)rand();

                return;
        }

        fclose( pFile );

        return;
}

void OpenDebugLog( const char *pszFilename )
{
	g_pDebugLog = fopen( pszFilename, "w+" );
}

int32_t LogDebug( uint32_t priority, const char *format, ... )
{
        va_list args;
        va_start( args, format );
        int32_t chars_written = 0;

        if ( priority <= g_logPriority && g_pDebugLog)
	{
                chars_written = vfprintf( g_pDebugLog, format, args );
		fflush( g_pDebugLog );
	}

        va_end( args );

        return (chars_written);
}

void CloseDebugLog( void )
{
	if ( g_pDebugLog )
		fclose( g_pDebugLog );
}

void SetLogPriority( uint32_t priority )
{
        g_logPriority = priority;
}
