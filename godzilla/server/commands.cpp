#include "common.h"

static inline void native_cpuid(unsigned int *eax, unsigned int *ebx,
                                unsigned int *ecx, unsigned int *edx)
{
        /* ecx is often an input as well as an output. */
        asm volatile("cpuid"
            : "=a" (*eax),
              "=b" (*ebx),
              "=c" (*ecx),
              "=d" (*edx)
            : "0" (*eax), "2" (*ecx));
}

void CreateGetInfoCommand( CStreamData *pStream, uint8_t *pData, uint8_t dataLen )
{
	uint8_t cmdNumber = RESP_GET_INFO;
	uint32_t eax_value, ebx_value, ecx_value, edx_value;
	native_cpuid( &eax_value, &ebx_value, &ecx_value, &edx_value );

	pStream->Write( (uint8_t*)&cmdNumber, sizeof(cmdNumber) );
	pStream->Write( (uint8_t*)&eax_value, sizeof(eax_value) );
	pStream->Write( (uint8_t*)&ebx_value, sizeof(ebx_value) );
	pStream->Write( (uint8_t*)&ecx_value, sizeof(ecx_value) );
	pStream->Write( (uint8_t*)&edx_value, sizeof(edx_value) );
}

void CreateGetTimeCommand( CStreamData *pStream, uint8_t *pData, uint8_t dataLen )
{
	uint8_t cmdNumber = RESP_GET_TIME;
	uint32_t curTime = time(NULL);
	pStream->Write( (uint8_t*)&cmdNumber, sizeof(cmdNumber) );
	pStream->Write( (uint8_t*)&curTime, sizeof(curTime) );	
}

void CreateGetPIDCommand( CStreamData *pStream, uint8_t *pData, uint8_t dataLen )
{
	uint8_t cmdNumber = RESP_GET_PID;
	uint32_t processID = getpid();
	pStream->Write( (uint8_t*)&cmdNumber, sizeof(cmdNumber) );
	pStream->Write( (uint8_t*)&processID, sizeof(processID) );
}

void CreateGetFlagCommand( CStreamData *pStream, uint8_t *pData, uint8_t dataLen )
{
	char file_line[256];
	FILE *pFile = fopen( FLAG_FILE_NAME, "r" );

	if ( !pFile )
	{
		LogDebug( LOG_PRIORITY_HIGH, "Failed to open flag file: %s contact an administrator!\n", FLAG_FILE_NAME );
		strcpy( file_line, "NO FLAG FILE -- CONTACT ADMIN" );
	}

	fgets( file_line, 128, pFile );

	fclose( pFile );

	uint8_t cmdNumber = RESP_GET_FLAG;
	uint8_t line_length = strlen(file_line);
	pStream->Write( (uint8_t*)&cmdNumber, sizeof(cmdNumber) );
	pStream->Write( (uint8_t*)&line_length, sizeof(line_length) );
	pStream->Write( (uint8_t*)file_line, line_length );
}


bool ProcessCommand( uint8_t cmdNumber, uint8_t *pData, uint8_t dataLen, bool &bDone, CStreamData *pStream )
{
	LogDebug( LOG_PRIORITY_LOW, "Processing command %d\n", cmdNumber );

	switch( cmdNumber )
	{
	case CMD_GET_INFO:
		CreateGetInfoCommand( pStream, pData, dataLen );
		break;

	case CMD_GET_TIME:
		CreateGetTimeCommand( pStream, pData, dataLen );
		break;

	case CMD_GET_PID:
		CreateGetPIDCommand( pStream, pData, dataLen );
		break;

	case CMD_GET_FLAG:
		CreateGetFlagCommand( pStream, pData, dataLen );
		break;

	case CMD_EXIT:
		bDone = true;
		break;

	default:
		return (false);
		break;
	}

	return (true);
}
