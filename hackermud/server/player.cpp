#include "common.h"

NUtil::LIST_DECLARE( CPlayer, m_worldLink ) g_worldPlayers;

tGlobalPlayerLoginCache g_oPlayerLoginCache( 23 );

uint32_t CPlayer::m_lastPlayerID = 0;

CPlayerLoginCache::CPlayerLoginCache( string &sName, CStreamData *pData, string &sPassword ) 
	: CPlayerLoginCacheID( sName ), m_sPassword( sPassword ), m_pPlayerStream( pData )
{

}

CPlayerLoginCache::~CPlayerLoginCache( )
{
	if ( m_pPlayerStream )
		delete m_pPlayerStream;
}

CPlayer *CPlayerLoginCache::CreateNewPlayer( void )
{
	if ( !m_pPlayerStream )
		return (NULL);

	CPlayer *pTempPlayer = new CPlayer();

	m_pPlayerStream->ResetReadPos( 0 );
	if ( !pTempPlayer->ReadPlayerData( m_pPlayerStream, true ) )
	{
        	delete pTempPlayer;
		return (NULL);
        }

	pTempPlayer->SetPasswordHash( m_sPassword );

	return (pTempPlayer);
}

string &CPlayerLoginCache::GetPassword( void )
{
	return m_sPassword;
}

void CPlayerLoginCache::SaveNewStream( CStreamData *pData )
{
	if ( m_pPlayerStream )
		delete m_pPlayerStream;

	m_pPlayerStream = pData;
}

CPlayer::CPlayer( )
	: m_pConnection( NULL )
{
	uint64_t newKey;

	newKey = g_oRNG.GetU32();
	newKey = (newKey << 32) | g_oRNG.GetU32();

	m_playerID = m_lastPlayerID;
	m_lastPlayerID++;

	m_playerKey = newKey;

	for ( uint8_t i = 0; i < EQUIP_SLOT_MAX; i++ )
		m_pEquipItems[i] = NULL;
}

CPlayer::~CPlayer( )
{
	for ( uint8_t i = 0; i < EQUIP_SLOT_MAX; i++ )
	{
		if ( m_pEquipItems[i] )
			delete m_pEquipItems[i];
	}
}

void CPlayer::ClearData( void )
{
        for ( uint8_t i = 0; i < EQUIP_SLOT_MAX; i++ )
        {
                if ( m_pEquipItems[i] )
                        delete m_pEquipItems[i];

                m_pEquipItems[i] = NULL;
        }

        m_itemsOnPlayer.DeleteAll();
        m_attackTargets.DeleteAll();

        m_pCurrentRoom = NULL;
}

bool CPlayer::CheckPasswordHash( string sPasswordHash )
{
	return (sPasswordHash == m_sPasswordHash);
}

bool CPlayer::ReadPlayerData( CStreamData *pData, bool bPlayerCache )
{
	if ( pData == NULL )
		return (false);

	ClearData();

	try
	{
		uint16_t playerNameLength;
		pData->Read( &playerNameLength, sizeof(playerNameLength) );
		char *pTempData = new char[playerNameLength+1];
		pData->Read( pTempData, playerNameLength );
		pTempData[playerNameLength] = '\0';

		m_sPlayerName = pTempData;
		delete pTempData;

		uint16_t playerDescriptionLength;
		pData->Read( &playerDescriptionLength, sizeof(playerDescriptionLength) );
		pTempData = new char[playerDescriptionLength+1];
		pData->Read( pTempData, playerDescriptionLength );
		pTempData[playerDescriptionLength] = '\0';

		m_sPlayerDescription = pTempData;
		delete pTempData;

		uint32_t roomVnum;
		pData->Read( &roomVnum, sizeof(roomVnum) );

		CRoom *pToRoom = g_oRoomHash.Find( roomVnum );
		
		if ( pToRoom == NULL )
			SetRoom( g_oRoomHash.Find( INITIAL_ROOM_VNUM ) );
		else
			SetRoom( g_oRoomHash.Find( roomVnum ) );

		pData->Read( &m_curDiskSpace, sizeof(m_curDiskSpace) );
		pData->Read( &m_curDiskItems, sizeof(m_curDiskItems) );
	
		pData->Read( &m_maxCarryDisk, sizeof(m_maxCarryDisk) );
                pData->Read( &m_maxCarryItems, sizeof(m_maxCarryItems) );

                // Get ID -- if it isn't for a player cache
		if ( !bPlayerCache )
		{
                	pData->Read( &m_playerID, sizeof(m_playerID) );
                	pData->Read( &m_playerKey, sizeof(m_playerKey) );
		}

                // Get stats
                pData->Read( &m_HP, sizeof(m_HP) );
                pData->Read( &m_CPU, sizeof(m_CPU) );
                pData->Read( &m_Memory, sizeof(m_Memory) );

		pData->Read( &m_MaxHP, sizeof(m_MaxHP) );
		pData->Read( &m_MaxCPU, sizeof(m_MaxCPU) );
		pData->Read( &m_MaxMemory, sizeof(m_MaxMemory) );

                pData->Read( &m_scriptLevel, sizeof(m_scriptLevel) );
                pData->Read( &m_codeLevel, sizeof(m_codeLevel) );
                pData->Read( &m_hackLevel, sizeof(m_hackLevel) );

		uint8_t equipCount;
		pData->Read( &equipCount, sizeof(equipCount) );
		
		for ( uint8_t i = 0; i < equipCount; i++ )
		{
			// Equip slot
			uint8_t equipSlot;
		
			pData->Read( &equipSlot, sizeof(equipSlot) );

			if ( equipSlot >= EQUIP_SLOT_MAX )
			{
				printf( "Invalid equip slot reading equipped item on player.\n" );
				return (false);
			}

			if ( m_pEquipItems[equipSlot] )
			{
				printf( "Item already equipped in that slot while reading player data.\n" );
				return (false);
			}

			CItemInstance *pNewItem = new CItemInstance( NULL );

			if ( !pNewItem->ReadItemData( pData, bPlayerCache ) )
			{
				delete pNewItem;

				printf( "Exception reading item data.\n" );
				return (false);
			}

			m_pEquipItems[equipSlot] = pNewItem;
		}

		uint8_t inventoryCount;
		pData->Read( &inventoryCount, sizeof(inventoryCount) );

		m_itemsOnPlayer.DeleteAll();
		for ( uint8_t i = 0; i < inventoryCount; i++ )
		{
			CItemInstance *pNewItem = new CItemInstance( NULL );

			if ( !pNewItem->ReadItemData( pData, bPlayerCache ) ) 
			{
				delete pNewItem;

				printf( "Exception reading item data.\n" );
				return (false);
			}

			m_itemsOnPlayer.InsertTail( pNewItem );
		}
	}
	catch ( CStreamException e )
	{
		string sExceptionText;

		e.GetExceptionString( sExceptionText );
		printf( "StreamException (ReadPlayerData) caught: %s\n", sExceptionText.c_str() );

		return (false);
	}		

	return (true);
}

bool CPlayer::WritePlayerData( CStreamData *pData, bool bPlayerCache )
{
	if ( pData == NULL )
		return (false);

	try
	{
	// Write player data
	uint16_t playerNameLength = m_sPlayerName.length();
	pData->Write( &playerNameLength, sizeof(playerNameLength) );
	pData->Write( m_sPlayerName.c_str(), playerNameLength );

	uint16_t playerDescriptionLength = m_sPlayerDescription.length();
	pData->Write( &playerDescriptionLength, sizeof(playerDescriptionLength) );
	pData->Write( m_sPlayerDescription.c_str(), playerDescriptionLength );

	uint32_t roomVnum;
	if ( m_pCurrentRoom == NULL )
		roomVnum = INITIAL_ROOM_VNUM;
	else
		roomVnum = m_pCurrentRoom->GetVnum();
	
	pData->Write( &roomVnum, sizeof(uint32_t) );
		
	pData->Write( &m_curDiskSpace, sizeof(m_curDiskSpace) );
	pData->Write( &m_curDiskItems, sizeof(m_curDiskItems) );
	
	pData->Write( &m_maxCarryDisk, sizeof(m_maxCarryDisk) );
	pData->Write( &m_maxCarryItems, sizeof(m_maxCarryItems) );

	// write the player ID (if it is not player cache)
	if ( !bPlayerCache )
	{
		pData->Write( &m_playerID, sizeof(m_playerID) );
		pData->Write( &m_playerKey, sizeof(m_playerKey) );
	}

	pData->Write( &m_HP, sizeof(m_HP) );
	pData->Write( &m_CPU, sizeof(m_CPU) );
	pData->Write( &m_Memory, sizeof(m_Memory) );
		
	pData->Write( &m_MaxHP, sizeof(m_MaxHP) );
	pData->Write( &m_MaxCPU, sizeof(m_MaxCPU) );
	pData->Write( &m_MaxMemory, sizeof(m_MaxMemory) );

	pData->Write( &m_scriptLevel, sizeof(m_scriptLevel) );
	pData->Write( &m_codeLevel, sizeof(m_codeLevel) );
	pData->Write( &m_hackLevel, sizeof(m_hackLevel) );

	uint8_t equipCount = 0;
	for ( uint8_t i = 0; i < EQUIP_SLOT_MAX; i++ )
	{
		if ( m_pEquipItems[i] )
			equipCount++;
	}

	pData->Write( &equipCount, sizeof(equipCount) );
	for ( uint8_t i = 0; i < EQUIP_SLOT_MAX; i++ )
	{
		if ( m_pEquipItems[i] )
		{
			uint8_t equipSlotTemp = i;
			pData->Write( &equipSlotTemp, sizeof(equipSlotTemp) );

			if ( !m_pEquipItems[i]->WriteItemData( pData, bPlayerCache ) )
			{
				printf( "Exception writing item data.\n" );
				return (false);
			}
		}
	}

	uint8_t inventoryCount = 0;
	for ( CItemInstance *pItem = m_itemsOnPlayer.Head(); pItem; pItem = m_itemsOnPlayer.Next( pItem ) )
		inventoryCount++;

	pData->Write( &inventoryCount, sizeof(inventoryCount) );
	for ( CItemInstance *pItem = m_itemsOnPlayer.Head(); pItem; pItem = m_itemsOnPlayer.Next( pItem ) )
	{
		if ( !pItem->WriteItemData( pData, bPlayerCache ) )
		{
			printf( "Exception writing item data.\n" );
			return (false);
		}
	}

	}
	catch ( CStreamException e )
	{
		string sExceptionText;

		e.GetExceptionString( sExceptionText );
		printf( "StreamException (WritePlayerData) caught: %s\n", sExceptionText.c_str() );

		return (false);
	}

	return (true);
}

CPlayer *CPlayer::LoadFile( const char *pszPlayerName, bool bToRoom )
{
	CPlayer *pNewPlayer;
	char szLine[1024];
	char szTemp[1024];
	char szEquipSlot[1024];
	bool bReset = false;
	long int lastOffset;
	CItemInstance *pEquipItem;
	tItemEquipType equipSlot;
	string sEquipSlot;

	// Attempt to load a player file from disk
	if ( !CheckPlayerName( pszPlayerName ) )	
	{
		printf( "Bad player name!\n" );
		return (NULL);
	}

	// create file name
	char szFileName[1024];

	snprintf( szFileName, 1024, "%s%s.%s", PLAYER_FILE_DIRECTORY, pszPlayerName, PLAYER_FILE_EXT );

	// Attempt to open file...
	FILE *pFile;

	pFile = fopen( szFileName, "r" );

	if ( pFile == NULL )
	{
		printf( "Couldn't find player file: %s\n", szFileName );
		return (NULL);	// player not found
	}

	pNewPlayer = new CPlayer();

	// Load data
	string sPlayerName;
 
	READ_FILE_STRING( "Name", sPlayerName )

	if ( strcmp( pszPlayerName, sPlayerName.c_str() ) != 0 )
	{
		printf( "[%s][%s]\n", pszPlayerName, sPlayerName.c_str() );	
		printf( "Critical error in pfile: %s -- player name and file name did not match.\n", szFileName );

		// Player name and file name did not match!!
		delete pNewPlayer;
		return (NULL);
	}

	pNewPlayer->m_sPlayerName = sPlayerName;

	READ_FILE_STRING( "PasswordHash", pNewPlayer->m_sPasswordHash );
	READ_FILE_STRING( "Description", pNewPlayer->m_sPlayerDescription );


	uint32_t roomNumber;
	READ_FILE_UINT32( "Room", roomNumber );

	// Find room!
	CRoom *pRoom;

	pRoom = g_oRoomHash.Find( roomNumber );

	if ( pRoom == NULL )
	{
		printf( "Player room not found -- placing in initial room.\n" );

		// Put them in initial room
		pRoom = g_oRoomHash.Find( INITIAL_ROOM_VNUM );

		if ( pRoom == NULL )
		{
			printf( "Critical error, can't place player in initial room.\n" );
			// goto bad_fileread;
		}
	}

	if ( bToRoom )
		pNewPlayer->SetRoom( pRoom );

	READ_FILE_UINT16( "HP", pNewPlayer->m_HP );
	READ_FILE_UINT16( "CPU", pNewPlayer->m_CPU );
	READ_FILE_UINT16( "Memory", pNewPlayer->m_Memory );
	
	READ_FILE_UINT16( "MAXHP", pNewPlayer->m_MaxHP );
	READ_FILE_UINT16( "MAXCPU", pNewPlayer->m_MaxCPU );
	READ_FILE_UINT16( "MAXMemory", pNewPlayer->m_MaxMemory );

	READ_FILE_UINT8( "ScriptLevel", pNewPlayer->m_scriptLevel );
	READ_FILE_UINT8( "CodeLevel", pNewPlayer->m_codeLevel );
	READ_FILE_UINT8( "HackLevel", pNewPlayer->m_hackLevel );

	READ_FILE_UINT8( "MaxItems", pNewPlayer->m_maxCarryItems );
	READ_FILE_UINT16( "MaxDisk", pNewPlayer->m_maxCarryDisk );
	
	READ_FILE_UINT8( "CurItems", pNewPlayer->m_curDiskItems );
	READ_FILE_UINT16( "CurDisk", pNewPlayer->m_curDiskSpace );

	lastOffset = ftell( pFile );
	for ( ;; )
	{
		if ( feof( pFile ) )
			break;

		if ( fgets( szLine, 1024, pFile ) == NULL )
			break;

		if ( sscanf( szLine, "%s %s\n", szTemp, szEquipSlot ) != 2 )
		{
			bReset = true;
			break;
		}	

		if ( strcasecmp( szTemp, "equip" ) != 0 )
		{
			bReset = true;
			break;
		}

		sEquipSlot = szEquipSlot;

		if ( !CItemInstance::GetEquipTypeFromString( sEquipSlot, equipSlot ) )
		{
			printf( "Invalid equip slot in pfile: %s\n", sEquipSlot.c_str() );
			goto bad_fileread;
		}

		if ( pNewPlayer->m_pEquipItems[equipSlot] )
		{
			printf( "Player file attempting to equip two items in one slot: %s\n", sEquipSlot.c_str() );
			goto bad_fileread;
		} 
		
		pEquipItem = CItemInstance::LoadFromFile( pFile );

		if ( pEquipItem == NULL )
			goto bad_fileread;

		pNewPlayer->m_pEquipItems[equipSlot] = pEquipItem;

		// Reset last offset	
		lastOffset = ftell( pFile );
	}

	if ( bReset )
		fseek( pFile, lastOffset, SEEK_SET );

	if ( feof( pFile ) )
		goto read_complete;

	if ( fgets( szLine, 1024, pFile ) == NULL )
		goto bad_fileread;

	if ( sscanf( szLine, "%s\n", szTemp ) != 1 )
		goto bad_fileread;

	if ( strcasecmp( szTemp, "inventory" ) != 0 )
		goto bad_fileread;

	for ( ;; )
	{
		if ( feof( pFile ) )
			break;

		pEquipItem = CItemInstance::LoadFromFile( pFile );

		if ( pEquipItem == NULL )
			break;

		// Add to inventory
		pNewPlayer->m_itemsOnPlayer.InsertTail( pEquipItem );		
	}

read_complete:

	// Now load inventory items!
	fclose( pFile );

	return pNewPlayer;

bad_fileread:
	// ERROR -- bad player file
	printf( "Critical error, corrupt pfile %s\n", szFileName );

	delete pNewPlayer;

	fclose( pFile );

	return (NULL);
}

bool CPlayer::SaveFile( void )
{
#if ENABLE_PLAYER_LOGIN_CACHE
	CPlayerLoginCache *pLoginCache = g_oPlayerLoginCache.Find( GetName() );

	if ( pLoginCache )
	{
		CStreamData *pNewStream = new CStreamData();
		
		if ( !WritePlayerData( pNewStream, true ) )
			delete pNewStream;
		else
			pLoginCache->SaveNewStream( pNewStream );
	}
#endif

#if ENABLE_PLAYER_SAVE

#else
	return (false);
#endif

	// create file name
	char szFileName[1024];

	snprintf( szFileName, 1024, "%s%s.%s", PLAYER_FILE_DIRECTORY, m_sPlayerName.c_str(), PLAYER_FILE_EXT );

	// Attempt to open file...
	FILE *pFile;

	pFile = fopen( szFileName, "w" );

	if ( pFile == NULL )
	{
		printf( "Couldn't open player file for writing: %s\n", szFileName );
		return (false);
	}

	// Save data
	WRITE_FILE_STRING( "Name", m_sPlayerName )
	WRITE_FILE_STRING( "PasswordHash", m_sPasswordHash );
	WRITE_FILE_STRING( "Description", m_sPlayerDescription );


	uint32_t roomNumber = m_pCurrentRoom->GetVnum();
	WRITE_FILE_UINT32( "Room", roomNumber );

	WRITE_FILE_UINT16( "HP", m_HP );
	WRITE_FILE_UINT16( "CPU", m_CPU );
	WRITE_FILE_UINT16( "Memory", m_Memory );

	WRITE_FILE_UINT16( "MAXHP", m_MaxHP );
	WRITE_FILE_UINT16( "MAXCPU", m_MaxCPU );
	WRITE_FILE_UINT16( "MAXMemory", m_MaxMemory );


	WRITE_FILE_UINT8( "ScriptLevel", m_scriptLevel );
	WRITE_FILE_UINT8( "CodeLevel", m_codeLevel );
	WRITE_FILE_UINT8( "HackLevel", m_hackLevel );

	WRITE_FILE_UINT8( "MaxItems", m_maxCarryItems );
	WRITE_FILE_UINT16( "MaxDisk", m_maxCarryDisk );

	WRITE_FILE_UINT8( "CurItems", m_curDiskItems );
	WRITE_FILE_UINT16( "CurDisk", m_curDiskSpace );

	for ( uint8_t i = 0; i < EQUIP_SLOT_MAX; i++ )
	{
		if ( !m_pEquipItems[i] )
			continue;

		CItemInstance *pItem = m_pEquipItems[i];
		string sEquipSlot;

		if ( !CItemInstance::GetEquipStringFromType( (tItemEquipType)i, sEquipSlot ) )
		{
			printf( "Critical error -- unable to get equip slot string from slot %d\n", i );
			goto bad_filewrite;
		}	

		WRITE_FILE_STRING( "Equip", sEquipSlot );

		if ( !pItem->SaveToFile( pFile ) )
		{
			printf( "Failed to save item in slot %s to player file.\n", sEquipSlot.c_str() );
			goto bad_filewrite;
		}
	}

	// Write out items in inventory
	if ( !m_itemsOnPlayer.Empty() )
	{
		fprintf( pFile, "INVENTORY\n" );

		CItemInstance *pItem;
		for ( pItem = m_itemsOnPlayer.Head(); pItem; pItem = m_itemsOnPlayer.Next( pItem ) )
		{
			if ( !pItem->SaveToFile( pFile ) )
			{
				printf( "Failed to save item in inventory.\n" );
				goto bad_filewrite;
			}
		}		
	}
	
	fclose( pFile );

	return (true);

bad_filewrite:
	// ERROR -- bad player file
	printf( "Critical error, could not write pfile %s\n", szFileName );

	fclose( pFile );

	return (false);
}

bool CPlayer::GetPasswordOnly( const char *pszPlayerName, string &sPassword )
{
	char szLine[1024];
	char szTemp[1024];
	char szEquipSlot[1024];
	bool bReset = false;
	long int lastOffset;
	CItemInstance *pEquipItem;
	tItemEquipType equipSlot;
	string sEquipSlot;

	// Attempt to load a player file from disk
	if ( !CheckPlayerName( pszPlayerName ) )	
	{
		printf( "Bad player name!\n" );
		return (false);
	}

	// create file name
	char szFileName[1024];

	snprintf( szFileName, 1024, "%s%s.%s", PLAYER_FILE_DIRECTORY, pszPlayerName, PLAYER_FILE_EXT );

	// Attempt to open file...
	FILE *pFile;

	pFile = fopen( szFileName, "r" );

	if ( pFile == NULL )
	{
		printf( "Couldn't find player file: %s\n", szFileName );
		return (false);	// player not found
	}

	// Load data
	string sPlayerName;
 
	READ_FILE_STRING( "Name", sPlayerName )

	if ( strcmp( pszPlayerName, sPlayerName.c_str() ) != 0 )
	{
		printf( "[%s][%s]\n", pszPlayerName, sPlayerName.c_str() );	
		printf( "Critical error in pfile: %s -- player name and file name did not match.\n", szFileName );

		// Player name and file name did not match!!
		return (NULL);
	}
	
	READ_FILE_STRING( "PasswordHash", sPassword );

	fclose( pFile );

	return true;

bad_fileread:
	// ERROR -- bad player file
	printf( "Critical error, corrupt pfile %s\n", szFileName );

	fclose( pFile );

	return (false);
}

void CPlayer::SetRoom( CRoom *pRoom )
{
	if ( !pRoom )
		return;

	// Puts a player in a room -- but doesn't send any notifications
	if ( m_pCurrentRoom )
		m_pCurrentRoom->RemovePlayer( this );
	
	m_pCurrentRoom = pRoom;

	if ( m_pCurrentRoom )
		m_pCurrentRoom->AddPlayer( this );

	// Clear any attacking targets
	m_attackTargets.DeleteAll();
}

bool CPlayer::EquipItem( CItemInstance *pItem, tItemEquipType slotEquipped, string &sFailReason )
{
	if ( pItem == NULL )
	{
		sFailReason = "no item";
		return (false);
	}

	if ( pItem->GetItemType() != eItemEquip )
	{
		sFailReason = "that item cannot be equipped";
		return (false);
	}

	if ( m_pEquipItems[slotEquipped] )
	{
		sFailReason = "item already equipped in that slot";
		return (false);
	}

	if ( pItem->GetEquipType() != slotEquipped )
	{
		sFailReason = "item can't be equipped there";
		return (false);
	}

	// Check levels...
	if ( m_scriptLevel < pItem->GetScriptLevel() )
	{
		sFailReason = "script level too low";
		return (false);
	}

	if ( m_codeLevel < pItem->GetCodeLevel() )
	{
		sFailReason = "code level too low";
		return (false);
	}

	if ( m_hackLevel < pItem->GetHackLevel() )
	{
		sFailReason = "hack level too low";
		return (false);
	}

	if ( !RemoveItemFromInventory( pItem, false ) )
	{
		sFailReason = "item not in inventory";
		return (false);
	}


	//
	// Equip item!!!
	m_pEquipItems[slotEquipped] = pItem;

	m_MaxHP += pItem->GetHPModifier();
	m_MaxCPU += pItem->GetCPUModifier();
	m_MaxMemory += pItem->GetMemoryModifier(); 

	// It is possible (but not likely) these could wrap-around!
        // Adjust stats 
        if ( m_HP > m_MaxHP )
                m_HP = m_MaxHP;

        if ( m_CPU > m_MaxCPU )
                m_CPU = m_MaxCPU;

        if ( m_Memory > m_MaxMemory )
                m_Memory = m_MaxMemory;
	
	// Save
	SaveFile();
	
	// Check for 
	return (true);
}

bool CPlayer::UnequipItem( tItemEquipType slot, CItemInstance *&pItem, string &sFailReason )
{
	if ( !m_pEquipItems[slot] )
	{
		sFailReason = "no item equipped there";
		return (false);
	}

	if ( m_curDiskSpace + m_pEquipItems[slot]->GetDiskUsage() > m_maxCarryDisk )
	{
		sFailReason = "no disk space available on your avatar";
		return (false);
	}

	if ( m_curDiskItems+1 > m_maxCarryItems )
	{
		sFailReason = "no more disk slots available on your avatar";
		return (false);
	}

	if ( !AddItemToInventory( m_pEquipItems[slot], false ) )
	{
		sFailReason = "can't add item into inventory";
		return (false);
	}

	// Unequip item...
	pItem = m_pEquipItems[slot];
	m_pEquipItems[slot] = NULL;

	m_MaxHP -= pItem->GetHPModifier();
	m_MaxCPU -= pItem->GetCPUModifier();
	m_MaxMemory -= pItem->GetMemoryModifier();

	// Adjust stats!
	if ( m_HP > m_MaxHP )
		m_HP = m_MaxHP;
	
	if ( m_CPU > m_MaxCPU )
		m_CPU = m_MaxCPU;
	
	if ( m_Memory > m_MaxMemory )
		m_Memory = m_MaxMemory;


	// Save
	SaveFile();

	return (true);
}

bool CPlayer::RemoveItemFromInventory( CItemInstance *pItemToRemove, bool bSaveFile )
{
        // Verify
        bool bFound = false;
        for ( CItemInstance *pItem = m_itemsOnPlayer.Head(); pItem; pItem = m_itemsOnPlayer.Next( pItem ) )
	{
                if ( pItem == pItemToRemove )
                {
                        bFound = true;
                        break;
                }
        }

        if ( !bFound )
                return (false);

        pItemToRemove->m_playerLink.Unlink();

	if ( pItemToRemove->GetDiskUsage() > m_curDiskSpace )
		m_curDiskSpace = 0; // Strange situation! -- Just set to zer0
	else	
		m_curDiskSpace -= pItemToRemove->GetDiskUsage();

	if ( m_curDiskItems > 0 )
		m_curDiskItems--; // Keep it at zer0


	// Save File
	if ( bSaveFile )
		SaveFile();

        return (true);
}

bool CPlayer::AddItemToInventory( CItemInstance *pItemToAdd, bool bSaveFile )
{
	if ( !pItemToAdd )
		return (false);

	if ( m_curDiskSpace + pItemToAdd->GetDiskUsage() > m_maxCarryDisk )
		return (false);

	if ( m_curDiskItems+1 > m_maxCarryItems )
		return (false);

	m_itemsOnPlayer.InsertHead( pItemToAdd );
	
	m_curDiskSpace += pItemToAdd->GetDiskUsage();
	m_curDiskItems++;

	// Save File
	if ( bSaveFile )
		SaveFile();

	return (true);
}

CItemInstance *CPlayer::GetItemFromInventoryByID( uint32_t itemID )
{
        CItemInstance *pMatch = NULL;

        for ( CItemInstance *pItem = m_itemsOnPlayer.Head(); pItem; pItem = m_itemsOnPlayer.Next( pItem ) )
        {
                if ( pItem->GetItemID() == itemID )
                {
                        pMatch = pItem;
                        break;
                }
        }

        return (pMatch);
}

void CPlayer::StopFighting( void )
{
	m_attackTargets.DeleteAll();
}

bool CPlayer::IsFighting( void )
{
        if ( m_attackTargets.Empty() )
                return (false);
        else
                return (true);
}

bool CPlayer::AttackNPC( uint32_t npcID )
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

bool CPlayer::AttackPlayer( uint32_t playerID )
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

bool CPlayer::IsFightingTarget( uint32_t targetID, uint8_t targetType )
{
	for ( CAttackTarget *pCur = m_attackTargets.Head(); pCur; pCur = m_attackTargets.Next( pCur ) )
	{
		if ( pCur->GetTargetType() == targetType && pCur->GetTargetID() == targetID )
			return (true);
	}

	return (false);
}

bool CPlayer::StopFightingTarget( uint32_t targetID, uint8_t targetType )
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

bool CPlayer::Kill( uint32_t killerID, string sKillerName, uint8_t killerType )
{
	uint32_t i;
        // Kill this NPC!!!

        // Inform everyone in room
        CRoom *pRoom = GetRoom();

        for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
        {
                if ( pCur->IsConnected() && pCur != this )
                        NWCPlayerKilledResponse( pCur, GetPlayerID(), NWC_ATTACK_PLAYER, killerID, sKillerName, killerType );
        }

	bool bHasItems = false;

	
	// Check for an exit that drops on PK
	for ( i = 0; i < EQUIP_SLOT_MAX; i++ )
	{
		if ( m_pEquipItems[i] && m_pEquipItems[i]->HasFlagDropOnPK() )
		{
			// Item will drop on PK
			CItemInstance *pItemToDrop = m_pEquipItems[i];
			m_pEquipItems[i] = NULL;

			// Adjust stats for unequipping item
			m_MaxHP -= pItemToDrop->GetHPModifier();
			m_MaxCPU -= pItemToDrop->GetCPUModifier();
			m_MaxMemory -= pItemToDrop->GetMemoryModifier();
			
			NWCDropItemToRoom( pRoom, pItemToDrop );

			bHasItems = true;
		}
	}

	CItemInstance *pNextItem = NULL;
	for ( CItemInstance *pItem = m_itemsOnPlayer.Head(); pItem; pItem = pNextItem )
	{
		pNextItem = m_itemsOnPlayer.Next( pItem );
		if ( pItem->HasFlagDropOnPK() )
		{
			CItemInstance *pItemToDrop = pItem;

			if ( RemoveItemFromInventory( pItemToDrop ) )
			{	
				NWCDropItemToRoom( pRoom, pItemToDrop );

				bHasItems = true;
			}
		}
	}

	// Set stats to 10% of normal!
	SetHP( GetMaxHP() * 0.1 );
	SetCPU( GetMaxCPU() * 0.1 );
	SetMemory( GetMaxMemory() * 0.1 );

	// RESPAWN at a spawn point
	if ( g_oPlayerSpawnPoints.size() > 0 )
	{
		// Pick a random spawn point
		CRoom *pSpawnRoom = g_oRoomHash.Find( g_oPlayerSpawnPoints[g_oRNG.GetRange( 0, g_oPlayerSpawnPoints.size()-1 )] );

		if ( pSpawnRoom )
			SetRoom( pSpawnRoom );
		else
			SetRoom( g_oRoomHash.Find( INITIAL_ROOM_VNUM ) );	// Default room!
	}
	else	
		SetRoom( g_oRoomHash.Find( INITIAL_ROOM_VNUM ) );	// Default room!

	// Do player death
	NWCPlayerDeath( this );

	// Save
	SaveFile();

	return (true);	
}

uint8_t *CPlayer::GetItemFingerprint( uint32_t &outLen )
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

void CPlayer::DoExit( void )
{
	bool bHasItems = false;
	CRoom *pRoom = GetRoom();

	// Check for an exit that drops on PK
        for ( uint32_t i = 0; i < EQUIP_SLOT_MAX; i++ )
        {
                if ( m_pEquipItems[i] && m_pEquipItems[i]->HasFlagDropOnExit() )
                {
                        // Item will drop on PK
                        CItemInstance *pItemToDrop = m_pEquipItems[i];
                        m_pEquipItems[i] = NULL;

                        // Adjust stats for unequipping item
                        m_MaxHP -= pItemToDrop->GetHPModifier();
                        m_MaxCPU -= pItemToDrop->GetCPUModifier();
                        m_MaxMemory -= pItemToDrop->GetMemoryModifier();

			if ( pRoom )
                        	NWCDropItemToRoom( pRoom, pItemToDrop );
			else
				delete pItemToDrop; // Destroy item.

                        bHasItems = true;
                }
        }
	
	CItemInstance *pNextItem = NULL;
	for ( CItemInstance *pItem = m_itemsOnPlayer.Head(); pItem; pItem = pNextItem )
	{
		pNextItem = m_itemsOnPlayer.Next( pItem );
		if ( pItem->HasFlagDropOnPK() )
		{
			CItemInstance *pItemToDrop = pItem;

			// Remove item -- don't save -- we will save later in this function
			if ( RemoveItemFromInventory( pItemToDrop, false ) )
			{
				if ( pRoom )
                        		NWCDropItemToRoom( pRoom, pItemToDrop );
				else
					delete pItemToDrop; // Destroy item.
			}

			bHasItems = true;
		}
	}
	
	// Adjust stats!
	if ( m_HP > m_MaxHP )
		m_HP = m_MaxHP;
	
	if ( m_CPU > m_MaxCPU )
		m_CPU = m_MaxCPU;
	
	if ( m_Memory > m_MaxMemory )
		m_Memory = m_MaxMemory;

	// Save
	SaveFile();
}
