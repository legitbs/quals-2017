#include "common.h"

CItemInstance::CItemInstance( )
	: m_itemID( 0 ), m_itemKey( 0 )
{

}

CItemInstance::CItemInstance( uint32_t itemID, uint64_t itemKey )
	: m_itemID( itemID ), m_itemKey( itemKey )
{

}

CItemInstance::~CItemInstance( )
{

}

bool CItemInstance::ReadItemData( CStreamData *pData )
{
#if SERVER
        return (false);
#else
        if ( pData == NULL )
                return (false);

        try
        {
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

                pData->Read( &m_modHP, sizeof(m_modHP) );
                pData->Read( &m_modCPU, sizeof(m_modCPU) );
                pData->Read( &m_modMemory, sizeof(m_modMemory) );

                pData->Read( &m_diskUsage, sizeof(m_diskUsage) );

                pData->Read( &m_scriptUseLeft, sizeof(m_scriptUseLeft) );
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

bool CItemInstance::WriteItemData( CStreamData *pData )
{
#if SERVER
        if ( pData == NULL )
                return (false);

        try
        {
		uint32_t flags;
                uint32_t itemID;
                uint64_t itemKey;
                uint8_t scriptLevel, codeLevel, hackLevel;
                int16_t modHP, modCPU, modMemory;
                uint16_t diskUsage;
                uint8_t scriptUseLeft;
		uint8_t itemType, itemEquipType;

                itemID = m_itemID;
                itemKey = m_itemKey;

		flags = GetFlags();

		itemType = GetItemType();
                itemEquipType = GetEquipType();

                scriptLevel = GetScriptLevel();
                codeLevel = GetCodeLevel();
                hackLevel = GetHackLevel();

                modHP = GetHPModifier();
                modCPU = GetCPUModifier();
                modMemory = GetMemoryModifier();

                diskUsage = GetDiskUsage();

                scriptUseLeft = GetScriptUseLeft();

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

                pData->Write( &modHP, sizeof(modHP) );
                pData->Write( &modCPU, sizeof(modCPU) );
                pData->Write( &modMemory, sizeof(modMemory) );

                pData->Write( &diskUsage, sizeof(diskUsage) );

                pData->Write( &scriptUseLeft, sizeof(scriptUseLeft) );

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

bool CItemInstance::GetEquipStringFromType( tItemEquipType slot, string &sEquipString )
{
        switch( slot )
        {
        case eItemEquipProcessor:
		sEquipString = "processor";
                break;

        case eItemEquipMemory:
		sEquipString = "memory";
                break;

        case eItemEquipCoprocessor:
		sEquipString = "coprocessor";
                break;

        case eItemEquipCard1:
		sEquipString = "card1";
                break;

        case eItemEquipCard2:
		sEquipString = "card2";
                break;

        case eItemEquipDisk:
		sEquipString = "disk";
                break;

        case eItemEquipAccelerator:
		sEquipString = "accelerator";
                break;

        default:
		sEquipString = "";
                return (false);
                break;
        }

        return (true);
}

void CItemInstance::GetItemFlagsString( string &sFlags )
{
	sFlags = "";
	if ( m_flags & ITEM_FLAG_DROPONPK )
		sFlags += "Drop on PK, ";
	if ( m_flags & ITEM_FLAG_DROPONEXIT )
		sFlags += "Drop on Exit, ";
	if ( m_flags & ITEM_FLAG_NODESTROY )
		sFlags += "No Destroy, ";
	if ( m_flags & ITEM_FLAG_FINGERREQUEST )
		sFlags += "Finger Request Script, ";

	return;
}

uint8_t CItemInstance::GetItemFingerprint( uint8_t *pOutData )
{
        CBitWriter oBitWriter( pOutData, 5 );

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
