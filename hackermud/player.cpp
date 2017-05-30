#include "common.h"

CPlayer::CPlayer( )
{
	uint64_t newKey;

	newKey = getpid();
	newKey = newKey << 32 + getpid();

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

bool CPlayer::ReadPlayerData( CStreamData *pData )
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

		pData->Read( &m_curDiskSpace, sizeof(m_curDiskSpace) );
                pData->Read( &m_curDiskItems, sizeof(m_curDiskItems) );

		pData->Read( &m_maxCarryDisk, sizeof(m_maxCarryDisk) );
		pData->Read( &m_maxCarryItems, sizeof(m_maxCarryItems) );

		// Get ID
		pData->Read( &m_playerID, sizeof(m_playerID) );
		pData->Read( &m_playerKey, sizeof(m_playerKey) );

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
                                printf( "Invalid equip slot (%d) reading equipped item on player.\n", equipSlot );
                                return (false);
                        }

                        if ( m_pEquipItems[equipSlot] )
                        {
                                printf( "Item already equipped in that slot while reading player data.\n" );
                                return (false);
                        }

                        CItemInstance *pNewItem = new CItemInstance( );

                        if ( !pNewItem->ReadItemData( pData ) )
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
                        CItemInstance *pNewItem = new CItemInstance( );

                        if ( !pNewItem->ReadItemData( pData ) )
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
		printf( "StreamException caught: %s\n", sExceptionText.c_str() );

		return (false);
	}		

	return (true);
}

bool CPlayer::WritePlayerData( CStreamData *pData )
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

	uint32_t roomVnum = m_pCurrentRoom->GetVnum();

	pData->Write( &m_curDiskSpace, sizeof(m_curDiskSpace) );
        pData->Write( &m_curDiskItems, sizeof(m_curDiskItems) );
	
	pData->Write( &roomVnum, sizeof(uint32_t) );
	pData->Write( &m_maxCarryDisk, sizeof(m_maxCarryDisk) );
	pData->Write( &m_maxCarryItems, sizeof(m_maxCarryItems) );

	// write the player ID
	pData->Write( &m_playerID, sizeof(m_playerID) );
	pData->Write( &m_playerKey, sizeof(m_playerKey) );

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
                        if ( !m_pEquipItems[i]->WriteItemData( pData ) )
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
                if ( !pItem->WriteItemData( pData ) )
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
		printf( "StreamException caught: %s\n", sExceptionText.c_str() );

		return (false);
	}

	return (true);
}

string &CPlayer::GetInstalledItemName( tItemEquipType slot, string &sOutStr )
{
	if ( slot >= EQUIP_SLOT_MAX )
		sOutStr = "NA";
	else if ( m_pEquipItems[slot] )
		sOutStr = m_pEquipItems[slot]->GetName();
	else
		sOutStr = "Not installed";

	return sOutStr;
}

bool CPlayer::EquipItem( CItemInstance *pItem, tItemEquipType slot )
{
	if ( !pItem )
		return (false);

	if ( m_pEquipItems[slot] )
		return (false);

	if ( pItem->GetEquipType() != slot )
		return (false);

	if ( pItem->GetScriptLevel() > GetScriptLevel() ||
	     pItem->GetCodeLevel() > GetCodeLevel() ||
	     pItem->GetHackLevel() > GetHackLevel() )
		return (false);

	// Remove it from inventory
	if ( !RemoveItemFromInventory( pItem ) )
		return (false);

	m_pEquipItems[slot] = pItem;

	// Update player stats
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


	return (true);
}

bool CPlayer::UnequipItem( tItemEquipType slot, CItemInstance *&pItemUnequipped )
{
	if ( !m_pEquipItems[slot] )
		return (false);

	if ( m_curDiskSpace + m_pEquipItems[slot]->GetDiskUsage() > m_maxCarryDisk )
		return (false);

	if ( m_curDiskItems+1 > m_maxCarryItems )
		return (false);

	if ( !AddItemToInventory( m_pEquipItems[slot] ) )
		return (false);

	pItemUnequipped = m_pEquipItems[slot];
	m_pEquipItems[slot] = NULL;

	m_MaxHP -= pItemUnequipped->GetHPModifier();
	m_MaxCPU -= pItemUnequipped->GetCPUModifier();
	m_MaxMemory -= pItemUnequipped->GetMemoryModifier();

	// Adjust stats!
        if ( m_HP > m_MaxHP )
                m_HP = m_MaxHP;

        if ( m_CPU > m_MaxCPU )
                m_CPU = m_MaxCPU;

        if ( m_Memory > m_MaxMemory )
                m_Memory = m_MaxMemory;


	return (true);
}

bool CPlayer::AddItemToInventory( CItemInstance *pItemToAdd )
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

        return (true);
}

bool CPlayer::RemoveItemFromInventory( CItemInstance *pItemToRemove )
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

bool CPlayer::AttackNPC( uint32_t npcID, CNPCInRoom *&pNPCAttacked )
{
	bool bFound = false;
	CRoom *pRoom = GetRoom();

	if ( !pRoom )
		return (false);

	for ( CNPCInRoom *pCur = pRoom->GetNPCsInRoom()->Head(); pCur; pCur = pRoom->GetNPCsInRoom()->Next( pCur ) )
	{
		if ( pCur->GetID() == npcID )
		{
			pNPCAttacked = pCur;
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

bool CPlayer::AttackPlayer( uint32_t playerID, CPlayerInRoom *&pPlayerAttacked )
{
	bool bFound = false;
	CRoom *pRoom = GetRoom();

	if ( !pRoom )
		return (false);

	for ( CPlayerInRoom *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
	{
		if ( pCur->GetID() == playerID )
		{
			pPlayerAttacked = pCur;
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
	for ( CAttackTarget *pCur = m_attackTargets.Head(); pCur; pCur = m_attackTargets.Next( pCur ) )
	{
		if ( pCur->GetTargetType() == targetType && pCur->GetTargetID() == targetID )
		{
			delete pCur;
			return (true);	
		}
	}

	return (false);	
}

uint32_t CPlayer::GetItemFingerprintString( char *pszFingerprintString, uint32_t stringMaxLen )
{
	uint8_t tempBuffer[(EQUIP_SLOT_MAX*5)]; // 5 bytes for items and 8 equip slots
        register uint32_t outPos = 0;
	register uint32_t i = 0;

        for ( i = 0; i < EQUIP_SLOT_MAX; i++ )
        {
                if ( m_pEquipItems[i] )
                        outPos += m_pEquipItems[i]->GetItemFingerprint( tempBuffer+outPos );
        }

	for ( i = 0; (i>>1) < outPos && i < stringMaxLen; i+=2 )
	{
		pszFingerprintString[i] = ToHexChar( (tempBuffer[i>>1] >> 4) & 0xF );
		pszFingerprintString[i+1] = ToHexChar( (tempBuffer[i>>1] & 0xF) );
	}	
	pszFingerprintString[i] = '\0';

        return i;
}
