#include "common.h"

tGlobalSpamTracker g_oSpamTracker( 23 );

typedef struct
{
	vector<uint32_t> m_zoneRooms;
} tZoneRoomInfo;

typedef struct
{
	uint32_t zoneNumber;
	uint32_t npcVnum;
	uint32_t npcCount;
} tZoneNPCInfo;

uint32_t g_updateRebootState = 0;

// Zone room counts:
// 0 = 0
// 1 = 4
// 2 = 1
// 3 = 0
// 4 = 33
// 5 = 7
// 6 = 12
// 7 = 6
// 8 = 6
// 9 = 6
// 10 = 20
// 11 = 3

tZoneNPCInfo g_oNPCZoneSpawnTable[] = {
{ 	1, 	0,	7	},
{	5,	1,	7	},
{	7,	14,	7	},	// Code gremline
{	6,	15,	9	},	// Crontab
{	4,	18,	15	},	// Server cleanup script (contains finger request items)
{	8,	17,	9	},	// Gatekeeper -- contains item necessary for exploit (vnum 9)
{	11,	7,	1	},	// Selir
{	6,	18,	10	},	// Server cleanup script
{	7,	17,	5	},	// Codesweeper
{	6,	17,	5	},	// Codesweeper
{	4,	17,	5	},	// Codesweeper
{	10,	19,	9	},	// Daemon Controller
{	4,	20,	7	},	// Task thread
{	2,	21,	7	},	// Simple Subroutine
{	2,	22,	7	},	// Beginner Goon
{	2,	23,	7	},	// Polling Service
{	3, 	24, 	7 	},	// Zombie Process
{	3,	25,	5	},	// Shell Monitor
{	5,	5,	1	},	// Gynophage
{	6,	6,	1	},	// Jymbolia
{	7,	8,	1	},	// Deadwood
{	12,	26,	1	},	// SirGoon
{	9,	27,	5	},	// Aggressive Code Filter
{	9,	17,	5	},	// Gatekeeper -- contains item necessary for exploit (vnum 9)
{ 	10,	28,	5	},	// Virus Scanner
{	6,	29,	6	},	// Adept Hacker
{	10,	29,	7	},	// Adept Hacker
{	2,	0,	3	},	// Code wisp in zone 2
{	1,	2,	5	}, 	// Code Fragment in zone 1
{	3,	2,	5	}, 	// Code Fragment in zone 3
{	3,	23,	3	},	// Polling Service
{	6, 	30,	3	},	// 
{	7,	30,	3	},
{	8,	30,	3	},
{	9,	30,	3	},
{	10,	30,	5	},
{	0,	0,	0	}
};

tZoneRoomInfo *g_pZoneRooms = NULL;
uint32_t g_maxZoneNumber = 0;

void InitWorld( void )
{
        CRoom::LoadRooms();
        CBaseItem::LoadItems();
        CBaseNPC::LoadNPCs();

	g_updateRebootState = 0;

	uint32_t lastZoneNumber = 0;

	// Build zone rooms
	for ( CRoom *pCur = g_oWorldRoomList.Head(); pCur; pCur = g_oWorldRoomList.Next( pCur ) )
	{
		if ( pCur->GetZoneNum() > lastZoneNumber )
			lastZoneNumber = pCur->GetZoneNum();
	}

	g_pZoneRooms = new tZoneRoomInfo[lastZoneNumber+1];
	g_maxZoneNumber = lastZoneNumber+1;

	printf( "Loaded %d zones.\n", g_maxZoneNumber );

	for ( CRoom *pCur = g_oWorldRoomList.Head(); pCur; pCur = g_oWorldRoomList.Next( pCur ) )
		g_pZoneRooms[pCur->GetZoneNum()].m_zoneRooms.push_back( pCur->GetVnum() );

	for ( uint32_t i = 0; i < lastZoneNumber+1; i++ )
	{
		printf( "Zone %d has %d rooms\n", i, g_pZoneRooms[i].m_zoneRooms.size() );
	}

	// Update NPC spawns
	UpdateNPCSpawn();	

	// Really shouldn't do this -- as hard coded world initialization --
	// BUT
	// It is just easier to do it this way instead of defining yet another file

#if 0
	CRoom *pRoom;

	pRoom = g_oRoomHash.Find( 100 );

	if ( pRoom )
	{
		uint32_t i;
		// Add a wisp!
		CNPC *pNPC = CNPC::SpawnNPC( 0 );

		pNPC->ToRoom( pRoom, NWC_NPC_ARRIVES_SPAWN );

		pNPC = CNPC::SpawnNPC( 1 );
	
		pNPC->ToRoom( pRoom, NWC_NPC_ARRIVES_SPAWN );

		pNPC = CNPC::SpawnNPC( 2 );

		pNPC->ToRoom( g_oRoomHash.Find( 101 ), NWC_NPC_ARRIVES_SPAWN );

		pNPC = CNPC::SpawnNPC( 3 );

		pNPC->ToRoom( g_oRoomHash.Find( 103 ), NWC_NPC_ARRIVES_SPAWN );

		pNPC = CNPC::SpawnNPC( 4 );
		
		pNPC->ToRoom( g_oRoomHash.Find( 102 ), NWC_NPC_ARRIVES_SPAWN );
		
		pNPC = CNPC::SpawnNPC( 4 );
		
		pNPC->ToRoom( g_oRoomHash.Find( 102 ), NWC_NPC_ARRIVES_SPAWN );

		CItemInstance *pItem = CItemInstance::CreateItem( 4, false );

		pRoom->AddItemToRoom( pItem );
		
		for ( i = 0; i < 5; i++ )
		{
		CItemInstance *pItem = CItemInstance::CreateItem( 5, false );

		pRoom->AddItemToRoom( pItem );
		}
	}
	else
		printf( "Can't find room 1\n" );
#endif
}

void DestroyWorld( void )
{
	if ( g_pZoneRooms )
		delete g_pZoneRooms;
}

void UpdateEvent( uint32_t tickCount )
{
	if ( tickCount % 8 == 0 )
		UpdatePlayers();

	if ( (tickCount+2) % 8 == 0 )
		UpdateNPCs();

	if ( (tickCount+1) % 7 == 0 )
		UpdateAttacks();

	if ( (tickCount+7) % 150 == 0 )
		UpdateSpamTracker();

	if ( (tickCount+27) % 37 == 0 )
		UpdateNPCSpawn();

	if ( (tickCount+7) % 9 == 0 )
		UpdateCheckReboot();
}

void WorldSendAllPlayersMessage( string &sMessage )
{
	CPlayer *pCur = NULL;
	for ( pCur = g_worldPlayers.Head(); pCur; pCur = g_worldPlayers.Next( pCur ) )
	{
		if ( pCur->IsConnected() && pCur->GetConnection() )
			NWCPlayerInfoMessage( pCur, sMessage );
	}
}

void UpdateCheckReboot( void )
{
	if ( g_serverRebootTimestamp == 0 )
		return;		// Do not reboot with (0 seconds for the timestamp)

	if ( g_serverRebootTimestamp <= time(NULL) )
	{
		printf( "Server reboot.\n" );

		string sRebootMessage = "Server rebooting";
		WorldSendAllPlayersMessage( sRebootMessage );

		exit(0);	
	}

	uint32_t current_timestamp = (uint32_t)time(NULL);
	uint32_t secondsUntilReboot = (g_serverRebootTimestamp - current_timestamp);

	if ( current_timestamp % 60 == 0 )
		printf( "[REBOOT IN][%d secs]\n", secondsUntilReboot );

	// OK give them a reboot message
	switch( g_updateRebootState )
	{
	case 0:	// 5 minute notification
		if ( secondsUntilReboot <= 300 )
		{
			string sRebootMessage = "Server rebooting in 5 minutes";
			WorldSendAllPlayersMessage( sRebootMessage );
		
			g_updateRebootState++;
		}
		break;

	case 1:	// 2 minute notification
		if ( secondsUntilReboot <= 120 )
		{
			string sRebootMessage = "Server rebooting in 2 minutes";
			WorldSendAllPlayersMessage( sRebootMessage );
		
			g_updateRebootState++;
		}
		break;

	case 2: // 1 minute notification
		if ( secondsUntilReboot <= 60 )
		{
			string sRebootMessage = "Server rebooting in 1 minute";
			WorldSendAllPlayersMessage( sRebootMessage );
		
			g_updateRebootState++;
		}
		break;

	case 3: // 30 second notification
		if ( secondsUntilReboot <= 30 )
		{
			string sRebootMessage = "Server rebooting in 30 seconds!";
			WorldSendAllPlayersMessage( sRebootMessage );
		
			g_updateRebootState++;
		}

		break;

	case 4:	// 10 notification
		if ( secondsUntilReboot <= 10 )
		{
			string sRebootMessage = "Server reboot imminent! All player data will be lost!!!";
			WorldSendAllPlayersMessage( sRebootMessage );
		
			g_updateRebootState++;
		}
		break;

	default:
		break;
	}
}

uint32_t GetNPCWorldCount( uint32_t npcVnum )
{
	uint32_t npcCount = 0;
	for ( CNPC *pCur = g_worldNPCs.Head(); pCur; pCur = g_worldNPCs.Next( pCur ) )
	{
		if ( pCur->GetVnum() == npcVnum )
			npcCount++;
	}

	return (npcCount);
}

uint32_t GetNPCZoneCount( uint32_t zoneNum, uint32_t npcVnum )
{
	uint32_t npcCount = 0;
	for ( CNPC *pCur = g_worldNPCs.Head(); pCur; pCur = g_worldNPCs.Next( pCur ) )
	{
		if ( pCur->GetRoom() )
		{
			if ( pCur->GetRoom()->GetZoneNum() != zoneNum )
				continue;
		
			if ( pCur->GetVnum() == npcVnum )
				npcCount++;
		}
	}

	return (npcCount);
}

void UpdateNPCSpawn( void )
{
	for ( uint32_t idx = 0; ; idx++ )
	{
		if ( g_oNPCZoneSpawnTable[idx].zoneNumber == 0 &&
		     g_oNPCZoneSpawnTable[idx].npcVnum == 0 &&
		     g_oNPCZoneSpawnTable[idx].npcCount == 0 )
			break;

		// zoneNum
		uint32_t zoneNum = g_oNPCZoneSpawnTable[idx].zoneNumber;

		// Skip if we don't actually have a zone for it
		if ( g_oNPCZoneSpawnTable[idx].zoneNumber >= g_maxZoneNumber )
			continue;

		uint32_t currentNPCCount = GetNPCZoneCount( zoneNum, g_oNPCZoneSpawnTable[idx].npcVnum ); // GetNPCWorldCount( g_oNPCZoneSpawnTable[idx].npcVnum );
		uint32_t spawnNPCCount = g_oNPCZoneSpawnTable[idx].npcCount;

		if ( currentNPCCount < spawnNPCCount )
		{
			uint32_t newSpawnCount = (spawnNPCCount - currentNPCCount);

			while ( newSpawnCount-- > 0 )
			{
				// Pick random room to spawn in....
				uint32_t spawnIdx = g_oRNG.GetRange( 0, g_pZoneRooms[zoneNum].m_zoneRooms.size()-1 );	
			
				uint32_t spawnRoomVnum = g_pZoneRooms[zoneNum].m_zoneRooms[spawnIdx];

				CRoom *pRoom = g_oRoomHash.Find( spawnRoomVnum );	

				if ( !pRoom )
					continue;
		
				CNPC *pNPC = CNPC::SpawnNPC( g_oNPCZoneSpawnTable[idx].npcVnum );

				if ( !pNPC )
					continue;
		
				pNPC->ToRoom( pRoom, NWC_NPC_ARRIVES_SPAWN );

				printf( "Spawned NPC %d in room %d (zone=%d)\n", pNPC->GetVnum(), pRoom->GetVnum(), pRoom->GetZoneNum() );
			} 
		}
		
	}
}

void UpdateSpamTracker( void )
{
	CPlayer *pNextPlayer = NULL;
	for ( CPlayer *pCur = g_worldPlayers.Head(); pCur; pCur = g_worldPlayers.Next( pCur ) )
	{
		CSpamTracker *pSpamTracker = g_oSpamTracker.Find( pCur->GetName() );

		if ( pSpamTracker )
			pSpamTracker->Tick();
	}
}

void UpdateAttacks( void )
{
	// Players get to attack first
	CPlayer *pNextPlayer = NULL;
	for ( CPlayer *pCur = g_worldPlayers.Head(); pCur; pCur = pNextPlayer )
	{
		pNextPlayer = g_worldPlayers.Next( pCur );

		if ( pCur->IsFighting() )
		{
			// Do damage...
			// Damage is calculated as a ratio of hack level of attacker -- to that of target
			
			// Find a suitable target to attack
			CAttackTarget *pTarget;	
			while( (pTarget = pCur->GetPrimaryTarget()) )
			{
				if ( pTarget->IsTargetNPC() )
				{
					// P v NPC
					CNPC *pTargetNPC = pCur->GetRoom()->GetNPCInRoomByID( pTarget->GetTargetID() );
					if ( !pTargetNPC )
					{
						// Target is gone... stop attacking
						delete pTarget;
						continue;
					}
					
					uint8_t hackLevelSelf = pCur->GetHackLevel();
					uint8_t hackLevelTarget = pTargetNPC->GetHackLevel();

					uint16_t damageAmount = CalcHackDamage( hackLevelSelf, hackLevelTarget );

					// Damage target
					DoHackDamage( pCur, pTargetNPC, damageAmount );
					
					break;	
				}
				else if ( pTarget->IsTargetPlayer() )
				{
					// P v P
					CPlayer *pTargetPlayer = pCur->GetRoom()->GetPlayerInRoomByID( pTarget->GetTargetID() );
		
					if ( !pTargetPlayer )
					{
						// Target is gone... stop attacking
						delete pTarget;
						continue;
					}
					
					uint8_t hackLevelSelf = pCur->GetHackLevel();
					uint8_t hackLevelTarget = pTargetPlayer->GetHackLevel();

					uint16_t damageAmount = CalcHackDamage( hackLevelSelf, hackLevelTarget );

					// Damage target
					DoHackDamage( pCur, pTargetPlayer, damageAmount );

					break;
				}
				else
					delete pTarget; // What the???
			}	
		}
	}	

	// NPCs turn
	CNPC *pNextNPC = NULL;
	for ( CNPC *pCur = g_worldNPCs.Head(); pCur; pCur = pNextNPC )
	{
		pNextNPC = g_worldNPCs.Next( pCur );

		if ( pCur->IsFighting() )
		{
			// Do damage...
			// Damage is calculated as a ratio of hack level of attacker -- to that of target
			
			CAttackTarget *pTarget;	
			while( (pTarget = pCur->GetPrimaryTarget()) )
			{
				if ( pTarget->IsTargetNPC() )
				{
					// P v NPC
					CNPC *pTargetNPC = pCur->GetRoom()->GetNPCInRoomByID( pTarget->GetTargetID() );
					if ( !pTargetNPC )
					{
						// Target is gone... stop attacking
						delete pTarget;
						continue;
					}

					uint8_t hackLevelSelf = pCur->GetHackLevel();
					uint8_t hackLevelTarget = pTargetNPC->GetHackLevel();

					uint16_t damageAmount = CalcHackDamage( hackLevelSelf, hackLevelTarget );

					// Damage target
					DoHackDamage( pCur, pTargetNPC, damageAmount );

					break;	
				}
				else if ( pTarget->IsTargetPlayer() )
				{
					// P v P
					CPlayer *pTargetPlayer = pCur->GetRoom()->GetPlayerInRoomByID( pTarget->GetTargetID() );
		
					if ( !pTargetPlayer )
					{
						// Target is gone... stop attacking
						delete pTarget;
						continue;
					}
					
					uint8_t hackLevelSelf = pCur->GetHackLevel();
					uint8_t hackLevelTarget = pTargetPlayer->GetHackLevel();

					uint16_t damageAmount = CalcHackDamage( hackLevelSelf, hackLevelTarget );

					// Damage target
					DoHackDamage( pCur, pTargetPlayer, damageAmount );

					break;
				}
				else
					delete pTarget; // What the???
			}	
		}
	}	
}

void UpdatePlayers( void )
{
	// Loop through all players and update there stats (move them if necessary)
	CPlayer *pCur = NULL;
	for ( pCur = g_worldPlayers.Head(); pCur; pCur = g_worldPlayers.Next( pCur ) )
	{
		// Perform player regen
		RegenPlayer( pCur );
	}
}

void UpdateNPCs( void )
{
	// Loop through all NPCs and update there stats (move them if necessary)
	CNPC *pCur = NULL;
	for ( pCur = g_worldNPCs.Head(); pCur; pCur = g_worldNPCs.Next( pCur ) )
	{
		// Perform regen
		RegenNPC( pCur );

		// Roam NPC
		DoNPCActions( pCur );
	}
}

void RegenPlayer( CPlayer *pPlayer )
{
	if ( !pPlayer )
		return;
	
	double multiplier = 1.0;

	// Calculate regen amount!
	uint16_t curHP = pPlayer->GetCurHP();
	uint16_t curCPU = pPlayer->GetCurCPU();
	uint16_t curMemory = pPlayer->GetCurMemory();

	uint16_t maxHP = pPlayer->GetMaxHP();
	uint16_t maxCPU = pPlayer->GetMaxCPU();
	uint16_t maxMemory = pPlayer->GetMaxMemory();

	if ( curHP < maxHP )
	{
		uint16_t hpAddMax = (maxHP * (double)HP_REGEN_RATE_PERCENTAGE / 100.0) * multiplier;

		// Slow regen of HP while fighting
		if ( pPlayer->IsFighting() )
			hpAddMax = (hpAddMax * 0.7);	

		uint16_t newHP = hpAddMax + curHP;
		if ( newHP > maxHP )
			newHP = maxHP;

		pPlayer->SetHP( newHP );
	}

	if ( curCPU < maxCPU )
	{
		uint16_t cpuAddMax = (maxCPU * (double)CPU_REGEN_RATE_PERCENTAGE / 100.0) * multiplier;
		
		uint16_t newCPU = cpuAddMax + curCPU;
		if ( newCPU > maxCPU )
			newCPU = maxCPU;

		pPlayer->SetCPU( newCPU );
	}
	
	if ( curMemory < maxMemory )
	{
		uint16_t memoryAddMax = (maxMemory * (double)MEMORY_REGEN_RATE_PERCENTAGE / 100.0) * multiplier;

		if ( pPlayer->IsFighting() )
			memoryAddMax = (memoryAddMax * 0.7);

		uint16_t newMemory = memoryAddMax + curMemory;
		if ( newMemory > maxMemory )
			newMemory = maxMemory;

		pPlayer->SetMemory( newMemory );
	}

	// Update the player stats
	NWCPlayerStatUpdate( pPlayer );
}

void RegenNPC( CNPC *pNPC )
{
	if ( !pNPC )
		return;

	double multiplier = 1.0;

	// Calculate regen amount!
	uint16_t curHP = pNPC->GetCurHP();
	uint16_t curCPU = pNPC->GetCurCPU();
	uint16_t curMemory = pNPC->GetCurMemory();

	uint16_t maxHP = pNPC->GetMaxHP();
	uint16_t maxCPU = pNPC->GetMaxCPU();
	uint16_t maxMemory = pNPC->GetMaxMemory();

	if ( curHP < maxHP )
	{
		uint16_t hpAddMax = (maxHP * (double)HP_REGEN_RATE_PERCENTAGE / 100.0) * multiplier;

		// Slow regen of HP while fighting
		if ( pNPC->IsFighting() )
			hpAddMax = (hpAddMax * 0.7);

		uint16_t newHP = hpAddMax + curHP;
		if ( newHP > maxHP )
			newHP = maxHP;

		pNPC->SetHP( newHP );
	}

	if ( curCPU < maxCPU )
	{
		uint16_t cpuAddMax = (maxCPU * (double)CPU_REGEN_RATE_PERCENTAGE / 100.0) * multiplier;
		
		uint16_t newCPU = cpuAddMax + curCPU;
		if ( newCPU > maxCPU )
			newCPU = maxCPU;

		pNPC->SetCPU( newCPU );
	}

	if ( curMemory < maxMemory )
	{
		uint16_t memoryAddMax = (maxMemory * (double)MEMORY_REGEN_RATE_PERCENTAGE / 100.0) * multiplier;
		
		if ( pNPC->IsFighting() )
			memoryAddMax = (memoryAddMax * 0.7);

		uint16_t newMemory = memoryAddMax + curMemory;
		if ( newMemory > maxMemory )
			newMemory = maxMemory;

		pNPC->SetMemory( newMemory );
	}
}

void DoNPCActions( CNPC *pNPC )
{
	bool bDidRoamAction = false;

	if ( !pNPC )
		return;

	// Let NPC's roam -- if they aren't stationary and they aren't fighting
	if ( !pNPC->IsStationary() && !pNPC->IsFighting() )
	{
		// Update the roam counter
		pNPC->UpdateRoamCounter();

		// NPC can roam -- check roam counter
		uint32_t npcRoamChance;

		if ( pNPC->IsFastRoamer() )
			npcRoamChance = NPC_FAST_ROAM_TICK_RATE;
		else
			npcRoamChance = NPC_NORMAL_ROAM_TICK_RATE;

		uint32_t npcRoamCounter = pNPC->GetRoamCounter();

		bool bRoam = false;
		if ( npcRoamCounter > 5 )
		{
			uint32_t npcRoamRange = npcRoamChance + npcRoamCounter;

			uint32_t randomChoice = g_oRNG.GetRange( 0, npcRoamRange-1 );

			if ( randomChoice > npcRoamChance )
				bRoam = true;
		}

		CRoom *pRoom = pNPC->GetRoom();
		if ( bRoam && pRoom )
		{
			// Find a random direction (must be in the same zone)
			uint8_t directionTotal = 0;
		
			for ( uint8_t i = 0; i < DIRECTION_MAX; i++ )
				if ( pRoom->HasDirection( i ) )
				{
					CRoom *pNextRoom = g_oRoomHash.Find( pRoom->GetDirection( i ).GetVnum() );

					if ( pNextRoom && pNextRoom->GetZoneNum() == pRoom->GetZoneNum() )
						directionTotal++;
				}

			if ( directionTotal > 0 )
			{
				uint8_t chosenDirection = g_oRNG.GetRange( 0, directionTotal-1 );
				uint8_t i;
	
				CRoom *pGotoRoom = NULL;	
				for ( i = 0; i < DIRECTION_MAX; i++ )
					if ( pRoom->HasDirection( i ) )
					{
						CRoom *pNextRoom = g_oRoomHash.Find( pRoom->GetDirection( i ).GetVnum() );

						if ( pNextRoom && pNextRoom->GetZoneNum() == pRoom->GetZoneNum() )
							if ( chosenDirection == 0 )
							{
								pGotoRoom = pNextRoom;
								break;
							}
							else
								chosenDirection--;
					}

				if ( pGotoRoom )
				{
					// Reset the roam counter to resume roaming
					pNPC->ResetRoamCounter();

					// Move NPC
					pNPC->FromRoom( NWC_NPC_LEAVES_EXITROOM, i );
					pNPC->ToRoom( pGotoRoom, NWC_NPC_ARRIVES_ENTERROOM );

					// Let the next action(s) know we did this action
					bDidRoamAction = true;
				}
			}
		}
	}

	if ( !bDidRoamAction && !pNPC->IsFighting() && pNPC->IsNPCAggressive() )
	{
		// Aggress random players in the room
		// Check for players in room
		bool bDoAggress = false;

		CRoom *pRoom = pNPC->GetRoom();

		if ( g_oRNG.GetRange( 0, 5 ) == 0 )
			bDoAggress = true;
	
		if ( bDoAggress && pRoom )
		{	
			uint32_t roomPlayerCount = 0;
			
			for ( CPlayer *pPlayer = pRoom->GetPlayersInRoom()->Head(); pPlayer; pPlayer = pRoom->GetPlayersInRoom()->Next( pPlayer ) )
				roomPlayerCount++;

			if ( roomPlayerCount > 0 )
			{
				uint32_t chosenPlayer = g_oRNG.GetRange( 0, roomPlayerCount-1 );

				for ( CPlayer *pPlayer = pRoom->GetPlayersInRoom()->Head(); pPlayer; pPlayer = pRoom->GetPlayersInRoom()->Next( pPlayer ) )
				{
					if ( chosenPlayer == 0 )
					{
						char szTemp[1024];
						snprintf( szTemp, 1024, "%s attacks you!", pNPC->GetName().c_str() );

						string sMessage = szTemp;

						NWCPlayerInfoMessage( pPlayer, sMessage );

						snprintf( szTemp, 1024, "%s attacks %s!", pNPC->GetName().c_str(), pPlayer->GetName().c_str() );
						sMessage = szTemp;
						for ( CPlayer *pOtherPlayer = pRoom->GetPlayersInRoom()->Head(); pOtherPlayer; pOtherPlayer = pRoom->GetPlayersInRoom()->Next( pOtherPlayer ) )
						{
							if ( pOtherPlayer->IsConnected() && pOtherPlayer != pPlayer )
								NWCPlayerInfoMessage( pOtherPlayer, sMessage );
						}

						pNPC->AttackPlayer( pPlayer->GetPlayerID() );

						if ( !pPlayer->IsFightingTarget( pNPC->GetNPCID(), NWC_ATTACK_NPC ) )
							NWCPlayerAttackResponse( pPlayer->GetConnection(), pNPC->GetNPCID(), NWC_ATTACK_NPC );

						pPlayer->AttackNPC( pNPC->GetNPCID() );

						break;
					}
					else
						chosenPlayer--;
				}
			}
		}
	}

	// Have an NPC do a script action
	if ( pNPC->IsFighting() )
	{
		bool bRanScript = false;
		if ( pNPC->HasHealScript() )
		{
			if ( pNPC->GetCurHP() < (pNPC->GetMaxHP() * 0.3) )
			{
				bRanScript = NPCRunHealScript( pNPC );
			}
		}

		if ( !bRanScript && pNPC->HasDamageScript() )
		{
			if ( g_oRNG.GetRange( 0, 1 ) == 0 )
			{
				bRanScript = NPCRunDamageScript( pNPC );
			}
		}
	}
}

void DoPlayerEnterRoomAction( CPlayer *pPlayer, uint8_t arrivalType )
{
	if ( !pPlayer )
		return;

	CRoom *pRoom;
	if ( !(pRoom = pPlayer->GetRoom()) )
		return;

	// Aggress only on enter room events (ignore logins and respawns)
	if ( arrivalType == NWC_PLAYER_ARRIVAL_ENTERROOM )
	{
		// Check for aggressive NPCs
		for ( CNPC *pNPC = pRoom->GetNPCsInRoom()->Head(); pNPC; pNPC = pRoom->GetNPCsInRoom()->Next( pNPC ) )
		{
			if ( !pNPC->IsFighting() && pNPC->IsNPCAggressOnEnter() )
			{
				char szTemp[1024];
				snprintf( szTemp, 1024, "%s attacks you as you enter!", pNPC->GetName().c_str() );

				string sMessage = szTemp;

				NWCPlayerInfoMessage( pPlayer, sMessage );
						
				snprintf( szTemp, 1024, "%s attacks %s as they enter!", pNPC->GetName().c_str(), pPlayer->GetName().c_str() );
				sMessage = szTemp;
				for ( CPlayer *pOtherPlayer = pRoom->GetPlayersInRoom()->Head(); pOtherPlayer; pOtherPlayer = pRoom->GetPlayersInRoom()->Next( pOtherPlayer ) )
				{
					if ( pOtherPlayer->IsConnected() && pOtherPlayer != pPlayer )
						NWCPlayerInfoMessage( pOtherPlayer, sMessage );
				}

				// Aggress that mofo
				pNPC->AttackPlayer( pPlayer->GetPlayerID() );
						
				if ( !pPlayer->IsFightingTarget( pNPC->GetNPCID(), NWC_ATTACK_NPC ) )
					NWCPlayerAttackResponse( pPlayer->GetConnection(), pNPC->GetNPCID(), NWC_ATTACK_NPC );

				pPlayer->AttackNPC( pNPC->GetNPCID() );
			}
		}
	}	
}
