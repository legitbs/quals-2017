#include "common.h"

tItemDatabaseHash g_oItemDBHash( ITEM_HASH_SIZE );

uint32_t CItemInstance::m_lastItemID = 0;

CItemInstance::CItemInstance( CBaseItem *pBaseItem )
	: m_pBaseItem( pBaseItem )
{
	// Generate ID
	m_itemID = m_lastItemID;
	m_lastItemID++;

	// Set base item
	m_pBaseItem = pBaseItem;

	// Generate key
	uint64_t newKey;

        newKey = g_oRNG.GetU32();
        newKey = (newKey << 32) |  g_oRNG.GetU32();

	m_itemKey = newKey;
}

CItemInstance::~CItemInstance( )
{

}

bool CItemInstance::ReadItemData( CStreamData *pData, bool bPlayerCache )
{
	if ( pData == NULL )
		return (false);

	try
	{
		if ( !bPlayerCache )
		{
#if SERVER

#else
			char *pTempData;

			uint16_t itemNameLength;
			pData->Read( &itemNameLength, sizeof(itemNameLength) );
			pTempData = new char[itemNameLength+1];
			pData->Read( pTempData, itemNameLength );
			pTempData[itemNameLength] = '\0';

			m_sName = pTempData;
			delete pTempData;

			
			uint16_t itemDescriptionLength;
			pData->Read( &itemDescriptionLength, sizeof(itemDescriptionLength) );
			pTempData = new char[itemDescriptionLength+1];
			pData->Read( pTempData, itemDescriptionLength );
			pTempData[itemDescriptionLength] = '\0';

			m_sDescription = pTempData;
			delete pTempData;

			
			uint16_t itemBriefLength;
			pData->Read( &itemBriefLength, sizeof(itemBriefLength) );
			pTempData = new char[itemBriefLength+1];
			pData->Read( pTempData, itemBriefLength );
			pTempData[itemBriefLength] = '\0';

			m_sBriefDescription = pTempData;
			delete pTempData;

			// Read the rest of the data
			pData->Read( &m_itemID, sizeof(m_itemID) );
			pData->Read( &m_itemKey, sizeof(m_itemKey) );

			uint8_t itemType = m_itemType;
			uint8_t itemEquipType = m_equipType;

			pData->Read( &m_flags, sizeof(m_flags) );

			pData->Read( &itemType, sizeof(itemType) );
			pData->Read( &itemEquipType, sizeof(itemEquipType) );

			m_itemType = (tItemType)itemType;
			m_equipType = (tItemEquipType)itemEquipType;

			pData->Read( &m_scriptLevel, sizeof(m_scriptLevel) );
			pData->Read( &m_codeLevel, sizeof(m_codeLevel) );
			pData->Read( &m_hackLevel, sizeof(m_hackLevel) );
#endif
		}
		else
		{
			uint32_t baseItemVnum;
			pData->Read( &baseItemVnum, sizeof(baseItemVnum) );

			CBaseItem *pBaseItem = g_oItemDBHash.Find( baseItemVnum );

			if ( pBaseItem == NULL )
			{
				printf( "Error loading item (player cache), invalid base vnum %d\n", baseItemVnum );
				return (false);
			}

			m_pBaseItem = pBaseItem;
		}

		pData->Read( &m_itemModHP, sizeof(m_itemModHP) );
		pData->Read( &m_itemModCPU, sizeof(m_itemModCPU) );
		pData->Read( &m_itemModMemory, sizeof(m_itemModMemory) );

		// Only write disk usage for non player cache writes
		if ( !bPlayerCache )
		{
#if SERVER

#else
			pData->Read( &m_diskUsage, sizeof(m_diskUsage) );
#endif
		}

		pData->Read( &m_scriptUseLeft, sizeof(m_scriptUseLeft) );		
	}
	catch ( CStreamException e )
	{
		string sExceptionText;

		e.GetExceptionString( sExceptionText );
		printf( "StreamException (ReadItemData) caught: %s\n", sExceptionText.c_str() );

		return (false);
	}

	return (true);	
}

bool CItemInstance::WriteItemData( CStreamData *pData, bool bPlayerCache )
{
	if ( pData == NULL )
		return (false);

	try
	{
		if ( !bPlayerCache )
		{
			uint32_t flags;
			uint32_t itemID;
			uint64_t itemKey;
			uint8_t scriptLevel, codeLevel, hackLevel;
			uint8_t itemType, itemEquipType;

			itemID = m_itemID;
			itemKey = m_itemKey;
	 
			flags = GetFlags();
		       
			itemType = GetItemType();
			itemEquipType = GetEquipType();

			scriptLevel = GetScriptLevel();
			codeLevel = GetCodeLevel();
			hackLevel = GetHackLevel();

			uint16_t nameLength, descriptionLength, briefLength;

			nameLength = GetName().length();
			pData->Write( &nameLength, sizeof(nameLength) );
			pData->Write( GetName().c_str(), nameLength );

			descriptionLength = GetDescription().length();
			pData->Write( &descriptionLength, sizeof(descriptionLength) );
			pData->Write( GetDescription().c_str(), descriptionLength );

			briefLength = GetBriefDescription().length();
			pData->Write( &briefLength, sizeof(briefLength) );
			pData->Write( GetBriefDescription().c_str(), briefLength );

			pData->Write( &itemID, sizeof(itemID) );
			pData->Write( &itemKey, sizeof(itemKey) );

			pData->Write( &flags, sizeof(flags) );

			pData->Write( &itemType, sizeof(itemType) );
			pData->Write( &itemEquipType, sizeof(itemEquipType) );

			pData->Write( &scriptLevel, sizeof(scriptLevel) );
			pData->Write( &codeLevel, sizeof(codeLevel) );
			pData->Write( &hackLevel, sizeof(hackLevel) );
		}
		else
		{
			uint32_t baseItemVnum = m_pBaseItem->GetVnum();
			pData->Write( &baseItemVnum, sizeof(baseItemVnum) );
		}

		uint16_t diskUsage;
		uint8_t scriptUseLeft;
		
			
		diskUsage = GetDiskUsage();

		scriptUseLeft = GetScriptUseLeft();
	
		if ( !bPlayerCache )
		{	
			int16_t modHP, modCPU, modMemory; 
			modHP = GetHPModifier();
			modCPU = GetCPUModifier();
			modMemory = GetMemoryModifier();
			
			pData->Write( &modHP, sizeof(modHP) );
			pData->Write( &modCPU, sizeof(modCPU) );
			pData->Write( &modMemory, sizeof(modMemory) );
		}
		else
		{
			pData->Write( &m_itemModHP, sizeof(m_itemModHP) );
			pData->Write( &m_itemModCPU, sizeof(m_itemModCPU) );
			pData->Write( &m_itemModMemory, sizeof(m_itemModMemory) );
		}

		if ( !bPlayerCache )
			pData->Write( &diskUsage, sizeof(diskUsage) );

		pData->Write( &scriptUseLeft, sizeof(scriptUseLeft) );

	}
	catch ( CStreamException e )
	{
		string sExceptionText;

		e.GetExceptionString( sExceptionText );
		printf( "StreamException (WriteItemData) caught: %s\n", sExceptionText.c_str() );

		return (false);
	}

	return (true);
}

int16_t CItemInstance::GetHPModifier( void )
{
	return m_pBaseItem->m_baseModHP + m_itemModHP;
}

int16_t CItemInstance::GetCPUModifier( void )
{
	return m_pBaseItem->m_baseModCPU + m_itemModCPU;
}

int16_t CItemInstance::GetMemoryModifier( void )
{
	return m_pBaseItem->m_baseModMemory + m_itemModMemory;
}

uint8_t CItemInstance::GetScriptUseLeft( void )
{
	return m_scriptUseLeft;
}

uint8_t CItemInstance::GetScriptUseOriginal( void )
{
	return m_pBaseItem->m_scriptUseCount;
}

string &CItemInstance::GetName( void )
{
	return m_pBaseItem->m_sName;
}

string &CItemInstance::GetDescription( void )
{
	return m_pBaseItem->m_sDescription;
}

string &CItemInstance::GetBriefDescription( void )
{
	return m_pBaseItem->m_sBriefDescription;
}

uint32_t CItemInstance::GetFlags( void )
{
	return m_pBaseItem->m_flags;
}

uint8_t CItemInstance::GetScriptLevel( void )
{
	return m_pBaseItem->m_baseScriptLevel;
}

uint8_t CItemInstance::GetCodeLevel( void )
{
	return m_pBaseItem->m_baseCodeLevel;
}

uint8_t CItemInstance::GetHackLevel( void )
{
	return m_pBaseItem->m_baseHackLevel;
}

uint16_t CItemInstance::GetDiskUsage( void )
{
	return m_pBaseItem->m_baseDiskUsage;
}

tItemType CItemInstance::GetItemType( void )
{
	return m_pBaseItem->m_itemType;
}

tItemEquipType CItemInstance::GetEquipType( void )
{
	return m_pBaseItem->m_equipType;
}

CBaseItem::CBaseItem( uint32_t vnum )
	: CBaseItemVnum( vnum )
{
	m_flags = 0;
}

CBaseItem::~CBaseItem( )
{

}

void CBaseItem::LoadItems( void )
{
	// Load rooms from disk
        char szFileName[1024];
        char szLine[1024];
        uint32_t itemVnum;
	uint16_t diskUsage;
	uint8_t scriptLevel;
	uint8_t codeLevel;
	uint8_t hackLevel;
		
	int16_t modHP;
	int16_t modCPU;
	int16_t modMemory;
		
	uint8_t scriptUseCount = 0;

        snprintf( szFileName, 1024, "%s/%s", ITEM_FILE_DIRECTORY, ITEM_FILE_NAME );

        FILE *pFile;

        pFile = fopen( szFileName, "r" );

        if ( !pFile )
        {
                printf( "Can't load items from disk: %s\n", szFileName );
                return;
        }

        do
        {
                string sName;
                string sBriefDescription;
                string sDescription;
		string sType;
		string sFlags;
		uint32_t flags;


                // Read in room data...
                READ_FILE_STRING( "Name", sName )
                READ_FILE_STRING( "BriefDescription", sBriefDescription )
                READ_FILE_STRING( "Description", sDescription )
                READ_FILE_UINT32( "Vnum", itemVnum )

		READ_FILE_STRING( "Flags", sFlags );

		vector<string> sFlagVector = SplitString( sFlags );

                flags = 0;
                for ( uint8_t i = 0; i < sFlagVector.size(); i++ )
                {
                        if ( strcasecmp( "pkdrop", sFlagVector[i].c_str() ) == 0 )
                                flags |= ITEM_FLAG_DROPONPK;
			else if ( strcasecmp( "exitdrop", sFlagVector[i].c_str() ) == 0 )
				flags |= ITEM_FLAG_DROPONEXIT;
			else if ( strcasecmp( "nodestroy", sFlagVector[i].c_str() ) == 0 )
				flags |= ITEM_FLAG_NODESTROY;
			else if ( strcasecmp( "fingerrequest", sFlagVector[i].c_str() ) == 0 )
				flags |= ITEM_FLAG_FINGERREQUEST;
                        else if ( strcasecmp( "none", sFlagVector[i].c_str() ) == 0 )
                                ;
                        else
                        {
                                printf( "Error loading flags for item: %s\n", sFlags.c_str() );
                                goto bad_fileread;
                        }
                }

		READ_FILE_STRING( "Type", sType );
	

		tItemType itemType;
		tItemEquipType itemEquipType = eItemEquipNotEquippable;

		if ( strcasecmp( sType.c_str(), "fixed" ) == 0 )
			itemType = eItemFixed;
		else if ( strcasecmp( sType.c_str(), "power" ) == 0 )
			itemType = eItemPower;
		else if ( strcasecmp( sType.c_str(), "script" ) == 0 )
			itemType = eItemScript;
		else if ( strcasecmp( sType.c_str(), "equip" ) == 0 )
		{
			itemType = eItemEquip;

			string sEquipSlot;
			READ_FILE_STRING( "Equip", sEquipSlot );

			if ( !CItemInstance::GetEquipTypeFromString( sEquipSlot, itemEquipType ) )
			{
				printf( "Invalid equip slot: %s\n", sEquipSlot.c_str() );
				goto bad_fileread;
			}
		}
		else
		{
			printf( "Bad item type read: %s\n", sType.c_str() );
			goto bad_fileread;
		}


		READ_FILE_UINT16( "Disk", diskUsage );
		
		READ_FILE_UINT8( "Script", scriptLevel );
		READ_FILE_UINT8( "Code", codeLevel );
		READ_FILE_UINT8( "Hack", hackLevel );

		READ_FILE_INT16( "HP", modHP );
		READ_FILE_INT16( "CPU", modCPU );
		READ_FILE_INT16( "Memory", modMemory );	
		
		scriptUseCount = 0;
		if ( itemType == eItemScript )
			READ_FILE_UINT8( "ScriptUse", scriptUseCount );

		// boosh, done loading item information -- create a new item for the world!
		if ( g_oItemDBHash.Find( itemVnum ) )
		{
			printf( "Can't add item (vnum: %u) it already exists in DB.\n", itemVnum );
			goto bad_fileread;
		}

		CBaseItem *pBaseItem = new CBaseItem( itemVnum );

		pBaseItem->m_sName = sName;
		pBaseItem->m_sBriefDescription = sBriefDescription;
		pBaseItem->m_sDescription = sDescription;

		pBaseItem->m_flags = flags;

		pBaseItem->m_itemType = itemType;
		pBaseItem->m_equipType = itemEquipType;

		pBaseItem->m_baseDiskUsage = diskUsage;
		
		pBaseItem->m_baseScriptLevel = scriptLevel;
		pBaseItem->m_baseCodeLevel = codeLevel;
		pBaseItem->m_baseHackLevel = hackLevel;

		pBaseItem->m_baseModHP = modHP;
		pBaseItem->m_baseModCPU = modCPU;
		pBaseItem->m_baseModMemory = modMemory;

		pBaseItem->m_scriptUseCount = scriptUseCount;

		printf( "Item %s (vnum: %u) added to database.\n", sName.c_str(), pBaseItem->GetVnum() );
		printf( "HP: %d CPU: %d Memory: %d\n", modHP, modCPU, modMemory );

		// Add it into the item database
		g_oItemDBHash.Add( pBaseItem );

	} while ( !feof( pFile ) );

	return;

bad_fileread:
	printf( "Critical error reading items.txt file.\n" );
	
	return;
}

CItemInstance *CItemInstance::LoadFromFile( FILE *pFile )
{
        char szLine[1024];
	if ( pFile == NULL )
		return (NULL);

	// Load item data from pfile...
	// Load base item vnum
	uint32_t baseItemVnum;
	int8_t modHP, modCPU, modMemory;
	uint8_t scriptUseLeft;
	CBaseItem *pBaseItem;
	CItemInstance *pNewItem;

	READ_FILE_UINT32( "BaseVnum", baseItemVnum )
	READ_FILE_INT8( "ModHP", modHP )
	READ_FILE_INT8( "ModCPU", modCPU )
	READ_FILE_INT8( "ModMemory", modMemory )
	READ_FILE_UINT8( "ScriptUseLeft", scriptUseLeft )

	// Find base item vnum
	pBaseItem = g_oItemDBHash.Find( baseItemVnum );

	if ( pBaseItem == NULL )
	{
		printf( "Error loading item, invalid base vnum %d\n", baseItemVnum );
		return (NULL);
	}

	pNewItem = new CItemInstance( pBaseItem );

	pNewItem->m_itemModHP = modHP;
	pNewItem->m_itemModCPU = modCPU;
	pNewItem->m_itemModMemory = modMemory;

	pNewItem->m_scriptUseLeft = scriptUseLeft;

	return (pNewItem);

bad_fileread:
	return (NULL);
}

CItemInstance *CItemInstance::CreateItem( uint32_t baseVnum, bool bModStats )
{
	CBaseItem *pBaseItem = g_oItemDBHash.Find( baseVnum );

	if ( !pBaseItem )
		return (NULL);

	CItemInstance *pNewItem = new CItemInstance( pBaseItem );

	// Default script use to maximum
	pNewItem->m_scriptUseLeft = pBaseItem->m_scriptUseCount;

	if ( bModStats )
	{
		int8_t modHPAmt = 0;
		int8_t modCPUAmt = 0;
		int8_t modMemoryAmt = 0;

		// Modify stats -- by a small amount
		if ( abs( (int32_t)(pBaseItem->m_baseModHP * 0.1) ) > 128 )
			modHPAmt = g_oRNG.GetRange( 0, 128 );
		else if ( (int32_t)(pBaseItem->m_baseModHP * 0.1) != 0 )
			modHPAmt = g_oRNG.GetRange( 0, abs( (int32_t)(pBaseItem->m_baseModHP * 0.1) ) );
		
		if ( abs( (int32_t)(pBaseItem->m_baseModCPU * 0.1) ) > 128 )
			modCPUAmt = g_oRNG.GetRange( 0, 128 );
		else if ( (int32_t)(pBaseItem->m_baseModCPU * 0.1) != 0 )
			modCPUAmt = g_oRNG.GetRange( 0, abs( (int32_t)(pBaseItem->m_baseModCPU * 0.1) ) );
		
		if ( abs( (int32_t)(pBaseItem->m_baseModMemory * 0.1) ) > 128 )
			modMemoryAmt = g_oRNG.GetRange( 0, 128 );
		else if ( (int32_t)(pBaseItem->m_baseModMemory * 0.1) != 0 )
			modMemoryAmt = g_oRNG.GetRange( 0, abs( (int32_t)(pBaseItem->m_baseModMemory * 0.1) ) );

		// Apply negative multipliers
		if ( pBaseItem->m_baseModHP < 0 )
			modHPAmt *= -1;
		
		if ( pBaseItem->m_baseModCPU < 0  )
			modCPUAmt *= -1;
		
		if ( pBaseItem->m_baseModMemory < 0 )
			modMemoryAmt *= -1;

		pNewItem->m_itemModHP = modHPAmt;
		pNewItem->m_itemModCPU = modCPUAmt;
		pNewItem->m_itemModMemory = modMemoryAmt;
	}
	else
	{
		pNewItem->m_itemModHP = 0;
		pNewItem->m_itemModCPU = 0;
		pNewItem->m_itemModMemory = 0;
	}

	return (pNewItem);	
}

bool CItemInstance::SaveToFile( FILE *pFile )
{
	if ( !pFile )
		return (false);

	// Save item information to pfile
	WRITE_FILE_UINT32( "BaseVnum", m_pBaseItem->GetVnum() )
	WRITE_FILE_INT8( "ModHP", m_itemModHP )
	WRITE_FILE_INT8( "ModCPU", m_itemModCPU )
	WRITE_FILE_INT8( "ModMemory", m_itemModMemory )
	WRITE_FILE_UINT8( "ScriptUseLeft", m_scriptUseLeft )

	return (true);

bad_filewrite:
	return (false);
}

bool CItemInstance::GetEquipTypeFromString( string &sEquipSlot, tItemEquipType &equipSlot )
{
	if ( strcasecmp( sEquipSlot.c_str(), "processor" ) == 0 )
		equipSlot = eItemEquipProcessor;
	else if ( strcasecmp( sEquipSlot.c_str(), "memory" ) == 0 )
		equipSlot = eItemEquipMemory;
	else if ( strcasecmp( sEquipSlot.c_str(), "coprocessor" ) == 0 )
		equipSlot = eItemEquipCoprocessor;
	else if ( strcasecmp( sEquipSlot.c_str(), "card1" ) == 0 )
		equipSlot = eItemEquipCard1;
	else if ( strcasecmp( sEquipSlot.c_str(), "card2" ) == 0 )
		equipSlot = eItemEquipCard2;
	else if ( strcasecmp( sEquipSlot.c_str(), "disk" ) == 0 )
		equipSlot = eItemEquipDisk;
	else if ( strcasecmp( sEquipSlot.c_str(), "accelerator" ) == 0 )
		equipSlot = eItemEquipAccelerator;
	else
		return (false);

	return (true);
}

bool CItemInstance::GetEquipStringFromType( tItemEquipType slot, string &sSlotName )
{
	switch( slot )
	{
	case eItemEquipProcessor:
		sSlotName = "processor";
		break;

	case eItemEquipMemory:
		sSlotName = "memory";
		break;

	case eItemEquipCoprocessor:
		sSlotName = "coprocessor";
		break;

	case eItemEquipCard1:
		sSlotName = "card1";
		break;

	case eItemEquipCard2:
		sSlotName = "card2";
		break;

	case eItemEquipDisk:
		sSlotName = "disk";
		break;

	case eItemEquipAccelerator:
		sSlotName = "accelerator";
		break;

	default:
		return (false);
		break;
	}

	return (true);
}

uint8_t CItemInstance::GetItemFingerprint( uint8_t *pOutData, uint8_t outMaxLength )
{
	CBitWriter oBitWriter( pOutData, outMaxLength );
	
	uint32_t hpMod = GetHPModifier();
	uint32_t cpuMod = GetCPUModifier();
	uint32_t memMod = GetMemoryModifier();

	// Compress HP modifier
	do
	{
		if ( hpMod < 16 )
		{
			oBitWriter.WriteBit( 1 );
			oBitWriter.WriteBits( hpMod, 4 );

			hpMod = 0;
		}
		else
		{
			oBitWriter.WriteBit( 0 );
			hpMod = hpMod >> 1;
		}
	} while ( hpMod > 0 );	
	
	// Compress CPU modifier
	do
	{
		if ( cpuMod < 16 )
		{
			oBitWriter.WriteBit( 1 );
			oBitWriter.WriteBits( cpuMod, 4 );

			cpuMod = 0;
		}
		else
		{
			oBitWriter.WriteBit( 0 );
			cpuMod = cpuMod >> 1;
		}
	} while ( cpuMod > 0 );	
	
	// Compress memory modifier
	do
	{
		if ( memMod < 16 )
		{
			oBitWriter.WriteBit( 1 );
			oBitWriter.WriteBits( memMod, 4 );

			memMod = 0;
		}
		else
		{
			oBitWriter.WriteBit( 0 );
			memMod = memMod >> 1;
		}
	} while ( memMod > 0 );	

	// DO a simple Huffman like tree (super simple) -- as they will need to find an item that generates
	// a specific hash value to get to a trampoline to get to their user data
	if ( GetHackLevel() < 16 )
	{
		oBitWriter.WriteBit( 0 );
		oBitWriter.WriteBits( GetHackLevel(), 4 );		
	}
	else
	{
		oBitWriter.WriteBit( 1 );
		oBitWriter.WriteBits( GetHackLevel()-16, 2 );
	}

	if ( GetCodeLevel() < 16 )
	{
		oBitWriter.WriteBit( 0 );
		oBitWriter.WriteBits( GetCodeLevel(), 4 );
	}
	else
	{
		oBitWriter.WriteBit( 1 );
		oBitWriter.WriteBits( GetCodeLevel()-16, 2 );
	}	
	
	if ( GetScriptLevel() < 16 )
	{
		oBitWriter.WriteBit( 0 );
		oBitWriter.WriteBits( GetScriptLevel(), 4 );
	}
	else
	{
		oBitWriter.WriteBit( 1 );
		oBitWriter.WriteBits( GetScriptLevel()-16, 2 );
	}

	// Do Disk Usage
        if ( GetDiskUsage() < 32 )
        {
                oBitWriter.WriteBit( 1 );
                oBitWriter.WriteBits( GetDiskUsage(), 5 );
        }
        else
        {
                oBitWriter.WriteBit( 0 );
                if ( GetDiskUsage() < 64 )
                {
                        oBitWriter.WriteBit( 1 );
                        oBitWriter.WriteBits( GetDiskUsage()-32, 5 );
                }
                else
                {
                        oBitWriter.WriteBit( 0 );
                        oBitWriter.WriteBits( (GetDiskUsage()-64), 8 );
                }
        }

	return oBitWriter.Flush();
}
