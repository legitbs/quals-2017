#ifndef __WORLD_H__
#define __WORLD_H__

#define MAX_SAY_COUNTER (20)
#define MAX_WALL_COUNTER (7)


class CSpamTrackerID
{
protected:
	hash<string> tStringHasher;

public:
	CSpamTrackerID( const CSpamTrackerID &rhs ) : m_sName( rhs.m_sName ) { };
	CSpamTrackerID( string &sName ) : m_sName( sName ) { };
	~CSpamTrackerID( ) { };

	uint32_t GetHashValue( void ) const { return tStringHasher(m_sName); };
	bool operator==( const class CSpamTrackerID &rhs ) const
	{
		return (m_sName == rhs.m_sName);
	}

private:
	string m_sName;
};

class CSpamTracker : public CSpamTrackerID
{
public:
	CSpamTracker( string &sName ) : CSpamTrackerID( sName )
	{
		m_sayCounter = 0;
		m_wallCounter = 0;
	}

	uint32_t GetSayCount( void ) const { return m_sayCounter; };
	uint32_t GetWallCount( void ) const { return m_wallCounter; };

	bool CanSay( void ) { return (m_sayCounter < MAX_SAY_COUNTER); };
	bool CanWall( void ) { return (m_wallCounter < MAX_WALL_COUNTER); };

	void UpdateSay( void ) { m_sayCounter++; };
	void UpdateWall( void ) { m_wallCounter++; };

	void Tick( void )
	{
		if ( m_sayCounter > 5 )
			m_sayCounter -= 5;
		else
			m_sayCounter = 0;
	
		if ( m_wallCounter > 3 )
			m_wallCounter -= 3;
		else
			m_wallCounter = 0;
	}

public:
	NUtil::LIST_LINK( CSpamTracker ) m_spamTrackerLink;
	
private:
	uint32_t m_sayCounter;
	uint32_t m_wallCounter;
};

void InitWorld( void );
void DestroyWorld( void );

void UpdateCheckReboot( void );
void UpdateNPCSpawn( void );
void UpdateSpamTracker( void );
void UpdateEvent( uint32_t tickCount );
void UpdatePlayers( void );
void UpdateNPCs( void );
void UpdateAttacks( void );

void RegenPlayer( CPlayer *pPlayer );
void RegenNPC( CNPC *pNPC );
void DoNPCActions( CNPC *pNPC );

void DoPlayerEnterRoomAction( CPlayer *pPlayer, uint8_t );

typedef NUtil::THashDeclare<CSpamTracker, CSpamTrackerID, offsetof(CSpamTracker, m_spamTrackerLink)>     tGlobalSpamTracker;

#endif // __WORLD_H__
