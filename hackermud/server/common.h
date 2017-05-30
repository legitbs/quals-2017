#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <string>
#include <map>
#include <vector>
#include <hash_map>
#include <iostream>
#include <sys/time.h>
#include <functional>
#include <algorithm>

using namespace std;

#include "sha256.h"
#include "MersenneRNG.h"
#include "bitwriter.h"
#include "streamdata.h"
#include "pthreadcc.h"
#include "socketcc.h"
#include "md5.h"
#include "nutil_list.h"
#include "nutil_hash.h"
#include "color.h"
#include "attacktarget.h"
#include "item.h"
#include "player.h"
#include "npc.h"
#include "room.h"
#include "world.h"
#include "connection.h"
#include "commands.h"
#include "networkcommands.h"
#include "damage.h"
#include "script.h"
#include "pow.h"


#define SERVER			(1)

#define ENABLE_SOCKET_EXCEPTION_PRINT	(1)
#define MAX_ITEMS_IN_ROOM 	(15)

// Inactivity Timer for server
//#define MAX_INACTIVITY_TICK_COUNT	(5*50)
#define MAX_INACTIVITY_TICK_COUNT	(50*50)

#define FLEE_CPU_THRESHOLD_PERCENTAGE	(0.5)
#define MOVEMENT_CPU_THRESHOLD_PERCENTAGE       (0.20)
#define MOVEMENT_CPU_THRESHOLD_MIN              (10)    // Movement consumes a minimum of 10 CPU

#define RUN_SCRIPT_CPU_SCALE_PER_LEVEL  (1.01)  // Running scripts is faster by leveling CPU by this amount per level
#define RUN_SCRIPT_BASE_CPU_PERCENTAGE  (0.25)  // Running scripts cost this much base CPU by default
#define RUN_SCRIPT_CPU_MIN              (20)    // Running scripts cost at a minimum this much CPU + base percentage

#define RUN_SCRIPT_MEMORY_SCALE_PER_LEVEL       (1.02)
#define RUN_SCRIPT_MEMORY_MIN                   (20)

#define PLAYER_MAX_CODE_LEVEL		(20)
#define PLAYER_MAX_HACK_LEVEL		(20)
#define PLAYER_MAX_SCRIPT_LEVEL		(20)

// Turn this on to enable player file saving
#define ENABLE_PROOF_OF_WORK	(1)
#define ENABLE_LOGIN_HASH	(false)
// #define ENABLE_PLAYER_SAVE	(1)
#define ENABLE_PLAYER_LOGIN_CACHE	(1)	// Player data will be saved across logins (not necessarily to file)
#define ENABLE_IMPLANT_BUG	(1)

#if ENABLE_LOGIN_HASH
#include <NTL/ZZ.h>

using namespace NTL;
#endif

#define SERVER_TICK_PER_SECOND	(5)

#define SERVER_IP_ADDRESS	"localhost"
#define SERVER_IP_PORT		5050
#define SERVER_BACKLOG_COUNT	(20)

#define MAX_USERNAME_LENGTH	12
#define MIN_USERNAME_LENGTH	(7)

#define PLAYER_FILE_DIRECTORY	"player/"
#define PLAYER_FILE_EXT		"plist"
#define MAX_PLAYER_NAME		(12)

#define ROOM_FILE_DIRECTORY	"rooms/"
#define ROOM_FILE_NAME		"rooms.txt"

#define ITEM_FILE_DIRECTORY	"items/"
#define ITEM_FILE_NAME		"items.txt"

#define NPC_FILE_DIRECTORY	"npcs/"
#define NPC_FILE_NAME		"npcs.txt"

#define INITIAL_ROOM_VNUM	(100)		// Initial room players start in (and when the room can't be found)

#define NEW_PLAYER_PFILE	("default")	// New player name (for pfile) default.plist

#define PROOF_OF_WORK_N		(105)
#define PROOF_OF_WORK_K		(4)

const unsigned int ROOM_HASH_SIZE = 17;
const unsigned int ITEM_HASH_SIZE = 17;
const unsigned int NPC_HASH_SIZE = 17;
extern tGlobalRoomHash g_oRoomHash;
extern tItemDatabaseHash g_oItemDBHash;
extern tNPCDatabaseHash g_oNPCDBHash;
extern tGlobalSpamTracker g_oSpamTracker;
extern tGlobalSpawnPoints g_oPlayerSpawnPoints;
extern tGlobalPlayerLoginCache g_oPlayerLoginCache;
extern tGlobalRoomList g_oWorldRoomList;

extern NUtil::LIST_DECLARE( CNPC, m_worldLink ) g_worldNPCs;
extern NUtil::LIST_DECLARE( CPlayer, m_worldLink ) g_worldPlayers;

extern MersenneRNG g_oRNG;

extern uint32_t g_serverRebootTimestamp;

vector<string> SplitString( string sInStr, char delim=' ' );

bool CheckPlayerName( const char *pszPlayerName );

void ReadDevURandom( uint8_t *pDest, uint32_t destLen );

void SanitizeString( char *pData, uint32_t dataLen );

// FILE helper defines
#define READ_FILE_STRING( name, value ) {\
					char szTemp[1024];\
					char szName[1024];\
					char *pTemp;\
					if ( feof(pFile) )\
						goto bad_fileread;\
					if ( fgets( szLine, 1024, pFile ) == NULL || sscanf( szLine, "%s %s", szName, szTemp ) != 2 || strcmp( szName, name ) != 0 || !(pTemp = strstr( szLine, " " ))  )\
						goto bad_fileread;\
					pTemp[strlen(pTemp)-1] = '\0';\
					value = pTemp+1;\
					}

#define READ_FILE_UINT16( name, value ) {\
					char szTemp[1024];\
					char szName[1024];\
					if ( feof(pFile) )\
						goto bad_fileread;\
					if ( fgets( szLine, 1024, pFile ) == NULL || sscanf( szLine, "%s %hu", szName, &value ) != 2 || strcmp( szName, name ) != 0 )\
						goto bad_fileread;\
					}

#define READ_FILE_UINT32( name, value ) {\
					char szTemp[1024];\
					char szName[1024];\
					if ( feof(pFile) )\
						goto bad_fileread;\
					if ( fgets( szLine, 1024, pFile ) == NULL || sscanf( szLine, "%s %u", szName, &value ) != 2 || strcmp( szName, name ) != 0 )\
						goto bad_fileread;\
					}

					
#define READ_FILE_UINT8( name, value ) {\
					char szTemp[1024];\
					char szName[1024];\
					if ( feof(pFile) )\
						goto bad_fileread;\
					if ( fgets( szLine, 1024, pFile ) == NULL || sscanf( szLine, "%s %hhu", szName, &value ) != 2 || strcmp( szName, name ) != 0 )\
						goto bad_fileread;\
					}

#define READ_FILE_INT8( name, value ) {\
					char szTemp[1024];\
					char szName[1024];\
					if ( feof(pFile) )\
						goto bad_fileread;\
					if ( fgets( szLine, 1024, pFile ) == NULL || sscanf( szLine, "%s %hhi", szName, &value ) != 2 || strcmp( szName, name ) != 0 )\
						goto bad_fileread;\
					}

#define READ_FILE_INT16( name, value ) {\
					char szTemp[1024];\
					char szName[1024];\
					if ( feof(pFile) )\
						goto bad_fileread;\
					if ( fgets( szLine, 1024, pFile ) == NULL || sscanf( szLine, "%s %hi", szName, &value ) != 2 || strcmp( szName, name ) != 0 )\
						goto bad_fileread;\
					}

#define WRITE_FILE_STRING( name, value ) {\
						fprintf( pFile, "%s %s\n", name, value.c_str() );\
					}

#define WRITE_FILE_UINT32( name, value ) {\
						fprintf( pFile, "%s %u\n", name, value );\
					}

#define WRITE_FILE_UINT16( name, value ) {\
						fprintf( pFile, "%s %hu\n", name, value );\
					}

#define WRITE_FILE_UINT8( name, value ) {\
						fprintf( pFile, "%s %hhu\n", name, value );\
					}

#define WRITE_FILE_INT16( name, value ) {\
						fprintf( pFile, "%s %hi\n", name, value );\
					}


#define WRITE_FILE_INT8( name, value ) {\
						fprintf( pFile, "%s %hhi\n", name, value );\
					}
#endif // __COMMON_H__
