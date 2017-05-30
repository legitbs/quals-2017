#ifndef __ITEM_H__
#define __ITEM_H__

typedef enum
{
	eItemFixed,		// Fixed item can't move
	eItemPower,		// Fixed item can't power
	eItemScript,		// Item that is a script
	eItemEquip		// Item can be equipped (used)
} tItemType;

typedef enum
{
	eItemEquipProcessor = 0,	// Processor slot
	eItemEquipMemory,	// Memory slot
	eItemEquipCoprocessor,	// Coprocessor slot
	eItemEquipCard1,	// Card slot 1
	eItemEquipCard2,	// Card slot 2
	eItemEquipDisk,		// Disk slot
	eItemEquipAccelerator,	// Accelerator slot (GPU for example)
	EQUIP_SLOT_MAX,
	eItemEquipNotEquippable, // Dummy slot for non-equippable items
} tItemEquipType;

#define ITEM_FLAG_DROPONPK		(1)
#define ITEM_FLAG_DROPONEXIT		(2)
#define ITEM_FLAG_NODESTROY		(4)
#define ITEM_FLAG_FINGERREQUEST		(8)

class CBaseItemVnum
{
public:
	CBaseItemVnum( uint32_t vnum ) : m_vnum( vnum ) { };
	
	uint32_t GetHashValue( void ) const { return m_vnum; };
	bool operator==(const class CBaseItemVnum &t ) const
	{
		return (m_vnum == t.m_vnum);
	}

	uint32_t GetVnum( void ) const { return m_vnum; };

private:
	uint32_t m_vnum;
};

class CBaseItem : public CBaseItemVnum
{
public:
	CBaseItem( uint32_t vnum );
	~CBaseItem( );
	
public:
	NUtil::HASH_LINK(CBaseItem)	m_worldDatabase;
	friend class CItemInstance;

	static void LoadItems( void );

private:
	tItemType m_itemType;			// The type of item
	tItemEquipType m_equipType;		// If the item is equippable it will have this set

	string m_sName;
	string m_sDescription;
	string m_sBriefDescription;	
	
	uint16_t	m_baseDiskUsage;	// How much this item uses in disk space
	
	uint8_t		m_baseScriptLevel;	// Minimum script level
	uint8_t		m_baseCodeLevel;	// Minimum code level
	uint8_t		m_baseHackLevel;	// Minimum hack level

	int16_t		m_baseModHP;		// Modifies HP
	int16_t		m_baseModCPU;		// Modifies CPU
	int16_t		m_baseModMemory;	// Modifies Memory

	uint8_t		m_scriptUseCount;	// Number of base uses for script

	uint32_t	m_flags;		// Item flags
};

class CItemInstance
{
public:
	NUtil::LIST_LINK(CItemInstance)		m_roomLink;	// Item in room list
	NUtil::LIST_LINK(CItemInstance)		m_playerLink;	// Item on player list

public:
	CItemInstance( CBaseItem *pBaseItem );
	~CItemInstance( );

	bool ReadItemData( CStreamData *pData, bool bPlayerCache = false );
	bool WriteItemData( CStreamData *pData, bool bPlayerCache = false );

	uint32_t GetItemID( void ) { return m_itemID; };
	uint64_t GetItemKey( void ) { return m_itemKey; };

	int16_t GetHPModifier( void );
	int16_t GetCPUModifier( void );
	int16_t GetMemoryModifier( void );

	uint8_t GetScriptUseLeft( void );
	uint8_t GetScriptUseOriginal( void );

	uint32_t GetFlags( void );
	bool HasFlagDropOnPK( void ) { return (m_pBaseItem->m_flags & ITEM_FLAG_DROPONPK); };
	bool HasFlagDropOnExit( void ) { return (m_pBaseItem->m_flags & ITEM_FLAG_DROPONEXIT); };
	bool HasFlagNoDestroy( void ) { return (m_pBaseItem->m_flags & ITEM_FLAG_NODESTROY); };
	bool HasFlagFingerRequest( void ) { return (m_pBaseItem->m_flags & ITEM_FLAG_FINGERREQUEST); };

	tItemType GetItemType( void );
	tItemEquipType GetEquipType( void );

	string &GetName( void );
	string &GetDescription( void );
	string &GetBriefDescription( void );
	
	uint8_t GetScriptLevel( void );
	uint8_t GetCodeLevel( void );
	uint8_t GetHackLevel( void );

	uint16_t GetDiskUsage( void );

	static bool GetEquipTypeFromString( string &sSlotName, tItemEquipType &equipSlot );
	static bool GetEquipStringFromType( tItemEquipType slot, string &sSlotName );

	// Used by player files
	static CItemInstance *LoadFromFile( FILE *pFile );
	static CItemInstance *CreateItem( uint32_t baseVnum, bool bModStats = false );

	uint8_t GetItemHash( uint8_t *pOutData, uint8_t outMaxLength );

	bool SaveToFile( FILE *pFile );

	uint8_t GetItemFingerprint( uint8_t *pOutData, uint8_t outMaxLength );
	
private:
	CBaseItem	*m_pBaseItem;

	static uint32_t m_lastItemID;

	uint32_t	m_itemID;
	uint64_t	m_itemKey;

	int8_t		m_itemModHP;		// Additional item HP modifier
	int8_t		m_itemModCPU;		// Additional item CPU modifier
	int8_t		m_itemModMemory;	// Additional item Memory modifier

	uint8_t		m_scriptUseLeft;	// Number of uses left for script
};

typedef NUtil::HASH_DECLARE(CBaseItem, CBaseItemVnum, m_worldDatabase) tItemDatabaseHash;

#endif // __ITEM_H__
