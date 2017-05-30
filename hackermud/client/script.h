#ifndef __SCRIPT_H__
#define __SCRIPT_H__

bool PlayerHasScript( string sScriptName );

typedef bool (*fpRunScript)( CServerConnection *pCur, vector<string> &sArgs );

typedef struct
{
	char szScriptName[128];
	uint8_t scriptID;
	uint8_t scriptLevel;
	uint8_t scriptMemoryNeeded;
	fpRunScript pRunFunc;	
} tScriptTableStruct;

bool RunScript( CServerConnection *pCur, string sScriptName, vector<string> &sArgs );

bool DoScriptDelete( CServerConnection *pCur, vector<string> &sArgs );
bool DoScriptCorrupt( CServerConnection *pCur, vector<string> &sArgs );
bool DoScriptRepair( CServerConnection *pCur, vector<string> &sArgs );
bool DoScriptSpray( CServerConnection *pCur, vector<string> &sArgs );
bool DoScriptDDOS( CServerConnection *pCur, vector<string> &sArgs );
bool DoScriptBoost( CServerConnection *pCur, vector<string> &sArgs );
bool DoScriptRestore( CServerConnection *pCur, vector<string> &sArgs );
bool DoScriptExploit( CServerConnection *pCur, vector<string> &sArgs );
bool DoScriptImplant( CServerConnection *pCur, vector<string> &sArgs );

#endif // __SCRIPT_H__
