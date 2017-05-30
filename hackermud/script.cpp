#include "common.h"

#define SCRIPT_DELETE_ID        (0)
#define SCRIPT_CORRUPT_ID       (1)
#define SCRIPT_REPAIR_ID        (2)
#define SCRIPT_DDOS_ID          (3)
#define SCRIPT_SPRAY_ID         (4)
#define SCRIPT_BOOST_ID         (5)
#define SCRIPT_RESTORE_ID       (6)
#define SCRIPT_EXPLOIT_ID       (7)
#define SCRIPT_IMPLANT_ID       (8)
#define SCRIPT_END_ID           (9)

tScriptTableStruct g_runScripts[] = {
{ "delete", 	SCRIPT_DELETE_ID, 	1,	50, 	DoScriptDelete },
{ "corrupt", 	SCRIPT_CORRUPT_ID, 	1,	30, 	DoScriptCorrupt },
{ "repair", 	SCRIPT_REPAIR_ID, 	3, 	50,	DoScriptRepair },
{ "ddos", 	SCRIPT_DDOS_ID, 	5, 	70,	DoScriptDDOS },
{ "spray", 	SCRIPT_SPRAY_ID, 	7, 	100,	DoScriptSpray },
{ "boost", 	SCRIPT_BOOST_ID, 	6, 	100,	DoScriptBoost },
{ "restore", 	SCRIPT_RESTORE_ID, 	9, 	100,	DoScriptRestore },
{ "exploit", 	SCRIPT_EXPLOIT_ID, 	10, 	130,	DoScriptExploit },
{ "implant", 	SCRIPT_IMPLANT_ID, 	13, 	170,	DoScriptImplant },
{ "", 		SCRIPT_END_ID, 		0, 	NULL }	// Table end
};

uint16_t GetMemoryForScript( uint8_t myScriptLevel, uint8_t runScriptLevel, uint16_t baseMemoryRequired )
{
	double multiplier = RUN_SCRIPT_MEMORY_SCALE_PER_LEVEL;
	
	multiplier = pow( multiplier, (myScriptLevel - runScriptLevel)+1 );

	return (RUN_SCRIPT_MEMORY_MIN + (baseMemoryRequired * (1.0 / multiplier)));	
}

uint16_t GetCPUForScript( uint8_t myCodeLevel, uint16_t maxPlayerCPU )
{
	double multiplier = RUN_SCRIPT_CPU_SCALE_PER_LEVEL;
	
	multiplier = pow( multiplier, (myCodeLevel) );

	double baseMultiplier = (RUN_SCRIPT_BASE_CPU_PERCENTAGE * (1.0 / multiplier));

	return (RUN_SCRIPT_CPU_MIN + (maxPlayerCPU * baseMultiplier));
}

bool RunScript( CServerConnection *pCur, string sScriptName, vector<string> &sArgs )
{
	char szTemp[600];

	if ( !pCur )
		return (false);

	CPlayer *pPlayer = pCur->GetPlayer();

	if ( !pPlayer )
		return (false);

	for ( uint8_t i = 0; ; i++ )
	{
		if ( g_runScripts[i].pRunFunc == NULL )
			break;

		if ( strncasecmp( g_runScripts[i].szScriptName, sScriptName.c_str(), sScriptName.length() ) == 0 )
		{
			if ( pPlayer->GetScriptLevel() < g_runScripts[i].scriptLevel )
			{
				sprintf( szTemp, "%sCannot run that script without sufficient script level!\n", FC_NORMAL_RED );
				pCur->WriteOutput( szTemp );
				return (true);
			}

			uint16_t cpuNeeded = GetCPUForScript( pPlayer->GetCodeLevel(), pPlayer->GetMaxCPU() );

			if ( pPlayer->GetCurCPU() < cpuNeeded )
			{
				sprintf( szTemp, "%sInsufficient CPU to run script!\n", FC_NORMAL_RED );
				pCur->WriteOutput( szTemp );
				return (true);
			}

			uint16_t memoryRequired = GetMemoryForScript( pPlayer->GetScriptLevel(), g_runScripts[i].scriptLevel, g_runScripts[i].scriptMemoryNeeded );

			if ( pPlayer->GetCurMemory() < memoryRequired )
			{
				sprintf( szTemp, "%sInsufficient Memory to run script!\n", FC_NORMAL_RED );
				pCur->WriteOutput( szTemp );
				return (true);
			}

			// printf( "Running script %s (%d) CPU %d MEMORY %d\n", g_runScripts[i].szScriptName, i, cpuNeeded, memoryRequired );

			// Run the script!
			return (*g_runScripts[i].pRunFunc)( pCur, sArgs );
		}
	}

	sprintf( szTemp, "%sRun which script?\n", FC_NORMAL_YELLOW );
	pCur->WriteOutput( szTemp );
	
	return (true);
}

bool DoScriptDelete( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[512];

	// Allows a player to delete an item in the room
	if ( !pCur )
		return (false);
       
	CPlayer *pPlayer = pCur->GetPlayer();
 
	if ( !pPlayer )
                return false;

        if ( !pPlayer->GetRoom() )
                return false;

        if ( sArgs.size() < 1 )
        {
                sprintf( szTemp, "%sDelete what?\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
                return true;
        }

        // Get by item name
        std::ostringstream result_stream;
        ostream_iterator< string > oit( result_stream, " " );
        copy( sArgs.begin(), sArgs.end(), oit );

        string sItemName = result_stream.str();
        sItemName = rtrim( sItemName );

        // Find item in room!
        bool bFound = false;
        CItemInRoom *pItem = NULL;
        for ( pItem = pPlayer->GetRoom()->GetItemsInRoom()->Head(); pItem; pItem = pPlayer->GetRoom()->GetItemsInRoom()->Next( pItem ) )
        {
                if ( strncasecmp( sItemName.c_str(), pItem->GetName().c_str(), sItemName.length() ) == 0 )
                {
                        bFound = true;
                        break;
                }
        }

        if ( !bFound )
        {
		sprintf( szTemp, "%sNo item by that name here.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return true;
	}

	// Attempt to delete this item
	CStreamData oStream;

	uint32_t itemID = pItem->GetID();	
	oStream.Write( &itemID, sizeof(itemID) );

	NWCRunScriptRequest( pCur, SCRIPT_DELETE_ID, oStream );

	return (true);
}

bool DoScriptCorrupt( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[512];

	// Allows a player to attack or -- use against the current primary target
	if ( !pCur )
		return (false);
       
	CPlayer *pPlayer = pCur->GetPlayer();
 
	if ( !pPlayer )
                return false;

        if ( !pPlayer->GetRoom() )
                return false;

        if ( sArgs.size() < 1 )
        {
		// Currently attack the primary target
		if ( !pPlayer->IsFighting() )
		{
			sprintf( szTemp, "%sRun corrupt script on what?\n", FC_NORMAL_RED );
			pCur->WriteOutput( szTemp );
			return (true);
		}	
                
		// Request to attack player
		CStreamData oStream;

		uint8_t targetType = pPlayer->GetPrimaryTarget()->GetTargetType();
		uint32_t targetID = pPlayer->GetPrimaryTarget()->GetTargetID();

		oStream.Write( &targetType, sizeof(targetType) );
		oStream.Write( &targetID, sizeof(targetID) );

		// Run script request
		NWCRunScriptRequest( pCur, SCRIPT_CORRUPT_ID, oStream );
                
		return true;
        }

	CRoom *pRoom;

	if ( !(pRoom = pPlayer->GetRoom()) )
		return (false);

        // Get by item name
        std::ostringstream result_stream;
        ostream_iterator< string > oit( result_stream, " " );
        copy( sArgs.begin(), sArgs.end(), oit );

        string sTargetName = result_stream.str();
        sTargetName = rtrim( sTargetName );

	bool bFound = false;
	CPlayerInRoom *pPlayerInRoom = NULL;
        for ( pPlayerInRoom = pRoom->GetPlayersInRoom()->Head(); pPlayerInRoom; pPlayerInRoom = pRoom->GetPlayersInRoom()->Next( pPlayerInRoom ) )
        {
                if ( strncasecmp( sTargetName.c_str(), pPlayerInRoom->GetName().c_str(), sTargetName.length() ) == 0 )
                {
                        bFound = true;
                        break;
                }
        }

        if ( bFound )
        {
                // Attacking player
                if ( !pRoom->IsPKAllowed() )
                {
                        sprintf( szTemp, "%sYou can't corrupt another player in a non PK area.\n", FC_NORMAL_RED );
			pCur->WriteOutput( szTemp );
                        return true;
                }

                // Request to attack player
		CStreamData oStream;

		uint8_t targetType = NWC_ATTACK_PLAYER;
		uint32_t targetID = pPlayerInRoom->GetID();

		oStream.Write( &targetType, sizeof(targetType) );
		oStream.Write( &targetID, sizeof(targetID) );

		// Run script request
		NWCRunScriptRequest( pCur, SCRIPT_CORRUPT_ID, oStream );
                
		return true;
        }

        CNPCInRoom *pNPCInRoom = NULL;
        for ( pNPCInRoom = pRoom->GetNPCsInRoom()->Head(); pNPCInRoom; pNPCInRoom = pRoom->GetNPCsInRoom()->Next( pNPCInRoom ) )
        {
                if ( strncasecmp( sTargetName.c_str(), pNPCInRoom->GetName().c_str(), sTargetName.length() ) == 0 )
                {
                        bFound = true;
                        break;
                }
        }

        if ( bFound )
        {
		CStreamData oStream;

		uint8_t targetType = NWC_ATTACK_NPC;
		uint32_t targetID = pNPCInRoom->GetID();

		oStream.Write( &targetType, sizeof(targetType) );
		oStream.Write( &targetID, sizeof(targetID) );

		// Run script request
		NWCRunScriptRequest( pCur, SCRIPT_CORRUPT_ID, oStream );
                
		return true;
        }

        sprintf( szTemp, "%sCorrupt what?\n", FC_NORMAL_RED );
	pCur->WriteOutput( szTemp );

	return (true);
}

bool DoScriptRepair( CServerConnection *pCur, vector<string> &sArgs )
{
	// A player can restore HP of self
	if ( !pCur )
		return (false);
       
	CPlayer *pPlayer = pCur->GetPlayer();
 
	if ( !pPlayer )
                return false;

	CStreamData oStream;

	// Run script request
	NWCRunScriptRequest( pCur, SCRIPT_REPAIR_ID, oStream );
                
	return true;
}

bool DoScriptDDOS( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[300];

	// A player can cause the CPU of a player to (very little)
	if ( !pCur )
		return (false);
       
	CPlayer *pPlayer = pCur->GetPlayer();
 
	if ( !pPlayer )
                return false;

        if ( !pPlayer->GetRoom() )
                return false;

	// Currently attack the primary target
	if ( !pPlayer->IsFighting() )
	{
		sprintf( szTemp, "%sYou must be attacking to DDOS something.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return (true);
	}	
                
	// Run script request
	CStreamData oStream;

	// Run script request
	NWCRunScriptRequest( pCur, SCRIPT_DDOS_ID, oStream );
	
	return (true);
}

bool DoScriptSpray( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[330];

	// A player can heap spray a target reducing their Memory
	if ( !pCur )
		return (false);
       
	CPlayer *pPlayer = pCur->GetPlayer();
 
	if ( !pPlayer )
                return false;

        if ( !pPlayer->GetRoom() )
                return false;

	// Currently attack the primary target
	if ( !pPlayer->IsFighting() )
	{
		sprintf( szTemp, "%sYou must be attacking to heap spray something.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return (true);
	}	
	
	// Run script request
	CStreamData oStream;

	// Run script request
	NWCRunScriptRequest( pCur, SCRIPT_SPRAY_ID, oStream );
	
	return (true);
}

bool DoScriptBoost( CServerConnection *pCur, vector<string> &sArgs )
{
	// A player can boost their own CPU
	if ( !pCur )
		return (false);
       
	CPlayer *pPlayer = pCur->GetPlayer();
 
	if ( !pPlayer )
                return false;

	CStreamData oStream;

	// Run script request
	NWCRunScriptRequest( pCur, SCRIPT_BOOST_ID, oStream );
         
	return (true);       
}

bool DoScriptRestore( CServerConnection *pCur, vector<string> &sArgs )
{
	// A player can restore both CPU and HP
	if ( !pCur )
		return (false);
       
	CPlayer *pPlayer = pCur->GetPlayer();
 
	if ( !pPlayer )
                return false;

	CStreamData oStream;

	// Run script request
	NWCRunScriptRequest( pCur, SCRIPT_RESTORE_ID, oStream );
         
	return (true);       
}

bool DoScriptExploit( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[400];

	// A player can damage a player (more damage then corrupt)
	if ( !pCur )
		return (false);
       
	CPlayer *pPlayer = pCur->GetPlayer();
 
	if ( !pPlayer )
                return false;

        if ( !pPlayer->GetRoom() )
                return false;

        if ( sArgs.size() < 1 )
        {
		// Currently attack the primary target
		if ( !pPlayer->IsFighting() )
		{
			sprintf( szTemp, "%sRun exploit script on what?\n", FC_NORMAL_RED );
			pCur->WriteOutput( szTemp );
			return (true);
		}	
                
		// Request to attack player
		CStreamData oStream;

		uint8_t targetType = pPlayer->GetPrimaryTarget()->GetTargetType();
		uint32_t targetID = pPlayer->GetPrimaryTarget()->GetTargetID();

		oStream.Write( &targetType, sizeof(targetType) );
		oStream.Write( &targetID, sizeof(targetID) );

		// Run script request
		NWCRunScriptRequest( pCur, SCRIPT_EXPLOIT_ID, oStream );
                
		return true;
        }

	CRoom *pRoom;

	if ( !(pRoom = pPlayer->GetRoom()) )
		return (false);

        // Get by item name
        std::ostringstream result_stream;
        ostream_iterator< string > oit( result_stream, " " );
        copy( sArgs.begin(), sArgs.end(), oit );

        string sTargetName = result_stream.str();
        sTargetName = rtrim( sTargetName );

	bool bFound = false;
	CPlayerInRoom *pPlayerInRoom = NULL;
        for ( pPlayerInRoom = pRoom->GetPlayersInRoom()->Head(); pPlayerInRoom; pPlayerInRoom = pRoom->GetPlayersInRoom()->Next( pPlayerInRoom ) )
        {
                if ( strncasecmp( sTargetName.c_str(), pPlayerInRoom->GetName().c_str(), sTargetName.length() ) == 0 )
                {
                        bFound = true;
                        break;
                }
        }

        if ( bFound )
        {
                // Attacking player
                if ( !pRoom->IsPKAllowed() )
                {
                        sprintf( szTemp, "%sYou can't exploit another player in a non PK area.\n", FC_NORMAL_RED );
			pCur->WriteOutput( szTemp );
                        return true;
                }

                // Request to attack player
		CStreamData oStream;

		uint8_t targetType = NWC_ATTACK_PLAYER;
		uint32_t targetID = pPlayerInRoom->GetID();

		oStream.Write( &targetType, sizeof(targetType) );
		oStream.Write( &targetID, sizeof(targetID) );

		// Run script request
		NWCRunScriptRequest( pCur, SCRIPT_EXPLOIT_ID, oStream );
                
		return true;
        }

        CNPCInRoom *pNPCInRoom = NULL;
        for ( pNPCInRoom = pRoom->GetNPCsInRoom()->Head(); pNPCInRoom; pNPCInRoom = pRoom->GetNPCsInRoom()->Next( pNPCInRoom ) )
        {
                if ( strncasecmp( sTargetName.c_str(), pNPCInRoom->GetName().c_str(), sTargetName.length() ) == 0 )
                {
                        bFound = true;
                        break;
                }
        }

        if ( bFound )
        {
		CStreamData oStream;

		uint8_t targetType = NWC_ATTACK_NPC;
		uint32_t targetID = pNPCInRoom->GetID();

		oStream.Write( &targetType, sizeof(targetType) );
		oStream.Write( &targetID, sizeof(targetID) );

		// Run script request
		NWCRunScriptRequest( pCur, SCRIPT_EXPLOIT_ID, oStream );
                
		return true;
        }

        sprintf( szTemp, "%sExploit what?\n", FC_NORMAL_RED );
	pCur->WriteOutput( szTemp );

	return (true);
}

bool DoScriptImplant( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[450];

	// A player can steal HP and MEMORY from a target
	if ( !pCur )
		return (false);
       
	CPlayer *pPlayer = pCur->GetPlayer();
 
	if ( !pPlayer )
                return false;

        if ( !pPlayer->GetRoom() )
                return false;

        if ( sArgs.size() < 1 )
        {
		// Currently attack the primary target
		if ( !pPlayer->IsFighting() )
		{
			sprintf( szTemp, "%sRun implant script on what?\n", FC_NORMAL_RED );
			pCur->WriteOutput( szTemp );
			return (true);
		}	
                
		// Request to attack player
		CStreamData oStream;

		uint8_t targetType = pPlayer->GetPrimaryTarget()->GetTargetType();
		uint32_t targetID = pPlayer->GetPrimaryTarget()->GetTargetID();

		oStream.Write( &targetType, sizeof(targetType) );
		oStream.Write( &targetID, sizeof(targetID) );

		// Run script request
		NWCRunScriptRequest( pCur, SCRIPT_IMPLANT_ID, oStream );
                
		return true;
        }

	CRoom *pRoom;

	if ( !(pRoom = pPlayer->GetRoom()) )
		return (false);

        // Get by item name
        std::ostringstream result_stream;
        ostream_iterator< string > oit( result_stream, " " );
        copy( sArgs.begin(), sArgs.end(), oit );

        string sTargetName = result_stream.str();
        sTargetName = rtrim( sTargetName );

	bool bFound = false;
	CPlayerInRoom *pPlayerInRoom = NULL;
        for ( pPlayerInRoom = pRoom->GetPlayersInRoom()->Head(); pPlayerInRoom; pPlayerInRoom = pRoom->GetPlayersInRoom()->Next( pPlayerInRoom ) )
        {
                if ( strncasecmp( sTargetName.c_str(), pPlayerInRoom->GetName().c_str(), sTargetName.length() ) == 0 )
                {
                        bFound = true;
                        break;
                }
        }

        if ( bFound )
        {
                // Attacking player
                if ( !pRoom->IsPKAllowed() )
                {
                        sprintf( szTemp, "%sYou can't implant another player in a non PK area.\n", FC_NORMAL_RED );
			pCur->WriteOutput( szTemp );
                        return true;
                }

                // Request to attack player
		CStreamData oStream;

		uint8_t targetType = NWC_ATTACK_PLAYER;
		uint32_t targetID = pPlayerInRoom->GetID();

		oStream.Write( &targetType, sizeof(targetType) );
		oStream.Write( &targetID, sizeof(targetID) );

		// Run script request
		NWCRunScriptRequest( pCur, SCRIPT_IMPLANT_ID, oStream );
                
		return true;
        }

        CNPCInRoom *pNPCInRoom = NULL;
        for ( pNPCInRoom = pRoom->GetNPCsInRoom()->Head(); pNPCInRoom; pNPCInRoom = pRoom->GetNPCsInRoom()->Next( pNPCInRoom ) )
        {
                if ( strncasecmp( sTargetName.c_str(), pNPCInRoom->GetName().c_str(), sTargetName.length() ) == 0 )
                {
                        bFound = true;
                        break;
                }
        }

        if ( bFound )
        {
		CStreamData oStream;

		uint8_t targetType = NWC_ATTACK_NPC;
		uint32_t targetID = pNPCInRoom->GetID();

		oStream.Write( &targetType, sizeof(targetType) );
		oStream.Write( &targetID, sizeof(targetID) );

		// Run script request
		NWCRunScriptRequest( pCur, SCRIPT_IMPLANT_ID, oStream );
                
		return true;
        }

        sprintf( szTemp, "%sImplant what?\n", FC_NORMAL_RED );
	pCur->WriteOutput( szTemp );

	return (true);
}
