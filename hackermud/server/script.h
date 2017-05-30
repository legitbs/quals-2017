#ifndef __SCRIPT_H__
#define __SCRIPT_H__

bool PlayerHasScript( string sScriptName );

typedef bool (*fpRunScript)( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen );

typedef struct
{
	char szScriptName[128];
	uint8_t scriptID;
	uint8_t scriptLevel;
	uint16_t scriptMemoryNeeded;
	fpRunScript pRunFunc;	
} tScriptTableStruct;

bool RunScript( CServerConnection *pCur, uint8_t scriptNumber, uint8_t *pData, uint32_t dataLen );

bool DoScriptDelete( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen );
bool DoScriptCorrupt( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen );
bool DoScriptRepair( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen );
bool DoScriptSpray( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen );
bool DoScriptDDOS( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen );
bool DoScriptBoost( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen );
bool DoScriptRestore( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen );
bool DoScriptExploit( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen );
bool DoScriptImplant( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen );

bool NPCRunHealScript( CNPC *pNPC );
bool NPCRunDamageScript( CNPC *pNPC );

#endif // __SCRIPT_H__
