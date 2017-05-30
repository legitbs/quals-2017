#include "common.h"

#define SCRIPT_CORRUPT_BASE_DAMAGE		(21)
#define SCRIPT_CORRUPT_DAMAGE_MULTIPLIER	(1.05)

#define SCRIPT_REPAIR_BASE_HP			(13)
#define SCRIPT_REPAIR_HP_MULTIPLIER		(1.07)

#define SCRIPT_DDOS_BASE_CPU			(23)
#define SCRIPT_DDOS_CPU_MULTIPLIER_BY_SCRIPT	(1.06)
#define SCRIPT_DDOS_CPU_MULTIPLIER_BY_CODE	(1.03)

#define SCRIPT_SPRAY_BASE_MEMORY		(25)
#define SCRIPT_SPRAY_MEMORY_MULTIPLIER_BY_SCRIPT	(1.03)
#define SCRIPT_SPRAY_MEMORY_MULTIPLIER_BY_CODE		(1.06)

#define SCRIPT_BOOST_BASE_CPU			(10)
#define SCRIPT_BOOST_CPU_MULTIPLIER_BY_SCRIPT	(1.05)
#define SCRIPT_BOOST_CPU_MULTIPLIER_BY_CODE	(1.04)

#define SCRIPT_RESTORE_BASE_HP			(25)
#define SCRIPT_RESTORE_HP_MULTIPLIER		(1.07)

#define SCRIPT_EXPLOIT_BASE_DAMAGE			(43)
#define SCRIPT_EXPLOIT_DAMAGE_MULTIPLIER_FOR_HACK	(1.03)
#define SCRIPT_EXPLOIT_DAMAGE_MULTIPLIER_FOR_CODE	(1.03)

#define SCRIPT_IMPLANT_BASE_DAMAGE_MIN			(53)
#define SCRIPT_IMPLANT_BASE_DAMAGE_MAX			(57)
#define SCRIPT_IMPLANT_DAMAGE_MULTIPLIER_FOR_HACK	(1.03)
#define SCRIPT_IMPLANT_DAMAGE_MULTIPLIER_FOR_CODE	(1.03)

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
{ "delete",     SCRIPT_DELETE_ID,       1,      50,     DoScriptDelete },
{ "corrupt",    SCRIPT_CORRUPT_ID,      1,      30,     DoScriptCorrupt },
{ "repair",     SCRIPT_REPAIR_ID,       3,      50,     DoScriptRepair },
{ "ddos",       SCRIPT_DDOS_ID,         5,      70,     DoScriptDDOS },
{ "spray",      SCRIPT_SPRAY_ID,        7,      100,    DoScriptSpray },
{ "boost",      SCRIPT_BOOST_ID,        6,      100,    DoScriptBoost },
{ "restore",    SCRIPT_RESTORE_ID,      9,      100,    DoScriptRestore },
{ "exploit",    SCRIPT_EXPLOIT_ID,      8,      130,    DoScriptExploit },
{ "implant",    SCRIPT_IMPLANT_ID,      13,     170,    DoScriptImplant },
{ "",           SCRIPT_END_ID,          0,      NULL }  // Table end
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

uint16_t GetCorruptDamage( uint16_t attackerScriptLevel, uint16_t defenderScriptLevel )
{
	double multiplier = SCRIPT_CORRUPT_DAMAGE_MULTIPLIER;

	if ( attackerScriptLevel > defenderScriptLevel )
		multiplier = pow( multiplier, (attackerScriptLevel - defenderScriptLevel) );
	else if ( defenderScriptLevel < attackerScriptLevel )
		multiplier = (1.0 / pow( multiplier, (defenderScriptLevel - attackerScriptLevel) ) );
	else
		multiplier = 1.0;

	return ((SCRIPT_CORRUPT_BASE_DAMAGE/2) + ((SCRIPT_CORRUPT_BASE_DAMAGE/2) * multiplier));
}

uint16_t GetExploitDamage( uint16_t attackerHackLevel, uint16_t defenderHackLevel, uint16_t attackerCodeLevel, uint16_t defenderCodeLevel )
{
	double multiplier1 = SCRIPT_EXPLOIT_DAMAGE_MULTIPLIER_FOR_HACK;
	double multiplier2 = SCRIPT_EXPLOIT_DAMAGE_MULTIPLIER_FOR_CODE;

	if ( attackerHackLevel > defenderHackLevel )
		multiplier1 = pow( multiplier1, (attackerHackLevel - defenderHackLevel) );
	else if ( defenderHackLevel < attackerHackLevel )
		multiplier1 = (1.0 / pow( multiplier1, (defenderHackLevel - attackerHackLevel) ) );
	else
		multiplier1 = 1.0;
	
	if ( attackerCodeLevel > defenderCodeLevel )
		multiplier2 = pow( multiplier2, (attackerCodeLevel - defenderCodeLevel) );
	else if ( defenderCodeLevel < attackerCodeLevel )
		multiplier2 = (1.0 / pow( multiplier2, (defenderCodeLevel - attackerCodeLevel) ) );
	else
		multiplier2 = 1.0;

	return (SCRIPT_EXPLOIT_BASE_DAMAGE * multiplier1 * multiplier2);
}

uint16_t GetImplantDamage( uint16_t attackerHackLevel, uint16_t defenderHackLevel, uint16_t attackerCodeLevel, uint16_t defenderCodeLevel )
{
	double multiplier1 = SCRIPT_IMPLANT_DAMAGE_MULTIPLIER_FOR_HACK;
	double multiplier2 = SCRIPT_IMPLANT_DAMAGE_MULTIPLIER_FOR_CODE;

	if ( attackerHackLevel > defenderHackLevel )
		multiplier1 = pow( multiplier1, (attackerHackLevel - defenderHackLevel) );
	else if ( defenderHackLevel < attackerHackLevel )
		multiplier1 = (1.0 / pow( multiplier1, (defenderHackLevel - attackerHackLevel) ) );
	else
		multiplier1 = 1.0;
	
	if ( attackerCodeLevel > defenderCodeLevel )
		multiplier2 = pow( multiplier2, (attackerCodeLevel - defenderCodeLevel) );
	else if ( defenderCodeLevel < attackerCodeLevel )
		multiplier2 = (1.0 / pow( multiplier2, (defenderCodeLevel - attackerCodeLevel) ) );
	else
		multiplier2 = 1.0;

	return (g_oRNG.GetRange( SCRIPT_IMPLANT_BASE_DAMAGE_MIN, SCRIPT_IMPLANT_BASE_DAMAGE_MAX ) * multiplier1 * multiplier2);
}

uint16_t GetRepairAmount( uint16_t userScriptLevel )
{
	double multiplier = SCRIPT_REPAIR_HP_MULTIPLIER;

	multiplier = pow( multiplier, userScriptLevel );

	return (SCRIPT_REPAIR_BASE_HP * multiplier);
}

uint16_t GetRestoreAmount( uint16_t userScriptLevel )
{
	double multiplier = SCRIPT_RESTORE_HP_MULTIPLIER;
	
	multiplier = pow( multiplier, userScriptLevel );

	return (SCRIPT_RESTORE_BASE_HP * multiplier);
}

uint16_t GetDDOSAmount( uint16_t attackerScriptLevel, uint16_t defenderScriptLevel, uint16_t attackerCodeLevel, uint16_t defenderCodeLevel )
{
	double multiplier1 = SCRIPT_DDOS_CPU_MULTIPLIER_BY_SCRIPT;
	double multiplier2 = SCRIPT_DDOS_CPU_MULTIPLIER_BY_CODE;

	if ( attackerScriptLevel > defenderScriptLevel )
		multiplier1 = pow( multiplier1, (attackerScriptLevel - defenderScriptLevel) );
	else if ( defenderScriptLevel < attackerScriptLevel )
		multiplier1 = (1.0 / pow( multiplier1, (defenderScriptLevel - attackerScriptLevel) ) );
	else
		multiplier1 = 1.0;

	if ( attackerCodeLevel > defenderCodeLevel )
		multiplier2 = pow( multiplier2, (attackerCodeLevel - defenderCodeLevel) );
	else if ( defenderScriptLevel < attackerScriptLevel )
		multiplier2 = (1.0 / pow( multiplier2, (defenderCodeLevel - attackerCodeLevel) ) );
	else
		multiplier2 = 1.0;

	return (SCRIPT_DDOS_BASE_CPU * multiplier1 * multiplier2);
}

uint16_t GetSprayAmount( uint16_t attackerScriptLevel, uint16_t defenderScriptLevel, uint16_t attackerCodeLevel, uint16_t defenderCodeLevel )
{
	double multiplier1 = SCRIPT_SPRAY_MEMORY_MULTIPLIER_BY_SCRIPT;
	double multiplier2 = SCRIPT_SPRAY_MEMORY_MULTIPLIER_BY_CODE;

	if ( attackerScriptLevel > defenderScriptLevel )
		multiplier1 = pow( multiplier1, (attackerScriptLevel - defenderScriptLevel) );
	else if ( defenderScriptLevel < attackerScriptLevel )
		multiplier1 = (1.0 / pow( multiplier1, (defenderScriptLevel - attackerScriptLevel) ) );
	else
		multiplier1 = 1.0;

	if ( attackerCodeLevel > defenderCodeLevel )
		multiplier2 = pow( multiplier2, (attackerCodeLevel - defenderCodeLevel) );
	else if ( defenderScriptLevel < attackerScriptLevel )
		multiplier2 = (1.0 / pow( multiplier2, (defenderCodeLevel - attackerCodeLevel) ) );
	else
		multiplier2 = 1.0;

	return (SCRIPT_SPRAY_BASE_MEMORY * multiplier1 * multiplier2);
}

uint16_t GetBoostAmount( uint16_t userScriptLevel, uint16_t userCodeLevel )
{
	double multiplier1 = SCRIPT_BOOST_CPU_MULTIPLIER_BY_SCRIPT;
	double multiplier2 = SCRIPT_BOOST_CPU_MULTIPLIER_BY_CODE;

	multiplier1 = pow( multiplier1, userScriptLevel );

	multiplier2 = pow( multiplier2, userCodeLevel );

	return (SCRIPT_BOOST_BASE_CPU * multiplier1 * multiplier2);
}


string GetDDOSString( uint16_t cpuTakenAmount, uint16_t maxCPU )
{
	string sCPUString;
        if ( maxCPU == 0 )
        {
                sCPUString = "";
                return sCPUString;
        }

        // Damage as a percentage of HP
        uint16_t percentage = (uint16_t)(((double)cpuTakenAmount / (double)maxCPU) * 100.0);

        if ( percentage == 0 )
                sCPUString = "nothing";
        else if ( percentage <= 5 )
                sCPUString = "extremely little";
        else if ( percentage <= 10 )
                sCPUString = "very little";
        else if ( percentage <= 15 )
                sCPUString = "a small amount";
        else if ( percentage <= 20 )
                sCPUString = "nearly a quarter";
        else if ( percentage <= 25 )
                sCPUString = "a quarter";
        else if ( percentage <= 35 )
                sCPUString = "a not insignificant";
        else if ( percentage <= 50 )
                sCPUString = "almost half";
        else if ( percentage <= 75 )
                sCPUString = "nearly all of";
        else if ( percentage <= 99 )
                sCPUString = "a significant portion";
        else if ( percentage >= 100 )
                sCPUString = "the entire";
        else
                sCPUString = "";

        return sCPUString;
}

bool RunScript( CServerConnection *pCur, uint8_t scriptID, uint8_t *pData, uint32_t dataLen )
{
	if ( !pCur )
		return (false);

	CPlayer *pPlayer = pCur->GetPlayer();

	if ( !pPlayer )
		return (false);

	for ( uint8_t i = 0; ; i++ )
	{
		if ( g_runScripts[i].scriptID == scriptID )
		{
			if ( pPlayer->GetScriptLevel() < g_runScripts[i].scriptLevel )
			{
				string sFailMessage = "Insufficient script level to run that script";
				NWCPlayerInfoMessage( pPlayer, sFailMessage );
		
				return (true);
			}

			uint16_t cpuNeeded = GetCPUForScript( pPlayer->GetCodeLevel(), pPlayer->GetMaxCPU() );

			if ( scriptID == SCRIPT_BOOST_ID )
				cpuNeeded = 0; 

                        if ( pPlayer->GetCurCPU() < cpuNeeded )
                        {
				string sFailMessage = "Insufficient CPU to run script";
				NWCPlayerInfoMessage( pPlayer, sFailMessage );

                                return (true);
                        }

                        uint16_t memoryRequired = GetMemoryForScript( pPlayer->GetScriptLevel(), g_runScripts[i].scriptLevel, g_runScripts[i].scriptMemoryNeeded );

                        if ( pPlayer->GetCurMemory() < memoryRequired )
                        {
				string sFailMessage = "Insufficient Memory to run script";
				NWCPlayerInfoMessage( pPlayer, sFailMessage );

                                return (true);
                        }

			// Adjust CPU and memory
			pPlayer->SetCPU( pPlayer->GetCurCPU() - cpuNeeded );
			pPlayer->SetMemory( pPlayer->GetCurMemory() - memoryRequired );

			// Send run script message
			char szTemp[1024];
			snprintf( szTemp, 1024, "You run a %s script", g_runScripts[i].szScriptName );
			string sRunMessage = szTemp;

			NWCPlayerRunsScript( pCur, g_runScripts[i].scriptID, cpuNeeded, memoryRequired, sRunMessage );

			// For everyone in room ... inform them that player ran a script
			if ( pPlayer->GetRoom() )
			{
				snprintf( szTemp, 1024, "%s runs a %s script", pPlayer->GetName().c_str(), g_runScripts[i].szScriptName );
				string sRunMessage = szTemp;
				
				CRoom *pRoom = pPlayer->GetRoom();
				for ( CPlayer *pPlayerInRoom = pRoom->GetPlayersInRoom()->Head(); pPlayerInRoom; pPlayerInRoom = pRoom->GetPlayersInRoom()->Next( pPlayerInRoom ) )
				{
					if ( pPlayerInRoom->IsConnected() && pPlayerInRoom != pPlayer )
						NWCPlayerInfoMessage( pPlayerInRoom, sRunMessage );
				}
			}
			
			return (*g_runScripts[i].pRunFunc)( pCur, pData, dataLen );
		}
	}

	return (false);
}

bool DoScriptDelete( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	// Allows a player to delete an item in the room
	// Get an item
        if ( !pCur || !pCur->GetPlayer() )
                return (false);

        if ( dataLen != 4 )
                return (false);

        CPlayer *pPlayer = pCur->GetPlayer();

        uint32_t itemID = *((uint32_t*)(pData));

        CRoom *pRoom = pCur->GetPlayer()->GetRoom();

        if ( !pRoom )
                return (false);

        // Find item in room
        CItemInstance *pMatchItem = pRoom->GetItemInRoomByID( itemID );

        if ( pMatchItem == NULL )
        {
                // Someone might have already taken this item!
                string sFailReason = "That item is no longer in the room";
                NWCPlayerInfoMessage( pCur->GetPlayer(), sFailReason );

                return (true);
        }

        // Prevent fixed items from moving
        if ( pMatchItem->HasFlagNoDestroy() )
        {
                string sFailReason = "That item cannot be deleted";
                NWCPlayerInfoMessage( pCur->GetPlayer(), sFailReason );

                return (true);
        }

        // Delete this item
        pMatchItem->m_roomLink.Unlink();

        // Inform player that he got the item (send item data as well)
        NWCPlayerDeletesItem( pCur, pPlayer, pMatchItem );

        // Inform other players in room -- of player getting item       
        NUtil::LIST_PTR( CPlayer ) pPlayersInRoomList = pRoom->GetPlayersInRoom();

        // Now say to everyone in the room...
        for ( CPlayer *pCurPlayerInRoom = pPlayersInRoomList->Head(); pCurPlayerInRoom; pCurPlayerInRoom = pPlayersInRoomList->Next( pCurPlayerInRoom ) )
        {
                // Send to other players in room
                if ( pCurPlayerInRoom->IsConnected() && pCurPlayerInRoom != pPlayer )
                        NWCPlayerDeletesItem( pCurPlayerInRoom->GetConnection(), pPlayer, pMatchItem );
        }

        // Delete the item from the realm       
        printf( "%s deletes item [%d] %s\n", pPlayer->GetName().c_str(), pMatchItem->GetItemID(), pMatchItem->GetName().c_str() );

        // Remove item from the world
        delete pMatchItem;

        return (true);
}

bool DoScriptCorrupt( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	// A player can attack and damage something with this
        if ( !pCur || !pCur->GetPlayer() )
                return (false);

        if ( dataLen != 5 )
                return (false);

        CPlayer *pPlayer = pCur->GetPlayer();

	uint8_t targetType = pData[0];
        uint32_t targetID = *((uint32_t*)(pData+1));

        CRoom *pRoom = pCur->GetPlayer()->GetRoom();

        if ( !pRoom )
                return (false);

	bool bStartAttack = false;	
	if ( targetType != NWC_ATTACK_PLAYER && targetType != NWC_ATTACK_NPC )
	{
		if ( !pPlayer->IsFighting() )
		{
			string sFailMessage = "Run corrupt on what?";

			NWCPlayerInfoMessage( pPlayer, sFailMessage );
			return (true);
		}

		// Run corrupt on primary target!
		targetID = pPlayer->GetPrimaryTarget()->GetTargetID();
		targetType = pPlayer->GetPrimaryTarget()->GetTargetType();
	}

        // Find target in room
	if ( targetType == NWC_ATTACK_PLAYER )
	{
		if ( !pRoom->IsPKAllowed() )
		{
			string sFailMessage = "PK is not allowed in this room!";

			NWCPlayerInfoMessage( pPlayer, sFailMessage );
			return (true);
		}

		CPlayer *pTarget = pRoom->GetPlayerInRoomByID( targetID ); 

                if ( !pTarget )
                {
                        string sFailMessage = "They are no longer here to corrupt";
                        NWCPlayerInfoMessage( pCur->GetPlayer(), sFailMessage );
                        return (true);
                }

                if ( pTarget == pPlayer )
                {
                        string sFailMessage = "You can't corrupt yourself";
                        NWCPlayerInfoMessage( pCur->GetPlayer(), sFailMessage );
                        return (true);
                }

		// Attack them
		if ( !pPlayer->IsFightingTarget( pTarget->GetPlayerID(), NWC_ATTACK_PLAYER ) )
			bStartAttack = true;

		if ( !pPlayer->AttackPlayer( pTarget->GetPlayerID() ) )
			return (false);

		if ( bStartAttack )
		{
			// Inform this player they are attacking the target...
			NWCPlayerAttackResponse( pCur, pTarget->GetPlayerID(), NWC_ATTACK_PLAYER );

			// Inform the target player they are now attacking this player!
			if ( !pTarget->IsFightingTarget( pPlayer->GetPlayerID(), NWC_ATTACK_PLAYER ) )
				NWCPlayerAttackResponse( pTarget->GetConnection(), pPlayer->GetPlayerID(), NWC_ATTACK_PLAYER );
		}

		// Have the player attack them back
		pTarget->AttackPlayer( pPlayer->GetPlayerID() );

		// Damage target!!
		uint16_t corruptDamage = GetCorruptDamage( pPlayer->GetScriptLevel(), pTarget->GetScriptLevel() );

		string sDamageSource = "corrupt script";
		string sDamageString = GetDamageString( corruptDamage, pTarget->GetMaxHP() );

		// Inform attacker they have damaged a target!
		NWCPlayerDamagePlayer( pPlayer, pTarget, corruptDamage, sDamageString, sDamageSource );
	
		if ( corruptDamage >= pTarget->GetCurHP() )
        	{
                	uint8_t targetHackLevel = pTarget->GetHackLevel();
                	uint8_t targetCodeLevel = pTarget->GetCodeLevel();
                	uint8_t targetScriptLevel = pTarget->GetScriptLevel();

			// Stop fighting target
			pPlayer->StopFightingTarget( pTarget->GetPlayerID(), CAttackTarget::TARGET_PLAYER );

                	// Kill em
                	pTarget->Kill( pPlayer->GetPlayerID(), pPlayer->GetName(), KILLER_TYPE_PLAYER );

                	// Level the attacker (potentially)
                	DoLevelPlayer( pPlayer, targetHackLevel, targetCodeLevel, targetScriptLevel, NWC_ATTACK_PLAYER );

                	// Returning true -- all done
                	return (true);
        	}

        	// Modify targets hp
        	pTarget->SetHP( pTarget->GetCurHP() - corruptDamage );

        	// Inform target they have been damaged
        	NWCPlayerTakesDamage( pTarget, pPlayer->GetPlayerID(), DAMAGE_TYPE_PLAYER, corruptDamage, sDamageString, sDamageSource );

        	if ( pRoom )
        	{
                	// Print damage message
                	char szTemp[1024];

                	snprintf( szTemp, 1024, "%s's %s %s %s", pPlayer->GetName().c_str(), sDamageSource.c_str(), sDamageString.c_str(), pTarget->GetName().c_str() );
                	string sMessage = szTemp;

			for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
                	{
                        	if ( pCur->IsConnected() && pCur != pPlayer && pCur != pTarget )
                                	NWCPlayerInfoMessage( pCur, sMessage );
                	}
		}

		return (true);	
	}
	else if ( targetType == NWC_ATTACK_NPC )
	{
		CNPC *pTarget = pRoom->GetNPCInRoomByID( targetID );

                if ( !pTarget )
                {
                        string sFailMessage = "They are no longer here to attack";
                        NWCPlayerInfoMessage( pPlayer, sFailMessage );
                        return (true);
                }

		// Inform this player they are attacking the target...
		if ( !pPlayer->IsFightingTarget( pTarget->GetNPCID(), NWC_ATTACK_NPC ) )
			NWCPlayerAttackResponse( pCur, pTarget->GetNPCID(), NWC_ATTACK_NPC );
                
                // Attack!!
		if ( !pPlayer->AttackNPC( pTarget->GetNPCID() ) )
                        return (false);

                // Have the target -- attack them back
                pTarget->AttackPlayer( pPlayer->GetPlayerID() );
	
		// Damage target!!
                uint16_t corruptDamage = GetCorruptDamage( pPlayer->GetScriptLevel(), pTarget->GetScriptLevel() );

		// Amplify damage for players attacking NPC's
		corruptDamage = corruptDamage + (corruptDamage  / 10);

                string sDamageSource = "corrupt script";
                string sDamageString = GetDamageString( corruptDamage, pTarget->GetMaxHP() );

                // Inform attacker they have damaged a target!
                NWCPlayerDamageNPC( pPlayer, pTarget, corruptDamage, sDamageString, sDamageSource );

		if ( corruptDamage >= pTarget->GetCurHP() )
                {
                        uint8_t targetHackLevel = pTarget->GetHackLevel();
                        uint8_t targetCodeLevel = pTarget->GetCodeLevel();
                        uint8_t targetScriptLevel = pTarget->GetScriptLevel();

			// Stop fighting target
			pPlayer->StopFightingTarget( pTarget->GetNPCID(), CAttackTarget::TARGET_NPC );
                        
			// Kill em
                        pTarget->Kill( pPlayer->GetPlayerID(), pPlayer->GetName(), KILLER_TYPE_PLAYER );

                        // Level the attacker (potentially)
                        DoLevelPlayer( pPlayer, targetHackLevel, targetCodeLevel, targetScriptLevel, NWC_ATTACK_PLAYER );
                        // Returning true -- all done
                        return (true);
                }

		// Modify targets hp
                pTarget->SetHP( pTarget->GetCurHP() - corruptDamage );

                if ( pRoom )
                {
                        // Print damage message
                        char szTemp[1024];

                        snprintf( szTemp, 1024, "%s's %s %s %s", pPlayer->GetName().c_str(), sDamageSource.c_str(), sDamageString.c_str(), pTarget->GetName().c_str() );
                        string sMessage = szTemp;

                        for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
                        {
                                if ( pCur->IsConnected() && pCur != pPlayer )
                                        NWCPlayerInfoMessage( pCur, sMessage );
                        }
                }

                return (true);	
	}

	return (true);
}

bool DoScriptRepair( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	// A player can restore HP of the target
        if ( !pCur || !pCur->GetPlayer() )
                return (false);

        if ( dataLen != 0 )
                return (false);

        CPlayer *pPlayer = pCur->GetPlayer();

	uint16_t repairAmount = GetRepairAmount( pPlayer->GetScriptLevel() );
	
	uint16_t newHP = pPlayer->GetCurHP() + repairAmount;

	if ( newHP > pPlayer->GetMaxHP() )
		newHP = pPlayer->GetMaxHP();

	// Update hit points
	pPlayer->SetHP( newHP );

	// Use stat update to gain health
	NWCPlayerStatUpdate( pPlayer );
                
	char szTemp[1024];
	snprintf( szTemp, 1024, "You repair yourself" );
	string sMessage = szTemp;

	NWCPlayerInfoMessage( pPlayer, sMessage );
        
	CRoom *pRoom = pPlayer->GetRoom();        
	if ( pRoom )
        {
        	// Print damage message

                snprintf( szTemp, 1024, "%s repairs themself", pPlayer->GetName().c_str() );
                string sMessage = szTemp;

                for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
                {
                	if ( pCur->IsConnected() && pCur != pPlayer )
                        	NWCPlayerInfoMessage( pCur, sMessage );
                }
	}

	return (true);
}

bool DoScriptDDOS( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	// A player can cause the CPU of a player to drop
        if ( !pCur || !pCur->GetPlayer() )
                return (false);

        if ( dataLen != 0 )
                return (false);

        CPlayer *pPlayer = pCur->GetPlayer();

	// Get a target...
	if ( !pPlayer->IsFighting() )
	{
		string sInfoMessage = "You must be attacking something to DDOS it";
		NWCPlayerInfoMessage( pPlayer, sInfoMessage );

		return (true);
	}

	CRoom *pRoom = pPlayer->GetRoom();

	if ( !pRoom )
		return (false);
		
	// Run corrupt on primary target!
	uint32_t targetID = pPlayer->GetPrimaryTarget()->GetTargetID();
	uint8_t targetType = pPlayer->GetPrimaryTarget()->GetTargetType();

        // Find target in room
	if ( targetType == NWC_ATTACK_PLAYER )
	{
		if ( !pRoom->IsPKAllowed() )
		{
			string sFailMessage = "PK is not allowed in this room!";

			NWCPlayerInfoMessage( pPlayer, sFailMessage );
			return (true);
		}

		CPlayer *pTarget = pRoom->GetPlayerInRoomByID( targetID ); 
                
		if ( !pTarget )
                {
                        string sFailMessage = "They are no longer here to DDOS";
                        NWCPlayerInfoMessage( pPlayer, sFailMessage );
                        return (true);
                }


                if ( pTarget == pPlayer )
                {
                        string sFailMessage = "You can't DDOS yourself";
                        NWCPlayerInfoMessage( pPlayer, sFailMessage );
                        return (true);
                }

		// Get DDOS amount
		uint16_t ddosAmount = GetDDOSAmount( pPlayer->GetScriptLevel(), pTarget->GetScriptLevel(), pPlayer->GetCodeLevel(), pTarget->GetCodeLevel() );

		uint16_t newCPU;
		if ( pTarget->GetCurCPU() < ddosAmount )
		{
			ddosAmount = pTarget->GetCurCPU();
			newCPU = 0;
		}
		else 
			newCPU = pTarget->GetCurCPU() - ddosAmount;

		pTarget->SetCPU( newCPU );

		// Get the display string
		string sTakenString = GetDDOSString( ddosAmount, pTarget->GetMaxCPU() );

		// Send stat update
		NWCPlayerStatUpdate( pTarget );	

		// Inform players
		char szTemp[1024];
		snprintf( szTemp, 1024, "Your DDOS script consumes %s of their CPU cycles", sTakenString.c_str() );
		string sInfoMessage = szTemp;

		NWCPlayerInfoMessage( pPlayer, sInfoMessage );

		snprintf( szTemp, 1024, "%s's DDOS script consumes %s of your CPU cycles!", pPlayer->GetName().c_str(), sTakenString.c_str() );
		sInfoMessage = szTemp;

		NWCPlayerInfoMessage( pTarget, sInfoMessage );

		// All players in room
		snprintf( szTemp, 1024, "%s's DDOS script consumes %s of %s's CPU cycles", pPlayer->GetName().c_str(), pTarget->GetName().c_str(), sTakenString.c_str() );
		sInfoMessage = szTemp;

		for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
                {
                       	if ( pCur->IsConnected() && pCur != pPlayer && pCur != pTarget )
                               	NWCPlayerInfoMessage( pCur, sInfoMessage );
		}

		return (true);	
	} 
	else if ( targetType == NWC_ATTACK_NPC )
	{
		CNPC *pTarget = pRoom->GetNPCInRoomByID( targetID );

                if ( !pTarget )
                {
                        string sFailMessage = "They are no longer here to attack";
                        NWCPlayerInfoMessage( pPlayer, sFailMessage );
                        return (true);
                }

		// Get DDOS amount
		uint16_t ddosAmount = GetDDOSAmount( pPlayer->GetScriptLevel(), pTarget->GetScriptLevel(), pPlayer->GetCodeLevel(), pTarget->GetCodeLevel() );
		
		// Amplify damage for players attacking NPC's
		ddosAmount = ddosAmount + (ddosAmount  / 10);

		uint16_t newCPU;
		if ( pTarget->GetCurCPU() < ddosAmount )
		{
			ddosAmount = pTarget->GetCurCPU();
			newCPU = 0;
		}
		else 
			newCPU = pTarget->GetCurCPU() - ddosAmount;

		pTarget->SetCPU( newCPU );

		// Get the display string
		string sTakenString = GetDDOSString( ddosAmount, pTarget->GetMaxCPU() );

		// Inform players
		char szTemp[1024];
		snprintf( szTemp, 1024, "Your DDOS script consumes %s of their CPU cycles", sTakenString.c_str() );
		string sInfoMessage = szTemp;

		// Send to the casting player
		NWCPlayerInfoMessage( pPlayer, sInfoMessage );

		snprintf( szTemp, 1024, "%s's DDOS script consumes %s of %s's CPU cycles", pPlayer->GetName().c_str(), pTarget->GetName().c_str(), sTakenString.c_str() );
		sInfoMessage = szTemp;

		for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
                {
                       	if ( pCur->IsConnected() && pCur != pPlayer )
                               	NWCPlayerInfoMessage( pCur, sInfoMessage );
		}

		return (true);	
	}

	return (true);
}

bool DoScriptSpray( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	// A player can attack and consume a players memory
        if ( !pCur || !pCur->GetPlayer() )
                return (false);

        if ( dataLen != 0 )
                return (false);

        CPlayer *pPlayer = pCur->GetPlayer();

	// Get a target...
	if ( !pPlayer->IsFighting() )
	{
		string sInfoMessage = "You must be attacking something to heap spray it";
		NWCPlayerInfoMessage( pPlayer, sInfoMessage );

		return (true);
	}

	CRoom *pRoom = pPlayer->GetRoom();

	if ( !pRoom )
		return (false);
		
	// Run corrupt on primary target!
	uint32_t targetID = pPlayer->GetPrimaryTarget()->GetTargetID();
	uint8_t targetType = pPlayer->GetPrimaryTarget()->GetTargetType();

        // Find target in room
	if ( targetType == NWC_ATTACK_PLAYER )
	{
		if ( !pRoom->IsPKAllowed() )
		{
			string sFailMessage = "PK is not allowed in this room!";

			NWCPlayerInfoMessage( pPlayer, sFailMessage );
			return (true);
		}

		CPlayer *pTarget = pRoom->GetPlayerInRoomByID( targetID ); 
                
		if ( !pTarget )
                {
                        string sFailMessage = "They are no longer here to heap spray";
                        NWCPlayerInfoMessage( pPlayer, sFailMessage );
                        return (true);
                }


                if ( pTarget == pPlayer )
                {
                        string sFailMessage = "You can't heap spray yourself";
                        NWCPlayerInfoMessage( pPlayer, sFailMessage );
                        return (true);
                }

		// Get spray amount
		uint16_t sprayAmount = GetSprayAmount( pPlayer->GetScriptLevel(), pTarget->GetScriptLevel(), pPlayer->GetCodeLevel(), pTarget->GetCodeLevel() );

		uint16_t newMemory;
		if ( pTarget->GetCurMemory() < sprayAmount )
		{
			sprayAmount = pTarget->GetCurMemory();
			newMemory = 0;
		}
		else 
			newMemory = pTarget->GetCurMemory() - sprayAmount;

		pTarget->SetMemory( newMemory );

		// Get the display string
		string sTakenString = GetDDOSString( sprayAmount, pTarget->GetMaxMemory() );

		// Send stat update
		NWCPlayerStatUpdate( pTarget );	

		// Inform players
		char szTemp[1024];
		snprintf( szTemp, 1024, "Your heap spraying script consumes %s of their Memory", sTakenString.c_str() );
		string sInfoMessage = szTemp;

		NWCPlayerInfoMessage( pPlayer, sInfoMessage );

		snprintf( szTemp, 1024, "%s's heap spraying script consumes %s of your Memory!", pPlayer->GetName().c_str(), sTakenString.c_str() );
		sInfoMessage = szTemp;

		NWCPlayerInfoMessage( pTarget, sInfoMessage );

		// All players in room
		snprintf( szTemp, 1024, "%s's heap spray script consumes %s of %s's Memory", pPlayer->GetName().c_str(), pTarget->GetName().c_str(), sTakenString.c_str() );
		sInfoMessage = szTemp;

		for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
                {
                       	if ( pCur->IsConnected() && pCur != pPlayer && pCur != pTarget )
                               	NWCPlayerInfoMessage( pCur, sInfoMessage );
		}

		return (true);	
	} 
	else if ( targetType == NWC_ATTACK_NPC )
	{
		CNPC *pTarget = pRoom->GetNPCInRoomByID( targetID );

                if ( !pTarget )
                {
                        string sFailMessage = "They are no longer here to attack";
                        NWCPlayerInfoMessage( pPlayer, sFailMessage );
                        return (true);
                }

		// Get spray amount
		uint16_t sprayAmount = GetSprayAmount( pPlayer->GetScriptLevel(), pTarget->GetScriptLevel(), pPlayer->GetCodeLevel(), pTarget->GetCodeLevel() );
		
		// Amplify damage for players attacking NPC's
		sprayAmount = sprayAmount + (sprayAmount  / 10);

		uint16_t newMemory;
		if ( pTarget->GetCurMemory() < sprayAmount )
		{
			sprayAmount = pTarget->GetCurMemory();
			newMemory = 0;
		}
		else 
			newMemory = pTarget->GetCurMemory() - sprayAmount;

		pTarget->SetMemory( newMemory );

		// Get the display string
		string sTakenString = GetDDOSString( sprayAmount, pTarget->GetMaxMemory() );

		// Inform players
		char szTemp[1024];
		snprintf( szTemp, 1024, "Your heap spraying script consumes %s of their Memory", sTakenString.c_str() );
		string sInfoMessage = szTemp;

		// Send to the casting player
		NWCPlayerInfoMessage( pPlayer, sInfoMessage );

		snprintf( szTemp, 1024, "%s's heap spray script consumes %s of %s's Memory", pPlayer->GetName().c_str(), pTarget->GetName().c_str(), sTakenString.c_str() );
		sInfoMessage = szTemp;

		for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
                {
                       	if ( pCur->IsConnected() && pCur != pPlayer )
                               	NWCPlayerInfoMessage( pCur, sInfoMessage );
		}

		return (true);	
	}

	return (true);
}

bool DoScriptBoost( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	// A player can boost their own CPU
        if ( !pCur || !pCur->GetPlayer() )
                return (false);

        if ( dataLen != 0 )
                return (false);

        CPlayer *pPlayer = pCur->GetPlayer();

	uint16_t boostAmount = GetBoostAmount( pPlayer->GetScriptLevel(), pPlayer->GetCodeLevel() );

	uint16_t newCPU = pPlayer->GetCurCPU() + boostAmount;

	if ( newCPU > pPlayer->GetMaxCPU() )
		newCPU = pPlayer->GetMaxCPU();

	// Update hit points
	pPlayer->SetCPU( newCPU );

	// Use stat update to gain health
	NWCPlayerStatUpdate( pPlayer );
                
	char szTemp[1024];
	snprintf( szTemp, 1024, "You modify your algorithms consuming Memory for a CPU boost" );
	string sMessage = szTemp;

	NWCPlayerInfoMessage( pPlayer, sMessage );
        
	CRoom *pRoom = pPlayer->GetRoom();        
	if ( pRoom )
        {
        	// Print boost message
                snprintf( szTemp, 1024, "%s boosts themself", pPlayer->GetName().c_str() );
                string sMessage = szTemp;

                for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
                {
                	if ( pCur->IsConnected() && pCur != pPlayer )
                        	NWCPlayerInfoMessage( pCur, sMessage );
                }
	}

	return (true);
}

bool DoScriptRestore( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	// A player can restore HP faster
        if ( !pCur || !pCur->GetPlayer() )
                return (false);

        if ( dataLen != 0 )
                return (false);

        CPlayer *pPlayer = pCur->GetPlayer();

	uint16_t restoreAmount = GetRestoreAmount( pPlayer->GetScriptLevel() );

	printf( "RESTORE AMOUNT: %d\n", restoreAmount );

	uint16_t newHP = pPlayer->GetCurHP() + restoreAmount;

	if ( newHP > pPlayer->GetMaxHP() )
		newHP = pPlayer->GetMaxHP();

	// Update hit points
	pPlayer->SetHP( newHP );

	// Use stat update to gain health
	NWCPlayerStatUpdate( pPlayer );
                
	char szTemp[1024];
	snprintf( szTemp, 1024, "You restore yourself" );
	string sMessage = szTemp;

	NWCPlayerInfoMessage( pPlayer, sMessage );
        
	CRoom *pRoom = pPlayer->GetRoom();        
	if ( pRoom )
        {
        	// Print damage message

                snprintf( szTemp, 1024, "%s restores themself", pPlayer->GetName().c_str() );
                string sMessage = szTemp;

                for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
                {
                	if ( pCur->IsConnected() && pCur != pPlayer )
                        	NWCPlayerInfoMessage( pCur, sMessage );
                }
	}

	return (true);
}

bool DoScriptExploit( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	// A player deals significant damage to a target
        if ( !pCur || !pCur->GetPlayer() )
                return (false);

        if ( dataLen != 5 )
                return (false);

        CPlayer *pPlayer = pCur->GetPlayer();

	uint8_t targetType = pData[0];
        uint32_t targetID = *((uint32_t*)(pData+1));

        CRoom *pRoom = pCur->GetPlayer()->GetRoom();

        if ( !pRoom )
                return (false);

	bool bStartAttack = false;	
	if ( targetType != NWC_ATTACK_PLAYER && targetType != NWC_ATTACK_NPC )
	{
		if ( !pPlayer->IsFighting() )
		{
			string sFailMessage = "Run exploit on what?";

			NWCPlayerInfoMessage( pPlayer, sFailMessage );
			return (true);
		}

		// Run corrupt on primary target!
		targetID = pPlayer->GetPrimaryTarget()->GetTargetID();
		targetType = pPlayer->GetPrimaryTarget()->GetTargetType();
	}

        // Find target in room
	if ( targetType == NWC_ATTACK_PLAYER )
	{
		if ( !pRoom->IsPKAllowed() )
		{
			string sFailMessage = "PK is not allowed in this room!";

			NWCPlayerInfoMessage( pPlayer, sFailMessage );
			return (true);
		}

		CPlayer *pTarget = pRoom->GetPlayerInRoomByID( targetID ); 

                if ( !pTarget )
                {
                        string sFailMessage = "They are no longer here to exploit";
                        NWCPlayerInfoMessage( pCur->GetPlayer(), sFailMessage );
                        return (true);
                }

                if ( pTarget == pPlayer )
                {
                        string sFailMessage = "You can't exploit yourself";
                        NWCPlayerInfoMessage( pCur->GetPlayer(), sFailMessage );
                        return (true);
                }

		// Attack them
		if ( !pPlayer->IsFightingTarget( pTarget->GetPlayerID(), NWC_ATTACK_PLAYER ) )
			bStartAttack = true;

		if ( !pPlayer->AttackPlayer( pTarget->GetPlayerID() ) )
			return (false);

		if ( bStartAttack )
		{
			// Inform this player they are attacking the target...
			NWCPlayerAttackResponse( pCur, pTarget->GetPlayerID(), NWC_ATTACK_PLAYER );

			// Inform the target player they are now attacking this player!
			if ( !pTarget->IsFightingTarget( pPlayer->GetPlayerID(), NWC_ATTACK_PLAYER ) )
				NWCPlayerAttackResponse( pTarget->GetConnection(), pPlayer->GetPlayerID(), NWC_ATTACK_PLAYER );
		}

		// Have the player attack them back
		pTarget->AttackPlayer( pPlayer->GetPlayerID() );

		// Damage target!!
		uint16_t exploitDamage = GetExploitDamage( pPlayer->GetHackLevel(), pTarget->GetHackLevel(), pPlayer->GetCodeLevel(), pTarget->GetCodeLevel() );

		string sDamageSource = "exploit";
		string sDamageString = GetDamageString( exploitDamage, pTarget->GetMaxHP() );

		// Inform attacker they have damaged a target!
		NWCPlayerDamagePlayer( pPlayer, pTarget, exploitDamage, sDamageString, sDamageSource );
	
		if ( exploitDamage >= pTarget->GetCurHP() )
        	{
                	uint8_t targetHackLevel = pTarget->GetHackLevel();
                	uint8_t targetCodeLevel = pTarget->GetCodeLevel();
                	uint8_t targetScriptLevel = pTarget->GetScriptLevel();

			// Stop fighting target
			pPlayer->StopFightingTarget( pTarget->GetPlayerID(), CAttackTarget::TARGET_PLAYER );

                	// Kill em
                	pTarget->Kill( pPlayer->GetPlayerID(), pPlayer->GetName(), KILLER_TYPE_PLAYER );

                	// Level the attacker (potentially)
                	DoLevelPlayer( pPlayer, targetHackLevel, targetCodeLevel, targetScriptLevel, NWC_ATTACK_PLAYER );

                	// Returning true -- all done
                	return (true);
        	}

        	// Modify targets hp
        	pTarget->SetHP( pTarget->GetCurHP() - exploitDamage );

        	// Inform target they have been damaged
        	NWCPlayerTakesDamage( pTarget, pPlayer->GetPlayerID(), DAMAGE_TYPE_PLAYER, exploitDamage, sDamageString, sDamageSource );

        	if ( pRoom )
        	{
                	// Print damage message
                	char szTemp[1024];

                	snprintf( szTemp, 1024, "%s's %s %s %s", pPlayer->GetName().c_str(), sDamageSource.c_str(), sDamageString.c_str(), pTarget->GetName().c_str() );
                	string sMessage = szTemp;

			for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
                	{
                        	if ( pCur->IsConnected() && pCur != pPlayer && pCur != pTarget )
                                	NWCPlayerInfoMessage( pCur, sMessage );
                	}
		}

		return (true);	
	}
	else if ( targetType == NWC_ATTACK_NPC )
	{
		CNPC *pTarget = pRoom->GetNPCInRoomByID( targetID );

                if ( !pTarget )
                {
                        string sFailMessage = "They are no longer here to attack";
                        NWCPlayerInfoMessage( pPlayer, sFailMessage );
                        return (true);
                }

		// Inform this player they are attacking the target...
		if ( !pPlayer->IsFightingTarget( pTarget->GetNPCID(), NWC_ATTACK_NPC ) )
			NWCPlayerAttackResponse( pCur, pTarget->GetNPCID(), NWC_ATTACK_NPC );
                
                // Attack!!
		if ( !pPlayer->AttackNPC( pTarget->GetNPCID() ) )
                        return (false);

                // Have the target -- attack them back
                pTarget->AttackPlayer( pPlayer->GetPlayerID() );
	
		// Damage target!!
		uint16_t exploitDamage = GetExploitDamage( pPlayer->GetHackLevel(), pTarget->GetHackLevel(), pPlayer->GetCodeLevel(), pTarget->GetCodeLevel() );
		
		// Amplify damage for players attacking NPC's
		exploitDamage = exploitDamage + (exploitDamage  / 10);

                string sDamageSource = "exploit";
                string sDamageString = GetDamageString( exploitDamage, pTarget->GetMaxHP() );

                // Inform attacker they have damaged a target!
                NWCPlayerDamageNPC( pPlayer, pTarget, exploitDamage, sDamageString, sDamageSource );

		if ( exploitDamage >= pTarget->GetCurHP() )
                {
                        uint8_t targetHackLevel = pTarget->GetHackLevel();
                        uint8_t targetCodeLevel = pTarget->GetCodeLevel();
                        uint8_t targetScriptLevel = pTarget->GetScriptLevel();
			
			// Stop fighting target
			pPlayer->StopFightingTarget( pTarget->GetNPCID(), CAttackTarget::TARGET_NPC );

                        // Kill em
                        pTarget->Kill( pPlayer->GetPlayerID(), pPlayer->GetName(), KILLER_TYPE_PLAYER );

                        // Level the attacker (potentially)
                        DoLevelPlayer( pPlayer, targetHackLevel, targetCodeLevel, targetScriptLevel, NWC_ATTACK_PLAYER );
                        // Returning true -- all done
                        return (true);
                }

		// Modify targets hp
                pTarget->SetHP( pTarget->GetCurHP() - exploitDamage );

                if ( pRoom )
                {
                        // Print damage message
                        char szTemp[1024];

                        snprintf( szTemp, 1024, "%s's %s %s %s", pPlayer->GetName().c_str(), sDamageSource.c_str(), sDamageString.c_str(), pTarget->GetName().c_str() );
                        string sMessage = szTemp;

                        for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
                        {
                                if ( pCur->IsConnected() && pCur != pPlayer )
                                        NWCPlayerInfoMessage( pCur, sMessage );
                        }
                }

                return (true);	
	}

	return (true);
}

bool DoScriptImplant( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	// A player can steal HP, CPU and MEMORY from a target
        if ( !pCur || !pCur->GetPlayer() )
                return (false);

        if ( dataLen != 5 )
                return (false);

        CPlayer *pPlayer = pCur->GetPlayer();

	uint8_t targetType = pData[0];
        uint32_t targetID = *((uint32_t*)(pData+1));

        CRoom *pRoom = pCur->GetPlayer()->GetRoom();

        if ( !pRoom )
                return (false);

	bool bStartAttack = false;	
	if ( targetType != NWC_ATTACK_PLAYER && targetType != NWC_ATTACK_NPC )
	{
		if ( !pPlayer->IsFighting() )
		{
			string sFailMessage = "Run implant on what?";

			NWCPlayerInfoMessage( pPlayer, sFailMessage );
			return (true);
		}

		// Run corrupt on primary target!
		targetID = pPlayer->GetPrimaryTarget()->GetTargetID();
		targetType = pPlayer->GetPrimaryTarget()->GetTargetType();
	}

        // Find target in room
	if ( targetType == NWC_ATTACK_PLAYER )
	{
		if ( !pRoom->IsPKAllowed() )
		{
			string sFailMessage = "PK is not allowed in this room!";

			NWCPlayerInfoMessage( pPlayer, sFailMessage );
			return (true);
		}

		CPlayer *pTarget = pRoom->GetPlayerInRoomByID( targetID ); 

                if ( !pTarget )
                {
                        string sFailMessage = "They are no longer here to implant";
                        NWCPlayerInfoMessage( pCur->GetPlayer(), sFailMessage );
                        return (true);
                }

#ifdef ENABLE_IMPLANT_BUG

#else
                if ( pTarget == pPlayer )
                {
                        string sFailMessage = "You can't implant yourself";
                        NWCPlayerInfoMessage( pCur->GetPlayer(), sFailMessage );
                        return (true);
                }
#endif

		// Attack them
		if ( !pPlayer->IsFightingTarget( pTarget->GetPlayerID(), NWC_ATTACK_PLAYER ) )
			bStartAttack = true;

		if ( !pPlayer->AttackPlayer( pTarget->GetPlayerID() ) )
			return (false);

		if ( bStartAttack )
		{
			// Inform this player they are attacking the target...
			NWCPlayerAttackResponse( pCur, pTarget->GetPlayerID(), NWC_ATTACK_PLAYER );

			// Inform the target player they are now attacking this player!
			if ( !pTarget->IsFightingTarget( pPlayer->GetPlayerID(), NWC_ATTACK_PLAYER ) )
				NWCPlayerAttackResponse( pTarget->GetConnection(), pPlayer->GetPlayerID(), NWC_ATTACK_PLAYER );
		}

		// Have the player attack them back
		pTarget->AttackPlayer( pPlayer->GetPlayerID() );

		// Damage target!!
		uint16_t implantDamage = GetImplantDamage( pPlayer->GetHackLevel(), pTarget->GetHackLevel(), pPlayer->GetCodeLevel(), pTarget->GetCodeLevel() );

		string sDamageSource = "implant";
		string sDamageString = GetDamageString( implantDamage, pTarget->GetMaxHP() );

		// Inform attacker they have damaged a target!
		NWCPlayerDamagePlayer( pPlayer, pTarget, implantDamage, sDamageString, sDamageSource );
	
		if ( implantDamage >= pTarget->GetCurHP() )
        	{
                	uint8_t targetHackLevel = pTarget->GetHackLevel();
                	uint8_t targetCodeLevel = pTarget->GetCodeLevel();
                	uint8_t targetScriptLevel = pTarget->GetScriptLevel();

			// Stop fighting target
			pPlayer->StopFightingTarget( pTarget->GetPlayerID(), CAttackTarget::TARGET_PLAYER );

                	// Kill em
                	pTarget->Kill( pPlayer->GetPlayerID(), pPlayer->GetName(), KILLER_TYPE_PLAYER );

                	// Level the attacker (potentially)
                	DoLevelPlayer( pPlayer, targetHackLevel, targetCodeLevel, targetScriptLevel, NWC_ATTACK_PLAYER );

                	// Returning true -- all done
                	return (true);
        	}

        	// Modify targets hp
        	pTarget->SetHP( pTarget->GetCurHP() - implantDamage );

        	// Inform target they have been damaged
        	NWCPlayerTakesDamage( pTarget, pPlayer->GetPlayerID(), DAMAGE_TYPE_PLAYER, implantDamage, sDamageString, sDamageSource );

        	if ( pRoom )
        	{
                	// Print damage message
                	char szTemp[1024];

                	snprintf( szTemp, 1024, "%s's %s %s %s", pPlayer->GetName().c_str(), sDamageSource.c_str(), sDamageString.c_str(), pTarget->GetName().c_str() );
                	string sMessage = szTemp;

			for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
                	{
                        	if ( pCur->IsConnected() && pCur != pPlayer && pCur != pTarget )
                                	NWCPlayerInfoMessage( pCur, sMessage );
                	}
		}

		return (true);	
	}
	else if ( targetType == NWC_ATTACK_NPC )
	{
		CNPC *pTarget = pRoom->GetNPCInRoomByID( targetID );

                if ( !pTarget )
                {
                        string sFailMessage = "They are no longer here to attack";
                        NWCPlayerInfoMessage( pPlayer, sFailMessage );
                        return (true);
                }

		// Inform this player they are attacking the target...
		if ( !pPlayer->IsFightingTarget( pTarget->GetNPCID(), NWC_ATTACK_NPC ) )
			NWCPlayerAttackResponse( pCur, pTarget->GetNPCID(), NWC_ATTACK_NPC );
                
                // Attack!!
		if ( !pPlayer->AttackNPC( pTarget->GetNPCID() ) )
                        return (false);

                // Have the target -- attack them back
                pTarget->AttackPlayer( pPlayer->GetPlayerID() );
	
		// Damage target!!
		uint16_t implantDamage = GetImplantDamage( pPlayer->GetHackLevel(), pTarget->GetHackLevel(), pPlayer->GetCodeLevel(), pTarget->GetCodeLevel() );
		
		// Amplify damage for players attacking NPC's
		implantDamage = implantDamage + (implantDamage  / 10);

                string sDamageSource = "implant";
                string sDamageString = GetDamageString( implantDamage, pTarget->GetMaxHP() );

                // Inform attacker they have damaged a target!
                NWCPlayerDamageNPC( pPlayer, pTarget, implantDamage, sDamageString, sDamageSource );

		if ( implantDamage >= pTarget->GetCurHP() )
                {
                        uint8_t targetHackLevel = pTarget->GetHackLevel();
                        uint8_t targetCodeLevel = pTarget->GetCodeLevel();
                        uint8_t targetScriptLevel = pTarget->GetScriptLevel();

			// Stop fighting target
			pPlayer->StopFightingTarget( pTarget->GetNPCID(), CAttackTarget::TARGET_NPC );
                        
			// Kill em
                        pTarget->Kill( pPlayer->GetPlayerID(), pPlayer->GetName(), KILLER_TYPE_PLAYER );

                        // Level the attacker (potentially)
                        DoLevelPlayer( pPlayer, targetHackLevel, targetCodeLevel, targetScriptLevel, NWC_ATTACK_PLAYER );
                        // Returning true -- all done
                        return (true);
                }

		// Modify targets hp
                pTarget->SetHP( pTarget->GetCurHP() - implantDamage );

                if ( pRoom )
                {
                        // Print damage message
                        char szTemp[1024];

                        snprintf( szTemp, 1024, "%s's %s %s %s", pPlayer->GetName().c_str(), sDamageSource.c_str(), sDamageString.c_str(), pTarget->GetName().c_str() );
                        string sMessage = szTemp;

                        for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
                        {
                                if ( pCur->IsConnected() && pCur != pPlayer )
                                        NWCPlayerInfoMessage( pCur, sMessage );
                        }
                }

                return (true);	
	}

	return (true);
}

bool NPCRunHealScript( CNPC *pNPC )
{
	if ( !pNPC )
		return (false);	// Didn't run

	// Run a heal script
	uint16_t cpuNeeded = GetCPUForScript( pNPC->GetCodeLevel(), pNPC->GetMaxCPU() );

	if ( pNPC->GetCurCPU() < cpuNeeded )
		return (false);	// Didn't run

	uint16_t memoryRequired = GetMemoryForScript( pNPC->GetScriptLevel(), g_runScripts[SCRIPT_REPAIR_ID].scriptLevel, g_runScripts[SCRIPT_REPAIR_ID].scriptMemoryNeeded );

	if ( pNPC->GetCurMemory() < memoryRequired )
		return (false); // Didn't run

	// Adjust CPU and memory
	pNPC->SetCPU( pNPC->GetCurCPU() - cpuNeeded );
	pNPC->SetMemory( pNPC->GetCurMemory() - memoryRequired );

	// For everyone in room ... inform them that player ran a script
	if ( pNPC->GetRoom() )
	{
		char szTemp[1024];
		snprintf( szTemp, 1024, "%s runs a %s script", pNPC->GetName().c_str(), g_runScripts[SCRIPT_REPAIR_ID].szScriptName );
		string sRunMessage = szTemp;
		
		CRoom *pRoom = pNPC->GetRoom();
		for ( CPlayer *pPlayerInRoom = pRoom->GetPlayersInRoom()->Head(); pPlayerInRoom; pPlayerInRoom = pRoom->GetPlayersInRoom()->Next( pPlayerInRoom ) )
		{
			if ( pPlayerInRoom->IsConnected() )
				NWCPlayerInfoMessage( pPlayerInRoom, sRunMessage );
		}
	}
	
	uint16_t repairAmount = GetRepairAmount( pNPC->GetScriptLevel() );
	
	uint16_t newHP = pNPC->GetCurHP() + repairAmount;

	if ( newHP > pNPC->GetMaxHP() )
		newHP = pNPC->GetMaxHP();

	// Update hit points
	pNPC->SetHP( newHP );

	return (true);
}

bool NPCRunDamageScript( CNPC *pNPC )
{
	if ( !pNPC )
		return (false);	// Didn't run

	CAttackTarget *pAttackTarget = pNPC->GetPrimaryTarget();

	if ( !pAttackTarget )
		return (false);

	if ( !pAttackTarget->IsTargetPlayer() )
		return (false);	// Only run scripts against a player

	CRoom *pRoom = pNPC->GetRoom();

	if ( !pRoom )
		return (false);

	CPlayer *pTarget = pRoom->GetPlayerInRoomByID( pAttackTarget->GetTargetID() );

	if ( !pTarget )
		return (false); 

	uint32_t scriptIDToRun = 0;

	// Run script
	uint16_t cpuNeeded = GetCPUForScript( pNPC->GetCodeLevel(), pNPC->GetMaxCPU() );
	uint16_t memoryRequired = 0;

	if ( pNPC->GetCurCPU() < cpuNeeded )
		return (false);	// Didn't run

	// Try different scripts	
	uint32_t tryCount = 0;
	for ( tryCount = 0; tryCount < 3; tryCount++ )
	{
		if ( tryCount == 0 )
			scriptIDToRun = SCRIPT_IMPLANT_ID;
		else if ( tryCount == 1 )
			scriptIDToRun = SCRIPT_EXPLOIT_ID;
		else if ( tryCount == 2 )
			scriptIDToRun = SCRIPT_CORRUPT_ID;	

		// Check level to run script
		if ( pNPC->GetScriptLevel() < g_runScripts[scriptIDToRun].scriptLevel )
			continue;
	
		memoryRequired = GetMemoryForScript( pNPC->GetScriptLevel(), g_runScripts[scriptIDToRun].scriptLevel, g_runScripts[scriptIDToRun].scriptMemoryNeeded );

		if ( pNPC->GetCurMemory() >= memoryRequired )
			break;
	}

	if ( tryCount == 3 )
		return (false); // Didn't run

	// Adjust CPU and memory
	pNPC->SetCPU( pNPC->GetCurCPU() - cpuNeeded );
	pNPC->SetMemory( pNPC->GetCurMemory() - memoryRequired );

	// For everyone in room ... inform them that player ran a script
	if ( pNPC->GetRoom() )
	{
		char szTemp[1024];
		snprintf( szTemp, 1024, "%s runs a %s script", pNPC->GetName().c_str(), g_runScripts[scriptIDToRun].szScriptName );
		string sRunMessage = szTemp;
		
		CRoom *pRoom = pNPC->GetRoom();
		for ( CPlayer *pPlayerInRoom = pRoom->GetPlayersInRoom()->Head(); pPlayerInRoom; pPlayerInRoom = pRoom->GetPlayersInRoom()->Next( pPlayerInRoom ) )
		{
			if ( pPlayerInRoom->IsConnected() )
				NWCPlayerInfoMessage( pPlayerInRoom, sRunMessage );
		}
	}
		
	uint16_t damageAmount = 0;
	string sDamageSource = "";

	if ( scriptIDToRun == SCRIPT_IMPLANT_ID )
	{
		damageAmount = GetImplantDamage( pNPC->GetHackLevel(), pTarget->GetHackLevel(), pNPC->GetCodeLevel(), pTarget->GetCodeLevel() );

                sDamageSource = "implant script";
	}
	else if ( scriptIDToRun == SCRIPT_EXPLOIT_ID )
	{
		damageAmount = GetExploitDamage( pNPC->GetHackLevel(), pTarget->GetHackLevel(), pNPC->GetCodeLevel(), pTarget->GetCodeLevel() );

                sDamageSource = "exploit script";
	}
	else if ( scriptIDToRun == SCRIPT_CORRUPT_ID )
	{
		damageAmount = GetCorruptDamage( pNPC->GetScriptLevel(), pTarget->GetScriptLevel() );

		sDamageSource = "corrupt script";
	}
	else
		return (false);

	string sDamageString = GetDamageString( damageAmount, pTarget->GetMaxHP() );

	 // Inform target they have been damaged
        NWCPlayerTakesDamage( pTarget, pNPC->GetNPCID(), DAMAGE_TYPE_NPC, damageAmount, sDamageString, sDamageSource );

	if ( damageAmount >= pTarget->GetCurHP() )
        {
		uint8_t targetHackLevel = pTarget->GetHackLevel();
		uint8_t targetCodeLevel = pTarget->GetCodeLevel();
		uint8_t targetScriptLevel = pTarget->GetScriptLevel();
			
		// Stop fighting target
		pNPC->StopFightingTarget( pTarget->GetPlayerID(), CAttackTarget::TARGET_PLAYER );

		// Kill em
		pTarget->Kill( pNPC->GetNPCID(), pNPC->GetName(), KILLER_TYPE_NPC );

		// Done -- player is killed
		return (true);
	}
	
       	if ( pRoom )
       	{
		// Print damage message
		char szTemp[1024];

		snprintf( szTemp, 1024, "%s's %s %s %s", pNPC->GetName().c_str(), sDamageSource.c_str(), sDamageString.c_str(), pTarget->GetName().c_str() );
		string sMessage = szTemp;

		for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
		{
			if ( pCur->IsConnected() && pCur != pTarget )
				NWCPlayerInfoMessage( pCur, sMessage );
		}
	}

	return (true);
}
