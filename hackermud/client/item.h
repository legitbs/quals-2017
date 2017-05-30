#ifndef __ITEM_H__
#define __ITEM_H__

typedef enum
{
	eItemFixed,		// Fixed item can't move
	eItemPower,		// Fixed item can't power
	eItemScript,		// Item is a script
	eItemEquip		// Item can be equipped
} tItemType;

typedef enum
{
        eItemEquipProcessor = 0,        // Processor slot
        eItemEquipMemory,       // Memory slot
        eItemEquipCoprocessor,  // Coprocessor slot
        eItemEquipCard1,        // Card slot 1
        eItemEquipCard2,        // Card slot 2
        eItemEquipDisk,         // Disk slot
        eItemEquipAccelerator,  // Accelerator slot (GPU for example)
        EQUIP_SLOT_MAX,
	eItemEquipNotEquippable
} tItemEquipType;

#define ITEM_FLAG_DROPONPK              (1)
#define ITEM_FLAG_DROPONEXIT            (2)
#define ITEM_FLAG_NODESTROY             (4)
#define ITEM_FLAG_FINGERREQUEST         (8)

/*
// ONLY SERVER CARES ABOUT THIS
class CBaseItemVnum
{
public:
	CBaseItemVnum( uint32_t vnum ) : m_vnum( vnum ) { };
	
	uint32_t GetHasValue( void ) const { return m_vnum; };
	bool operator==(const class CBaseItemVnum &t ) const
	{
		return (m_vnum == t.m_vnum);
	}

private:
	uint32_t m_vnum;
};

class CBaseItem : public CBaseItemVnum
{
public:
	NUtil::HASH_LINK(CBaseItem)	m_worldDatabase;

private:
};
*/

class CItemInstance
{
public:
	NUtil::LIST_LINK(CItemInstance)		m_playerLink;	// Item on player list

public:
	CItemInstance( );
	CItemInstance( uint32_t itemID, uint64_t itemKey );
	~CItemInstance( );

	bool ReadItemData( CStreamData *pData );
	bool WriteItemData( CStreamData *pData );

	uint32_t GetItemID( void ) const { return m_itemID; };
	uint64_t GetItemKey( void ) const { return m_itemKey; };

	uint32_t GetFlags( void ) const { return m_flags; };

	int16_t GetHPModifier( void ) const { return m_modHP; };
	int16_t GetCPUModifier( void ) const { return m_modCPU; };
	int16_t GetMemoryModifier( void ) const { return m_modMemory; };

	string &GetName( void ) { return m_sName; };
	string &GetDescription( void ) { return m_sDescription; };
	string &GetBriefDescription( void ) { return m_sBriefDescription; };

	tItemType GetItemType( void ) { return m_itemType; };
	tItemEquipType GetEquipType( void ) { return m_equipType; };

	uint8_t GetScriptLevel( void ) { return m_scriptLevel; };
	uint8_t GetCodeLevel( void ) { return m_codeLevel; };
	uint8_t GetHackLevel( void ) { return m_hackLevel; };
	
	uint8_t GetDiskUsage( void ) { return m_diskUsage; };

	uint8_t GetScriptUseLeft( void ) { return m_scriptUseLeft; };

        static bool GetEquipTypeFromString( string &sSlotName, tItemEquipType &equipSlot );
        static bool GetEquipStringFromType( tItemEquipType slot, string &sEquipString );

	bool HasFlagFingerRequest( void ) { return (m_flags & ITEM_FLAG_FINGERREQUEST); };
	void GetItemFlagsString( string &sFlags );

	uint8_t GetItemFingerprint( uint8_t *pOutData );

private:
	tItemType m_itemType;
	tItemEquipType m_equipType;
	
	string m_sName;
	string m_sDescription;
	string m_sBriefDescription;	

	uint32_t m_itemID;
	uint64_t m_itemKey;

	uint32_t m_flags;

	uint16_t m_diskUsage;
	uint8_t m_scriptLevel;
	uint8_t m_codeLevel;
	uint8_t m_hackLevel;

	int16_t m_modHP;
	int16_t m_modCPU;
	int16_t m_modMemory;

	uint8_t m_scriptUseLeft;
};

#endif // __ITEM_H__
