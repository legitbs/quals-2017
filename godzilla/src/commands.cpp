#include "common.h"

void CreateGetInfoCommand( CStreamData *pStream, uint8_t *pData, uint8_t dataLen )
{
	uint8_t cmdNumber = CMD_GET_INFO;
	pStream->Write( (uint8_t*)&cmdNumber, sizeof(cmdNumber) );
	pStream->Write( (uint8_t*)&dataLen, sizeof(dataLen) );
	pStream->Write( pData, dataLen );
}

void CreateGetTimeCommand( CStreamData *pStream, uint8_t *pData, uint8_t dataLen )
{
	uint8_t cmdNumber = CMD_GET_TIME;
	pStream->Write( (uint8_t*)&cmdNumber, sizeof(cmdNumber) );
	pStream->Write( (uint8_t*)&dataLen, sizeof(dataLen) );
	pStream->Write( pData, dataLen );
}

void CreateGetPIDCommand( CStreamData *pStream, uint8_t *pData, uint8_t dataLen )
{
	uint8_t cmdNumber = CMD_GET_PID;
	pStream->Write( (uint8_t*)&cmdNumber, sizeof(cmdNumber) );
	pStream->Write( (uint8_t*)&dataLen, sizeof(dataLen) );
	pStream->Write( pData, dataLen );
}


bool ProcessCommand( uint8_t cmdNumber, uint8_t *pData, uint8_t dataLen, bool &bDone, CStreamData *pStream )
{
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
		bDone = false;
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
