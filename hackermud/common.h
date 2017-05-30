#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <iterator>
#include <sstream>
#include <functional>
#include <algorithm>

using namespace std;

#include "bitwriter.h"
#include "streamdata.h"
#include "pthreadcc.h"
#include "socketcc.h"
#include "md5.h"
#include "nutil_list.h"
#include "nutil_hash.h"
#include "color.h"
#include "item.h"
#include "player.h"
#include "room.h"
#include "connection.h"
#include "commands.h"
#include "networkcommands.h"
#include "script.h"

// Turn on to enable sig alarm timeouts in final build
#define MAX_IDLE_SECS			(50)
#define ENABLE_SIG_ALARM_HANDLER	(1)

// Enable for login hashing (must match server!!!!)
#define ENABLE_LOGIN_HASH		false	
#define ENABLE_PROOF_OF_WORK		true

#define FLEE_CPU_THRESHOLD_PERCENTAGE   	(0.5)
#define MOVEMENT_CPU_THRESHOLD_PERCENTAGE	(0.20)
#define MOVEMENT_CPU_THRESHOLD_MIN		(10)	// Movement consumes a minimum of 10 CPU

#define RUN_SCRIPT_CPU_SCALE_PER_LEVEL	(1.01)	// Running scripts is faster by leveling CPU by this amount per level
#define RUN_SCRIPT_BASE_CPU_PERCENTAGE  (0.25)	// Running scripts cost this much base CPU by default
#define RUN_SCRIPT_CPU_MIN              (20) 	// Running scripts cost at a minimum this much CPU + base percentage

#define RUN_SCRIPT_MEMORY_SCALE_PER_LEVEL	(1.02)
#define RUN_SCRIPT_MEMORY_MIN			(20)

#define PLAYER_MAX_CODE_LEVEL           (20)
#define PLAYER_MAX_HACK_LEVEL           (20)
#define PLAYER_MAX_SCRIPT_LEVEL         (20)

#define SERVER_IP_ADDRESS	"localhost"
#define SERVER_PORT		5050

#define MAX_PASSWORD_LENGTH	(32)
#define MIN_PASSWORD_LENGTH	(12)

#define MAX_USERNAME_LENGTH	12
#define MIN_USERNAME_LENGTH	(7)

#define MAX_INPUT_LENGTH		(512)
#define NO_INPUT_DISCONNECT_COUNTER	(8*45)

const unsigned int ROOM_HASH_SIZE = 17;

vector<string> SplitString( string sInStr, char delim=' ' );

std::string& ltrim(std::string& s);
std::string& rtrim(std::string& s);

// Helper function to calculate the amount of CPU needed to move
uint16_t GetMovementCPUNeeded( CPlayer *pPlayer );
bool CanMoveCPUCheck( CPlayer *pPlayer );

bool ReadInputLine( char *pszLine, uint32_t maxLen );

extern CCommandHandler g_cmdHandler;

inline char ToHexChar( uint8_t input )
{
        if ( input < 10 )
                return '0' + (input);
        else if ( input < 16 )
                return 'A' + (input-10);
        else
                return '@';
}

inline uint8_t FromHexChar( char ch )
{
        if ( ch >= '0' && ch <= '9' )
                return (ch - '0');
        else if ( ch >= 'A' && ch <= 'F' )
                return (ch - 'A') + 10;
        else if ( ch >= 'a' && ch <= 'f' )
                return (ch - 'a') + 10;
        else
                return 0;
}

#endif // __COMMON_H__
