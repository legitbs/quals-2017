#include "common.h"

tGlobalRoomHash g_oRoomHash( ROOM_HASH_SIZE );

CRoom::CRoom( )
	: CRoomVnum( NO_ROOM_VNUM )
{
	m_flags = 0;
}

CRoom::CRoom( string sName, string sShortDescription, string sDescription, uint32_t roomVnum )
	: CRoomVnum( roomVnum )
{
	m_sName = sName;
	m_sShortDescription = sShortDescription;
	m_sDescription = sDescription;

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

		pData->Write( &m_flags, sizeof(m_flags) );

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

void CRoom::GetExitList( string &sExitList )
{
	if ( GetNorth().GetVnum() != NO_ROOM_VNUM )
		sExitList += "north ";

	if ( GetEast().GetVnum() != NO_ROOM_VNUM )
		sExitList += "east ";
	
	if ( GetSouth().GetVnum() != NO_ROOM_VNUM )
		sExitList += "south ";
	
	if ( GetWest().GetVnum() != NO_ROOM_VNUM )
		sExitList += "west ";

	if ( GetUp().GetVnum() != NO_ROOM_VNUM )
		sExitList += "up ";
	
	if ( GetDown().GetVnum() != NO_ROOM_VNUM )
		sExitList += "down "; 
}

bool CRoom::RemoveItemFromRoom( uint32_t itemID )
{
	CItemInRoom *pItem;
	for ( pItem = m_itemsInRoom.Head(); pItem; pItem = m_itemsInRoom.Next( pItem ) )
	{
		if ( pItem->GetID() == itemID )
		{
			delete pItem;
			return (true);
		}
	}

	return (false);
}

CPlayerInRoom *CRoom::GetPlayerInRoomByID( uint32_t id )
{
	for ( CPlayerInRoom *pCur = m_playersInRoom.Head(); pCur; pCur = m_playersInRoom.Next( pCur ) )
	{
		if ( pCur->GetID() == id )
			return (pCur);
	}
	
	return (NULL);
}

CNPCInRoom *CRoom::GetNPCInRoomByID( uint32_t id )
{
	for ( CNPCInRoom *pCur = m_npcsInRoom.Head(); pCur; pCur = m_npcsInRoom.Next( pCur ) )
	{
		if ( pCur->GetID() == id )
			return (pCur);
	}

	return (NULL);
}

CItemInRoom *CRoom::GetItemInRoomByID( uint32_t id )
{
	for ( CItemInRoom *pCur = m_itemsInRoom.Head(); pCur; pCur = m_itemsInRoom.Next( pCur ) )
	{
		if ( pCur->GetID() == id )
			return (pCur);
	}

	return (NULL);
}
