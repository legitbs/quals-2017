#ifndef __ATTACK_TARGET_H__
#define __ATTACK_TARGET_H__

class CAttackTarget
{
public:
        enum
        {
                TARGET_NPC,	// Must match NWC_ATTACK_NPC
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

        uint32_t GetTargetID( void ) { return m_targetID; };
	uint8_t GetTargetType( void ) { return m_targetType; };

private:
        uint8_t m_targetType;
        uint32_t m_targetID;

public:
        NUtil::LIST_LINK( CAttackTarget ) m_playerLink;
};

#endif // __ATACK_TARGET_H__
