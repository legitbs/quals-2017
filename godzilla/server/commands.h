#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <stdint.h>

typedef struct
{
	uint8_t responseKey[16];
	uint8_t forwardKey[16];
	uint32_t magic1;
	uint32_t magic2;
	uint32_t magic3;
	uint32_t magic4;
	uint8_t padding[40];
	uint32_t zero1;
	uint32_t zero2;
} __attribute__((packed)) tServerRequestEncryptedHeader;

#define CMD_GET_TIME	(1)
#define CMD_GET_PID	(10)
#define CMD_GET_INFO	(23)
#define CMD_GET_FLAG	(57)
#define CMD_EXIT	(32)

#define RESP_INVALID_COMMAND	(11)
#define RESP_GET_TIME		(CMD_GET_TIME+10)
#define RESP_GET_PID		(CMD_GET_PID+10)
#define RESP_GET_INFO		(CMD_GET_INFO+10)
#define RESP_GET_FLAG		(CMD_GET_FLAG+10)

bool ProcessCommand( uint8_t cmdNumber, uint8_t *pData, uint8_t dataLen, bool &bDone, CStreamData *pStream );

#endif // __COMMANDS_H__
