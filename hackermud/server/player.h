#ifndef __PLAYER_H__
#define __PLAYER_H__

class CPlayer;
class CRoom;
class CServerConnection;
class CAttackTarget;

class CPlayerLoginCacheID
{
protected:
        hash<string> tStringHasher;

public:
        CPlayerLoginCacheID( const CPlayerLoginCacheID &rhs ) : m_sName( rhs.m_sName ) { };
        CPlayerLoginCacheID( string &sName ) : m_sName( sName ) { };
        ~CPlayerLoginCacheID( ) { };

        uint32_t GetHashValue( void ) const { return tStringHasher(m_sName); };
        bool operator==( const class CPlayerLoginCacheID &rhs ) const
        {
                return (m_sName == rhs.m_sName);
        }

private:
        string m_sName;
};

class CPlayerLoginCache : public CPlayerLoginCacheID
{
public:
        CPlayerLoginCache( string &sName, CStreamData *pData, string &sPassword );
	~CPlayerLoginCache();

	CPlayer *CreateNewPlayer( void );

	string &GetPassword( void );

	void SaveNewStream( CStreamData *pData );

public:
        NUtil::LIST_LINK( CPlayerLoginCache ) m_playerLoginCacheLink;

private:
	CStreamData *m_pPlayerStream;
	string m_sPassword;	// Password is not stored in player stream data
};

class CPlayer
{
public:
	CPlayer( );
	~CPlayer();

	bool IsConnected( void ) { return (m_pConnection != NULL); };

	void ClearData( void );

	bool ReadPlayerData( CStreamData *pData, bool bPlayerCache = false );
	bool WritePlayerData( CStreamData *pData, bool bPlayerCache = false );

	void SetName( string &sNewName ) { m_sPlayerName = sNewName; };
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

	uint16_t GetCurrentDiskSpace( void ) const { return m_curDiskSpace; };
	uint16_t GetMaxDiskSpace( void ) const { return m_maxCarryDisk; };

	uint8_t GetCurrentCarryItems( void ) const { return m_curDiskItems; };
	uint8_t GetMaxCarryItems( void ) const { return m_maxCarryItems; };

	string &GetPasswordHash( void ) { return m_sPasswordHash; };
	bool CheckPasswordHash( string sPasswordHash );

	static CPlayer *LoadFile( const char *pszPlayerName, bool bToRoom = true );
	bool SaveFile( void );

	void SetPasswordHash( string &sPasswordHash ) { m_sPasswordHash = sPasswordHash; };
	static bool GetPasswordOnly( const char *pszPlayerName, string &sPassword );

	CRoom *GetRoom( void ) { return m_pCurrentRoom; };
	void SetRoom( CRoom *pRoom );

	CServerConnection *GetConnection( void ) { return m_pConnection; };
	void SetConnection( CServerConnection *pConnection ) 
	{
		m_pConnection = pConnection; 
	}
	void Disconnect( void ) { m_pConnection = NULL; };

	CItemInstance *GetInstallItem( tItemEquipType slot ) { return m_pEquipItems[slot]; };

        NUtil::LIST_PTR( CItemInstance ) GetItemsOnPlayer( void ) { return &m_itemsOnPlayer; };

	bool RemoveItemFromInventory( CItemInstance *pItemToRemove, bool bSaveFile = true );
	bool AddItemToInventory( CItemInstance *pItemToAdd, bool bSaveFile = true );
	CItemInstance *GetItemFromInventoryByID( uint32_t itemID );

	bool EquipItem( CItemInstance *pItem, tItemEquipType slotEquipped, string &sFailReason );	// Equip item from inventory into an equip slot
	bool UnequipItem( tItemEquipType slot, CItemInstance *&pItem, string &sFailReason );	// During unequip -- item goes into inventory

	void StopFighting( void );
	bool IsFighting( void );
	bool AttackNPC( uint32_t npcID );
        bool AttackPlayer( uint32_t playerID );
	bool StopFightingTarget( uint32_t targetID, uint8_t targetType );
	
	CAttackTarget *GetPrimaryTarget( void ) { return m_attackTargets.Head(); };

	bool IsFightingTarget( uint32_t targetID, uint8_t targetType );

	bool Kill( uint32_t killerID, string sKillerName, uint8_t killerType );

	uint8_t *GetItemFingerprint( uint32_t &outLen );

	void DoExit( void );	// Player exiting world

private:
	CItemInstance *m_pEquipItems[EQUIP_SLOT_MAX];
	CRoom *m_pCurrentRoom;		// Current room player is in

	uint16_t m_maxCarryDisk;	// Maximum carry disk space of player
	uint8_t m_maxCarryItems;	// Maximum number of items a player can carry

	uint16_t m_curDiskSpace;	// Current in use disk space
	uint8_t m_curDiskItems;		// Current in use disk items

	NUtil::LIST_DECLARE( CItemInstance, m_playerLink ) m_itemsOnPlayer;	

	string m_sPlayerName;
	string m_sPlayerDescription;
	string m_sPasswordHash;

	// used to identify a player
	uint32_t m_playerID;	// Player ID
	uint64_t m_playerKey;	// Player KEY (unique that only your client and server know)

	uint16_t m_HP;		// Hitpoints	
	uint16_t m_CPU;		// CPU time
	uint16_t m_Memory;	// Memory consumption

	uint16_t m_MaxHP;	// Maximum hitpoints for the player
	uint16_t m_MaxCPU;	// Maximum CPU for the player
	uint16_t m_MaxMemory;	// Maximum memory for the player

	uint8_t m_scriptLevel;	// Level of scripts
	uint8_t m_codeLevel;	// Level of coding
	uint8_t m_hackLevel;	// Level of hacking

	CServerConnection *m_pConnection;	// Server connection

	NUtil::LIST_DECLARE( CAttackTarget, m_playerLink ) m_attackTargets;

	static uint32_t m_lastPlayerID;

public:
	NUtil::LIST_LINK( CPlayer ) m_roomLink;
	NUtil::LIST_LINK( CPlayer ) m_worldLink;	// Used by server	
};

typedef NUtil::THashDeclare<CPlayerLoginCache, CPlayerLoginCacheID, offsetof(CPlayerLoginCache, m_playerLoginCacheLink)> tGlobalPlayerLoginCache;

#endif // __PLAYER_H__
