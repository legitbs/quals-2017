#include "common.h"

tNPCDatabaseHash g_oNPCDBHash( NPC_HASH_SIZE );

// Linked list of all NPCs in the world... so we can do automated actions on them
NUtil::LIST_DECLARE( CNPC, m_worldLink ) g_worldNPCs;


uint32_t CNPC::m_lastNPCID = 0;

CBaseNPC::CBaseNPC( uint32_t vnum )
	: CBaseNPCVnum( vnum )
{
	m_baseHP = 0;
	m_baseCPU = 0;
	m_baseMemory = 0;

	m_baseScriptLevel = 0;
	m_baseCodeLevel = 0;
	m_baseHackLevel = 0;

	m_flags = 0;


	for ( uint8_t i = 0; i < EQUIP_SLOT_MAX; i++ )
	{
		m_baseEquipItems[i].vnum = 0;
		m_baseEquipItems[i].bPopulated = false;
		m_baseEquipItems[i].bModStats = false;
	}
}

CNPC::CNPC( CBaseNPC *pBaseNPC )
	: m_pBaseNPC( pBaseNPC )
{
	// Generate ID
	m_npcID = m_lastNPCID;
	m_lastNPCID++;

	// Add to list of world NPCs
	g_worldNPCs.InsertHead( this );

	m_pCurrentRoom = NULL;
	
	m_roamCounter = 0;

	for ( uint8_t i = 0; i < EQUIP_SLOT_MAX; i++ )
		m_pEquipItems[i] = NULL;

	Init();
}

CNPC::~CNPC( )
{
	uint32_t i;
	for ( i = 0; i < EQUIP_SLOT_MAX; i++ )
	{
		if (  m_pEquipItems[i] )
			delete m_pEquipItems[i];
	}

	for ( i = 0; i < m_itemsOnNPC.size(); i++ )
	{
		delete m_itemsOnNPC[i];
		m_itemsOnNPC[i] = NULL;
	}

	m_itemsOnNPC.clear();
}

void CNPC::Init( void )
{
	if ( !m_pBaseNPC )
		return;

	m_sName = m_pBaseNPC->m_sName;
	m_sDescription = m_pBaseNPC->m_sDescription;

	m_flags = m_pBaseNPC->m_flags;

	// Get levels for now...
	m_maxHP = m_pBaseNPC->m_baseHP;
	m_maxCPU = m_pBaseNPC->m_baseCPU;
	m_maxMemory = m_pBaseNPC->m_baseMemory;

	// Init current stats
	m_curHP = m_maxHP;
	m_curCPU = m_maxCPU;
	m_curMemory = m_maxMemory;

	m_scriptLevel = m_pBaseNPC->m_baseScriptLevel;
	m_codeLevel = m_pBaseNPC->m_baseCodeLevel;
	m_hackLevel = m_pBaseNPC->m_baseHackLevel;

	// Allocate equipment (if any)
	uint32_t i = 0;

	for ( i = 0; i < EQUIP_SLOT_MAX; i++ )
	{
		if ( m_pBaseNPC->m_baseEquipItems[i].bPopulated )
		{
			CItemInstance *pNewItem = CItemInstance::CreateItem( m_pBaseNPC->m_baseEquipItems[i].vnum, m_pBaseNPC->m_baseEquipItems[i].bModStats );

			if ( pNewItem )
				m_pEquipItems[i] = pNewItem;
			else
				printf( "ERROR::NPC (%d::%d) unable to allocate equip slot (%d) item vnum (%d)\n", m_npcID, m_pBaseNPC->GetVnum(), i, m_pBaseNPC->m_baseEquipItems[i].vnum );	
		}
	}

	// Allocate items (if any)
	for ( i = 0; i < m_pBaseNPC->m_baseItems.size(); i++ )
	{
		CItemInstance *pNewItem = CItemInstance::CreateItem( m_pBaseNPC->m_baseItems[i].vnum, m_pBaseNPC->m_baseItems[i].bModStats );

		m_itemsOnNPC.push_back( pNewItem );
	}

	// Done generating NPC 
}

CNPC *CNPC::SpawnNPC( uint32_t baseVnum )
{
	// Spawn a new NPC
	CBaseNPC *pBaseNPC = g_oNPCDBHash.Find( baseVnum );

	if ( pBaseNPC == NULL )
		return (NULL);

	// OK spawn a new NPC with base
	CNPC *pNewNPC = new CNPC( pBaseNPC );

	return (pNewNPC);
}

bool CNPC::ReadNPCData( CStreamData *pData )
{

}

bool CNPC::WriteNPCData( CStreamData *pData )
{

}

void CBaseNPC::LoadNPCs( void )
{
	// Load NPCs from disk
	char szFileName[1024];
        char szLine[1024];

        snprintf( szFileName, 1024, "%s/%s", NPC_FILE_DIRECTORY, NPC_FILE_NAME );

        FILE *pFile;

        pFile = fopen( szFileName, "r" );

        if ( !pFile )
        {
                printf( "Can't load NPCs from disk: %s\n", szFileName );
                return;
        }

        do
        {
		uint32_t i;

                string sName;
                string sDescription;
        	string sFlags;
	
		uint32_t npcVnum;
        	uint8_t scriptLevel;
	        uint8_t codeLevel;
		uint8_t hackLevel;

		int16_t baseHP;
		int16_t baseCPU;
		int16_t baseMemory;

		tBaseEquipItem baseEquipItems[EQUIP_SLOT_MAX];
		vector<tBaseInventoryItem> baseItems;

		for ( uint8_t i = 0; i < EQUIP_SLOT_MAX; i++ )
			baseEquipItems[i].bPopulated = false;

                // Read in room data...
                READ_FILE_STRING( "Name", sName )
                READ_FILE_STRING( "Description", sDescription )
                READ_FILE_UINT32( "Vnum", npcVnum )
		READ_FILE_UINT16( "HP", baseHP )
		READ_FILE_UINT16( "CPU", baseCPU )
		READ_FILE_UINT16( "Memory", baseMemory )

		READ_FILE_UINT8( "ScriptLevel", scriptLevel )
		READ_FILE_UINT8( "CodeLevel", codeLevel )
		READ_FILE_UINT8( "HackLevel", hackLevel )
		
		READ_FILE_STRING( "Flags", sFlags )

		if ( g_oNPCDBHash.Find( npcVnum ) )
		{
			printf( "Duplicate NPC vnum %d already exists.\n", npcVnum );
			goto bad_fileread;
		}

		uint32_t flags = 0;
		vector<string> sFlagVector = SplitString( sFlags );

		for ( uint8_t i = 0; i < sFlagVector.size(); i++ )
		{
			if ( strcasecmp( "stationary", sFlagVector[i].c_str() ) == 0 )
				flags |= NPC_FLAG_STATIONARY;
			else if ( strcasecmp( "blocksflee", sFlagVector[i].c_str() ) == 0 )
				flags |= NPC_FLAG_BLOCKSFLEE;
			else if ( strcasecmp( "extraregen", sFlagVector[i].c_str() ) == 0 )
				flags |= NPC_FLAG_EXTRAREGEN;
			else if ( strcasecmp( "roams", sFlagVector[i].c_str() ) == 0 )
				flags |= NPC_FLAG_ROAMS;
			else if ( strcasecmp( "aggressive", sFlagVector[i].c_str() ) == 0 )
				flags |= NPC_FLAG_AGGRESSIVE;
			else if ( strcasecmp( "attackonenter", sFlagVector[i].c_str() ) == 0 )
				flags |= NPC_FLAG_ATTACKONARRIVE;
			else if ( strcasecmp( "flees", sFlagVector[i].c_str() ) == 0 )
				flags |= NPC_FLAG_FLEES;
			else if ( strcasecmp( "damagescript", sFlagVector[i].c_str() ) == 0 )
				flags |= NPC_FLAG_DAMAGESCRIPT;
			else if ( strcasecmp( "healscript", sFlagVector[i].c_str() ) == 0 )
				flags |= NPC_FLAG_HEALSCRIPT;
			else if ( strcasecmp( "none", sFlagVector[i].c_str() ) == 0 )
				;
			else
			{
				printf( "Error loading flags for NPC: %s\n", sFlags.c_str() );
				goto bad_fileread;
			}
		}

		// Now load equip items (if available)
		long int lastOffset = ftell( pFile );
		bool bReset = false;
		for ( ;; )
		{
			char szLine[1024];

			if ( feof( pFile ) )
				break;

			if ( fgets( szLine, 1024, pFile ) == NULL )
				break;

			if ( strlen(szLine) <= 1 )
				goto bad_fileread;

			szLine[strlen(szLine)-1] = '\0';

			vector<string> sLineVector = SplitString( string(szLine) );

			if ( sLineVector.size() == 0 )
				goto bad_fileread;

			if ( strcasecmp( "equip", sLineVector[0].c_str() ) != 0 )
			{
				bReset = true;
				break;
			}

			if ( sLineVector.size() < 3 )
				goto bad_fileread;

			tItemEquipType equipSlot;
			if ( !CItemInstance::GetEquipTypeFromString( sLineVector[1], equipSlot ) )
			{
				printf( "Invalid equip slot %d in npc, for npc ID %d\n", equipSlot, npcVnum );
				goto bad_fileread;
			}

			uint32_t equipVnum;
			if ( sscanf( sLineVector[2].c_str(), "%u", &equipVnum ) != 1 )
			{
				printf( "Invalid vnum for equip item, for npc ID %d\n", npcVnum );
				goto bad_fileread;
			}

			CBaseItem *pBaseItem = NULL;
			if ( (pBaseItem = g_oItemDBHash.Find( equipVnum )) == NULL )
			{
				printf( "Invalid item vnum (%d) for equip item (slot %d), npc ID %d\n", equipVnum, equipSlot, npcVnum );
				goto bad_fileread;
			}	

			bool bModItemStats = false;
			if ( sLineVector.size() > 3 )
			{
				// Read mod indicator
				if ( strcasecmp( sLineVector[3].c_str(), "mod" ) == 0 )
					bModItemStats = true;
				else
				{
					printf( "Invalid 4th parameter %s in equip slot %d for NPC ID %d\n", sLineVector[3].c_str(), equipSlot, npcVnum );
					goto bad_fileread; 
				}
			
			}

			// OK save info
			baseEquipItems[equipSlot].vnum = equipVnum;
			baseEquipItems[equipSlot].bPopulated = true;
			baseEquipItems[equipSlot].bModStats = true;	
	
			// Save last offset	
			lastOffset = ftell( pFile );
		}

		if ( bReset )
			fseek( pFile, lastOffset, SEEK_SET );

		if ( feof( pFile ) )
			goto read_complete;
		
		lastOffset = ftell( pFile );
		bReset = false;
		for ( ;; )
		{
			char szLine[1024];

			if ( feof( pFile ) )
				break;

			if ( fgets( szLine, 1024, pFile ) == NULL )
				break;

			if ( strlen(szLine) <= 1 )
				goto bad_fileread;

			szLine[strlen(szLine)-1] = '\0';

			vector<string> sLineVector = SplitString( string(szLine) );

			if ( sLineVector.size() == 0 )
				goto bad_fileread;

			if ( strcasecmp( "inventory", sLineVector[0].c_str() ) != 0 )
			{
				bReset = true;
				break;
			}
			
			if ( sLineVector.size() < 2 )
				goto bad_fileread;

			uint32_t inventoryVnum;
			if ( sscanf( sLineVector[1].c_str(), "%u", &inventoryVnum ) != 1 )
			{
				printf( "Invalid vnum for inventory item, for npc ID %d\n", npcVnum );
				goto bad_fileread;
			}

			CBaseItem *pBaseItem = NULL;
			if ( (pBaseItem = g_oItemDBHash.Find( inventoryVnum )) == NULL )
			{
				printf( "Invalid item vnum (%d) for inventory item npc ID %d\n", inventoryVnum, npcVnum );
				goto bad_fileread;
			}	

			bool bModItemStats = false;
			if ( sLineVector.size() > 2 )
			{
				// Read mod indicator
				if ( strcasecmp( sLineVector[2].c_str(), "mod" ) == 0 )
					bModItemStats = true;
				else
				{
					printf( "Invalid 3rd parameter %s in inventory %d for NPC ID %d\n", sLineVector[2].c_str(), inventoryVnum, npcVnum );
					goto bad_fileread; 
				}
			}
		
			// Add to inventory items
			tBaseInventoryItem oTempItem;
			oTempItem.vnum = inventoryVnum;
			oTempItem.bModStats = bModItemStats;

			baseItems.push_back( oTempItem );
	
			// Save last offset	
			lastOffset = ftell( pFile );
		}

		if ( bReset )
			fseek( pFile, lastOffset, SEEK_SET );

read_complete:
		CBaseNPC *pNewBaseNPC = new CBaseNPC( npcVnum );

		pNewBaseNPC->m_sName = sName;
		pNewBaseNPC->m_sDescription = sDescription;
		
		pNewBaseNPC->m_baseHP = baseHP;
		pNewBaseNPC->m_baseCPU = baseCPU;
		pNewBaseNPC->m_baseMemory = baseMemory;

		pNewBaseNPC->m_baseScriptLevel = scriptLevel;
		pNewBaseNPC->m_baseCodeLevel = codeLevel;
		pNewBaseNPC->m_baseHackLevel = hackLevel;

		pNewBaseNPC->m_flags = flags;

		for ( i = 0; i < EQUIP_SLOT_MAX; i++ )
		{
			pNewBaseNPC->m_baseEquipItems[i].vnum = baseEquipItems[i].vnum;
			pNewBaseNPC->m_baseEquipItems[i].bPopulated = baseEquipItems[i].bPopulated;
			pNewBaseNPC->m_baseEquipItems[i].bModStats = baseEquipItems[i].bModStats;
		}

		for ( i = 0; i < baseItems.size(); i++ )
			pNewBaseNPC->m_baseItems.push_back( baseItems[i] );

		// Loaded NPC
		printf( "Loaded NPC %d\n", pNewBaseNPC->GetVnum() );

		// Add new NPC
		g_oNPCDBHash.Add( pNewBaseNPC );

	} while ( !feof( pFile ) );

        return;

bad_fileread:
        printf( "Critical error reading npcs.txt file.\n" );
        return;
}

bool CNPC::ToRoom( CRoom *pRoom, uint8_t arrivalType )
{
	if ( !pRoom )
		return (false);

	if ( m_pCurrentRoom )
		return (false);


	if ( pRoom->AddNPC( this ) )
	{
		m_pCurrentRoom = pRoom;
	
		NWCNPCArrivesInRoom( this, arrivalType );
		return (true);
	}
	else
		return (false);
}

bool CNPC::FromRoom( uint8_t leaveType, uint8_t leaveDirection )
{
	if ( !m_pCurrentRoom )
		return (false);

	if ( m_pCurrentRoom->RemoveNPC( this ) )
	{
		NWCNPCLeavesFromRoom( this, leaveType, leaveDirection );

		m_pCurrentRoom = NULL;
	
		return (true);
	}
	else
		return (false);
}

bool CNPC::IsFighting( void )
{
	if ( m_attackTargets.Empty() )
                return (false);
        else
                return (true);
}

bool CNPC::AttackNPC( uint32_t npcID )
{
        bool bFound = false;
        CRoom *pRoom = GetRoom();

        if ( !pRoom )
                return (false);

        for ( CNPC *pCur = pRoom->GetNPCsInRoom()->Head(); pCur; pCur = pRoom->GetNPCsInRoom()->Next( pCur ) )
        {
                if ( pCur->GetNPCID() == npcID )
                {
                        bFound = true;
                        break;
                }
        }

        if ( !bFound )
                return (false);

        // Attack! -- check if we are already attacking that NPC
        for ( CAttackTarget *pCur = m_attackTargets.Head(); pCur; pCur = m_attackTargets.Next( pCur ) )
        {
                if ( pCur->IsTargetNPC() && pCur->GetTargetID() == npcID )
                {
                        delete pCur;
                        break;
                }
        }

        // Now add to list at top (this sets the primary target)
        m_attackTargets.InsertHead( new CAttackTarget( npcID, CAttackTarget::TARGET_NPC ) );

        return (true);
}

bool CNPC::AttackPlayer( uint32_t playerID )
{
	bool bFound = false;
        CRoom *pRoom = GetRoom();

        if ( !pRoom )
                return (false);

        for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
        {
                if ( pCur->GetPlayerID() == playerID )
                {
                        bFound = true;
                        break;
                }
        }

        if ( !bFound )
                return (false);

        // Attack! -- check if we are already attacking that NPC
        for ( CAttackTarget *pCur = m_attackTargets.Head(); pCur; pCur = m_attackTargets.Next( pCur ) )
        {
                if ( pCur->IsTargetPlayer() && pCur->GetTargetID() == playerID )
                {
                        delete pCur;
                        break;
                }
        }

        // Now add to list at top (this sets the primary target)
        m_attackTargets.InsertHead( new CAttackTarget( playerID, CAttackTarget::TARGET_PLAYER ) );

        return (true);
}

bool CNPC::Kill( uint32_t killerID, string sKillerName, uint8_t killerType )
{
	uint32_t i;
	// Kill this NPC!!!
	
	// Inform everyone in room
	CRoom *pRoom = GetRoom();
	
	for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
	{
		if ( pCur->IsConnected() )
			NWCPlayerKilledResponse( pCur, GetNPCID(), NWC_ATTACK_NPC, killerID, sKillerName, killerType );
	}

	bool bHasItems = false;
	// Check for items to drop in room... NPCs drop every item automatically
	for ( i = 0; i < EQUIP_SLOT_MAX; i++ )
	{
		if ( m_pEquipItems[i] )
		{
			CItemInstance *pItemToDrop = m_pEquipItems[i];
			m_pEquipItems[i] = NULL;			
			
			NWCDropItemToRoom( pRoom, pItemToDrop );	

			bHasItems = true;
		}
	}

	for ( i = 0; i < m_itemsOnNPC.size(); i++ )
	{
		if ( m_itemsOnNPC[i] )
		{
			CItemInstance *pItemToDrop = m_itemsOnNPC[i];

			NWCDropItemToRoom( pRoom, pItemToDrop );

			bHasItems = true;
		}
	}

	m_itemsOnNPC.clear();

	if ( bHasItems )
	{
		char szTemp[1024];
		snprintf( szTemp, 1024, "As %s is terminated its items drop to the room", GetName().c_str() );

		string sDropMessage = szTemp;

		for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
		{
			if ( pCur->IsConnected() )
				NWCPlayerInfoMessage( pCur, sDropMessage );
		}

	}

	// Destroy ourself!
	delete this;

	return (true);
}

bool CNPC::IsFightingPlayer( CPlayer *pPlayer )
{
	// Check that they are in fact in the same room
	if ( !GetRoom() || !pPlayer || !pPlayer->GetRoom() )
		return (false);

	if ( pPlayer->GetRoom() != GetRoom() )
		return (false);

        // Check if this NPC is fighting this player
        for ( CAttackTarget *pCur = m_attackTargets.Head(); pCur; pCur = m_attackTargets.Next( pCur ) )
        {
                if ( pCur->IsTargetPlayer() && pCur->GetTargetID() == pPlayer->GetPlayerID() )
                {
			return (true);
                }
        }

	return (false);
}

bool CNPC::StopFightingTarget( uint32_t targetID, uint8_t targetType )
{
        // Check if this NPC is fighting this player
        for ( CAttackTarget *pCur = m_attackTargets.Head(); pCur; pCur = m_attackTargets.Next( pCur ) )
        {
                if ( pCur->GetTargetID() == targetID && pCur->GetTargetType() == targetType )
                {
			delete pCur;
			return (true);
                }
        }

	return (false);
}

uint8_t *CNPC::GetItemFingerprint( uint32_t &outLen )
{
        uint8_t *pFingerprintBuffer = new uint8_t[ EQUIP_SLOT_MAX * 16 ];
        uint32_t outPos = 0;

        for ( uint8_t i = 0; i < EQUIP_SLOT_MAX; i++ )
        {
                if ( m_pEquipItems[i] )
                        outPos += m_pEquipItems[i]->GetItemFingerprint( pFingerprintBuffer+outPos, 8 );
        }

        outLen = outPos;
        return (pFingerprintBuffer);
}
