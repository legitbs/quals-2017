#ifndef __DAMAGE_H__
#define __DAMAGE_H__

#define KILLER_TYPE_PLAYER	(1)
#define KILLER_TYPE_NPC		(2)
#define KILLER_TYPE_WORLD	(3) // No specific killer -- the world killed them!

#define DAMAGE_TYPE_PLAYER	(1)
#define DAMAGE_TYPE_NPC		(2)
#define DAMAGE_TYPE_WORLD	(3) // No specific damage source -- world damage (room on fire etc.)

uint16_t CalcHackDamage( uint8_t hackLevelAttacker, uint8_t hackLevelDefender );

string GetDamageString( uint16_t damageAmount, uint16_t maxHP );

void DoLevelPlayer( CPlayer *pPlayer, uint8_t targetHackLevel, uint8_t targetCodeLevel, uint8_t targetScriptLevel, uint8_t targetType );

bool DoHackDamage( CPlayer *pAttacker, CNPC *pTarget, uint16_t damageAmount );
bool DoHackDamage( CPlayer *pAttacker, CPlayer *pTarget, uint16_t damageAmount );
bool DoHackDamage( CNPC *pAttacker, CPlayer *pTarget, uint16_t damageAmount );
bool DoHackDamage( CNPC *pAttacker, CNPC *pTarget, uint16_t damageAmount );

#endif // __DAMAGE_H__
