#include "common.h"

#define HACK_LEVEL_BASE_DAMAGE_MIN		(5)
#define HACK_LEVEL_BASE_DAMAGE_MAX		(13)
#define HACK_BASE_DAMAGE_MULTIPLIER		(1.07)
#define HACK_LEVEL_MULTIPLIER_PERCENTAGE	(1.1)

void DoLevelPlayer( CPlayer *pPlayer, uint8_t targetHackLevel, uint8_t targetCodeLevel, uint8_t targetScriptLevel, uint8_t targetType )
{
	// Possibly level a player if one of their levels is higher than yours...
	uint8_t myHackLevel = pPlayer->GetHackLevel();
	uint8_t myCodeLevel = pPlayer->GetCodeLevel();
	uint8_t myScriptLevel = pPlayer->GetScriptLevel();

	// Level a player based on two things...
	// Randomly level your lowest level area and level your highest skill delta...
	// if the skill delta is the same for one (or two, or three) skills
	// then randomly pick a skill to level
	uint8_t hackDelta = 0;
	uint8_t codeDelta = 0;
	uint8_t scriptDelta = 0;

	uint8_t newHackLevel = myHackLevel;
	uint8_t newCodeLevel = myCodeLevel;
	uint8_t newScriptLevel = myScriptLevel;

	if ( myHackLevel < targetHackLevel )
		hackDelta = targetHackLevel - myHackLevel;

	if ( myCodeLevel < targetCodeLevel )
		codeDelta = targetCodeLevel - myCodeLevel;

	if ( myScriptLevel < targetScriptLevel )
		scriptDelta = targetScriptLevel - myScriptLevel;

	// Cap our leveling...
	if ( myHackLevel == 255 )
		hackDelta = 0;

	if ( myCodeLevel == 255 )
		codeDelta = 0;

	if ( myScriptLevel == 255 )
		scriptDelta = 0;

	// Strange situation... if all zeros...
	bool bExtraLevel = false;
	if ( hackDelta == 0 && codeDelta == 0 && scriptDelta == 0 )
	{
		// Very low chance of leveling the lowest attribute if at least one attribute is
		// close!
		bool bLevelChance = false;
		
		if ( myHackLevel - targetHackLevel < 3 || myCodeLevel - targetCodeLevel < 3 || myScriptLevel - targetScriptLevel < 3 )
			if ( g_oRNG.GetRange( 0, 5 ) == 0 )
				bExtraLevel = true;
	}
	else
	{	
		bool bLevelHack = false;
		bool bLevelCode = false;
		bool bLevelScript = false;

		if ( hackDelta > codeDelta && hackDelta > scriptDelta )
			bLevelHack = true;
		else if ( codeDelta > hackDelta && codeDelta > scriptDelta )
			bLevelCode = true;
		else if ( scriptDelta > hackDelta && scriptDelta > codeDelta )
			bLevelScript = true;
		else if ( hackDelta == scriptDelta && hackDelta > codeDelta )
		{
			if ( g_oRNG.GetRange( 0, 1 ) == 0 )
				bLevelHack = true;
			else
				bLevelScript = true;
		}		
		else if ( hackDelta == codeDelta && hackDelta > scriptDelta )
		{
			if ( g_oRNG.GetRange( 0, 1 ) == 0 )
				bLevelHack = true;
			else
				bLevelCode = true;
		}
		else if ( scriptDelta == codeDelta && scriptDelta > hackDelta )
		{
			if ( g_oRNG.GetRange( 0, 1 ) == 0 )
				bLevelScript = true;
			else
				bLevelCode = true;
		}
		else if ( hackDelta == scriptDelta && hackDelta == codeDelta )
		{
			switch( g_oRNG.GetRange( 0, 2 ) )
			{
			case 0:
				bLevelHack = true;
				break;
			case 1:
				bLevelCode = true;
				break;
			case 2:
				bLevelScript = true;
				break;
			}
		}
	
		if ( g_oRNG.GetRange( 0, 30 ) == 0 )
			bExtraLevel = true;	

		if ( bLevelHack && myHackLevel < 255 )
			newHackLevel++;
		
		if ( bLevelScript && myScriptLevel < 255 )
			newScriptLevel++;

		if ( bLevelCode && myCodeLevel < 255 )
			newCodeLevel++;
	}

	if ( bExtraLevel )
	{
		switch( g_oRNG.GetRange( 0, 2 ) )
		{
		case 0:
			if ( myHackLevel < 255 )
				newHackLevel++;
			break;
		
		case 1:
			if ( myCodeLevel < 255 )
				newCodeLevel++;
			break;

		case 2:
			if ( myScriptLevel < 255 )
				newScriptLevel++;
			break;
		}
	}

	if ( newHackLevel > myHackLevel || newCodeLevel > myCodeLevel || newScriptLevel > myScriptLevel )
		;
	else
		return; // No leveling occurred

	// Cap levels...
	if ( newHackLevel > PLAYER_MAX_HACK_LEVEL )
		newHackLevel = PLAYER_MAX_HACK_LEVEL;
	
	if ( newCodeLevel > PLAYER_MAX_CODE_LEVEL )
		newCodeLevel = PLAYER_MAX_CODE_LEVEL;

	if ( newScriptLevel > PLAYER_MAX_SCRIPT_LEVEL )
		newScriptLevel = PLAYER_MAX_SCRIPT_LEVEL;

	// OK level up message
	pPlayer->SetHackLevel( newHackLevel );
	pPlayer->SetCodeLevel( newCodeLevel );
	pPlayer->SetScriptLevel( newScriptLevel );

	// Save player data
	pPlayer->SaveFile();

	// Send leveling to player
	NWCPlayerLevel( pPlayer, (newHackLevel-myHackLevel), (newCodeLevel-myCodeLevel), (newScriptLevel-myScriptLevel) );

	CRoom *pRoom = pPlayer->GetRoom();
	
	if ( !pRoom )
		return;

	char szTemp[1024];
	snprintf( szTemp, 1024, "%s gains experience!", pPlayer->GetName().c_str() );
	string sInfoMessage = szTemp;

	for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
	{
		if ( pCur != pPlayer && pCur->IsConnected() )
			NWCPlayerInfoMessage( pCur, sInfoMessage );
	}
}

string GetDamageString( uint16_t damageAmount, uint16_t maxHP )
{
	string sDamageString;
	if ( maxHP == 0 )
	{
		sDamageString = "";
		return sDamageString;
	}

	// Damage as a percentage of HP
	uint16_t percentage = (uint16_t)(((double)damageAmount / (double)maxHP) * 100.0);

	if ( percentage == 0 )
		sDamageString = "does nothing to";
	else if ( percentage <= 2 )
		sDamageString = "barely affects";
	else if ( percentage <= 5 )
		sDamageString = "noticeably affects";
	else if ( percentage <= 7 )
		sDamageString = "significantly affects";
	else if ( percentage <= 10 )
		sDamageString = "noticeably disrupts";
	else if ( percentage <= 13 )
		sDamageString = "significantly disrupts";
	else if ( percentage <= 17 )
		sDamageString = "majorly disrupts";
	else if ( percentage <= 22 )
		sDamageString = "majorly damages";
	else if ( percentage <= 27 )
		sDamageString = "severly damages";
	else if ( percentage <= 35 )
		sDamageString = "massively damages";
	else if ( percentage <= 50 )
		sDamageString = "causes considerable system wide damages to";
	else if ( percentage <= 75 )
		sDamageString = "causes severe system wide damages to";
	else if ( percentage <= 99 )
		sDamageString = "eviscerates the entire system of";
	else if ( percentage >= 100 )
		sDamageString = "annihilates the entire system of";
	else
		sDamageString = "";

	return sDamageString;
		
}

uint16_t CalcHackDamage( uint8_t hackLevelAttacker, uint8_t hackLevelDefender )
{
	// Calculate ratio...
	uint16_t baseDamageAmount = g_oRNG.GetRange( HACK_LEVEL_BASE_DAMAGE_MIN, HACK_LEVEL_BASE_DAMAGE_MAX );

	double baseMultiplier = HACK_BASE_DAMAGE_MULTIPLIER;
	baseMultiplier = pow( baseMultiplier, hackLevelAttacker );

	baseDamageAmount = ((double)baseDamageAmount * baseMultiplier);

	double multiplier = 1.0;
	if ( hackLevelAttacker > hackLevelDefender )
		multiplier = pow( HACK_LEVEL_MULTIPLIER_PERCENTAGE, hackLevelAttacker-hackLevelDefender );	
	else if ( hackLevelAttacker < hackLevelDefender )
		multiplier = (1.0 / pow( HACK_LEVEL_MULTIPLIER_PERCENTAGE, hackLevelDefender - hackLevelAttacker ));	
	else
		multiplier = 1.0;

	uint16_t damageAmount;
	if ( ((double)baseDamageAmount * multiplier) > 10000 )
		damageAmount = 10000;
	else
		damageAmount = ((double)baseDamageAmount * multiplier);
	
	return (damageAmount);
}

bool DoHackDamage( CPlayer *pAttacker, CNPC *pTarget, uint16_t damageAmount )
{
	uint16_t targetCurHP = pTarget->GetCurHP();
	uint16_t targetMaxHP = pTarget->GetMaxHP();

	string sDamageString = GetDamageString( damageAmount, targetMaxHP );
	string sDamageSource = "hacking";

	// printf( "Player %s damages NPC %s (amount: %d/%d)\n", pAttacker->GetName().c_str(), pTarget->GetName().c_str(), damageAmount, targetCurHP );

	// Inform attacker they have damage a target
	NWCPlayerDamageNPC( pAttacker, pTarget, damageAmount, sDamageString, sDamageSource );
	
	if ( damageAmount >= targetCurHP )
	{
		uint8_t targetHackLevel = pTarget->GetHackLevel();
		uint8_t targetCodeLevel = pTarget->GetCodeLevel();
		uint8_t targetScriptLevel = pTarget->GetScriptLevel();

		// Stop fighting the target
		pAttacker->StopFightingTarget( pTarget->GetNPCID(), CAttackTarget::TARGET_NPC );

		// Kill em
		pTarget->Kill( pAttacker->GetPlayerID(), pAttacker->GetName(), KILLER_TYPE_PLAYER );
		
		// Level the attacker (potentially)
		DoLevelPlayer( pAttacker, targetHackLevel, targetCodeLevel, targetScriptLevel, NWC_ATTACK_PLAYER );

		// Returning true -- informs caller that the target has been killed
		return (true);
	}

	// Modify targets hp
	pTarget->SetHP( targetCurHP - damageAmount );

	
	// Inform neighbors someone has damaged a target!
	CRoom *pRoom = pAttacker->GetRoom();

	if ( pRoom )
	{
		// Print damage message
		char szTemp[1024];
	
		snprintf( szTemp, 1024, "%s's %s %s %s", pAttacker->GetName().c_str(), sDamageSource.c_str(), sDamageString.c_str(), pTarget->GetName().c_str() );
		string sMessage = szTemp;

		for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
		{
			if ( pCur->IsConnected() && pCur != pAttacker )
				NWCPlayerInfoMessage( pCur, sMessage );
		}
	}

	return (false);
}

bool DoHackDamage( CPlayer *pAttacker, CPlayer *pTarget, uint16_t damageAmount )
{
	uint16_t targetCurHP = pTarget->GetCurHP();
	uint16_t targetMaxHP = pTarget->GetMaxHP();

	string sDamageString = GetDamageString( damageAmount, targetMaxHP );
	string sDamageSource = "hacking";

	// printf( "Player %s damages player %s (amount: %d/%d)\n", pAttacker->GetName().c_str(), pTarget->GetName().c_str(), damageAmount, targetCurHP );

	// Inform attacker they have damage a target
	NWCPlayerDamagePlayer( pAttacker, pTarget, damageAmount, sDamageString, sDamageSource );
	
	if ( damageAmount >= targetCurHP )
	{
		uint8_t targetHackLevel = pTarget->GetHackLevel();
		uint8_t targetCodeLevel = pTarget->GetCodeLevel();
		uint8_t targetScriptLevel = pTarget->GetScriptLevel();

		// Stop fighting the target
		pAttacker->StopFightingTarget( pTarget->GetPlayerID(), CAttackTarget::TARGET_PLAYER );
		
		// Kill em
		pTarget->Kill( pAttacker->GetPlayerID(), pAttacker->GetName(), KILLER_TYPE_PLAYER );

		// Level the attacker (potentially)
		DoLevelPlayer( pAttacker, targetHackLevel, targetCodeLevel, targetScriptLevel, NWC_ATTACK_PLAYER );
		
		// Returning true -- informs caller that the target has been killed
		return (true);
	}

	// Modify targets hp
	pTarget->SetHP( targetCurHP - damageAmount );

	// Inform target they have been damaged
	NWCPlayerTakesDamage( pTarget, pAttacker->GetPlayerID(), DAMAGE_TYPE_PLAYER, damageAmount, sDamageString, sDamageSource );
	
	// Inform neighbors someone has damaged a target!
	CRoom *pRoom = pAttacker->GetRoom();

	if ( pRoom )
	{
		// Print damage message
		char szTemp[1024];
	
		snprintf( szTemp, 1024, "%s's %s %s %s", pAttacker->GetName().c_str(), sDamageSource.c_str(), sDamageString.c_str(), pTarget->GetName().c_str() );
		string sMessage = szTemp;

		for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
		{
			if ( pCur->IsConnected() && pCur != pAttacker && pCur != pTarget )
				NWCPlayerInfoMessage( pCur, sMessage );
		}
	}

	return (false);
}

bool DoHackDamage( CNPC *pAttacker, CPlayer *pTarget, uint16_t damageAmount )
{
	uint16_t targetCurHP = pTarget->GetCurHP();
	uint16_t targetMaxHP = pTarget->GetMaxHP();

	string sDamageString = GetDamageString( damageAmount, targetMaxHP );
	string sDamageSource = "hacking";

	// printf( "NPC %s damages player %s (amount: %d/%d)\n", pAttacker->GetName().c_str(), pTarget->GetName().c_str(), damageAmount, targetCurHP );

	if ( damageAmount >= targetCurHP )
	{
		// Stop fighting the target
		pAttacker->StopFightingTarget( pTarget->GetPlayerID(), CAttackTarget::TARGET_PLAYER );
		
		// Kill em
		pTarget->Kill( pAttacker->GetNPCID(), pAttacker->GetName(), KILLER_TYPE_NPC );

		// Returning true -- informs caller that the target has been killed
		return (true);
	}

	// Modify targets hp
	pTarget->SetHP( targetCurHP - damageAmount );

	// Inform target they have been damaged
	NWCPlayerTakesDamage( pTarget, pAttacker->GetNPCID(), DAMAGE_TYPE_NPC, damageAmount, sDamageString, sDamageSource );
	
	// Inform neighbors someone has damaged a target!
	CRoom *pRoom = pAttacker->GetRoom();

	if ( pRoom )
	{
		// Print damage message
		char szTemp[1024];
	
		snprintf( szTemp, 1024, "%s's %s %s %s", pAttacker->GetName().c_str(), sDamageSource.c_str(), sDamageString.c_str(), pTarget->GetName().c_str() );
		string sMessage = szTemp;

		for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
		{
			if ( pCur->IsConnected() && pCur != pTarget )
				NWCPlayerInfoMessage( pCur, sMessage );
		}
	}

	return (false);
}

bool DoHackDamage( CNPC *pAttacker, CNPC *pTarget, uint16_t damageAmount )
{
	uint16_t targetCurHP = pTarget->GetCurHP();
	uint16_t targetMaxHP = pTarget->GetMaxHP();

	string sDamageString = GetDamageString( damageAmount, targetMaxHP );
	string sDamageSource = "hacking";

	// printf( "NPC %s damages player %s (amount: %d/%d)\n", pAttacker->GetName().c_str(), pTarget->GetName().c_str(), damageAmount, targetCurHP );

	if ( damageAmount >= targetCurHP )
	{
		// Stop fighting the target
		pAttacker->StopFightingTarget( pTarget->GetNPCID(), CAttackTarget::TARGET_NPC );
		
		// Kill em
		pTarget->Kill( pAttacker->GetNPCID(), pAttacker->GetName(), KILLER_TYPE_NPC );

		// Returning true -- informs caller that the target has been killed
		return (true);
	}

	// Modify targets hp
	pTarget->SetHP( targetCurHP - damageAmount );

	// Inform neighbors someone has damaged a target!
	CRoom *pRoom = pAttacker->GetRoom();

	if ( pRoom )
	{
		// Print damage message
		char szTemp[1024];
	
		snprintf( szTemp, 1024, "%s's %s %s %s", pAttacker->GetName().c_str(), sDamageSource.c_str(), sDamageString.c_str(), pTarget->GetName().c_str() );
		string sMessage = szTemp;

		for ( CPlayer *pCur = pRoom->GetPlayersInRoom()->Head(); pCur; pCur = pRoom->GetPlayersInRoom()->Next( pCur ) )
		{
			if ( pCur->IsConnected() )
				NWCPlayerInfoMessage( pCur, sMessage );
		}
	}

	return (false);
}
