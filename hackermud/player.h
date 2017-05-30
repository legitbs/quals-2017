#ifndef __PLAYER_H__
#define __PLAYER_H__

class CRoom;
class CNPCInRoom;
class CPlayerInRoom;

class CAttackTarget
{
public:
	enum
	{
		TARGET_NPC,
		TARGET_PLAYER
	};

public:
	CAttackTarget( uint32_t id, uint8_t targetType )
	{
		m_targetID = id;	
		m_targetType = targetType;
	}

	~CAttackTarget( ) { };

	bool IsTargetNPC( void ) { return m_targetType == TARGET_NPC; };
	bool IsTargetPlayer( void ) { return m_targetType == TARGET_PLAYER; };

	uint8_t GetTargetType( void ) { return m_targetType; };
	uint32_t GetTargetID( void ) { return m_targetID; };

private:
	uint8_t m_targetType;
	uint32_t m_targetID;

public:
	NUtil::LIST_LINK( CAttackTarget ) m_playerLink;
};

class CPlayer
{
public:
	CPlayer( );
	~CPlayer();

	void ClearData( void );

	bool ReadPlayerData( CStreamData *pData );
	bool WritePlayerData( CStreamData *pData );

	string &GetName( void ) { return m_sPlayerName; };
	string &GetDescription( void ) { return m_sPlayerDescription; };

	uint16_t GetCurHP( void ) const { return m_HP; };
	uint16_t GetCurCPU( void ) const { return m_CPU; };
	uint16_t GetCurMemory( void ) const { return m_Memory; };

	uint16_t GetMaxHP( void ) const { return m_MaxHP; };
        uint16_t GetMaxCPU( void ) const { return m_MaxCPU; };
        uint16_t GetMaxMemory( void ) const { return m_MaxMemory; };

	void SetHP( uint16_t hp ) { m_HP = hp; };
	void SetCPU( uint16_t cpu ) { m_CPU = cpu; };
	void SetMemory( uint16_t memory ) { m_Memory = memory; };

	void SetHackLevel( uint8_t newHackLevel ) { m_hackLevel = newHackLevel; };
        void SetCodeLevel( uint8_t newCodeLevel ) { m_codeLevel = newCodeLevel; };
        void SetScriptLevel( uint8_t newScriptLevel ) { m_scriptLevel = newScriptLevel; };
	
	uint8_t GetScriptLevel( void ) const { return m_scriptLevel; };
	uint8_t GetCodeLevel( void ) const { return m_codeLevel; };
	uint8_t GetHackLevel( void ) const { return m_hackLevel; };

	uint32_t GetPlayerID( void ) const { return m_playerID; };
	uint64_t GetPlayerKey( void ) const { return m_playerKey; };	

	uint16_t GetCurDiskSpace( void ) const { return m_curDiskSpace; };
	uint16_t GetMaxDiskSpace( void ) const { return m_maxCarryDisk; };

	uint8_t GetCurDiskItems( void ) const { return m_curDiskItems; };
	uint8_t GetMaxDiskItems( void ) const { return m_maxCarryItems; };

	bool EquipItem( CItemInstance *pItem, tItemEquipType slot );
	bool UnequipItem( tItemEquipType slot, CItemInstance *&pItemUnequipped );	
	bool RemoveItemFromInventory( CItemInstance *pItem );
	bool AddItemToInventory( CItemInstance *pItemToAdd );
	
	CItemInstance *GetItemFromInventoryByID( uint32_t itemID );

	string &GetInstalledItemName( tItemEquipType slot, string &sOutString );
	CItemInstance *GetInstallItem( tItemEquipType slot ) { return m_pEquipItems[slot]; };

	void SetRoom( CRoom *pNewRoom ) { m_pCurrentRoom = pNewRoom; m_attackTargets.DeleteAll(); };
	CRoom *GetRoom( void ) { return m_pCurrentRoom; };

	NUtil::LIST_PTR( CItemInstance ) GetItemsOnPlayer( void ) { return &m_itemsOnPlayer; };

	void StopFighting( void );
	bool IsFighting( void );
	bool AttackNPC( uint32_t npcID, CNPCInRoom *&pNPCAttacked );
	bool AttackPlayer( uint32_t playerID, CPlayerInRoom *&pPlayerAttacked );

	CAttackTarget *GetPrimaryTarget( void ) { return m_attackTargets.Head(); };

	bool IsFightingTarget( uint32_t targetID, uint8_t targetType );
	bool StopFightingTarget( uint32_t targetID, uint8_t targetType );

	uint32_t GetItemFingerprintString( char *pszFingerprintString, uint32_t outLen );

private:
	CItemInstance *m_pEquipItems[EQUIP_SLOT_MAX];
	CRoom *m_pCurrentRoom;		// Current room player is in

	uint16_t m_maxCarryDisk;	// Maximum carry disk space of player
	uint8_t m_maxCarryItems;	// Maximum number of items a player can carry

	uint16_t m_curDiskSpace;        // Current in use disk space
        uint8_t m_curDiskItems;         // Current in use disk items

	NUtil::LIST_DECLARE( CItemInstance, m_playerLink ) m_itemsOnPlayer;	

	string m_sPlayerName;
	string m_sPlayerDescription;

	// used to identify a player
	uint32_t m_playerID;	// Player ID
	uint64_t m_playerKey;	// Player KEY (unique that only your client and server know)

	uint16_t m_HP;		// Hitpoints	
	uint16_t m_CPU;		// CPU time
	uint16_t m_Memory;	// Memory consumption

	uint16_t m_MaxHP;       // Maximum hitpoints for the player
        uint16_t m_MaxCPU;      // Maximum CPU for the player
        uint16_t m_MaxMemory;   // Maximum memory for the player

	uint8_t m_scriptLevel;	// Level of scripts
	uint8_t m_codeLevel;	// Level of coding
	uint8_t m_hackLevel;	// Level of hacking

	NUtil::LIST_DECLARE( CAttackTarget, m_playerLink ) m_attackTargets;

public:
	NUtil::LIST_LINK( CPlayer ) m_roomLink;
	NUtil::LIST_LINK( CPlayer ) m_worldLink;	// Used by server	
};

#endif // __PLAYER_H__
