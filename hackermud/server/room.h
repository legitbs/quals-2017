#ifndef __ROOM_H__
#define __ROOM_H__

#define NO_ROOM_VNUM		(0)

#define DIRECTION_NORTH		(0)
#define DIRECTION_EAST		(1)
#define DIRECTION_SOUTH		(2)
#define DIRECTION_WEST		(3)
#define DIRECTION_UP		(4)
#define DIRECTION_DOWN		(5)

#define DIRECTION_MAX		(6)

#define ROOM_FLAG_PK		(1)	// PK allowed when set
#define ROOM_FLAG_PLAYER_SPAWN	(2)	// Room is a spawn point for players

class CPlayer;
class CNPC;

class CRoomVnum
{
public:
	CRoomVnum( ) : m_vnum( NO_ROOM_VNUM ) { };
	CRoomVnum( const CRoomVnum &rhs ) : m_vnum( rhs.m_vnum ) { };
	CRoomVnum( uint32_t vnum ) : m_vnum( vnum ) { };
	~CRoomVnum( ) { };

	uint32_t GetHashValue( void ) const { return m_vnum; };
	bool operator==( const class CRoomVnum &rhs ) const
	{
		return (m_vnum == rhs.m_vnum);
	}
	
	uint32_t GetVnum( void ) const { return m_vnum; };

protected:	
	void SetVnum( uint32_t vnum ) { m_vnum = vnum; };
	
private:
	uint32_t m_vnum;
};

class CRoom : public CRoomVnum
{
public:
	CRoom( string sName, string sShortDescription, string sDescription, uint32_t roomVnum );
	~CRoom();

	bool ReadRoomData( CStreamData *pData );
	bool WriteRoomData( CStreamData *pData );

	string &GetShortDescription( void ) { return m_sShortDescription; };
	string &GetDescription( void ) { return m_sDescription; };
	string &GetName( void ) { return m_sName; };

	uint32_t GetZoneNum( void ) { return m_zoneNum; };

	bool HasDirection( uint8_t direction ) const { return (m_neighbors[direction].GetVnum() != NO_ROOM_VNUM); };

	bool HasNorth( void ) const { return (m_neighbors[DIRECTION_NORTH].GetVnum() != NO_ROOM_VNUM); };
	bool HasEast( void ) const { return (m_neighbors[DIRECTION_EAST].GetVnum() != NO_ROOM_VNUM); };
	bool HasSouth( void ) const { return (m_neighbors[DIRECTION_SOUTH].GetVnum() != NO_ROOM_VNUM); };
	bool HasWest( void ) const { return (m_neighbors[DIRECTION_WEST].GetVnum() != NO_ROOM_VNUM); };
	bool HasUp( void ) const { return (m_neighbors[DIRECTION_UP].GetVnum() != NO_ROOM_VNUM); };
	bool HasDown( void ) const { return (m_neighbors[DIRECTION_DOWN].GetVnum() != NO_ROOM_VNUM); };

	CRoomVnum GetDirection( uint8_t direction ) const { return m_neighbors[direction]; };
	CRoomVnum GetNorth( void ) const { return m_neighbors[DIRECTION_NORTH]; };
	CRoomVnum GetEast( void ) const { return m_neighbors[DIRECTION_EAST]; };
	CRoomVnum GetSouth( void ) const { return m_neighbors[DIRECTION_SOUTH]; };
	CRoomVnum GetWest( void ) const { return m_neighbors[DIRECTION_WEST]; };
	CRoomVnum GetUp( void ) const { return m_neighbors[DIRECTION_UP]; };
	CRoomVnum GetDown( void ) const { return m_neighbors[DIRECTION_DOWN]; };

	bool GetRandomDirection( uint8_t &chosenDirection );

	static void LoadRooms( void );

	NUtil::LIST_PTR( CPlayer ) GetPlayersInRoom( void ) { return &m_playersInRoom; };
	NUtil::LIST_PTR( CItemInstance ) GetItemsInRoom( void ) { return &m_itemsInRoom; };
	NUtil::LIST_PTR( CNPC ) GetNPCsInRoom( void ) { return &m_npcsInRoom; };

	CItemInstance *GetItemInRoomByID( uint32_t itemID );
	CNPC *GetNPCInRoomByID( uint32_t id );
	CPlayer *GetPlayerInRoomByID( uint32_t id );

	bool AddItemToRoom( CItemInstance *pItem );

	bool RemovePlayer( CPlayer *pPlayer );
	bool AddPlayer( CPlayer *pPlayer );

	bool RemoveNPC( CNPC *pNPC );
	bool AddNPC( CNPC *pNPC );
	
	bool IsPKAllowed( void ) const { return (m_flags & ROOM_FLAG_PK); };
	bool IsPlayerSpawnPoint( void ) const { return (m_flags & ROOM_FLAG_PLAYER_SPAWN); };

private:
	string m_sName;
	string m_sShortDescription;
	string m_sDescription;
	uint32_t m_zoneNum;	// A unique number denoting what zone this room is a part of
	uint8_t m_flags;	// Room flags!

	CRoomVnum m_neighbors[DIRECTION_MAX];

	NUtil::LIST_DECLARE( CItemInstance, m_roomLink ) m_itemsInRoom;
	NUtil::LIST_DECLARE( CPlayer, m_roomLink ) m_playersInRoom;
	NUtil::LIST_DECLARE( CNPC, m_roomLink ) m_npcsInRoom;

public:
	// List of rooms in the world
	NUtil::LIST_LINK( CRoom ) m_worldLink;
	NUtil::LIST_LINK( CRoom ) m_masterRoomListLink;
};

typedef NUtil::THashDeclare<CRoom, CRoomVnum, offsetof(CRoom, m_worldLink)>	tGlobalRoomHash;
typedef NUtil::LIST_DECLARE( CRoom, m_masterRoomListLink ) tGlobalRoomList;
typedef vector<CRoomVnum> tGlobalSpawnPoints;

#endif // __ROOM_H__
