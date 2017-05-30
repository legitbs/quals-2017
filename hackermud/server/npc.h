#ifndef __NPC_H__
#define __NPC_H__

#define NPC_FLAG_STATIONARY		(1)	// NPC does not move
#define NPC_FLAG_BLOCKSFLEE		(2)	// NPC does not allow you to flee
#define NPC_FLAG_EXTRAREGEN		(4)	// NPC enjoys extra regeneration
#define NPC_FLAG_ROAMS			(8)	// NPC allowed to aggressively roam zone
#define NPC_FLAG_AGGRESSIVE		(16)	// NPC is aggressive -- will attack randomly
#define NPC_FLAG_ATTACKONARRIVE		(32)	// NPC attacks anyone arriving
#define NPC_FLAG_FLEES			(64)	// NPC will flee when getting low
#define NPC_FLAG_DAMAGESCRIPT		(128)	// NPC will run damage scripts
#define NPC_FLAG_HEALSCRIPT		(256)	// NPC will run heal scripts

#define NPC_NORMAL_ROAM_TICK_RATE	(180)
#define NPC_FAST_ROAM_TICK_RATE		(60)

#define HP_REGEN_RATE_PERCENTAGE	(5)
#define CPU_REGEN_RATE_PERCENTAGE	(19)
#define MEMORY_REGEN_RATE_PERCENTAGE	(10)

class CRoom;

class CBaseNPCVnum
{
public:
	CBaseNPCVnum( uint32_t vnum ) : m_vnum( vnum ) { };
	
	uint32_t GetHashValue( void ) const { return m_vnum; };
	bool operator==(const class CBaseNPCVnum &t ) const
	{
		return (m_vnum == t.m_vnum);
	}

	uint32_t GetVnum( void ) const { return m_vnum; };
	
private:
	uint32_t m_vnum;
};

class CBaseNPC : public CBaseNPCVnum
{
public:
	CBaseNPC( uint32_t vnum );
	~CBaseNPC( );

public:
	NUtil::HASH_LINK(CBaseNPC)	m_worldDatabase;
	friend class CNPC;

	static void LoadNPCs( void );

private:
	string m_sName;
	string m_sDescription;
	
	uint16_t m_baseHP;
	uint16_t m_baseCPU;
	uint16_t m_baseMemory;

	uint8_t m_baseScriptLevel;
	uint8_t m_baseCodeLevel;
	uint8_t m_baseHackLevel;

	uint32_t m_flags;	// NPC flags

	// NPC base equipment
	typedef struct
	{
		uint32_t vnum;
		bool bPopulated;
		bool bModStats;
	} tBaseEquipItem;

	tBaseEquipItem m_baseEquipItems[EQUIP_SLOT_MAX];

	typedef struct
	{
		uint32_t vnum;
		bool bModStats;
	} tBaseInventoryItem;	

	vector<tBaseInventoryItem> m_baseItems;	// NPC items	
};

class CNPC
{
public:
	CNPC( CBaseNPC *pBaseNPC );
	~CNPC( );

	static CNPC *SpawnNPC( uint32_t baseVnum );

	uint32_t GetVnum( void )
	{
		if ( m_pBaseNPC )
			return m_pBaseNPC->GetVnum();
	}

	bool ReadNPCData( CStreamData *pData );
	bool WriteNPCData( CStreamData *pData );

	string &GetName( void ) { return m_sName; };
	string &GetDescription( void ) { return m_sDescription; };

	uint16_t GetCurHP( void ) const { return m_curHP; };
	uint16_t GetCurCPU( void ) const { return m_curCPU; };
	uint16_t GetCurMemory( void ) const { return m_curMemory; };

	uint16_t GetMaxHP( void ) const { return m_maxHP; };
	uint16_t GetMaxCPU( void ) const { return m_maxCPU; };
	uint16_t GetMaxMemory( void ) const { return m_maxMemory; };

	void SetHP( uint16_t hp ) { m_curHP = hp; };
	void SetCPU( uint16_t cpu ) { m_curCPU = cpu; };
	void SetMemory( uint16_t memory ) { m_curMemory = memory; };

	uint8_t GetCodeLevel( void ) { return m_codeLevel; };
	uint8_t GetHackLevel( void ) { return m_hackLevel; };
	uint8_t GetScriptLevel( void ) { return m_scriptLevel; };

	uint32_t GetNPCID( void ) const { return m_npcID; };

	uint32_t GetRoamCounter( void ) const { return m_roamCounter; };
	void ResetRoamCounter( void ) { m_roamCounter = 0; };
	void UpdateRoamCounter( void ) { m_roamCounter++; };

	bool CanFlee( void ) { return (m_flags & NPC_FLAG_FLEES); };
	bool CanBlockFlee( void ) { return (m_flags & NPC_FLAG_BLOCKSFLEE); };
	bool IsFastRoamer( void ) { return (m_flags & NPC_FLAG_ROAMS); };
	bool HasExtraRegen( void ) { return (m_flags & NPC_FLAG_EXTRAREGEN); };
	bool IsNPCAggressive( void ) { return (m_flags & NPC_FLAG_AGGRESSIVE); };
	bool IsNPCAggressOnEnter( void ) { return (m_flags & NPC_FLAG_ATTACKONARRIVE); };
	bool IsStationary( void ) { return (m_flags & NPC_FLAG_STATIONARY); };
	bool HasHealScript( void ) { return (m_flags & NPC_FLAG_HEALSCRIPT); };
	bool HasDamageScript( void ) { return (m_flags & NPC_FLAG_DAMAGESCRIPT); };

	bool ToRoom( CRoom *pRoom, uint8_t arrivalType );
	bool FromRoom( uint8_t leaveType, uint8_t leaveDirection );
	CRoom *GetRoom( void ) const { return m_pCurrentRoom; };

	bool IsFighting( void );
	bool AttackNPC( uint32_t npcID );
        bool AttackPlayer( uint32_t playerID );
	bool StopFightingTarget( uint32_t targetID, uint8_t targetType );

	bool IsFightingPlayer( CPlayer *pPlayer );

	CAttackTarget *GetPrimaryTarget( void ) { return m_attackTargets.Head(); };

	bool Kill( uint32_t killerID, string sKillerName, uint8_t killerType );

	uint8_t *GetItemFingerprint( uint32_t &outLen );

private:
	void Init( void );

	CItemInstance *m_pEquipItems[EQUIP_SLOT_MAX];
	CRoom *m_pCurrentRoom;

	vector<CItemInstance *> m_itemsOnNPC;

	string m_sName;
	string m_sDescription;

	uint32_t m_flags;
	
	uint32_t m_npcID;
	
	uint16_t m_curHP;
	uint16_t m_curCPU;
	uint16_t m_curMemory;

	uint16_t m_maxHP;
	uint16_t m_maxCPU;
	uint16_t m_maxMemory;

	uint8_t m_scriptLevel;
	uint8_t m_codeLevel;
	uint8_t m_hackLevel;	

	static uint32_t m_lastNPCID;

	CBaseNPC *m_pBaseNPC;

	uint32_t m_roamCounter;		// Tick counter for roaming

	NUtil::LIST_DECLARE( CAttackTarget, m_playerLink ) m_attackTargets;
	
public:
	NUtil::LIST_LINK( CNPC ) m_roomLink;
	NUtil::LIST_LINK( CNPC ) m_worldLink;	
	
};

typedef NUtil::HASH_DECLARE( CBaseNPC, CBaseNPCVnum, m_worldDatabase ) tNPCDatabaseHash;


#endif // __NPC_H__
