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

// Set the MAX_LINE_LEN
#define MAX_LINE_LEN		1024

#define LOGGING_OFF             (0)
#define LOG_PRIORITY_LOW        1
#define LOG_PRIORITY_MEDIUM     2
#define LOG_PRIORITY_HIGH       3

#define DEBUG_LOG_FILENAME	"/home/jdub/quals_15/source_code/sirgoon/quals_17/babyint/server/debug.log"

#define DEFAULT_LOG_PRIORITY	(LOG_PRIORITY_HIGH)

#define NO_LOGGING

#include "doublelist.h"

void ReadDevURandom( uint8_t *, uint32_t );
void SetLogPriority( uint32_t priority );

#ifdef NO_LOGGING
#define OpenDebugLog( a )
#define LogDebug( a, b, ... )
#define CloseDebugLog( )
#else
void OpenDebugLog( const char *pszFilename );
int32_t LogDebug( uint32_t priority, const char *format, ... );
void CloseDebugLog( void );
#endif

#endif // __COMMON_H__
