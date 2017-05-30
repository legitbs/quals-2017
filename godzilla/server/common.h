#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <sys/time.h>

using namespace std;

#define ENABLE_SIG_ALARM_HANDLER	(1)

// Set the MAX_LINE_LEN
#define MAX_LINE_LEN		1024

#define MAX_COMMUNICATION_LENGTH	(1024)

// Signal alarm timeout for service
#define MAX_IDLE_SECS		40

#define REMOTE_SERVER_PORT	(25860)

#define LOGGING_OFF             (0)
#define LOG_PRIORITY_LOW        1
#define LOG_PRIORITY_MEDIUM     2
#define LOG_PRIORITY_HIGH       3

#define DEBUG_LOG_FILENAME	"/home/jdub/quals_15/source_code/sirgoon/quals_17/custom_rsa/src/debug_server.log"

#define DEFAULT_LOG_PRIORITY	(LOG_PRIORITY_HIGH)

#define PRIVATE_KEY_FILENAME	("/home/jdub/quals_15/source_code/sirgoon/quals_17/custom_rsa/src/private.key")

#define FLAG_FILE_NAME		("/home/jdub/quals_15/source_code/sirgoon/quals_17/custom_rsa/src/flag" )

#define ENCRYPT_MAGIC1          (0x4791EB26)
#define ENCRYPT_MAGIC2          (0x726DA66D)
#define ENCRYPT_MAGIC3          (0x9345B7FC)
#define ENCRYPT_MAGIC4          (0x9345B7FC)

#include "bigint.h"
#include "FastCodeTimer.h"
#include "cioconnection.h"
#include "streamdata.h"
#include "commands.h"
#include "cipher.h"

void ReadDevURandom( uint8_t *, uint32_t );
void OpenDebugLog( const char *pszFilename );
int32_t LogDebug( uint32_t priority, const char *format, ... );
void CloseDebugLog( void );
void SetLogPriority( uint32_t priority );


#endif // __COMMON_H__
