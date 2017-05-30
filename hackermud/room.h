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

#define ROOM_FLAG_PK		(1)

class CPlayer;

class CPlayerInRoom
{
public:
	CPlayerInRoom( string sName, uint32_t playerID )
		: m_sName( sName ), m_playerID( playerID )
	{

	}

	~CPlayerInRoom() { };

	string &GetName( void ) { return m_sName; };
	uint32_t GetID( void ) { return m_playerID; };

private:
	uint32_t m_playerID;
	string m_sName;

public:
	NUtil::LIST_LINK( CPlayerInRoom ) m_roomLink;
};

class CItemInRoom
{
public:
	CItemInRoom( string sName, uint32_t itemID )
		: m_sName( sName ), m_itemID( itemID )
	{

	}

	~CItemInRoom( ) { };

	string &GetName( void ) { return m_sName; };
	uint32_t GetID( void ) { return m_itemID; };

private:
	uint32_t m_itemID;
	string m_sName;

public:
	NUtil::LIST_LINK( CItemInRoom ) m_roomLink;
};

class CNPCInRoom
{
public:
	CNPCInRoom( string sName, uint32_t npcID )
		: m_sName( sName ), m_npcID( npcID )
	{
	
	}

	~CNPCInRoom( ) { };

	string &GetName( void ) { return m_sName; };
	uint32_t GetID( void ) { return m_npcID; };

private:
	string m_sName;
	uint32_t m_npcID;

public:
	NUtil::LIST_LINK( CNPCInRoom ) m_roomLink;
};

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
	CRoom();
	CRoom( string sName, string sShortDescription, string sDescription, uint32_t roomVnum );
	~CRoom();

	bool ReadRoomData( CStreamData *pData );
	bool WriteRoomData( CStreamData *pData );

	string &GetShortDescription( void ) { return m_sShortDescription; };
	string &GetDescription( void ) { return m_sDescription; };
	string &GetName( void ) { return m_sName; };

	bool HasNorth( void ) const { return (m_neighbors[DIRECTION_NORTH].GetVnum() != NO_ROOM_VNUM); };
	bool HasEast( void ) const { return (m_neighbors[DIRECTION_EAST].GetVnum() != NO_ROOM_VNUM); };
	bool HasSouth( void ) const { return (m_neighbors[DIRECTION_SOUTH].GetVnum() != NO_ROOM_VNUM); };
	bool HasWest( void ) const { return (m_neighbors[DIRECTION_WEST].GetVnum() != NO_ROOM_VNUM); };
	bool HasUp( void ) const { return (m_neighbors[DIRECTION_UP].GetVnum() != NO_ROOM_VNUM); };
	bool HasDown( void ) const { return (m_neighbors[DIRECTION_DOWN].GetVnum() != NO_ROOM_VNUM); };

	CRoomVnum GetNorth( void ) const { return m_neighbors[DIRECTION_NORTH]; };
	CRoomVnum GetEast( void ) const { return m_neighbors[DIRECTION_EAST]; };
	CRoomVnum GetSouth( void ) const { return m_neighbors[DIRECTION_SOUTH]; };
	CRoomVnum GetWest( void ) const { return m_neighbors[DIRECTION_WEST]; };
	CRoomVnum GetUp( void ) const { return m_neighbors[DIRECTION_UP]; };
	CRoomVnum GetDown( void ) const { return m_neighbors[DIRECTION_DOWN]; };

	void GetExitList( string &sExitList );

	NUtil::LIST_PTR( CPlayerInRoom ) GetPlayersInRoom( void ) { return &m_playersInRoom; };
	NUtil::LIST_PTR( CItemInRoom ) GetItemsInRoom( void ) { return &m_itemsInRoom; };
	NUtil::LIST_PTR( CNPCInRoom ) GetNPCsInRoom( void ) { return &m_npcsInRoom; };

	CPlayerInRoom *GetPlayerInRoomByID( uint32_t id );
	CNPCInRoom *GetNPCInRoomByID( uint32_t id );
	CItemInRoom *GetItemInRoomByID( uint32_t id );

	bool RemoveItemFromRoom( uint32_t itemID );

	bool IsPKAllowed( void ) const { return (m_flags & ROOM_FLAG_PK); };
private:
	string m_sName;
	string m_sShortDescription;
	string m_sDescription;

	uint8_t m_flags;		// Room flags

	CRoomVnum m_neighbors[DIRECTION_MAX];

	NUtil::LIST_DECLARE( CItemInRoom, m_roomLink ) m_itemsInRoom;
	NUtil::LIST_DECLARE( CPlayerInRoom, m_roomLink ) m_playersInRoom;
	NUtil::LIST_DECLARE( CNPCInRoom, m_roomLink ) m_npcsInRoom;

public:
	// List of rooms in the world
	NUtil::LIST_LINK( CRoom ) m_worldLink;
};

typedef NUtil::THashDeclare<CRoom, CRoomVnum, offsetof(CRoom, m_worldLink)>	tGlobalRoomHash;


#endif // __ROOM_H__
