#include "common.h"

tGlobalRoomHash g_oRoomHash( ROOM_HASH_SIZE );
tGlobalSpawnPoints g_oPlayerSpawnPoints;
tGlobalRoomList g_oWorldRoomList;

CRoom::CRoom( string sName, string sShortDescription, string sDescription, uint32_t roomVnum )
	: CRoomVnum( roomVnum )
{
	m_sName = sName;
	m_sShortDescription = sShortDescription;
	m_sDescription = sDescription;

	m_zoneNum = 0;
	m_flags = 0;
}

CRoom::~CRoom( )
{

}

bool CRoom::ReadRoomData( CStreamData *pData )
{
#if SERVER
        return (false);
#else
        if ( pData == NULL )
                return (false);

        try
        {
                char *pTempData;

                uint16_t roomNameLength;
                pData->Read( &roomNameLength, sizeof(roomNameLength) );
                pTempData = new char[roomNameLength+1];
                pData->Read( pTempData, roomNameLength );
                pTempData[roomNameLength] = '\0';

                m_sName = pTempData;
                delete pTempData;

                uint16_t roomShortLength;
                pData->Read( &roomShortLength, sizeof(roomShortLength) );
                pTempData = new char[roomShortLength+1];
                pData->Read( pTempData, roomShortLength );
                pTempData[roomShortLength] = '\0';

                m_sShortDescription = pTempData;
                delete pTempData;

                uint16_t roomDescriptionLength;
                pData->Read( &roomDescriptionLength, sizeof(roomDescriptionLength) );
                pTempData = new char[roomDescriptionLength+1];
                pData->Read( pTempData, roomDescriptionLength );
                pTempData[roomDescriptionLength] = '\0';

                m_sDescription = pTempData;
                delete pTempData;

		uint32_t roomVnum;
		pData->Read( &roomVnum, sizeof(roomVnum) );
		SetVnum( roomVnum );

		pData->Read( &m_flags, sizeof(m_flags) );

		uint32_t neighbors[DIRECTION_MAX];

		for ( uint8_t i = 0; i < DIRECTION_MAX; i++ )
		{
			pData->Read( &(neighbors[i]), sizeof(neighbors[i]) );
       			m_neighbors[i] = neighbors[i]; 
		}
	}
        catch ( CStreamException e )
        {
                string sExceptionText;

                e.GetExceptionString( sExceptionText );
                printf( "StreamException (read) caught: %s\n", sExceptionText.c_str() );

                return (false);
        }

        return (true);
#endif
}

bool CRoom::WriteRoomData( CStreamData *pData )
{
#if SERVER
        if ( pData == NULL )
                return (false);

        try
        {
		uint16_t nameLength, shortLength, descriptionLength;

                nameLength = GetName().length();
                pData->Write( &nameLength, sizeof(nameLength) );
                pData->Write( GetName().c_str(), nameLength );

                shortLength = GetShortDescription().length();
                pData->Write( &shortLength, sizeof(shortLength) );
                pData->Write( GetShortDescription().c_str(), shortLength );

                descriptionLength = GetDescription().length();
                pData->Write( &descriptionLength, sizeof(descriptionLength) );
                pData->Write( GetDescription().c_str(), descriptionLength );
               
		uint32_t vnum = GetVnum(); 
		pData->Write( &vnum, sizeof(vnum) );

		uint8_t roomFlags = m_flags;
		pData->Write( &roomFlags, sizeof(roomFlags) );

		for ( uint8_t i = 0; i < DIRECTION_MAX; i++ )
		{
			uint32_t dirVnum = m_neighbors[i].GetVnum();

			pData->Write( &dirVnum, sizeof(dirVnum) );
		}

        }
        catch ( CStreamException e )
        {
                string sExceptionText;

                e.GetExceptionString( sExceptionText );
                printf( "StreamException (write) caught: %s\n", sExceptionText.c_str() );

                return (false);
        }

        return (true);
#else
        return (false); // Client doesn't have this function
#endif
}

void CRoom::LoadRooms( void )
{
	// Load rooms from disk
	char szFileName[1024];
	char szLine[1024];

	snprintf( szFileName, 1024, "%s/%s", ROOM_FILE_DIRECTORY, ROOM_FILE_NAME );

	FILE *pFile;

	pFile = fopen( szFileName, "r" );

	if ( !pFile )
	{
		printf( "Can't load rooms from disk: %s\n", szFileName );
		return;
	}

	do
	{
		string sName;
		string sShortDescription;
		string sDescription;

		string sFlags;

		uint32_t directions[6];
		uint32_t vnum;
		uint32_t zoneNum;
		uint8_t flags;

		// Read in room data...
		READ_FILE_STRING( "Name", sName )
		READ_FILE_STRING( "ShortDescription", sShortDescription )
		READ_FILE_STRING( "Description", sDescription )
		READ_FILE_UINT32( "Vnum", vnum )
		READ_FILE_UINT32( "Zone", zoneNum );
		
		READ_FILE_STRING( "Flags", sFlags );

		vector<string> sFlagVector = SplitString( sFlags );

		flags = 0;
		for ( uint8_t i = 0; i < sFlagVector.size(); i++ )
		{
			if ( strcasecmp( "pk", sFlagVector[i].c_str() ) == 0 )
				flags |= ROOM_FLAG_PK;
			else if ( strcasecmp( "pcspawn", sFlagVector[i].c_str() ) == 0 )
				flags |= ROOM_FLAG_PLAYER_SPAWN;
			else if ( strcasecmp( "none", sFlagVector[i].c_str() ) == 0 )
				;
			else
			{
				printf( "Error loading flags for room (%d): %s\n", vnum, sFlags.c_str() );
				goto bad_fileread;
			}
		}
	
		directions[0] = 0;
		directions[1] = 0;
		directions[2] = 0;
		directions[3] = 0;
		directions[4] = 0;
		directions[5] = 0;

		long int lastOffset = ftell( pFile );

		bool bResetLocation = false;
		for ( uint8_t i = 0; i < 6; i++ )
		{
			char szTemp[1024];
			uint32_t tempDirection;

			if ( feof( pFile ) )
				break;
	
			if ( fgets( szLine, 1024, pFile ) == NULL )
				break;

			if ( sscanf( szLine, "%s %ul", szTemp, &tempDirection ) != 2 )
				break;

			if ( strcasecmp( szTemp, "north" ) == 0 )
				directions[DIRECTION_NORTH] = tempDirection;
			else if ( strcasecmp( szTemp, "east" ) == 0 )
				directions[DIRECTION_EAST] = tempDirection;
			else if ( strcasecmp( szTemp, "south" ) == 0 )
				directions[DIRECTION_SOUTH] = tempDirection;
			else if ( strcasecmp( szTemp, "west" ) == 0 )
				directions[DIRECTION_WEST] = tempDirection;
			else if ( strcasecmp( szTemp, "up" ) == 0 )
				directions[DIRECTION_UP] = tempDirection;
			else if ( strcasecmp( szTemp, "down" ) == 0 )
				directions[DIRECTION_DOWN] = tempDirection;
			else
			{
				bResetLocation = true;
				break;
			}

			lastOffset = ftell( pFile );
		}

		fseek( pFile, lastOffset, SEEK_SET );

		if ( g_oRoomHash.Find( vnum ) )
		{
			// Duplicate
			printf( "Duplicate room %d found, skipping.\n", vnum );
			continue;
		}

		CRoom *pNewRoom = new CRoom( sName, sShortDescription, sDescription, vnum );

		pNewRoom->m_zoneNum = zoneNum;
		pNewRoom->m_flags = flags;

		pNewRoom->m_neighbors[0] = directions[0];
		pNewRoom->m_neighbors[1] = directions[1];
		pNewRoom->m_neighbors[2] = directions[2];
		pNewRoom->m_neighbors[3] = directions[3];
		pNewRoom->m_neighbors[4] = directions[4];
		pNewRoom->m_neighbors[5] = directions[5];

		printf( "New room: %s created directions[%d][%d][%d][%d][%d][%d]\n", sName.c_str(), directions[0], directions[1], directions[2], directions[3], directions[4], directions[5] );
		
		g_oRoomHash.Add( pNewRoom );

		g_oWorldRoomList.InsertTail( pNewRoom );

		if ( pNewRoom->IsPlayerSpawnPoint() )
			g_oPlayerSpawnPoints.push_back( vnum );

	} while ( !feof( pFile ) );

	return;

bad_fileread:
	printf( "Critical error reading rooms.txt file.\n" );
	return;
}

bool CRoom::RemovePlayer( CPlayer *pPlayer )
{
	if ( pPlayer )
	{
		if ( pPlayer->m_roomLink.IsLinked() )
			pPlayer->m_roomLink.Unlink();
		else
			return (false);

		return (true);	
	}

	return (false);
}

bool CRoom::AddPlayer( CPlayer *pPlayer )
{
	if ( pPlayer )
	{
		if ( pPlayer->m_roomLink.IsLinked() )
			return (false);

		m_playersInRoom.InsertHead( pPlayer );

		return (true);
	}

	return (false);
}

bool CRoom::AddNPC( CNPC *pNPC )
{
	if ( !pNPC )
		return (false);

	if ( pNPC->m_roomLink.IsLinked() )
		return (false);

	m_npcsInRoom.InsertHead( pNPC );

	return (true);
}

bool CRoom::RemoveNPC( CNPC *pNPC )
{
	if ( !pNPC )
		return (false);

	if ( !pNPC->m_roomLink.IsLinked() )
	{
		printf( "NPC is not linked???\n" );
		return (false);
	}

	pNPC->m_roomLink.Unlink();

	return (true);
}

CItemInstance *CRoom::GetItemInRoomByID( uint32_t itemID )
{
	// Find an item in the room by id...
	CItemInstance *pMatchItem = NULL;

	for ( CItemInstance *pItem = m_itemsInRoom.Head(); pItem; pItem = m_itemsInRoom.Next( pItem ) )
	{
		if ( pItem->GetItemID() == itemID )
		{
			pMatchItem = pItem;
			break;
		}
	}

	return (pMatchItem);
}

bool CRoom::AddItemToRoom( CItemInstance *pItem )
{
	if ( !pItem )
		return (false);

	if ( pItem->m_roomLink.IsLinked() )
		return (false);

	m_itemsInRoom.InsertHead( pItem );

	// Keep the number of items in the room from accumulating
	uint32_t itemCount = 0;

	for ( CItemInstance *pCur = (CItemInstance *)m_itemsInRoom.Head(); pCur; pCur = (CItemInstance *)m_itemsInRoom.Next( pCur ) )
		itemCount++;

	if ( itemCount > MAX_ITEMS_IN_ROOM )
	{
		CItemInstance *pItemToDelete = (CItemInstance *)m_itemsInRoom.Tail();

		string sDeleteMessage = "is consumed by the server cleanup script";
		NWCDeleteItemInRoom( this, pItemToDelete, sDeleteMessage );

		delete pItemToDelete;
	}
	

	return (true);	
}

bool CRoom::GetRandomDirection( uint8_t &chosenDirection )
{
	uint8_t randomDirectionsTotal = 0;
	uint8_t i = 0;

	for ( i = 0; i < DIRECTION_MAX; i++ )
	{
		if ( HasDirection( i ) )
			randomDirectionsTotal++;
	}

	if ( randomDirectionsTotal == 0 )
		return false;

	uint8_t randomDirectionChoice = g_oRNG.GetRange( 0, randomDirectionsTotal-1 );

	for ( i = 0; i < DIRECTION_MAX; i++ )
	{
		if ( HasDirection( i ) )
		{
			if ( randomDirectionChoice == 0 )
			{
				chosenDirection = i;
				return (true);
			}	
			else
				randomDirectionChoice--;
		}	
	}

	return (false);
}

CPlayer *CRoom::GetPlayerInRoomByID( uint32_t id )
{
        for ( CPlayer *pCur = m_playersInRoom.Head(); pCur; pCur = m_playersInRoom.Next( pCur ) )
        {
                if ( pCur->GetPlayerID() == id )
                        return (pCur);
        }

        return (NULL);
}

CNPC *CRoom::GetNPCInRoomByID( uint32_t id )
{
        for ( CNPC *pCur = m_npcsInRoom.Head(); pCur; pCur = m_npcsInRoom.Next( pCur ) )
        {
                if ( pCur->GetNPCID() == id )
                        return (pCur);
        }

        return (NULL);
}
