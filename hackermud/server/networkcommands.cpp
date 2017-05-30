#include "common.h"

#if ENABLE_LOGIN_HASH
ZZ g_privateKey = conv<ZZ>("783385286155252386550853732006353453961511019120694890455706379577026400378627256676750396616085482867396016499192952783233266082678571787973880614417804190096907169707099871816633013626556489941241920474258038215833520312939101626937089838867160387929438132279063907792225777899327401847981743401634291985");

ZZ g_privateN = conv<ZZ>("127713237559096456859162440384329319184765043930629306059193106961043729357248991345333807818478592762886897346536339668046663082737576018080706999569899607904576509200118346219709320734780197104056159818430712295438274249934006534622016896686853614596024838698725229857848176809494403034568907629979751897751");
#endif

// Helper function to calculate the amount of CPU needed to move
uint16_t GetMovementCPUNeeded( CPlayer *pPlayer )
{
        uint16_t moveCPUNeeded = 0;

        moveCPUNeeded = (uint16_t)(pPlayer->GetMaxCPU() * MOVEMENT_CPU_THRESHOLD_PERCENTAGE);

        if ( moveCPUNeeded < MOVEMENT_CPU_THRESHOLD_MIN )
                moveCPUNeeded = MOVEMENT_CPU_THRESHOLD_MIN;

        return (moveCPUNeeded);
}

bool CanMoveCPUCheck( CPlayer *pPlayer )
{
        if ( !pPlayer )
                return (false);

        if ( pPlayer->GetCurCPU() < GetMovementCPUNeeded( pPlayer ) )
                return (false);
        else
                return (true);
}

void NWCCreatePlayerResponse( CServerConnection *pCur, uint8_t response )
{
	if ( !pCur )
		return;

	CStreamData oStream;

	oStream.Write( &response, sizeof(response) );

	if ( !pCur->SendCommand( NWC_PLAYER_CREATE_RESPONSE, &oStream ) )
		pCur->Disconnect();
}

void NWCSendAuthProof( CServerConnection *pCur, uint8_t *pProofSeed, uint32_t seedLength )
{
	if ( !pCur )
		return;

	if ( seedLength != 16 )
		return;

	CStreamData oStream;

	uint8_t POW_K = PROOF_OF_WORK_K;
	uint8_t POW_N = PROOF_OF_WORK_N;

	oStream.Write( (uint8_t*)&POW_K, sizeof(POW_K) );
	oStream.Write( (uint8_t*)&POW_N, sizeof(POW_N) );
	
	oStream.Write( pProofSeed, seedLength );

	if ( !pCur->SendCommand( NWC_AUTH_PROOF, &oStream ) )
		pCur->Disconnect();
}

void NWCSendAuthChallenge( CServerConnection *pCur, uint8_t *pChallengeData, uint32_t challengeLength )
{
	if ( !pCur )
		return;

	CStreamData oStream;

	oStream.Write( &challengeLength, sizeof(challengeLength) );
	oStream.Write( pChallengeData, challengeLength );

	if ( !pCur->SendCommand( NWC_AUTH_CHALLENGE, &oStream ) )
		pCur->Disconnect();
}

void NWCDeleteItemInRoom( CRoom *pRoom, CItemInstance *pItem, string &sMessage )
{
	if ( !pRoom )
		return;

	if ( !pItem )
		return;

	for ( CPlayer *pPlayer = (CPlayer *)pRoom->GetPlayersInRoom()->Head(); pPlayer; pPlayer = (CPlayer *)pRoom->GetPlayersInRoom()->Next( pPlayer ) )
	{
		if ( pPlayer->IsConnected() && pPlayer->GetConnection() )
		{
			CStreamData oStream;

			uint32_t playerID = pPlayer->GetPlayerID();
			uint32_t itemID = pItem->GetItemID();

			oStream.Write( &playerID, sizeof(playerID) );
			oStream.Write( &itemID, sizeof(itemID) );
			
			uint8_t deleteMessageLength = sMessage.length();
			oStream.Write( &deleteMessageLength, sizeof(deleteMessageLength) );
			oStream.Write( sMessage.c_str(), deleteMessageLength );

			if ( !pPlayer->GetConnection()->SendCommand( NWC_ITEM_IN_ROOM_DELETED, &oStream ) )
				pPlayer->GetConnection()->Disconnect();
		}
	}
}

void NWCPlayerFingerResponse( CServerConnection *pCur, uint32_t consumeItemID, uint32_t targetID, uint8_t targetType, uint8_t targetHackLevel, uint8_t targetCodeLevel, uint8_t targetScriptLevel, uint16_t targetCurHP, uint16_t targetMaxHP, uint8_t *pItemFingerprint, uint8_t outLen )
{
	if ( !pCur || !pCur->GetPlayer() )
		return;

	uint32_t playerID = pCur->GetPlayer()->GetPlayerID();

	CStreamData oStream;

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &consumeItemID, sizeof(consumeItemID) );
	oStream.Write( &targetID, sizeof(targetID) );
	oStream.Write( &targetType, sizeof(targetType) );
	oStream.Write( &targetHackLevel, sizeof(targetHackLevel) );
	oStream.Write( &targetCodeLevel, sizeof(targetCodeLevel) );
	oStream.Write( &targetScriptLevel, sizeof(targetScriptLevel) );
	oStream.Write( &targetCurHP, sizeof(targetCurHP) );
	oStream.Write( &targetMaxHP, sizeof(targetMaxHP) );

	oStream.Write( &outLen, sizeof(outLen) );
	oStream.Write( pItemFingerprint, outLen );
	
	if ( !pCur->SendCommand( NWC_PLAYER_FINGER_RESPONSE, &oStream ) )
	{
		pCur->Disconnect();
		return;
	} 
}

void NWCPlayerRunsScript( CServerConnection *pCur, uint8_t scriptNum, uint16_t cpuUsed, uint16_t memoryUsed, string sDisplayText )
{
	if ( !pCur || !pCur->GetPlayer() )
		return;

	uint32_t playerID = pCur->GetPlayer()->GetPlayerID();
	
	CStreamData oStream;

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &scriptNum, sizeof(scriptNum) );
	oStream.Write( &cpuUsed, sizeof(cpuUsed) );
	oStream.Write( &memoryUsed, sizeof(memoryUsed) );
	
	uint8_t displayTextLength = sDisplayText.length();
	oStream.Write( &displayTextLength, sizeof(displayTextLength) );
	oStream.Write( sDisplayText.c_str(), sDisplayText.length() );

	if ( !pCur->SendCommand( NWC_PLAYER_RUNS_SCRIPT, &oStream ) )
	{
		pCur->Disconnect();
		return;
	} 
}

void NWCPlayerDeletesItem( CServerConnection *pCur, CPlayer *pPlayer, CItemInstance *pMatchItem )
{
	if ( !pCur || !pCur->GetPlayer() )
		return;

	if ( !pPlayer )
		return;

	if ( !pMatchItem )
		return;

	uint32_t playerID = pCur->GetPlayer()->GetPlayerID();
	uint32_t playerDeletingItemID = pPlayer->GetPlayerID();
	uint32_t itemID = pMatchItem->GetItemID();

	CStreamData oStream;

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &playerDeletingItemID, sizeof(playerDeletingItemID) );
	oStream.Write( &itemID, sizeof(itemID) );

	if ( !pCur->SendCommand( NWC_PLAYER_DELETES_ITEM, &oStream ) )
	{
		pCur->Disconnect();
		return;
	}		
}

void NWCPlayerLevel( CPlayer *pPlayer, uint8_t hackLevelGain, uint8_t codeLevelGain, uint8_t scriptLevelGain )
{
	if ( !pPlayer || !pPlayer->GetConnection() )
		return;

	uint32_t playerID = pPlayer->GetPlayerID();
	uint8_t currentHackLevel = pPlayer->GetHackLevel();
	uint8_t currentCodeLevel = pPlayer->GetCodeLevel();
	uint8_t currentScriptLevel = pPlayer->GetScriptLevel();

	CStreamData oStream;
	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &hackLevelGain, sizeof(hackLevelGain) );
	oStream.Write( &codeLevelGain, sizeof(codeLevelGain) );
	oStream.Write( &scriptLevelGain, sizeof(scriptLevelGain) );
	
	oStream.Write( &currentHackLevel, sizeof(currentHackLevel) );
	oStream.Write( &currentCodeLevel, sizeof(currentCodeLevel) );
	oStream.Write( &currentScriptLevel, sizeof(currentScriptLevel) );

	if ( !pPlayer->GetConnection()->SendCommand( NWC_PLAYER_LEVEL, &oStream ) )
	{
		pPlayer->GetConnection()->Disconnect();
		return;
	}
}

void NWCPlayerDeath( CPlayer *pPlayer )
{
	if ( !pPlayer || !pPlayer->GetConnection() )
	{
		printf( "FAILED: player was null or connection null!\n" );
		return;
	}

	CRoom *pRoom = pPlayer->GetRoom();

	if ( !pRoom )
	{
		printf( "FAILED: player room was null!\n" );
		return;
	}

	CStreamData oStream;
	
	uint32_t playerID = pPlayer->GetPlayerID();

	oStream.Write( &playerID, sizeof(playerID) );

	if ( !pPlayer->WritePlayerData( &oStream ) )
	{
		pPlayer->GetConnection()->Disconnect();
		return;
	}

	if ( !pPlayer->GetConnection()->SendCommand( NWC_PLAYER_DEATH, &oStream ) )
	{
		pPlayer->GetConnection()->Disconnect();
		return;	
	}
	
	NWCEnterRoomResponse( pPlayer->GetConnection() );	
	
	// Inform players in room that this player logged in
	NWCPlayerArrivesInRoom( pPlayer->GetConnection(), NWC_PLAYER_ARRIVAL_RESPAWN );
}

void NWCDropItemToRoom( CRoom *pRoom, CItemInstance *pItem )
{
	if ( !pRoom || !pItem )
		return;

	if ( !pRoom->AddItemToRoom( pItem ) )
		return;

	for ( CPlayer *pPlayer = pRoom->GetPlayersInRoom()->Head(); pPlayer; pPlayer = pRoom->GetPlayersInRoom()->Next( pPlayer ) )
	{
		if ( pPlayer->IsConnected() && pPlayer->GetConnection() )
		{
			CStreamData oStream;
		
			uint32_t playerID = pPlayer->GetPlayerID();
			uint32_t itemID = pItem->GetItemID();
			uint8_t itemNameLength = pItem->GetName().length();
			
			oStream.Write( &playerID, sizeof(playerID) );
			oStream.Write( &itemID, sizeof(itemID) );
			oStream.Write( &itemNameLength, sizeof(itemNameLength) );
			oStream.Write( pItem->GetName().c_str(), itemNameLength );

			if ( !pPlayer->GetConnection()->SendCommand( NWC_ITEM_TO_ROOM, &oStream ) )
				pPlayer->GetConnection()->Disconnect(); 
		}
	}
}

void NWCPlayerKilledResponse( CPlayer *pPlayer, uint32_t killedID, uint8_t killedType, uint32_t killerID, string sKillerName, uint8_t killerType )
{
	if ( !pPlayer )
		return;

	CServerConnection *pCur = pPlayer->GetConnection();

	if ( !pCur )
		return;

	// OK send information
	CStreamData oStream;

	uint32_t playerID = pPlayer->GetPlayerID();

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &killedID, sizeof(killedID) );
	oStream.Write( &killedType, sizeof(killedType) );
	oStream.Write( &killerID, sizeof(killerID) );
	oStream.Write( &killerType, sizeof(killerType) );
	
	uint8_t killerNameLength = sKillerName.length();
	oStream.Write( &killerNameLength, sizeof(killerNameLength) );
	oStream.Write( sKillerName.c_str(), killerNameLength );

	if ( !pCur->SendCommand( NWC_PLAYER_KILLED_RESPONSE, &oStream ) )
		pCur->Disconnect();	
}

void NWCPlayerTakesDamage( CPlayer *pPlayer, uint32_t sourceID, uint8_t sourceType, uint16_t damageAmount, string sDamageString, string sDamageSource )
{
	if ( !pPlayer || !pPlayer->GetConnection() )
		return;

	CServerConnection *pCur = pPlayer->GetConnection();

	CStreamData oStream;
	uint32_t playerID = pPlayer->GetPlayerID();
	
	uint8_t damageStringLength = sDamageString.length();
	uint8_t damageSourceLength = sDamageSource.length();

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &sourceID, sizeof(sourceID) );
	oStream.Write( &sourceType, sizeof(sourceType) );
	oStream.Write( &damageAmount, sizeof(damageAmount) );
	oStream.Write( &damageStringLength, sizeof(damageStringLength) );
	oStream.Write( sDamageString.c_str(), damageStringLength );
	oStream.Write( &damageSourceLength, sizeof(damageSourceLength) );
	oStream.Write( sDamageSource.c_str(), damageSourceLength );

	if ( !pCur->SendCommand( NWC_PLAYER_TAKES_DAMAGE_RESPONSE, &oStream ) )
		pCur->Disconnect();
}

void NWCPlayerDamagePlayer( CPlayer *pAttacker, CPlayer *pTarget, uint16_t damageAmount, string sDamageString, string sDamageSource )
{
	if ( !pAttacker || !pAttacker->GetConnection() )
		return;

	CServerConnection *pCur = pAttacker->GetConnection();

	if ( !pTarget )
		return;

	CStreamData oStream;
	uint32_t playerID = pAttacker->GetPlayerID();
	uint32_t otherID = pTarget->GetPlayerID();
	uint8_t targetType = NWC_ATTACK_PLAYER;
	
	uint8_t damageStringLength = sDamageString.length();
	uint8_t damageSourceLength = sDamageSource.length();

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &otherID, sizeof(otherID) );
	oStream.Write( &targetType, sizeof(targetType) );
	oStream.Write( &damageAmount, sizeof(damageAmount) );
	oStream.Write( &damageStringLength, sizeof(damageStringLength) );
	oStream.Write( sDamageString.c_str(), damageStringLength );
	oStream.Write( &damageSourceLength, sizeof(damageSourceLength) );
	oStream.Write( sDamageSource.c_str(), damageSourceLength );

	if ( !pCur->SendCommand( NWC_PLAYER_DAMAGE_RESPONSE, &oStream ) )
		pCur->Disconnect();
}

void NWCPlayerDamageNPC( CPlayer *pAttacker, CNPC *pTarget, uint16_t damageAmount, string sDamageString, string sDamageSource )
{
	if ( !pAttacker || !pAttacker->GetConnection() )
		return;

	CServerConnection *pCur = pAttacker->GetConnection();

	if ( !pTarget )
		return;

	CStreamData oStream;
	uint32_t playerID = pAttacker->GetPlayerID();
	uint32_t npcID = pTarget->GetNPCID();
	uint8_t targetType = NWC_ATTACK_NPC;
	
	uint8_t damageStringLength = sDamageString.length();
	uint8_t damageSourceLength = sDamageSource.length();

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &npcID, sizeof(npcID) );
	oStream.Write( &targetType, sizeof(targetType) );
	oStream.Write( &damageAmount, sizeof(damageAmount) );
	oStream.Write( &damageStringLength, sizeof(damageStringLength) );
	oStream.Write( sDamageString.c_str(), damageStringLength );
	oStream.Write( &damageSourceLength, sizeof(damageSourceLength) );
	oStream.Write( sDamageSource.c_str(), damageSourceLength );

	if ( !pCur->SendCommand( NWC_PLAYER_DAMAGE_RESPONSE, &oStream ) )
		pCur->Disconnect();
}

void NWCPlayerAttackResponse( CServerConnection *pCur, uint32_t targetID, uint8_t targetType )
{
	if ( !pCur || !pCur->GetPlayer() )
		return;

	uint32_t playerID = pCur->GetPlayer()->GetPlayerID();
	
	CStreamData oStream;
	
	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &targetID, sizeof(targetID) );
	oStream.Write( &targetType, sizeof(targetType) );

	if ( !pCur->SendCommand( NWC_PLAYER_ATTACK_RESPONSE, &oStream ) )
		pCur->Disconnect();
}

void NWCPlayerStatUpdate( CPlayer *pPlayer )
{
	if ( !pPlayer )
		return;

	CServerConnection *pCur = pPlayer->GetConnection();

	if ( !pCur )
		return;

	CStreamData oStream;
	
	uint32_t playerID = pPlayer->GetPlayerID();
	uint16_t curHP = pPlayer->GetCurHP();
	uint16_t curCPU = pPlayer->GetCurCPU();
	uint16_t curMemory = pPlayer->GetCurMemory();

	uint16_t maxHP = pPlayer->GetMaxHP();
	uint16_t maxCPU = pPlayer->GetMaxCPU();
	uint16_t maxMemory = pPlayer->GetMaxMemory();

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &curHP, sizeof(curHP) );
	oStream.Write( &curCPU, sizeof(curCPU) );
	oStream.Write( &curMemory, sizeof(curMemory) );
	
	oStream.Write( &maxHP, sizeof(maxHP) );
	oStream.Write( &maxCPU, sizeof(maxCPU) );
	oStream.Write( &maxMemory, sizeof(maxMemory) );

	if ( !pCur->SendCommand( NWC_PLAYER_STAT_UPDATE_RESPONSE, &oStream ) )
		pCur->Disconnect();
}

void NWCCurrentPlayerDropsItem( CServerConnection *pCur, CItemInstance *pItem )
{
	if ( !pCur || !pCur->GetPlayer() )
		return;

	if ( !pItem )
		return;

	CStreamData oStream;

	uint32_t playerID = pCur->GetPlayer()->GetPlayerID();
	uint32_t itemID = pItem->GetItemID();
	
	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &itemID, sizeof(itemID) );

	if ( !pCur->SendCommand( NWC_PLAYER_DROPITEM_RESPONSE, &oStream ) )
		pCur->Disconnect();
}

void NWCOtherPlayerDropsItem( CServerConnection *pCur, CPlayer *pOtherPlayer, CItemInstance *pItem )
{
	if ( !pCur || !pOtherPlayer || !pCur->GetPlayer() )
		return;

	if ( !pItem )
		return;

	CStreamData oStream;

	uint32_t currentPlayerID = pCur->GetPlayer()->GetPlayerID();
	uint32_t otherPlayerID = pOtherPlayer->GetPlayerID();
	uint32_t itemID = pItem->GetItemID();
	uint16_t itemNameLength = pItem->GetName().length();

	oStream.Write( &currentPlayerID, sizeof(currentPlayerID) );
	oStream.Write( &otherPlayerID, sizeof(otherPlayerID) );
	oStream.Write( &itemID, sizeof(itemID) );
	oStream.Write( &itemNameLength, sizeof(itemNameLength) );
	oStream.Write( pItem->GetName().c_str(), itemNameLength );

	if ( !pCur->SendCommand( NWC_OTHERPLAYER_DROPITEM_RESPONSE, &oStream ) )
		pCur->Disconnect();

}

void NWCCurrentPlayerGetsItem( CServerConnection *pCur, CItemInstance *pItem )
{
	if ( !pCur || !pCur->GetPlayer() )
		return;

	if ( !pItem )
		return;

	CStreamData oStream;

	uint32_t playerID = pCur->GetPlayer()->GetPlayerID();
	
	oStream.Write( &playerID, sizeof(playerID) );

	// NOW send item data!
	if ( !pItem->WriteItemData( &oStream ) )
	{
		pCur->Disconnect();
		return;
	}

	if ( !pCur->SendCommand( NWC_PLAYER_GETITEM_RESPONSE, &oStream ) )
		pCur->Disconnect();
}

void NWCOtherPlayerGetsItem( CServerConnection *pCur, CPlayer *pOtherPlayer, CItemInstance *pItem )
{
	if ( !pCur || !pOtherPlayer || !pCur->GetPlayer() )
		return;

	if ( !pItem )
		return;

	CStreamData oStream;

	uint32_t currentPlayerID = pCur->GetPlayer()->GetPlayerID();
	uint32_t otherPlayerID = pOtherPlayer->GetPlayerID();
	uint32_t itemID = pItem->GetItemID();

	oStream.Write( &currentPlayerID, sizeof(currentPlayerID) );
	oStream.Write( &otherPlayerID, sizeof(otherPlayerID) );
	oStream.Write( &itemID, sizeof(itemID) );

	if ( !pCur->SendCommand( NWC_OTHERPLAYER_GETITEM_RESPONSE, &oStream ) )
		pCur->Disconnect();
}

void NWCPlayerInfoMessage( CPlayer *pToPlayer, string &sMessage )
{
	if ( !pToPlayer )
		return;
	
	CServerConnection *pCur = pToPlayer->GetConnection();

	if ( !pCur )
		return;

	CStreamData oStream;
	
	uint32_t playerID = pCur->GetPlayer()->GetPlayerID();
	uint16_t infoMessageLength = sMessage.length();
	
	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &infoMessageLength, sizeof(infoMessageLength) );
	oStream.Write( sMessage.c_str(), infoMessageLength );

	if ( !pCur->SendCommand( NWC_PLAYER_INFOMSG_RESPONSE, &oStream ) )
		pCur->Disconnect();
}


void NWCPlayerUnequipsItem( CServerConnection *pCur, tItemEquipType slot, CItemInstance *pItem )
{
	if ( !pCur || !pCur->GetPlayer() )
		return;

	if ( !pItem )
		return;

	string sSlotName;
	if ( !CItemInstance::GetEquipStringFromType( slot, sSlotName ) )
		return;
	
	CStreamData oStream;
	
	uint32_t playerID = pCur->GetPlayer()->GetPlayerID();
	uint32_t itemID = pItem->GetItemID();
	uint64_t itemKey = pItem->GetItemKey();
	uint8_t itemSlot = slot;
	uint8_t status = 1;

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &itemID, sizeof(itemID) );
	oStream.Write( &itemKey, sizeof(itemKey) );
	oStream.Write( &itemSlot, sizeof(itemSlot) );
	oStream.Write( &status, sizeof(status) );

	if ( !pCur->SendCommand( NWC_PLAYER_UNEQUIPITEM_RESPONSE, &oStream ) )
		pCur->Disconnect();

	// NOW inform other players!	
	CPlayer *pPlayer = pCur->GetPlayer();
	CRoom *pRoom = pPlayer->GetRoom();

	string sInfoMessage = pCur->GetPlayer()->GetName() + " uninstalls a " + pItem->GetName() + " in slot " + sSlotName;

	for ( CPlayer *pPlayerInRoom = pRoom->GetPlayersInRoom()->Head(); pPlayerInRoom; pPlayerInRoom = pRoom->GetPlayersInRoom()->Next( pPlayerInRoom ) )
	{
		if ( pPlayerInRoom == pPlayer )
			continue;	// Don't send info message to ourself

		if ( pPlayerInRoom->IsConnected() )
			NWCPlayerInfoMessage( pPlayerInRoom, sInfoMessage ); 	
	}
}

void NWCPlayerFailsUnequipItem( CServerConnection *pCur, tItemEquipType slot, CItemInstance *pItem, string &sFailReason )
{
	if ( !pCur || !pCur->GetPlayer() )
		return;

	if ( !pItem )
		return;

	CStreamData oStream;

	uint32_t playerID = pCur->GetPlayer()->GetPlayerID();
	uint32_t itemID = pItem->GetItemID();
	uint64_t itemKey = pItem->GetItemKey();
	uint8_t itemSlot = slot;
	uint8_t status = 0;

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &itemID, sizeof(itemID) );
	oStream.Write( &itemKey, sizeof(itemKey) );
	oStream.Write( &itemSlot, sizeof(itemSlot) );
	oStream.Write( &status, sizeof(status) );

	// Now send failure reason
	uint16_t failMessageLength = sFailReason.length();
	oStream.Write( &failMessageLength, sizeof(failMessageLength) );
	oStream.Write( sFailReason.c_str(), failMessageLength );

	if ( !pCur->SendCommand( NWC_PLAYER_UNEQUIPITEM_RESPONSE, &oStream ) )
		pCur->Disconnect();
}

void NWCPlayerEquipsItem( CServerConnection *pCur, tItemEquipType slot, CItemInstance *pItem )
{
	if ( !pCur || !pCur->GetPlayer() )
		return;

	if ( !pItem )
		return;

	string sSlotName;
	if ( !CItemInstance::GetEquipStringFromType( slot, sSlotName ) )
		return;
	
	CStreamData oStream;
	
	uint32_t playerID = pCur->GetPlayer()->GetPlayerID();
	uint32_t itemID = pItem->GetItemID();
	uint64_t itemKey = pItem->GetItemKey();
	uint8_t itemSlot = slot;
	uint8_t status = 1;

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &itemID, sizeof(itemID) );
	oStream.Write( &itemKey, sizeof(itemKey) );
	oStream.Write( &itemSlot, sizeof(itemSlot) );
	oStream.Write( &status, sizeof(status) );

	if ( !pCur->SendCommand( NWC_PLAYER_EQUIPITEM_RESPONSE, &oStream ) )
		pCur->Disconnect();


	// NOW inform other players!	
	CPlayer *pPlayer = pCur->GetPlayer();
	CRoom *pRoom = pPlayer->GetRoom();

	string sInfoMessage = pCur->GetPlayer()->GetName() + " installs a " + pItem->GetName() + " in slot " + sSlotName;

	for ( CPlayer *pPlayerInRoom = pRoom->GetPlayersInRoom()->Head(); pPlayerInRoom; pPlayerInRoom = pRoom->GetPlayersInRoom()->Next( pPlayerInRoom ) )
	{
		if ( pPlayerInRoom == pPlayer )
			continue; // Don't send info message to ourself

		if ( pPlayerInRoom->IsConnected() )
			NWCPlayerInfoMessage( pPlayerInRoom, sInfoMessage ); 	
	}
}

void NWCPlayerFailsEquipItem( CServerConnection *pCur, tItemEquipType slot, CItemInstance *pItem, string &sFailReason )
{
	if ( !pCur || !pCur->GetPlayer() )
		return;

	if ( !pItem )
		return;

	CStreamData oStream;

	uint32_t playerID = pCur->GetPlayer()->GetPlayerID();
	uint32_t itemID = pItem->GetItemID();
	uint64_t itemKey = pItem->GetItemKey();
	uint8_t itemSlot = slot;
	uint8_t status = 0;

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &itemID, sizeof(itemID) );
	oStream.Write( &itemKey, sizeof(itemKey) );
	oStream.Write( &itemSlot, sizeof(itemSlot) );
	oStream.Write( &status, sizeof(status) );

	// Now send failure reason
	uint16_t failMessageLength = sFailReason.length();
	oStream.Write( &failMessageLength, sizeof(failMessageLength) );
	oStream.Write( sFailReason.c_str(), failMessageLength );

	if ( !pCur->SendCommand( NWC_PLAYER_EQUIPITEM_RESPONSE, &oStream ) )
		pCur->Disconnect();
}

void NWCPlayerWalls( CPlayer *pFromPlayer, CServerConnection *pToConnection, string &sWallText )
{
	if ( !pFromPlayer || !pToConnection )
		return;

	if ( !pToConnection->GetPlayer() )
		return;

	CStreamData oStream;
	
	uint32_t fromID = pFromPlayer->GetPlayerID();
	uint32_t toID = pToConnection->GetPlayer()->GetPlayerID();
	
	uint8_t fromPlayerNameLength = pFromPlayer->GetName().length();
	uint8_t wallMessageLength = sWallText.length();

	oStream.Write( &fromID, sizeof(fromID) );
	oStream.Write( &toID, sizeof(toID) );
	
	oStream.Write( &fromPlayerNameLength, sizeof(fromPlayerNameLength) );
	oStream.Write( &wallMessageLength, sizeof(wallMessageLength) );

	oStream.Write( pFromPlayer->GetName().c_str(), fromPlayerNameLength );
	oStream.Write( sWallText.c_str(), wallMessageLength );

	if ( !pToConnection->SendCommand( NWC_PLAYER_WALL_RESPONSE, &oStream ) )
		pToConnection->Disconnect();
}

void NWCPlayerSays( CPlayer *pFromPlayer, CPlayer *pToPlayer, char *pszSayText )
{
	if ( !pFromPlayer || !pToPlayer )
		return;

	if ( pszSayText == NULL )
		return;

	// OK send message to this player!
	CStreamData oStream;

	uint32_t fromID = pFromPlayer->GetPlayerID();
	uint32_t toID = pToPlayer->GetPlayerID();

	uint8_t sayLength = strlen( pszSayText );

	oStream.Write( &fromID, sizeof(fromID) );
	oStream.Write( &toID, sizeof(toID) );
	
	oStream.Write( &sayLength, sizeof(sayLength) );
	oStream.Write( pszSayText, sayLength );

	if ( !pToPlayer->GetConnection()->SendCommand( NWC_PLAYER_SAYS_RESPONSE, &oStream ) )
	{
		pToPlayer->GetConnection()->Disconnect();
	}
}

void NWCPlayerArrives( CServerConnection *pCur, CPlayer *pArrivingPlayer, uint8_t arrivalType )
{
	if ( !pCur )
		return;

	CStreamData oStream;

	oStream.Write( &arrivalType, sizeof(arrivalType) );
	
	// Write player data
	uint32_t playerID = pArrivingPlayer->GetPlayerID();
	uint8_t playerNameLength = pArrivingPlayer->GetName().length();

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &playerNameLength, sizeof(playerNameLength) );
	oStream.Write( pArrivingPlayer->GetName().c_str(), playerNameLength );

	if ( !pCur->SendCommand( NWC_PLAYER_ARRIVES_RESPONSE, &oStream ) )
	{
		pCur->Disconnect();
	}
}

void NWCPlayerLeaves( CServerConnection *pCur, CPlayer *pLeavingPlayer, uint8_t leaveType, uint8_t leaveDirection )
{
	if ( !pCur )
		return;

	CStreamData oStream;
	
	oStream.Write( &leaveType, sizeof(leaveType) );
	oStream.Write( &leaveDirection, sizeof(leaveDirection) );
	
	uint32_t playerID = pLeavingPlayer->GetPlayerID();
	uint8_t playerNameLength = pLeavingPlayer->GetName().length();

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &playerNameLength, sizeof(playerNameLength) );
	oStream.Write( pLeavingPlayer->GetName().c_str(), playerNameLength );

	if ( !pCur->SendCommand( NWC_PLAYER_LEAVES_RESPONSE, &oStream ) )
	{
		pCur->Disconnect();
	}
}

void NWCPlayerArrivesInRoom( CServerConnection *pCur, uint8_t arrivalType )
{
	if ( !pCur->GetPlayer() )
		return;

	CPlayer *pPlayer = pCur->GetPlayer();
	CRoom *pRoom = pPlayer->GetRoom();

	for ( CPlayer *pPlayerInRoom = pRoom->GetPlayersInRoom()->Head(); pPlayerInRoom; pPlayerInRoom = pRoom->GetPlayersInRoom()->Next( pPlayerInRoom ) )
	{
		if ( pPlayerInRoom->IsConnected() )
			NWCPlayerArrives( pPlayerInRoom->GetConnection(), pPlayer, arrivalType ); 	
	}
}

// Helper function
void NWCPlayerLeavesFromRoom( CServerConnection *pCur, uint8_t leaveType, uint8_t leaveDirection )
{
	if ( !pCur->GetPlayer() )
		return;

	CPlayer *pPlayer = pCur->GetPlayer();
	CRoom *pRoom = pPlayer->GetRoom();

	for ( CPlayer *pPlayerInRoom = pRoom->GetPlayersInRoom()->Head(); pPlayerInRoom; pPlayerInRoom = pRoom->GetPlayersInRoom()->Next( pPlayerInRoom ) )
	{
		if ( pPlayerInRoom->IsConnected() )
			NWCPlayerLeaves( pPlayerInRoom->GetConnection(), pPlayer, leaveType, leaveDirection ); 	
	}
}

void NWCNPCArrivesInRoom( CNPC *pNPC, uint8_t arrivalType )
{
	if ( !pNPC || !pNPC->GetRoom() )
		return;

	CRoom *pRoom = pNPC->GetRoom();
	
	for ( CPlayer *pPlayer = pRoom->GetPlayersInRoom()->Head(); pPlayer; pPlayer = pRoom->GetPlayersInRoom()->Next( pPlayer ) )
	{
		if ( pPlayer->IsConnected() && pPlayer->GetConnection() )
		{
			CStreamData oStream;
			uint32_t npcID = pNPC->GetNPCID();
			uint8_t nameLength = pNPC->GetName().length();
	
			oStream.Write( &npcID, sizeof(npcID) );			
			oStream.Write( &arrivalType, sizeof(arrivalType) );
			oStream.Write( &nameLength, sizeof(nameLength) );
			oStream.Write( pNPC->GetName().c_str(), nameLength );

			if ( !pPlayer->GetConnection()->SendCommand( NWC_NPC_ARRIVES_RESPONSE, &oStream ) )
			{
				pPlayer->GetConnection()->Disconnect();
			}
		}
	}
}

void NWCNPCLeavesFromRoom( CNPC *pNPC, uint8_t leaveType, uint8_t leaveDirection )
{
	if ( !pNPC || !pNPC->GetRoom() )
		return;

	CRoom *pRoom = pNPC->GetRoom();
	
	for ( CPlayer *pPlayer = pRoom->GetPlayersInRoom()->Head(); pPlayer; pPlayer = pRoom->GetPlayersInRoom()->Next( pPlayer ) )
	{
		if ( pPlayer->IsConnected() && pPlayer->GetConnection() )
		{
			CStreamData oStream;
			uint32_t npcID = pNPC->GetNPCID();

			oStream.Write( &npcID, sizeof(npcID) );
			oStream.Write( &leaveType, sizeof(leaveType) );
			oStream.Write( &leaveDirection, sizeof(leaveDirection) );
			
			if ( !pPlayer->GetConnection()->SendCommand( NWC_NPC_LEAVES_RESPONSE, &oStream ) )
			{
				pPlayer->GetConnection()->Disconnect();
			}
		}
	}
}

bool NWCEnterRoomResponse( CServerConnection *pCur )
{
	if ( !pCur->GetPlayer() )
	{
		printf( "[NWCEnterRoomResponse] No player!\n" );
		return (false);
	}

	CPlayer *pPlayer = pCur->GetPlayer();

	CRoom *pRoom = pPlayer->GetRoom();

	if ( !pRoom )
	{
		printf( "[NWCEnterRoomResponse] Player has no room!\n" );
		return (false);
	}

	// Send room data
	CStreamData enterRoomResponse;

	pRoom->WriteRoomData( &enterRoomResponse );

	// For each player in room, send the player name and ID
	CPlayer *pCurPlayerInRoom;
	NUtil::LIST_PTR( CPlayer ) pPlayersInRoomList = pRoom->GetPlayersInRoom();

	if ( pPlayersInRoomList == NULL )
		return (false);

	uint32_t playerCount = 0;
	for ( pCurPlayerInRoom = pPlayersInRoomList->Head(); pCurPlayerInRoom; pCurPlayerInRoom = pPlayersInRoomList->Next( pCurPlayerInRoom ) ) 
		playerCount++;	

	// Send player count
	enterRoomResponse.Write( &playerCount, sizeof(playerCount) );

	// Now write out the player data	
	for ( pCurPlayerInRoom = pPlayersInRoomList->Head(); pCurPlayerInRoom; pCurPlayerInRoom = pPlayersInRoomList->Next( pCurPlayerInRoom ) ) 
	{
		uint32_t playerID = pCurPlayerInRoom->GetPlayerID();
		uint8_t playerNameLength = pCurPlayerInRoom->GetName().length();

		enterRoomResponse.Write( &playerID, sizeof(playerID) );
		enterRoomResponse.Write( &playerNameLength, sizeof(playerNameLength) );
		enterRoomResponse.Write( pCurPlayerInRoom->GetName().c_str(), playerNameLength );
	}

	// Get NPCs in ROOM
	CNPC *pCurNPCInRoom;
	NUtil::LIST_PTR( CNPC ) pNPCsInRoomList = pRoom->GetNPCsInRoom();

	if ( pNPCsInRoomList == NULL )
		return (false);

	uint32_t npcCount = 0;
	for ( pCurNPCInRoom = pNPCsInRoomList->Head(); pCurNPCInRoom; pCurNPCInRoom = pNPCsInRoomList->Next( pCurNPCInRoom ) )
		npcCount++;

	// Send NPC count
	enterRoomResponse.Write( &npcCount, sizeof(npcCount) );
	
	// Now write NPC data
	for ( pCurNPCInRoom = pNPCsInRoomList->Head(); pCurNPCInRoom; pCurNPCInRoom = pNPCsInRoomList->Next( pCurNPCInRoom ) )
	{
		uint32_t npcID = pCurNPCInRoom->GetNPCID();
		uint8_t npcNameLength = pCurNPCInRoom->GetName().length();

		enterRoomResponse.Write( &npcID, sizeof(npcID) );
		enterRoomResponse.Write( &npcNameLength, sizeof(npcNameLength) );
		enterRoomResponse.Write( pCurNPCInRoom->GetName().c_str(), npcNameLength );
	}

	// Get items in ROOM
	CItemInstance *pCurItemInRoom;
	NUtil::LIST_PTR( CItemInstance ) pItemsInRoomList = pRoom->GetItemsInRoom();

	if ( pItemsInRoomList == NULL )
		return (false);

	uint32_t itemCount = 0;
	for ( pCurItemInRoom = pItemsInRoomList->Head(); pCurItemInRoom; pCurItemInRoom = pItemsInRoomList->Next( pCurItemInRoom ) )
		itemCount++;

	// Write out number of items in room
	enterRoomResponse.Write( &itemCount, sizeof(itemCount) );

	// Now write out the item data	
	for ( pCurItemInRoom = pItemsInRoomList->Head(); pCurItemInRoom; pCurItemInRoom = pItemsInRoomList->Next( pCurItemInRoom ) )
	{
		uint32_t itemID = pCurItemInRoom->GetItemID();
		uint8_t itemNameLength = pCurItemInRoom->GetName().length();
		

		enterRoomResponse.Write( &itemID, sizeof(itemID) );
		enterRoomResponse.Write( &itemNameLength, sizeof(itemNameLength) );
		enterRoomResponse.Write( pCurItemInRoom->GetName().c_str(), itemNameLength );
	}

	// Send command
	if ( !pCur->SendCommand( NWC_PLAYER_ENTERROOM_RESPONSE, &enterRoomResponse ) )
	{
		pCur->Disconnect();
		return (false);
	}

	// Done sending room data
	return (true);
}

uint8_t ToHex( uint8_t value )
{
	if ( value < 10 )
		return '0' + value;
	else if ( value < 16 )
		return 'a' + (value - 10);
	else
		return '0';
}

#if ENABLE_PROOF_OF_WORK
bool NWCProofData( CServerConnection *pCur, NUtil::LIST_PTR( CServerConnection ) pConnectionList, uint8_t *pData, uint32_t dataLen )
{
	if ( !pCur )
	{
		printf( "NWCProofData -- pCur is NULL\n" );
		return (false);
	}
       
	// Only allowed to create a player during wait authentication 
	if ( pCur->GetConnectionState() != CServerConnection::CONNECTION_WAIT_PROOF )
                return (false);

	if ( dataLen != (17*4) )
		return (false);
        
	uint32_t proofData[16];
        uint32_t proofNonce;

        proofNonce = *((uint32_t*)(pData));
        memcpy( proofData, pData+4, (16*4) );

        std::vector<uint32_t> oProofInputs;
        oProofInputs.resize( 16 );

        for ( uint32_t i = 0; i < 16; i++ )
                oProofInputs[i] = proofData[i];

        uint8_t proofSeedTemp[16];
        pCur->GetProofSeed( proofSeedTemp, 16 );

        _POW::Seed oProofSeed;
        oProofSeed.SetSeed( (uint32_t*)proofSeedTemp );

        // Validate proof of work
        _POW::Proof oTestSolution( PROOF_OF_WORK_N, PROOF_OF_WORK_K, oProofSeed, proofNonce, oProofInputs );

        if ( !oTestSolution.Test() )
        {
                printf( "[AUTH]Proof of Work Solution incorrect!\n" );
                return (false);
        }

	// Accept the proof of work submission -- allow the connection
	pCur->AcceptProof();
	
	return (true);
}
#endif

bool NWCCreatePlayer( CServerConnection *pCur, NUtil::LIST_PTR( CServerConnection ) pConnectionList, uint8_t *pData, uint32_t dataLen )
{
	if ( !pCur )
	{
		printf( "NWCCreatePlayer -- pCur is NULL\n" );
		return (false);
	}
       
	// Only allowed to create a player during wait authentication 
	if ( pCur->GetConnectionState() != CServerConnection::CONNECTION_WAIT_AUTH )
                return (false);

	// OK attempt to create a new player with the provided data
	if ( dataLen < 17 )
		return (false);

	uint8_t userNameLength = pData[0];
	
	if ( dataLen != (userNameLength+16+1) )
		return (false);

	if ( userNameLength > MAX_USERNAME_LENGTH )
		return (false);

	if ( userNameLength < MIN_USERNAME_LENGTH )
		return (false);

	char szUserName[256];
	memcpy( szUserName, pData+1, userNameLength );
	szUserName[userNameLength] = '\0';

	uint8_t loginPlayerPasswordHash[16];
	memcpy( loginPlayerPasswordHash, pData+1+userNameLength, 16 );

	// Create password hash string
	char szPasswordHash[256];
	for ( uint8_t i = 0; i < 32; i+=2 )
	{
		szPasswordHash[i] = ToHex( (loginPlayerPasswordHash[i/2] >> 4) & 0xF );
		szPasswordHash[i+1] = ToHex( (loginPlayerPasswordHash[i/2] & 0xF) );
	}
	szPasswordHash[32] = '\0';

	string sPassword = szPasswordHash;

	// Validate new username	
	string sUserName = szUserName;

	if ( !CheckPlayerName( sUserName.c_str() ) )
	{
		printf( "[CREATEPLAYER] Username check failed!\n" );
		return (false);
	}
	
	// Lastly check pass
	bool bFoundUser = false;
	string sPlayerPassword;
	
	CPlayerLoginCache *pLoginCache = NULL;
	
	pLoginCache = g_oPlayerLoginCache.Find( sUserName );

	if ( pLoginCache )
		bFoundUser = true;
	else
	{
		if ( CPlayer::GetPasswordOnly( sUserName.c_str(), sPlayerPassword ) )
			bFoundUser = true;
	}

	if ( bFoundUser )
	{
		// Respond with user exists
		NWCCreatePlayerResponse( pCur, NWC_CREATEPLAYER_PLAYER_EXISTS );

		return (true);
	}

	// Ok create player
	CPlayer *pNewPlayer = NULL;

        pNewPlayer = CPlayer::LoadFile( NEW_PLAYER_PFILE, false );

	pNewPlayer->SetName( sUserName );
	pNewPlayer->SetPasswordHash( sPassword );


	if ( pNewPlayer )
	{
		// OK perform direction
		uint32_t roomVnum;
		
		uint32_t range = g_oRNG.GetRange( 0, 2 );
		if ( range == 0 )
			roomVnum = 100;
		else if ( range == 1 )
			roomVnum = 180;
		else if ( range == 2 )
			roomVnum = 182;

		CRoom *pToRoom = g_oRoomHash.Find( roomVnum );

		if ( pToRoom == NULL )
		{
			pToRoom = g_oRoomHash.Find( INITIAL_ROOM_VNUM );
		}

		if ( pToRoom == NULL )
			return (false);

        	pNewPlayer->SetRoom( pToRoom );
	
		CStreamData *pStream = new CStreamData();
	
		// Write out the player data cache	
		pNewPlayer->WritePlayerData( pStream, true );
		pLoginCache = new CPlayerLoginCache( sUserName, pStream, pNewPlayer->GetPasswordHash() );

		g_oPlayerLoginCache.Add( pLoginCache );
	}
	else
	{
		delete pNewPlayer;

		NWCCreatePlayerResponse( pCur, NWC_CREATEPLAYER_INVALID_DATA );
		return (true);
	}

	NWCCreatePlayerResponse( pCur, NWC_CREATEPLAYER_SUCCESS );

	return (true);
}

bool NWCAuth( CServerConnection *pCur, NUtil::LIST_PTR( CServerConnection ) pConnectionList, uint8_t *pData, uint32_t dataLen )
{
	if ( !pCur )
	{
		printf( "NWCAuth -- pCur is NULL\n" );
		return (false);
	}

#if ENABLE_LOGIN_HASH
	printf( "[AUTH=HASH]\n" );
#else
	printf( "[AUTH=PLAIN]\n" );
#endif


        if ( pCur->GetConnectionState() != CServerConnection::CONNECTION_WAIT_AUTH )
                return (false);

	uint8_t passwordHash[32];

#if ENABLE_LOGIN_HASH
	if ( dataLen != 128 )
		return (false);

	ZZ cipherdata;
	ZZFromBytes( cipherdata, pData, 128 );

	/*
	printf( "Cipher Data Bytes:\n" );
	for ( uint8_t i = 0; i < 128; i++ )
		printf( "%02x", pData[i] );

	printf( "\n" );
	cout << "Cipher Data Int: " << cipherdata << endl;
	*/
	
	ZZ result_data = PowerMod( cipherdata, g_privateKey, g_privateN );
	
	uint8_t plainData[128];


	BytesFromZZ( plainData, result_data, 128 );

	// Get password hash
	memcpy( passwordHash, plainData+32, 32 );
	
	// Get username
	uint8_t usernameLength = plainData[64];
	
	if ( usernameLength >= 32 )
	{
		printf( "UsernameLength=%d\n", usernameLength );
		return false;
	}

	// Calculate message digest and verify!	
	CMD5 oMessageHash;

        oMessageHash.InitDigest();

        oMessageHash.UpdateDigest( plainData, 97 );
        
	uint8_t messageDigest[16];

	// Get the message digest
        oMessageHash.GetDigest( messageDigest, 16 );

	// Check message digest!
	if ( memcmp( messageDigest, plainData+97, 16 ) != 0 )
	{
		printf( "[AUTH] [TOKENHASH] Failed message digest!\n" );
		return false;
	}

	char szUserName[65];
	
	memcpy( szUserName, plainData+65, usernameLength );
	szUserName[usernameLength] = '\0';
#else
	// Plain login
	if ( dataLen < 2+16 )
		return (false);

	uint8_t userNameLength = pData[0];

//#if ENABLE_PROOF_OF_WORK
#if 0
	if ( dataLen != (userNameLength+16+1+4+(4*16)) )
	{
		printf( "[AUTH] Invalid length for Proof of Work AUTH!\n" );
		return (false);
	}
#else	
	if ( dataLen != (userNameLength+16+1) )
		return (false);
#endif

	if ( userNameLength > MAX_USERNAME_LENGTH )
		return (false);

	char szUserName[256];
	memcpy( szUserName, pData+1, userNameLength );
	szUserName[userNameLength] = '\0';

	uint8_t loginPlayerPasswordHash[16];
	memcpy( loginPlayerPasswordHash, pData+1+userNameLength, 16 );

	// Plain login uses the password hash and compares it directly!		

//#if ENABLE_PROOF_OF_WORK
#if 0
	uint32_t proofData[16];
	uint32_t proofNonce;

	proofNonce = *((uint32_t*)(pData+userNameLength+16+1));
	memcpy( proofData, pData+userNameLength+16+1+4, (16*4) );

	std::vector<uint32_t> oProofInputs;
	oProofInputs.resize( 16 );

	for ( uint32_t i = 0; i < 16; i++ )
		oProofInputs[i] = proofData[i];

	uint8_t proofSeedTemp[16];
	pCur->GetProofSeed( proofSeedTemp, 16 );

	_POW::Seed oProofSeed;
	oProofSeed.SetSeed( (uint32_t*)proofSeedTemp );

	// Validate proof of work
	_POW::Proof oTestSolution( PROOF_OF_WORK_N, PROOF_OF_WORK_K, oProofSeed, proofNonce, oProofInputs );

	if ( !oTestSolution.Test() )
	{
		printf( "[AUTH]Proof of Work Solution incorrect!\n" );
		return (false);
	}
#endif
		
		
#endif

	string sUserName = szUserName;

	if ( !CheckPlayerName( sUserName.c_str() ) )
	{
		printf( "[AUTH] Username check failed!\n" );
		return (false);
	}
	
	// Lastly check password hash!!!
	string sPlayerPassword;
	
	CPlayerLoginCache *pLoginCache = NULL;
	
	pLoginCache = g_oPlayerLoginCache.Find( sUserName );

	if ( pLoginCache )
		sPlayerPassword = pLoginCache->GetPassword();
	else
	{
		if ( !CPlayer::GetPasswordOnly( sUserName.c_str(), sPlayerPassword ) )
			return (false);
	}

	if ( sPlayerPassword.length() > 128 )
	{
		printf( "[AUTH] Critical error -- password too long!\n" );
		return (false);
	}

#if ENABLE_LOGIN_HASH
	uint8_t *pStringToHash = new uint8_t[32+sPlayerPassword.length()];
	uint8_t loginChallengeData[64];
	
	pCur->GetLoginChallengeData( loginChallengeData, 32 );

	memcpy( pStringToHash, loginChallengeData, 32 );
	memcpy( pStringToHash+32, sPlayerPassword.c_str(), sPlayerPassword.length() );

	uint8_t passwordSha256[64];

	/*
	printf( "String to Hash:\n" );
	for ( uint32_t i = 0; i < 32+sPlayerPassword.length(); i++ )
		printf( "%02x", pStringToHash[i] );
	printf( "\n" );
	*/

	sha256_data( pStringToHash, 32+sPlayerPassword.length(), passwordSha256, 32 );

	/*
	printf( "SHA-256 Hash:\n" );
	for ( uint32_t i = 0; i < 32; i++ )
		printf( "%02x", passwordSha256[i] );
	printf( "\n" );
	*/
	
	if ( memcmp( passwordSha256, passwordHash, 32 ) != 0 )
	{
		printf( "HASH LOGIN: Password hash did not match (Player: %s)!\n", szUserName );
		delete pStringToHash;

		return (false);
	}

	// Done...
	delete pStringToHash;
#else
	// Plain password comparison!
	char szComparisonPassword[256];

	if ( sPlayerPassword.length() != 32 )
	{
		printf( "PLAIN LOGIN: Player password is not 32 characters (is it not a MD5 digest??)! (%d)\n", sPlayerPassword.length() );
		return (false);
	}
	
	for ( uint8_t i = 0; i < 32; i+=2 )
	{
		szComparisonPassword[i] = ToHex( (loginPlayerPasswordHash[i/2] >> 4) & 0xF );
		szComparisonPassword[i+1] = ToHex( (loginPlayerPasswordHash[i/2] & 0xF) );
	}
	szComparisonPassword[32] = '\0';

	string sComparisonPassword = szComparisonPassword;

	if ( sComparisonPassword != sPlayerPassword )
	{
		printf( "PLAIN LOGIN: Player password did not match (%s != %s)\n", sComparisonPassword.c_str(), sPlayerPassword.c_str() );
		return (false);
	}

#endif

	CPlayer *pNewPlayer = NULL;

	if ( pLoginCache )
	{
		printf( "Using login cache for: %s\n", sUserName.c_str() );
		pNewPlayer = pLoginCache->CreateNewPlayer();
	}
	else
	{
        	pNewPlayer = CPlayer::LoadFile( sUserName.c_str() );

		if ( pNewPlayer )
		{
			CStreamData *pStream = new CStreamData();
	
			// Write out the player data cache	
			pNewPlayer->WritePlayerData( pStream, true );
			pLoginCache = new CPlayerLoginCache( sUserName, pStream, pNewPlayer->GetPasswordHash() );

			g_oPlayerLoginCache.Add( pLoginCache );
		}
	}

	// Check if player could be loaded
        if ( pNewPlayer == NULL )
	{
		printf( "[AUTH] Player not found!\n" );
                return (false);
	}

        // Check for another player already connected as the same player!
        CServerConnection *pOtherConnection;
	CServerConnection *pNextConnection;
        for ( pOtherConnection = pConnectionList->Head(); pOtherConnection; pOtherConnection = pNextConnection )
	{
		pNextConnection = pConnectionList->Next( pOtherConnection );
                
		if ( pOtherConnection->GetConnectionState() == CServerConnection::CONNECTION_PLAYER &&
		     pOtherConnection->GetPlayer() && 
                     pOtherConnection->GetPlayer()->GetName() == pNewPlayer->GetName() )
                {
			printf( "[%s] force disconnect due to new connection.\n", pOtherConnection->GetPlayer()->GetName().c_str() );

                        if ( pOtherConnection->GetPlayer()->GetRoom() )
				NWCPlayerLeavesFromRoom( pOtherConnection, NWC_PLAYER_LEAVES_LOGOUT, 0 );
			
			// Exit!
			pOtherConnection->GetPlayer()->DoExit();

                        // Disconnect other player connection -- fast disconnect (destroy player)
                        pOtherConnection->Disconnect( true );
                }
        }

	// Add to world list
	g_worldPlayers.InsertHead( pNewPlayer );

	// Set connection for player
	pNewPlayer->SetConnection( pCur );
	
	// GOOD!
        pCur->Authenticate( pNewPlayer );

	// Set room
	pNewPlayer->SetRoom( g_oRoomHash.Find( INITIAL_ROOM_VNUM ) );

        // SEND AUTH data
        CStreamData authResponse;

        // Write out player data!
        uint8_t authSuccess = 1;
        authResponse.Write( &authSuccess, sizeof(authSuccess) );

        if ( !pNewPlayer->WritePlayerData( &authResponse ) )
        {
                pCur->Disconnect();

                return (false);
        }

        // WRITE OUT response
        if ( !pCur->SendCommand( NWC_AUTH_RESPONSE, &authResponse ) )
        {
                // Failed to write data
                pCur->Disconnect();

                return (false);
        }

	if ( !NWCEnterRoomResponse( pCur ) )
	{
                // Failed to write data
                pCur->Disconnect();

                return (false);
	}

	// Inform players in room that this player logged in
	NWCPlayerArrivesInRoom( pCur, NWC_PLAYER_ARRIVAL_LOGIN );

	// Do any enter room actions
	DoPlayerEnterRoomAction( pCur->GetPlayer(), (uint8_t)NWC_PLAYER_ARRIVAL_LOGIN );

        return (true);
}

bool NWCSay( CServerConnection *pCur, NUtil::LIST_PTR( CServerConnection ) pConnectionList, uint8_t *pData, uint32_t dataLen )
{
	// Say -- talk to everyone in the room
	if ( !pCur || !pCur->GetPlayer() )
		return (false);
	
	if ( dataLen < 9 )
		return (false);

	pData += 4;
	uint32_t playerID = *((uint32_t*)pData);
	uint8_t messageLen = pData[4];

	if ( playerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	if ( dataLen != messageLen+9 )
		return (false);

	CSpamTracker *pSpamTracker = g_oSpamTracker.Find( pCur->GetPlayer()->GetName() );
	
	if ( pSpamTracker && !pSpamTracker->CanSay() )
	{
		string sMessage = "Spamming detected by server... you have been muted";

		NWCPlayerInfoMessage( pCur->GetPlayer(), sMessage );

		return (true);
	}

	if ( !pSpamTracker )
	{
		pSpamTracker = new CSpamTracker( pCur->GetPlayer()->GetName() );

		g_oSpamTracker.Add( pSpamTracker );
	}

	pSpamTracker->UpdateSay();

	// Get message
	char *pTempData = new char[messageLen+1];

	memcpy( pTempData, pData+5, messageLen );
	pTempData[messageLen] = '\0';

	SanitizeString( pTempData, messageLen );

	CRoom *pRoom = pCur->GetPlayer()->GetRoom();

	if ( !pRoom )
	{
		delete pTempData;
		return (false);
	}

	NUtil::LIST_PTR( CPlayer ) pPlayersInRoomList = pRoom->GetPlayersInRoom();

	// Now say to everyone in the room...
	for ( CPlayer *pCurPlayerInRoom = pPlayersInRoomList->Head(); pCurPlayerInRoom; pCurPlayerInRoom = pPlayersInRoomList->Next( pCurPlayerInRoom ) ) 
		NWCPlayerSays( pCur->GetPlayer(), pCurPlayerInRoom, pTempData );

	delete pTempData;

	return (true);
}

bool NWCWall( CServerConnection *pCur, NUtil::LIST_PTR( CServerConnection ) pConnectionList, uint8_t *pData, uint32_t dataLen )
{
	// Wall -- talk to everyone on the server
	if ( !pCur || !pCur->GetPlayer() )
		return (false);
	
	if ( dataLen < 5 )
		return (false);

	uint32_t playerID = *((uint32_t*)pData);
	uint8_t messageLen = pData[4];

	if ( playerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	if ( dataLen != messageLen+5 )
		return (false);

	CSpamTracker *pSpamTracker = g_oSpamTracker.Find( pCur->GetPlayer()->GetName() );
	
	if ( pSpamTracker && !pSpamTracker->CanWall() )
	{
		string sMessage = "Spamming detected by server... you have been muted";

		NWCPlayerInfoMessage( pCur->GetPlayer(), sMessage );

		return (true);
	}

	if ( !pSpamTracker )
	{
		pSpamTracker = new CSpamTracker( pCur->GetPlayer()->GetName() );

		g_oSpamTracker.Add( pSpamTracker );
	}

	pSpamTracker->UpdateWall();
	
	// Get message
	char *pTempData = new char[messageLen+1];

	memcpy( pTempData, pData+5, messageLen );
	pTempData[messageLen] = '\0';

	SanitizeString( pTempData, messageLen );
	
	string sWallText = pTempData;
	delete pTempData;

	for ( CServerConnection *pConnection = pConnectionList->Head(); pConnection; pConnection = pConnectionList->Next( pConnection ) )
	{
		if ( !pConnection->IsDisconnected() && pConnection->GetPlayer() )
			NWCPlayerWalls( pCur->GetPlayer(), pConnection, sWallText );
	}

	return (true);
}

bool NWCDirection( CServerConnection *pCur, NUtil::LIST_PTR( CServerConnection ) pConnectionList, uint8_t *pData, uint32_t dataLen )
{
	// Exit to a direction
	if ( !pCur || !pCur->GetPlayer() )
		return (false);
	
	if ( dataLen != 5 )
		return (false);

	uint32_t playerID = *((uint32_t*)pData);
	uint8_t direction = pData[4];

	// Validate direction
	CPlayer *pCurPlayer = pCur->GetPlayer();

	if ( pCurPlayer->GetPlayerID() != playerID )
		return (false);

	if ( direction >= DIRECTION_MAX )
		return (false);

	if ( pCurPlayer->IsFighting() )
	{
		string sFailMessage = "You can't do that while fighting";
		NWCPlayerInfoMessage( pCur->GetPlayer(), sFailMessage );
		return (true);
	}

	// Perform direction
	CRoom *pRoom = pCurPlayer->GetRoom();
	if ( !pRoom )
	{
		printf( "Critical error -- received NWCDirection and in room is NULL\n" );
		return (false);
	}

	if ( !pRoom->HasDirection( direction ) )
	{
		string sFailMessage = "You can't go that way";
		NWCPlayerInfoMessage( pCur->GetPlayer(), sFailMessage );
		return (true);
	}
	
	// OK perform direction
	CRoom *pNextRoom = g_oRoomHash.Find( pRoom->GetDirection( direction ).GetVnum() );

	if ( pNextRoom == NULL )
		printf( "Next room is NULL.\n" );

	if ( pNextRoom == NULL )
		return (false);

	if ( !CanMoveCPUCheck( pCurPlayer ) )
	{
		string sFailMessage = "Insufficient CPU to move!";
		NWCPlayerInfoMessage( pCurPlayer, sFailMessage );

		return (true);
	}

	// Consume CPU on movement...
	uint16_t cpuMovementNeeded = GetMovementCPUNeeded( pCurPlayer );
	
	pCurPlayer->SetCPU( pCurPlayer->GetCurCPU() - cpuMovementNeeded );

	// Inform players in our current room we are leaving
	NWCPlayerLeavesFromRoom( pCur, NWC_PLAYER_LEAVES_EXITROOM, direction );
	
	pCurPlayer->SetRoom( pNextRoom );

	if ( !NWCEnterRoomResponse( pCur ) )
	{
		printf( "Enterroom response failed!\n" );

                // Failed to write data
                pCur->Disconnect();

                return (false);
	}

	// Inform others of our arrival
	NWCPlayerArrivesInRoom( pCur, NWC_PLAYER_ARRIVAL_ENTERROOM );

	// Check for enter room actions
	DoPlayerEnterRoomAction( pCur->GetPlayer(), (uint8_t)NWC_PLAYER_ARRIVAL_ENTERROOM );

	return (true);
}

bool NWCGetItem( CServerConnection *pCur, NUtil::LIST_PTR( CServerConnection ) pConnectionList, uint8_t *pData, uint32_t dataLen )
{
	// Get an item
	if ( !pCur || !pCur->GetPlayer() )
		return (false);

	if ( dataLen != 8 )
		return (false);

	uint32_t playerID = *((uint32_t*)(pData));
	uint32_t itemID = *((uint32_t*)(pData+4));

	if ( playerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	CRoom *pRoom = pCur->GetPlayer()->GetRoom();
	
	if ( !pRoom )
		return (false);

	// Find item in room
	CItemInstance *pMatchItem = pRoom->GetItemInRoomByID( itemID );

	if ( pMatchItem == NULL )
	{
		// Someone might have already taken this item!
		string sFailReason = "That item is no longer in the room";
		NWCPlayerInfoMessage( pCur->GetPlayer(), sFailReason );

		return (true);
	}

	// Attempt to get it!
	if ( pCur->GetPlayer()->GetCurrentCarryItems()+1 > pCur->GetPlayer()->GetMaxCarryItems() )
	{
		string sFailReason = "Already carrying too many items to get that";
		NWCPlayerInfoMessage( pCur->GetPlayer(), sFailReason );

		return (true);
	}

	if ( pMatchItem->GetDiskUsage() + pCur->GetPlayer()->GetCurrentDiskSpace() > pCur->GetPlayer()->GetMaxDiskSpace() )
	{
		string sFailReason = "Can't get that, you do not have enough disk space available";
		NWCPlayerInfoMessage( pCur->GetPlayer(), sFailReason );

		return (true);
	}

	// Prevent fixed items from moving
	if ( pMatchItem->GetItemType() == eItemFixed || pMatchItem->GetItemType() == eItemPower )
	{
		string sFailReason = "That item cannot be moved";
		NWCPlayerInfoMessage( pCur->GetPlayer(), sFailReason );

		return (true);
	}

	// OK get the item
	if ( !pCur->GetPlayer()->AddItemToInventory( pMatchItem ) )
	{
		string sFailReason = "Can't get that item!";
		NWCPlayerInfoMessage( pCur->GetPlayer(), sFailReason );

		return (true);
	}

	// Remove it from the room
	pMatchItem->m_roomLink.Unlink();

	// Inform player that he got the item (send item data as well)
	NWCCurrentPlayerGetsItem( pCur, pMatchItem );


	// Inform other players in room -- of player getting item	
	NUtil::LIST_PTR( CPlayer ) pPlayersInRoomList = pRoom->GetPlayersInRoom();

	// Now say to everyone in the room...
	for ( CPlayer *pCurPlayerInRoom = pPlayersInRoomList->Head(); pCurPlayerInRoom; pCurPlayerInRoom = pPlayersInRoomList->Next( pCurPlayerInRoom ) ) 
	{
		// Send to other players in room
		if ( pCurPlayerInRoom->IsConnected() && pCurPlayerInRoom != pCur->GetPlayer() )
			NWCOtherPlayerGetsItem( pCurPlayerInRoom->GetConnection(), pCur->GetPlayer(), pMatchItem );
	}

	return (true);
}

bool NWCDropItem( CServerConnection *pCur, NUtil::LIST_PTR( CServerConnection ) pConnectionList, uint8_t *pData, uint32_t dataLen )
{
	// Drop an item
	if ( !pCur || !pCur->GetPlayer() )
		return (false);

	if ( dataLen != 8 )
		return (false);

	uint32_t playerID = *((uint32_t*)(pData));
	uint32_t itemID = *((uint32_t*)(pData+4));

	if ( playerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	CRoom *pRoom = pCur->GetPlayer()->GetRoom();
	
	if ( !pRoom )
		return (false);

	// Find item in room
	CItemInstance *pMatchItem = pCur->GetPlayer()->GetItemFromInventoryByID( itemID );

	if ( pMatchItem == NULL )
	{
		// Someone might have already taken this item!
		string sFailReason = "That item is no longer on you";
		NWCPlayerInfoMessage( pCur->GetPlayer(), sFailReason );

		return (true);
	}

	// OK drop item
	if ( !pCur->GetPlayer()->RemoveItemFromInventory( pMatchItem ) )
	{
		string sFailReason = "Can't drop that item!";
		NWCPlayerInfoMessage( pCur->GetPlayer(), sFailReason );

		return (true);
	}
	
	// Add item to room
	if ( !pRoom->AddItemToRoom( pMatchItem ) )
	{
		string sFailReason = "Can't drop that item here!";
		NWCPlayerInfoMessage( pCur->GetPlayer(), sFailReason );

		return (true);
	}

	// Inform player that he got the item (send item data as well)
	NWCCurrentPlayerDropsItem( pCur, pMatchItem );

	// Inform other players in room -- of player getting item	
	NUtil::LIST_PTR( CPlayer ) pPlayersInRoomList = pRoom->GetPlayersInRoom();

	// Now say to everyone in the room...
	for ( CPlayer *pCurPlayerInRoom = pPlayersInRoomList->Head(); pCurPlayerInRoom; pCurPlayerInRoom = pPlayersInRoomList->Next( pCurPlayerInRoom ) ) 
	{
		// Send to other players in room
		if ( pCurPlayerInRoom->IsConnected() && pCurPlayerInRoom != pCur->GetPlayer() )
			NWCOtherPlayerDropsItem( pCurPlayerInRoom->GetConnection(), pCur->GetPlayer(), pMatchItem );
	}

	return (true);
}

bool NWCLogout( CServerConnection *pCur, NUtil::LIST_PTR( CServerConnection ) pConnectionList, uint8_t *pData, uint32_t dataLen )
{
	// Logout command
}

bool NWCEquipItem( CServerConnection *pCur, NUtil::LIST_PTR( CServerConnection ) pConnectionList, uint8_t *pData, uint32_t dataLen )
{
	// Equip an item
	if ( !pCur || !pCur->GetPlayer() )
		return (false);
	
	if ( dataLen != 17 )
		return (false);

	uint32_t playerID = *((uint32_t*)pData);
	uint32_t itemID = *((uint32_t*)(pData+4));
	uint64_t itemKey = *((uint64_t*)(pData+8));
	uint8_t itemEquipSlot = pData[16];

	if ( playerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	// Find item on player...
	CItemInstance *pMatchItem = pCur->GetPlayer()->GetItemFromInventoryByID( itemID );

	// Check for available slot...
	if ( itemEquipSlot >= EQUIP_SLOT_MAX )
		return (false);
	
	// Check for matching item!
	if ( pMatchItem == NULL )
	{
		string sFailReason = "You don't have that item";
	
		NWCPlayerInfoMessage( pCur->GetPlayer(), sFailReason );	
		return (true);
	}

	// Check key for match!
	if ( pMatchItem->GetItemKey() != itemKey )
		return (false);

	if ( pCur->GetPlayer()->GetInstallItem( (tItemEquipType)itemEquipSlot ) )
		return (false);

	// OK check appropriate levels of item!
	string sFailReason;
	if ( pCur->GetPlayer()->EquipItem( pMatchItem, (tItemEquipType)itemEquipSlot, sFailReason ) )
	{
		// Success! -- Inform player equip success
		//	    -- Inform other players in room of that player installing that item
		NWCPlayerEquipsItem( pCur, (tItemEquipType)itemEquipSlot, pMatchItem );
	}
	else
	{
		// Fail
		NWCPlayerFailsEquipItem( pCur, (tItemEquipType)itemEquipSlot, pMatchItem, sFailReason );
	}

	return (true);
}

bool NWCUnequipItem( CServerConnection *pCur, NUtil::LIST_PTR( CServerConnection ) pConnectionList, uint8_t *pData, uint32_t dataLen )
{
	// Unequip an item
	if ( !pCur || !pCur->GetPlayer() )
		return (false);
	
	if ( dataLen != 17 )
		return (false);

	uint32_t playerID = *((uint32_t*)pData);
	uint32_t itemID = *((uint32_t*)(pData+4));
	uint64_t itemKey = *((uint64_t*)(pData+8));
	uint8_t itemEquipSlot = pData[16];

	if ( playerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	// Check for available slot...
	if ( itemEquipSlot >= EQUIP_SLOT_MAX )
		return (false);
	
	// Find item on player...
	CItemInstance *pMatchItem = pCur->GetPlayer()->GetInstallItem( (tItemEquipType)itemEquipSlot );

	// Check for matching item!
	if ( pMatchItem == NULL )
		return (false);

	// Check key for match!
	if ( pMatchItem->GetItemID() != itemID || pMatchItem->GetItemKey() != itemKey )
		return (false);

	// OK try to unequip item!
	CItemInstance *pUnequippedItem;

	string sFailReason;
	if ( pCur->GetPlayer()->UnequipItem( (tItemEquipType)itemEquipSlot, pUnequippedItem, sFailReason ) )
	{
		printf( "Player uninstalled item!\n" );
		// Success! -- Inform player equip success
		//	    -- Inform other players in room of that player installing that item
		NWCPlayerUnequipsItem( pCur, (tItemEquipType)itemEquipSlot, pMatchItem );
	}
	else
	{
		// Fail
		NWCPlayerFailsUnequipItem( pCur, (tItemEquipType)itemEquipSlot, pMatchItem, sFailReason );
	}

	return (true);
}

bool NWCAttack( CServerConnection *pCur, NUtil::LIST_PTR( CServerConnection ) pConnectionList, uint8_t *pData, uint32_t dataLen )
{
	// Attack a target
	if ( !pCur || !pCur->GetPlayer() )
		return (false);
	
	if ( dataLen != 9 )
		return (false);

	uint32_t playerID = *((uint32_t*)pData);
	uint32_t targetID = *((uint32_t*)(pData+4));
	uint8_t targetType = pData[8];

	if ( playerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	CRoom *pRoom;
	if ( !(pRoom = pCur->GetPlayer()->GetRoom()) )
		return (false);

	// Find target
	if ( targetType == NWC_ATTACK_PLAYER )
	{
		CPlayer *pTarget = NULL;
		bool bFound = false;
	
		for ( pTarget = pRoom->GetPlayersInRoom()->Head(); pTarget; pTarget = pRoom->GetPlayersInRoom()->Next( pTarget ) )
		{
			if ( pTarget->GetPlayerID() == targetID )
			{
				bFound = true;
				break;
			}
		}

		if ( !bFound )
		{
			string sFailMessage = "They are no longer here to attack";
			NWCPlayerInfoMessage( pCur->GetPlayer(), sFailMessage );
			return (true);
		}

		if ( pTarget == pCur->GetPlayer() )
		{
			string sFailMessage = "You can't attack yourself";
			NWCPlayerInfoMessage( pCur->GetPlayer(), sFailMessage );
			return (true);
		}

		if ( !pRoom->IsPKAllowed() )
		{
			string sFailMessage = "PK is not allowed here";
			NWCPlayerInfoMessage( pCur->GetPlayer(), sFailMessage );
			return (true);
		}

		bool bAlreadyFighting = pCur->GetPlayer()->IsFightingTarget( targetID, NWC_ATTACK_PLAYER );

		// Attack!!
		if ( !pCur->GetPlayer()->AttackPlayer( pTarget->GetPlayerID() ) )
			return (false);

		// Have the player attack them back!
		pTarget->AttackPlayer( pCur->GetPlayer()->GetPlayerID() );

		// Now send attack response
		NWCPlayerAttackResponse( pCur, pTarget->GetPlayerID(), CAttackTarget::TARGET_PLAYER );

		if ( !bAlreadyFighting )
		{
			// Inform everyone in the room!
			for ( CPlayer *pOtherPlayers = pRoom->GetPlayersInRoom()->Head(); pOtherPlayers; pOtherPlayers = pRoom->GetPlayersInRoom()->Next( pOtherPlayers ) )
			{
				if ( pOtherPlayers != pCur->GetPlayer() && pOtherPlayers->IsConnected() )
				{
					char szTemp[1024];
					
					if ( pOtherPlayers == pTarget )
						snprintf( szTemp, 1024, "%s attacks you!", pCur->GetPlayer()->GetName().c_str() );
					else
						snprintf( szTemp, 1024, "%s attacks %s!", pCur->GetPlayer()->GetName().c_str(), pTarget->GetName().c_str() );

					string sMessage = szTemp;
					NWCPlayerInfoMessage( pOtherPlayers, sMessage );	
				}
			}
		}

		return (true);
	}
	else if ( targetType == NWC_ATTACK_NPC )
	{
		CNPC *pTarget = NULL;
		bool bFound = false;
	
		for ( pTarget = pRoom->GetNPCsInRoom()->Head(); pTarget; pTarget = pRoom->GetNPCsInRoom()->Next( pTarget ) )
		{
			if ( pTarget->GetNPCID() == targetID )
			{
				bFound = true;
				break;
			}
		}

		if ( !bFound )
		{
			string sFailMessage = "They are no longer here to attack";
			NWCPlayerInfoMessage( pCur->GetPlayer(), sFailMessage );
			return (true);
		}

		// Attack!!
		if ( !pCur->GetPlayer()->AttackNPC( pTarget->GetNPCID() ) )
			return (false);

		bool bAlreadyFighting = pCur->GetPlayer()->IsFightingTarget( targetID, CAttackTarget::TARGET_NPC );
		
		// Have the target -- attack them back
		pTarget->AttackPlayer( pCur->GetPlayer()->GetPlayerID() );

		// Now send attack response
		NWCPlayerAttackResponse( pCur, pTarget->GetNPCID(), NWC_ATTACK_NPC );

		if ( !bAlreadyFighting )
		{
			// Inform everyone in the room!
			for ( CPlayer *pOtherPlayers = pRoom->GetPlayersInRoom()->Head(); pOtherPlayers; pOtherPlayers = pRoom->GetPlayersInRoom()->Next( pOtherPlayers ) )
			{
				if ( pOtherPlayers != pCur->GetPlayer() && pOtherPlayers->IsConnected() )
				{
					char szTemp[1024];
					
					snprintf( szTemp, 1024, "%s attacks %s!", pCur->GetPlayer()->GetName().c_str(), pTarget->GetName().c_str() );

					string sMessage = szTemp;
					NWCPlayerInfoMessage( pOtherPlayers, sMessage );	
				}
			}
		}

		return (true);
	}
	else
	{
		string sFailMessage = "Attack what?";
		NWCPlayerInfoMessage( pCur->GetPlayer(), sFailMessage );
		return (true);
	}
}


bool NWCPlayerFlee( CServerConnection *pCur, NUtil::LIST_PTR( CServerConnection ) pConnectionList, uint8_t *pData, uint32_t dataLen )
{
	// Attack a target
	if ( !pCur || !pCur->GetPlayer() )
		return (false);
	
	if ( dataLen != 4 )
		return (false);

	CPlayer *pPlayer = pCur->GetPlayer();

	uint32_t playerID = *((uint32_t*)pData);

	if ( playerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	CRoom *pRoom;
	if ( !(pRoom = pCur->GetPlayer()->GetRoom()) )
		return (false);

	if ( pPlayer->GetCurCPU() < (pPlayer->GetMaxCPU() * FLEE_CPU_THRESHOLD_PERCENTAGE) )
	{
		string sInfoMessage = "You can't flee without sufficient CPU!";

		NWCPlayerInfoMessage( pPlayer, sInfoMessage );
		return (true);
	}

	// Attempt to flee -- see if we are fighting an NPC who won't let us flee!!
	bool bBlockFlee = false;
	CNPC *pNPC = NULL;

	for ( pNPC = pRoom->GetNPCsInRoom()->Head(); pNPC; pNPC = pRoom->GetNPCsInRoom()->Next( pNPC ) )
	{
		if ( pNPC->IsFightingPlayer( pPlayer ) && pNPC->CanBlockFlee() )
		{
			bBlockFlee = true;
			break;
		}
	}

	if ( bBlockFlee )
	{
		char szTemp[1024];
		snprintf( szTemp, 1024, "%s blocks you from fleeing!", pNPC->GetName().c_str() );
		string sInfoMessage = szTemp;

		NWCPlayerInfoMessage( pPlayer, sInfoMessage );	
		return (true);
	}

	// Find a random direction to flee
	uint8_t fleeDirection;
	if ( !pRoom->GetRandomDirection( fleeDirection ) )
	{
		string sInfoMessage = "Nowhere to flee!";

		NWCPlayerInfoMessage( pPlayer, sInfoMessage );
		return (true);
	}

	CRoom *pNextRoom = g_oRoomHash.Find( pRoom->GetDirection( fleeDirection ) );

	if ( !pNextRoom )
	{
		printf( "Error: Room not found for direction %s from room %d\n", fleeDirection, pRoom->GetVnum() );
		return (false);
	}

	// Flee in that direction!
	pPlayer->SetCPU( 0 );

	// Stop fighting
	pPlayer->StopFighting();

	// Inform players in our current room we are leaving
	NWCPlayerLeavesFromRoom( pCur, NWC_PLAYER_LEAVES_FLEE, fleeDirection );
	
	pPlayer->SetRoom( pNextRoom );

	if ( !NWCEnterRoomResponse( pCur ) )
	{
		printf( "Enterroom response failed!\n" );

                // Failed to write data
                pCur->Disconnect();

                return (false);
	}

	// Inform room of our arrival
	NWCPlayerArrivesInRoom( pCur, NWC_PLAYER_ARRIVAL_ENTERROOM );
	
	// Check for enter room actions
	DoPlayerEnterRoomAction( pCur->GetPlayer(), (uint8_t)NWC_PLAYER_ARRIVAL_ENTERROOM );

	return (true);
}

/*
bool NWCDeleteItem( CServerConnection *pCur, NUtil::LIST_PTR( CServerConnection ) pConnectionList, uint8_t *pData, uint32_t dataLen )
{
	// Get an item
	if ( !pCur || !pCur->GetPlayer() )
		return (false);

	if ( dataLen != 8 )
		return (false);

	CPlayer *pPlayer = pCur->GetPlayer();

	uint32_t playerID = *((uint32_t*)(pData));
	uint32_t itemID = *((uint32_t*)(pData+4));

	if ( playerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	CRoom *pRoom = pCur->GetPlayer()->GetRoom();
	
	if ( !pRoom )
		return (false);

	// Find item in room
	CItemInstance *pMatchItem = pRoom->GetItemInRoomByID( itemID );

	if ( pMatchItem == NULL )
	{
		// Someone might have already taken this item!
		string sFailReason = "That item is no longer in the room";
		NWCPlayerInfoMessage( pCur->GetPlayer(), sFailReason );

		return (true);
	}

	// Prevent fixed items from moving
	if ( pMatchItem->HasFlagNoDestroy() )
	{
		string sFailReason = "That item cannot be deleted";
		NWCPlayerInfoMessage( pCur->GetPlayer(), sFailReason );

		return (true);
	}

	// Check CPU consumption
	if ( !CanDeleteCPUCheck( pPlayer ) )
	{
		string sFailReason = "Insufficient CPU to delete that item";
		NWCPlayerInfoMessage( pCur->GetPlayer(), sFailReason );

		return (true);
	}

	uint16_t cpuDeleteAmount = GetDeleteCPUNeeded( pPlayer );

	// Consume CPU
	pPlayer->SetCPU( pPlayer->GetCurCPU() - cpuDeleteAmount );

	// Delete this item
	pMatchItem->m_roomLink.Unlink();

	// Inform player that he got the item (send item data as well)
	NWCPlayerDeletesItem( pCur, pPlayer, pMatchItem );

	// Inform other players in room -- of player getting item	
	NUtil::LIST_PTR( CPlayer ) pPlayersInRoomList = pRoom->GetPlayersInRoom();

	// Now say to everyone in the room...
	for ( CPlayer *pCurPlayerInRoom = pPlayersInRoomList->Head(); pCurPlayerInRoom; pCurPlayerInRoom = pPlayersInRoomList->Next( pCurPlayerInRoom ) ) 
	{
		// Send to other players in room
		if ( pCurPlayerInRoom->IsConnected() && pCurPlayerInRoom != pPlayer )
			NWCPlayerDeletesItem( pCurPlayerInRoom->GetConnection(), pPlayer, pMatchItem );
	}

	// Delete the item from the realm	
	printf( "%s deletes item [%d] %s\n", pPlayer->GetName().c_str(), pMatchItem->GetItemID(), pMatchItem->GetName().c_str() );

	// Remove item from the world
	delete pMatchItem;

	return (true);
}
*/

bool NWCRunScript( CServerConnection *pCur, NUtil::LIST_PTR( CServerConnection ) pConnectionList, uint8_t *pData, uint32_t dataLen )
{
	// Get an item
	if ( !pCur || !pCur->GetPlayer() )
		return (false);

	if ( dataLen < 5 )
		return (false);

	CPlayer *pPlayer = pCur->GetPlayer();

	uint32_t playerID = *((uint32_t*)(pData));
	uint8_t scriptID = pData[4];

	if ( playerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	// Run the script!!
	return RunScript( pCur, scriptID, pData+5, dataLen-5 );
}


bool NWCPlayerFinger( CServerConnection *pCur, NUtil::LIST_PTR( CServerConnection ) pConnectionList, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[1024];

	// Get an item
	if ( !pCur || !pCur->GetPlayer() )
		return (false);

	if ( dataLen != 13 )
		return (false);

	CPlayer *pPlayer = pCur->GetPlayer();

	CRoom *pRoom;
	if ( !(pRoom = pPlayer->GetRoom()) )
		return (false);

	uint32_t playerID = *((uint32_t*)(pData));
	uint32_t targetID = *((uint32_t*)(pData+4));
	uint8_t targetType = pData[8];
	uint32_t requestItemID = *((uint32_t*)(pData+9));

	if ( playerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	// Find a suitable item to use for finger request
	CItemInstance *pRequestItem = pPlayer->GetItemFromInventoryByID( requestItemID );

	if ( !pRequestItem )
	{
		string sFailMessage = "You do not have a finger request script item on you";
		NWCPlayerInfoMessage( pCur->GetPlayer(), sFailMessage );
		return (true);
	}

	// Finger player in room
	if ( targetType == NWC_PLAYER_FINGER_PLAYER )
	{
		CPlayer *pTarget = pRoom->GetPlayerInRoomByID( targetID );	

		if ( !pTarget )
		{
			string sFailMessage = "They are no longer here";
			NWCPlayerInfoMessage( pCur->GetPlayer(), sFailMessage );
			return (true);
		}

		// Consume a finger print request item!
		if ( !pCur->GetPlayer()->RemoveItemFromInventory( pRequestItem ) )
		{
			string sFailMessage = "You do not have a finger request script item on you";
			NWCPlayerInfoMessage( pCur->GetPlayer(), sFailMessage );

			return (true);
		}

		snprintf( szTemp, 1024, "The server consumes a %s item to run your finger request", pRequestItem->GetName().c_str() );
		string sConsumeMessage = szTemp;

		NWCPlayerInfoMessage( pCur->GetPlayer(), sConsumeMessage );
		delete pRequestItem;	
		
		// Save
		pCur->GetPlayer()->SaveFile();	

		// Send finger response
		uint32_t outLen;
		uint8_t *pItemFingerprint = pTarget->GetItemFingerprint( outLen );

		NWCPlayerFingerResponse( pCur, requestItemID, pTarget->GetPlayerID(), NWC_PLAYER_FINGER_PLAYER, pTarget->GetHackLevel(), pTarget->GetCodeLevel(), pTarget->GetScriptLevel(), pTarget->GetCurHP(), pTarget->GetMaxHP(), pItemFingerprint, outLen );

		// Delete it if it was available
		if ( pItemFingerprint )
			delete pItemFingerprint;


		// Inform everyone in the room!
		for ( CPlayer *pOtherPlayers = pRoom->GetPlayersInRoom()->Head(); pOtherPlayers; pOtherPlayers = pRoom->GetPlayersInRoom()->Next( pOtherPlayers ) )
		{
			if ( pOtherPlayers != pCur->GetPlayer() && pOtherPlayers->IsConnected() )
			{
				if ( pOtherPlayers == pTarget )
					snprintf( szTemp, 1024, "%s runs a finger request on you", pCur->GetPlayer()->GetName().c_str() );
				else
					snprintf( szTemp, 1024, "%s runs a finger request on %s", pCur->GetPlayer()->GetName().c_str(), pTarget->GetName().c_str() );

				string sMessage = szTemp;
				NWCPlayerInfoMessage( pOtherPlayers, sMessage );	
			}
		}

		return (true);
	}
	else if ( targetType == NWC_PLAYER_FINGER_NPC )
	{
		CNPC *pTarget = pRoom->GetNPCInRoomByID( targetID );
		
		if ( !pTarget )
		{
			string sFailMessage = "They are no longer here";
			NWCPlayerInfoMessage( pCur->GetPlayer(), sFailMessage );
			return (true);
		}
		
		// Consume a finger print request item!
		if ( !pCur->GetPlayer()->RemoveItemFromInventory( pRequestItem ) )
		{
			string sFailMessage = "You do not have a finger request script item on you";
			NWCPlayerInfoMessage( pCur->GetPlayer(), sFailMessage );

			return (true);
		}

		snprintf( szTemp, 1024, "The server consumes a %s item to run your finger request", pRequestItem->GetName().c_str() );
		string sConsumeMessage = szTemp;

		NWCPlayerInfoMessage( pCur->GetPlayer(), sConsumeMessage );
		delete pRequestItem;

		// Save
		pCur->GetPlayer()->SaveFile();	

		// Send finger response
		uint32_t outLen;
		uint8_t *pItemFingerprint = pTarget->GetItemFingerprint( outLen );

		NWCPlayerFingerResponse( pCur, requestItemID, pTarget->GetNPCID(), NWC_PLAYER_FINGER_NPC, pTarget->GetHackLevel(), pTarget->GetCodeLevel(), pTarget->GetScriptLevel(), pTarget->GetCurHP(), pTarget->GetMaxHP(), pItemFingerprint, outLen );

		// Delete it if it was available
		if ( pItemFingerprint )
			delete pItemFingerprint;
		
		// Inform everyone in the room!
		for ( CPlayer *pOtherPlayers = pRoom->GetPlayersInRoom()->Head(); pOtherPlayers; pOtherPlayers = pRoom->GetPlayersInRoom()->Next( pOtherPlayers ) )
		{
			if ( pOtherPlayers != pCur->GetPlayer() && pOtherPlayers->IsConnected() )
			{
				snprintf( szTemp, 1024, "%s runs a finger request on %s", pCur->GetPlayer()->GetName().c_str(), pTarget->GetName().c_str() );

				string sMessage = szTemp;
				NWCPlayerInfoMessage( pOtherPlayers, sMessage );	
			}
		}

		return true;
	}
	else
		return (false);
}

typedef bool (*tNWCCmdFptr)( CServerConnection *, NUtil::LIST_PTR( CServerConnection ) pConnectionList, uint8_t *, uint32_t dataLen );

tNWCCmdFptr g_cmdTable[NWC_MAX_REQUESTS] =
{
	NWCAuth,
	NWCSay,
	NWCWall,
	NWCDirection,
	NWCGetItem,
	NWCDropItem,
	NWCLogout,
	NWCEquipItem,
	NWCUnequipItem,
	NWCRunScript,
	NWCAttack,
	NWCPlayerFlee,
	NWCRunScript,
	NWCPlayerFinger,
	NWCCreatePlayer,
#if ENABLE_PROOF_OF_WORK
	NWCProofData,
#endif
};

bool CNetworkCommands::DoNetworkCommand( CServerConnection *pCur, NUtil::LIST_PTR( CServerConnection ) pConnectionList, uint8_t cmdNumber, uint8_t *pData, uint32_t dataLen )
{
	if ( cmdNumber >= NWC_MAX_REQUESTS )
		return (false);

	// printf( "CMD[%d]\n", cmdNumber );

	// Check connection state
#if ENABLE_PROOF_OF_WORK
	if ( pCur->GetConnectionState() == CServerConnection::CONNECTION_WAIT_PROOF )
	{
		if ( cmdNumber != NWC_PROOF_DATA )
		{
			printf( "[COMMAND] Connection in WAIT_AUTH will only accept AUTH_REQUEST and CREATE_PLAYER commands!\n" );
			return (false);
		}
	}
#endif

	if ( pCur->GetConnectionState() == CServerConnection::CONNECTION_WAIT_AUTH )
	{
		if ( cmdNumber != NWC_AUTH_REQUEST && cmdNumber != NWC_CREATE_PLAYER )
		{
			printf( "[COMMAND] Connection in WAIT_AUTH will only accept AUTH_REQUEST and CREATE_PLAYER commands!\n" );
			return (false);
		}
	}

	if ( pCur->GetConnectionState() == CServerConnection::CONNECTION_DISCONNECT )
	{
		printf( "[COMMAND] Connection in DISCONNECT will not accept commands!\n" );
		return (false);
	}

	// Run command
	return (*g_cmdTable[cmdNumber])( pCur, pConnectionList, pData, dataLen );
}
