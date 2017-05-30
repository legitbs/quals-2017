#include "common.h"

void NWCPlayerFingerRequest( CServerConnection *pCur, uint32_t targetID, uint8_t targetType, uint32_t requestItemID )
{
	if ( !pCur )
		return;

	CPlayer *pPlayer = pCur->GetPlayer();
	
	if ( !pPlayer )
		return;

	CStreamData oStream;

	uint32_t playerID = pPlayer->GetPlayerID();

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &targetID, sizeof(targetID) );
	oStream.Write( &targetType, sizeof(targetType) );
	oStream.Write( &requestItemID, sizeof(requestItemID) );

	if ( !pCur->SendCommand( NWC_PLAYER_FINGER_REQUEST, &oStream ) )
	{
		pCur->Disconnect();
		return;
	}
}

void NWCRunScriptRequest( CServerConnection *pCur, uint8_t scriptID, CStreamData &oExtraData )
{
	if ( !pCur )
		return;

	CPlayer *pPlayer = pCur->GetPlayer();
	
	if ( !pPlayer )
		return;

	CStreamData oStream;

	uint32_t playerID = pPlayer->GetPlayerID();

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &scriptID, sizeof(scriptID) );

	uint8_t *pData = new uint8_t[oExtraData.GetReadAvailable()];
	oExtraData.Peak( pData, oExtraData.GetReadAvailable() );
	
	oStream.Write( pData, oExtraData.GetReadAvailable() );
	
	delete pData;

	if ( !pCur->SendCommand( NWC_RUN_SCRIPT_REQUEST, &oStream ) )
	{
		pCur->Disconnect();
		return;
	}
}

void NWCPlayerFleeRequest( CServerConnection *pCur )
{
	if ( !pCur )
		return;

	CPlayer *pPlayer = pCur->GetPlayer();

	if ( !pPlayer )
		return;

	CStreamData oStream;
	
	uint32_t playerID = pPlayer->GetPlayerID();

	oStream.Write( &playerID, sizeof(playerID) );

	if ( !pCur->SendCommand( NWC_FLEE_REQUEST, &oStream ) )
	{
		pCur->Disconnect();
		return;
	}
}

void NWCAttackRequest( CServerConnection *pCur, uint32_t targetID, uint8_t targetType )
{
	if ( !pCur )
		return;

	CPlayer *pPlayer = pCur->GetPlayer();
	
	if ( !pPlayer )
		return;

	CStreamData oStream;

	uint32_t playerID = pPlayer->GetPlayerID();

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &targetID, sizeof(targetID) );
	oStream.Write( &targetType, sizeof(targetType) );

	if ( !pCur->SendCommand( NWC_ATTACK_REQUEST, &oStream ) )
	{
		pCur->Disconnect();
		return;
	}
}

void NWCDropItemRequest( CServerConnection *pCur, CItemInstance *pItem )
{
	if ( !pCur )
		return;

	CPlayer *pPlayer = pCur->GetPlayer();

	if ( !pPlayer )
		return;

	if ( !pItem )
		return;

	CStreamData oStream;

	uint32_t playerID = pPlayer->GetPlayerID();
	uint32_t itemID = pItem->GetItemID();

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &itemID, sizeof(itemID) );
	
	if ( !pCur->SendCommand( NWC_DROPITEM_REQUEST, &oStream ) )
	{
		pCur->Disconnect();
		return;
	}
}

void NWCGetItemRequest( CServerConnection *pCur, CItemInRoom *pItem )
{
	if ( !pCur )
		return;

	CPlayer *pPlayer = pCur->GetPlayer();

	if ( !pPlayer )
		return;

	if ( !pItem )
		return;

	CStreamData oStream;

	uint32_t playerID = pPlayer->GetPlayerID();
	uint32_t itemID = pItem->GetID();

	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &itemID, sizeof(itemID) );
	
	if ( !pCur->SendCommand( NWC_GETITEM_REQUEST, &oStream ) )
	{
		pCur->Disconnect();
		return;
	}
}

void NWCUnequipItemRequest( CServerConnection *pCur, CItemInstance *pItem, tItemEquipType slot )
{
	if ( !pCur )
		return;

	if ( !pCur->GetPlayer() )
		return;

	if ( !pItem )
		return;

	// Generate command
	CStreamData oStream;

	uint8_t equipSlotTemp = slot;
	uint32_t playerID = pCur->GetPlayer()->GetPlayerID();
	uint32_t itemID = pItem->GetItemID();
	uint64_t itemKey = pItem->GetItemKey();
	
	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &itemID, sizeof(itemID) );
	oStream.Write( &itemKey, sizeof(itemKey) );
	oStream.Write( &equipSlotTemp, sizeof(equipSlotTemp) );

	if ( !pCur->SendCommand( NWC_UNEQUIPITEM_REQUEST, &oStream ) )
	{
		pCur->Disconnect();
		return;
	}	
}

void NWCEquipItemRequest( CServerConnection *pCur, CItemInstance *pItem, tItemEquipType slot )
{
	if ( !pCur )
		return;

	if ( !pCur->GetPlayer() )
		return;

	if ( !pItem )
		return;

	// Generate command
	CStreamData oStream;

	uint8_t equipSlotTemp = slot;
	uint32_t playerID = pCur->GetPlayer()->GetPlayerID();
	uint32_t itemID = pItem->GetItemID();
	uint64_t itemKey = pItem->GetItemKey();
	
	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &itemID, sizeof(itemID) );
	oStream.Write( &itemKey, sizeof(itemKey) );
	oStream.Write( &equipSlotTemp, sizeof(equipSlotTemp) );

	if ( !pCur->SendCommand( NWC_EQUIPITEM_REQUEST, &oStream ) )
	{
		pCur->Disconnect();
		return;
	}	
}

void NWCDirectionRequest( CServerConnection *pCur, uint8_t direction )
{
	if ( !pCur )
		return;

	if ( !pCur->GetPlayer() )
		return;

	if ( !pCur->GetPlayer()->GetRoom() )
		return;

	if ( direction >= DIRECTION_MAX )
		return;

	// Direction request
	CStreamData oStream;

	uint32_t playerID = pCur->GetPlayer()->GetPlayerID();
	
	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &direction, sizeof(direction) );

	if ( !pCur->SendCommand( NWC_DIRECTION_REQUEST, &oStream ) )
	{
		pCur->Disconnect();
		return;
	}
}

void NWCWallRequest( CServerConnection *pCur, string sText )
{
	if ( !pCur )
		return;

	if ( !pCur->GetPlayer() )
		return;

	// Say something
	CStreamData oStream;
	
	uint32_t playerID = pCur->GetPlayer()->GetPlayerID();
	uint8_t wallLength = sText.length();
	
	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &wallLength, sizeof(wallLength) );
	oStream.Write( sText.c_str(), wallLength );

	if ( !pCur->SendCommand( NWC_WALL_REQUEST, &oStream ) )
	{
		pCur->Disconnect();
		return;
	} 
}

void NWCSayRequest( CServerConnection *pCur, string sText )
{
#if 0
	if ( !pCur )
		return;

	if ( !pCur->GetPlayer() )
		return;
#endif

	// Say something
	CStreamData oStream;
	
	uint32_t playerID = pCur->GetPlayer()->GetPlayerID();
	uint8_t sayLength = sText.length();
	uint32_t nullByte = 0;

	oStream.Write( &nullByte, sizeof(nullByte) );
	
	oStream.Write( &playerID, sizeof(playerID) );
	oStream.Write( &sayLength, sizeof(sayLength) );
	oStream.Write( sText.c_str(), sayLength );

	if ( !pCur->SendCommand( NWC_SAY_REQUEST, &oStream ) )
	{
		pCur->Disconnect();
		return;
	} 
}

bool NWCAuthChallenge( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	// Should never get this message
	return (false);
}

bool NWCCreatePlayerResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	// Should never get this message
	return (false);
}

bool NWCAuthResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	if ( dataLen < 1 )
	{
		pCur->Disconnect();
		return (false);
	}

	if ( pData[0] != 1 )
	{
		pCur->Disconnect();
		return (false);
	}

	CPlayer *pNewPlayer = pCur->GetPlayerForAuthentication();

	CStreamData oStream;
	oStream.Write( pData+1, dataLen-1 );

	if ( !pNewPlayer->ReadPlayerData( &oStream ) )
	{
		pCur->Disconnect();
		return (false);
	}

	pCur->Authenticate( );

	return (true);
}

bool NWCPlayerSaysResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[1024];

	if ( !pCur )
		return (false);
	
	if ( !pCur->GetPlayer() )
		return (false);

	if ( !pCur->GetPlayer()->GetRoom() )
		return (false);

	if ( dataLen < 9 )
		return (false);

	uint32_t fromID = *((uint32_t*)(pData+0));
	uint32_t toID = *((uint32_t*)(pData+4));
	uint8_t sayLength = pData[8];

	if ( dataLen != sayLength+9 )
		return (false);

	if ( toID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	// Find the player name from fromID
	NUtil::LIST_PTR( CPlayerInRoom ) pPlayersInRoomList = pCur->GetPlayer()->GetRoom()->GetPlayersInRoom();

	CPlayerInRoom *pFromPlayer = NULL;
	for ( CPlayerInRoom *pPlayerInRoom = pPlayersInRoomList->Head(); pPlayerInRoom; pPlayerInRoom = pPlayersInRoomList->Next( pPlayerInRoom ) )
	{
		if ( pPlayerInRoom->GetID() == fromID )
		{
			pFromPlayer = pPlayerInRoom;
			break;
		}
	}

	// Validate! -- bad server??
	if ( !pFromPlayer )
	{
		printf( "Bad server message... say from player not in your room???\n" );
		return (false);
	}

	// Get say text
	char *pTempData = new char[sayLength+1];
	memcpy( pTempData, pData+9, sayLength );
	pTempData[sayLength] = '\0';

	string sSayText = pTempData;
	delete pTempData;

	// OK do say	
	if ( toID == fromID )
		snprintf( szTemp, 1024, "%sYou say '%s'\n", FC_NORMAL_YELLOW, sSayText.c_str() );
	else
		snprintf( szTemp, 1024, "%s%s%s says '%s'\n", FC_NORMAL_RED, pFromPlayer->GetName().c_str(), FC_NORMAL_YELLOW, sSayText.c_str() );

	pCur->WriteOutput( szTemp );

	return (true);
}

bool NWCPlayerArrivesResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[1024];

	if ( !pCur )
		return (false);

	if ( !pCur->GetPlayer() )
		return (false);

	if ( dataLen < 6 )
		return (false);

	uint8_t arrivalType = pData[0];
	uint32_t playerID = *((uint32_t*)(pData+1));
	uint8_t playerNameLength = pData[5];

	if ( dataLen != (playerNameLength+6) )
		return (false);

	// Get player name
	char *pTempData = new char[playerNameLength+1];
	
	memcpy( pTempData, pData+6, playerNameLength );
	pTempData[playerNameLength] = '\0';

	string sArrivingPlayerName = pTempData;
	delete pTempData;

	if ( playerID == pCur->GetPlayer()->GetPlayerID() )
	{
		return true;
	}
	else
	{
		// OK inform user that a player has arrived in the room...
		if ( arrivalType == NWC_PLAYER_ARRIVAL_LOGIN )
			snprintf( szTemp, 1024, "%s%s%s logs in to the realm.\n", FC_NORMAL_RED, sArrivingPlayerName.c_str(), FC_NORMAL_YELLOW );
		else if ( arrivalType == NWC_PLAYER_ARRIVAL_ENTERROOM )
			snprintf( szTemp, 1024, "%s%s%s arrives.\n", FC_NORMAL_RED, sArrivingPlayerName.c_str(), FC_NORMAL_YELLOW );
		else if ( arrivalType == NWC_PLAYER_ARRIVAL_RESPAWN )
			snprintf( szTemp, 1024, "%s%s%s respawns from death!\n", FC_NORMAL_RED, sArrivingPlayerName.c_str(), FC_NORMAL_YELLOW );

		pCur->WriteOutput( szTemp );
	}
	
	// Add player to the room list if they aren't already in it
	NUtil::LIST_PTR( CPlayerInRoom ) pPlayersInRoomList = pCur->GetPlayer()->GetRoom()->GetPlayersInRoom();

	bool bAdd = true;
	for ( CPlayerInRoom *pPlayerInRoom = pPlayersInRoomList->Head(); pPlayerInRoom; pPlayerInRoom = pPlayersInRoomList->Next( pPlayerInRoom ) )
	{
		if ( pPlayerInRoom->GetID() == playerID )
		{
			// Already in list -- don't add (strange???)
			bAdd = false;
			break;
		}
	}

	// Add player to room list if necessary
	if ( bAdd )
		pPlayersInRoomList->InsertHead( new CPlayerInRoom( sArrivingPlayerName.c_str(), playerID ) );

	return (true);
}

bool NWCPlayerLeavesResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[1024];

	if ( !pCur )
		return (false);
	
	CPlayer *pPlayer;
	if ( !(pPlayer = pCur->GetPlayer()) )
		return (false);

	if ( dataLen < 7 )
		return (false);

	uint8_t leaveType = pData[0];
	uint8_t leaveDirection = pData[1];
	uint32_t playerID = *((uint32_t*)(pData+2));
	uint8_t playerNameLength = pData[6];

	if ( dataLen != (playerNameLength+7) )
		return (false);

	// Get player name
	char *pTempData = new char[playerNameLength+1];
	
	memcpy( pTempData, pData+7, playerNameLength );
	pTempData[playerNameLength] = '\0';

	char szDirection[256];
	switch ( leaveDirection )
	{
	case DIRECTION_NORTH:
		strcpy( szDirection, "north" );
		break;

	case DIRECTION_EAST:
		strcpy( szDirection, "east" );
		break;
	
	case DIRECTION_WEST:
		strcpy( szDirection, "west" );
		break;

	case DIRECTION_SOUTH:
		strcpy( szDirection, "south" );
		break;
	
	case DIRECTION_UP:
		strcpy( szDirection, "up" );
		break;

	case DIRECTION_DOWN:
		strcpy( szDirection, "down" );
		break;

	default:
		szDirection[0] = '\0';
		break;
	}

	if ( playerID == pPlayer->GetPlayerID() )
	{
		if ( leaveType == NWC_PLAYER_LEAVES_EXITROOM )
		{
			// Consume CPU!
			uint16_t cpuMovementNeeded = GetMovementCPUNeeded( pPlayer );

			if ( cpuMovementNeeded > pCur->GetPlayer()->GetCurCPU() )
			{
				printf( "Critical error: Movement occured without sufficient CPU!\n" );
				pCur->Disconnect();
				return (false);
			}

			pPlayer->SetCPU( pPlayer->GetCurCPU() - cpuMovementNeeded );
		}

		// We are leaving!
		if ( leaveType == NWC_PLAYER_LEAVES_LOGOUT )
			snprintf( szTemp, 1024, "%sYou logout of the realm.\n", FC_NORMAL_RED );
		else if ( leaveType == NWC_PLAYER_LEAVES_EXITROOM )
			snprintf( szTemp, 1024, "%sYou leave %s\n", FC_NORMAL_YELLOW, szDirection );
		else if ( leaveType == NWC_PLAYER_LEAVES_FLEE )
		{
			// Stop fighting and set CPU to 0
			pPlayer->StopFighting();
			pPlayer->SetCPU( 0 );

			snprintf( szTemp, 1024, "%sYou flee %s\n", FC_NORMAL_RED, szDirection );	
		}

		pCur->WriteOutput( szTemp );

		return true;
	}

	// OK inform user that a player has arrived in the room...
	if ( leaveType == NWC_PLAYER_LEAVES_LOGOUT )
		snprintf( szTemp, 1024, "%s%s%s logs out of the realm.\n", FC_NORMAL_RED, pTempData, FC_NORMAL_YELLOW );
	else if ( leaveType == NWC_PLAYER_LEAVES_EXITROOM )
	{
		snprintf( szTemp, 1024, "%s%s%s leaves %s.\n", FC_NORMAL_RED, pTempData, FC_NORMAL_YELLOW, szDirection );
	}
	else if ( leaveType == NWC_PLAYER_LEAVES_FLEE )
	{
		snprintf( szTemp, 1024, "%s%s%s flees %s!\n", FC_NORMAL_RED, pTempData, FC_NORMAL_YELLOW, szDirection );
	}

	pCur->WriteOutput( szTemp );
	
	// Add player to the room list if they aren't already in it
	NUtil::LIST_PTR( CPlayerInRoom ) pPlayersInRoomList = pCur->GetPlayer()->GetRoom()->GetPlayersInRoom();

	for ( CPlayerInRoom *pPlayerInRoom = pPlayersInRoomList->Head(); pPlayerInRoom; pPlayerInRoom = pPlayersInRoomList->Next( pPlayerInRoom ) )
	{
		if ( pPlayerInRoom->GetID() == playerID )
		{
			pCur->GetPlayer()->StopFightingTarget( playerID, NWC_ATTACK_PLAYER );

			delete pPlayerInRoom;
			break;
		}
	}
	

	return (true);
}

bool NWCPlayerWallResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[1024];

	if ( !pCur )
		return (false);
	
	if ( !pCur->GetPlayer() )
		return (false);

	if ( !pCur->GetPlayer()->GetRoom() )
		return (false);

	if ( dataLen < 9 )
		return (false);

	uint32_t fromID = *((uint32_t*)(pData+0));
	uint32_t toID = *((uint32_t*)(pData+4));
	uint8_t fromPlayerLength = pData[8];
	uint8_t wallLength = pData[9];

	if ( dataLen != wallLength+fromPlayerLength+10 )
		return (false);

	if ( toID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	char *pTempData = new char[fromPlayerLength+1];
	memcpy( pTempData, pData+10, fromPlayerLength );
	pTempData[fromPlayerLength] = '\0';

	string sFromPlayerName = pTempData;
	delete pTempData;

	pTempData = new char[wallLength+1];
	memcpy( pTempData, pData+10+fromPlayerLength, wallLength );
	pTempData[wallLength] = '\0';

	string sWallText = pTempData;
	delete pTempData;

	// Validate! -- bad server??
	if ( fromID == toID )
		snprintf( szTemp, 1024, "%sYou wall '%s' to everyone.\n", FC_NORMAL_YELLOW, sWallText.c_str() );
	else
		snprintf( szTemp, 1024, "%s%s%s walls '%s'\n", FC_NORMAL_RED, sFromPlayerName.c_str(), FC_NORMAL_YELLOW, sWallText.c_str() );

	pCur->WriteOutput( szTemp );

	return (true);
}

bool NWCPlayerEnterroomResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	// THIS player enters a room
	if ( !pCur->GetPlayer() )
	{
		pCur->Disconnect();
		return (false);
	}

	// Enter a room...
	CStreamData oStream;

	oStream.Write( pData, dataLen );

	CRoom *pNewRoom = new CRoom( );
	
	if ( !pNewRoom->ReadRoomData( &oStream ) )
	{
		delete pNewRoom;

		pCur->Disconnect();
		return (false);
	}

	uint32_t playerCount;
	oStream.Read( &playerCount, sizeof(playerCount) );
	
	NUtil::LIST_PTR( CPlayerInRoom ) pPlayersInRoomList = pNewRoom->GetPlayersInRoom();

	// Clear old players in room	
	pPlayersInRoomList->DeleteAll();

	while ( playerCount-- > 0 )
	{
		uint32_t playerID;
		uint8_t nameLength;
		char *pTempData;

		oStream.Read( &playerID, sizeof(playerID) );
		oStream.Read( &nameLength, sizeof(nameLength) );
		
		pTempData = new char[nameLength+1];
		oStream.Read( pTempData, nameLength );
		pTempData[nameLength] = '\0';

		pPlayersInRoomList->InsertTail( new CPlayerInRoom( pTempData, playerID ) );		
		
		delete pTempData;	
	}

	// NPCs
	uint32_t npcCount;
	oStream.Read( &npcCount, sizeof(npcCount) );
	NUtil::LIST_PTR( CNPCInRoom ) pNPCsInRoomList = pNewRoom->GetNPCsInRoom();

	// Clear old NPCs in room
	pNPCsInRoomList->DeleteAll();

	while ( npcCount-- > 0 )
	{
		uint32_t npcID;
		uint8_t nameLength;
		char *pTempData;

		oStream.Read( &npcID, sizeof(npcID) );
		oStream.Read( &nameLength, sizeof(nameLength) );
	
		pTempData = new char[nameLength+1];
		oStream.Read( pTempData, nameLength );
		pTempData[nameLength] = '\0';

		pNPCsInRoomList->InsertTail( new CNPCInRoom( pTempData, npcID ) );

		delete pTempData;
	}

	// Read in items in room
	uint32_t itemCount;
	oStream.Read( &itemCount, sizeof(itemCount) );

	NUtil::LIST_PTR( CItemInRoom ) pItemsInRoomList = pNewRoom->GetItemsInRoom();
	
	// Clear old items in room	
	pItemsInRoomList->DeleteAll();		

	while ( itemCount-- > 0 )
	{
		uint32_t itemID;
		uint8_t nameLength;
		char *pTempData;

		oStream.Read( &itemID, sizeof(itemID) );
		oStream.Read( &nameLength, sizeof(nameLength) );

		pTempData = new char[nameLength+1];
		oStream.Read( pTempData, nameLength );
		pTempData[nameLength] = '\0';

		pItemsInRoomList->InsertTail( new CItemInRoom( pTempData, itemID ) );

		delete pTempData;
	}

	if ( oStream.GetReadAvailable() != 0 )
	{
		pCur->Disconnect();
		
		printf( "Invalid NWCPlayerEnterroomResponse command.\n" );
		return (false);
	}

	pCur->GetPlayer()->SetRoom( pNewRoom );

	// Look at the room after entering it
	g_cmdHandler.DoCommand( pCur, "ls" );

	return (true);	
}

string &GetEquipItemEffectString( CItemInstance *pMatchItem )
{
	string sEquipEffectString;
	char szTemp2[128]; // Was 64 -- for overwrite
	char szTemp[128] = "";
	
	// BUG:: szTemp can be exceeded causing a stack overwrite

	bool bPrintBoostMetrics = false;	

	if ( pMatchItem->GetHPModifier() )
	{
		if ( pMatchItem->GetHPModifier() < 0 )
			sprintf( szTemp2, "damages you decreasing your HP by %d%d%d", -pMatchItem->GetHPModifier() );	
		else
			sprintf( szTemp2, "boosts HP by %d", pMatchItem->GetHPModifier() );

		strcat( szTemp, szTemp2 );
		bPrintBoostMetrics = true;
	}

	if ( pMatchItem->GetCPUModifier() )
	{
		if ( bPrintBoostMetrics )
			strcat( szTemp, ", " );
		
		if ( pMatchItem->GetCPUModifier() < 0 )
			sprintf( szTemp2, "decreases your CPU by %d", -pMatchItem->GetCPUModifier() );
		else
			sprintf( szTemp2, "boosts CPU by %d", pMatchItem->GetCPUModifier() );
		
		strcat( szTemp, szTemp2 );	
		bPrintBoostMetrics = true;
	}

	if ( pMatchItem->GetMemoryModifier() )
	{
		if ( bPrintBoostMetrics )
			strcat( szTemp, ", " );
		
		if ( pMatchItem->GetMemoryModifier() < 0 )
			sprintf( szTemp2, "decreases your Memory by %d", -pMatchItem->GetMemoryModifier() );
		else
			sprintf( szTemp2, "boosts Memory by %d", pMatchItem->GetMemoryModifier() );

		strcat( szTemp, szTemp2 );	
		bPrintBoostMetrics = true;
	}

	if ( bPrintBoostMetrics )
		sEquipEffectString = szTemp;
	else
		sEquipEffectString = "has no effect";

	return (sEquipEffectString);
}

bool NWCPlayerEquipItemResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szFailString[1024];
	char szTemp[1024];

	// THIS player enters a room
	if ( !pCur->GetPlayer() )
	{
		pCur->Disconnect();
		return (false);
	}

	if ( dataLen < 18 )
	{
		pCur->Disconnect();
		return (false);
	}

	uint32_t playerID = *((uint32_t*)(pData));
	uint32_t itemID = *((uint32_t*)(pData+4));
	uint64_t itemKey = *((uint64_t*)(pData+8));
	uint8_t itemSlot = pData[16];
	uint8_t status = pData[17];

	if ( playerID != pCur->GetPlayer()->GetPlayerID() )
	{
		pCur->Disconnect();
		return (false);
	}

	if ( itemSlot >= EQUIP_SLOT_MAX )
	{
		pCur->Disconnect();
		return (false);
	}	

	// FIND ITEM::
	// Find item ID on player...
	CItemInstance *pMatchItem = pCur->GetPlayer()->GetItemFromInventoryByID( itemID );

        // Check for matching item!
        if ( pMatchItem == NULL )
	{
		pCur->Disconnect();	// OUT OF SYNC WITH SERVER
                return (false);
	}

	if ( pMatchItem->GetItemKey() != itemKey )
	{
		pCur->Disconnect();
		return (false);
	}

	if ( status == 0 )
	{
		// Handle failed equip attempt
		uint16_t failMessageLength;
	
		if ( dataLen < 20 )
		{
			pCur->Disconnect();
			return false;
		}
	
		failMessageLength = *((uint16_t*)(pData+18));
		if ( dataLen != 20+failMessageLength )
		{
			pCur->Disconnect();
			return (false);
		}

		if ( failMessageLength > 512 )
		{
			pCur->Disconnect();
			return (false);
		}
	
		memcpy( szFailString, pData+20, failMessageLength );

		snprintf( szTemp, 1024, "%sFailed to install item, reason: %s\n", FC_NORMAL_RED, szFailString );	
		pCur->WriteOutput( szTemp );
		return (true);	
	}

	// EQUIP item!
	if ( !pCur->GetPlayer()->EquipItem( pMatchItem, (tItemEquipType)itemSlot ) )
	{
		pCur->Disconnect();
		return (false);
	}

	string sEquipSlotName;
	CItemInstance::GetEquipStringFromType( (tItemEquipType)itemSlot, sEquipSlotName );
	
	// Print string equipping item
	snprintf( szTemp, 1024, "%sYou install %s into %s slot.\n", FC_NORMAL_YELLOW, pMatchItem->GetName().c_str(), sEquipSlotName.c_str() );
	pCur->WriteOutput( szTemp );

	snprintf( szTemp, 1024, "%sThis item %s.\n", FC_BRIGHT_CYAN, GetEquipItemEffectString( pMatchItem ).c_str() ); 
	pCur->WriteOutput( szTemp );

	return (true);
}

bool NWCPlayerUnequipItemResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[1024];

	// THIS player enters a room
	if ( !pCur->GetPlayer() )
	{
		pCur->Disconnect();
		return (false);
	}

	if ( dataLen < 18 )
	{
		pCur->Disconnect();
		return (false);
	}

	uint32_t playerID = *((uint32_t*)(pData));
	uint32_t itemID = *((uint32_t*)(pData+4));
	uint64_t itemKey = *((uint64_t*)(pData+8));
	uint8_t itemSlot = pData[16];
	uint8_t status = pData[17];

	if ( playerID != pCur->GetPlayer()->GetPlayerID() )
	{
		pCur->Disconnect();
		return (false);
	}

	if ( itemSlot >= EQUIP_SLOT_MAX )
	{
		pCur->Disconnect();
		return (false);
	}	

	// FIND ITEM::
	// Find item ID on player...
	CItemInstance *pMatchItem = pCur->GetPlayer()->GetInstallItem( (tItemEquipType)itemSlot );

        // Check for matching item!
        if ( pMatchItem == NULL )
	{
		pCur->Disconnect();	// OUT OF SYNC WITH SERVER
                return (false);
	}

	if ( pMatchItem->GetItemID() != itemID || pMatchItem->GetItemKey() != itemKey )
	{
		pCur->Disconnect();
		return (false);
	}

	if ( status == 0 )
	{
		// Handle failed equip attempt
		uint16_t failMessageLength;
		char *pTempData = NULL;
	
		if ( dataLen < 20 )
		{
			pCur->Disconnect();
			return false;
		}
	
		failMessageLength = *((uint16_t*)(pData+18));
		if ( dataLen != 20+failMessageLength )
		{
			pCur->Disconnect();
			return (false);
		}
	
		pTempData = new char[failMessageLength+1];
		memcpy( pTempData, pData+20, failMessageLength );
		pTempData[failMessageLength] = '\0';

		snprintf( szTemp, 1024, "%sFailed to uninstall item, reason: %s\n", FC_NORMAL_RED, pTempData );	
		pCur->WriteOutput( szTemp );

		delete pTempData;

		return (true);	
	}

	// UNEQUIP item!
	CItemInstance *pItemUnequipped;

	if ( !pCur->GetPlayer()->UnequipItem( (tItemEquipType)itemSlot, pItemUnequipped ) )
	{
		pCur->Disconnect();
		return (false);
	}

	string sEquipSlotName;
	if ( !CItemInstance::GetEquipStringFromType( (tItemEquipType)itemSlot, sEquipSlotName ) )
	{
		printf( "Critical error -- couldn't get install slot name.\n" );
		pCur->Disconnect();
		return (false);
	}

	snprintf( szTemp, 1024, "%sYou uninstall %s from %s slot.\n", FC_NORMAL_YELLOW, pMatchItem->GetName().c_str(), sEquipSlotName.c_str() ); 
	pCur->WriteOutput( szTemp );
	
	return (true);
}

bool NWCPlayerInfoMsgResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[1024];

	// Information message for this player
	if ( !pCur->GetPlayer() )
	{
		pCur->Disconnect();
		return (false);
	}

	if ( dataLen < 6 )
	{
		pCur->Disconnect();
		return (false);
	}

	uint32_t playerID = *((uint32_t*)(pData));
	uint16_t infoMessageLength = *((uint16_t*)(pData+4));

	if ( playerID != pCur->GetPlayer()->GetPlayerID() )
	{
		pCur->Disconnect();
		return (false);
	}

	if ( dataLen != 6+infoMessageLength )
	{
		pCur->Disconnect();
		return (false);
	}

	if ( infoMessageLength == 0 )
		return (true);

	char *pTempData = new char[infoMessageLength+1];
	memcpy( pTempData, pData+6, infoMessageLength );
	pTempData[infoMessageLength] = '\0';

	snprintf( szTemp, 1024, "%s%s.\n", FC_NORMAL_YELLOW, pTempData );
	pCur->WriteOutput( szTemp );
	
	delete pTempData;

	return (true); 
}

bool NWCPlayerGetItemResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[880];

	// Information message for this player
	if ( !pCur->GetPlayer() )
	{
		pCur->Disconnect();
		return (false);
	}

	CRoom *pRoom = pCur->GetPlayer()->GetRoom();

	if ( !pRoom )
	{
		pCur->Disconnect();
		return (false);
	}

	if ( dataLen < 8 )
		return (false);

	uint32_t playerID = *((uint32_t*)(pData));

	if ( playerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	CStreamData oStream;

	oStream.Write( pData+4, dataLen-4 );

	CItemInstance *pItem = new CItemInstance();

	if ( !pItem->ReadItemData( &oStream ) )
	{
		delete pItem;

		return (false);
	}

	// Add this item to the players inventory!
	if ( !pCur->GetPlayer()->AddItemToInventory( pItem ) )
	{
		delete pItem;

		return (false);
	}

	// Item successfully added -- remove from room
	if ( !pRoom->RemoveItemFromRoom( pItem->GetItemID() ) )
	{
		printf( "Critical error -- item wasn't in room to remove???\n" );
		return (false);
	}

	snprintf( szTemp, 880, "%sYou get a %s.\n", FC_NORMAL_YELLOW, pItem->GetName().c_str() );
	pCur->WriteOutput( szTemp );

	return (true);
}

bool NWCOtherPlayerGetItemResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[1200];

	// Information message for this player
	if ( !pCur->GetPlayer() )
	{
		pCur->Disconnect();
		return (false);
	}

	if ( dataLen != 12 )
		return (false);

	uint32_t currentPlayerID = *((uint32_t*)(pData));
	uint32_t otherPlayerID = *((uint32_t*)(pData+4));
	uint32_t itemID = *((uint32_t*)(pData+8));

	if ( currentPlayerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	CRoom *pRoom = pCur->GetPlayer()->GetRoom();

	if ( !pRoom )
		return (false);

	// Find player...
	CPlayerInRoom *pOtherPlayer;
	bool bFound = false;
	for ( pOtherPlayer = pRoom->GetPlayersInRoom()->Head(); pOtherPlayer; pOtherPlayer = pRoom->GetPlayersInRoom()->Next( pOtherPlayer ) )
	{
		if ( pOtherPlayer->GetID() == otherPlayerID )
		{
			bFound = true;
			break;
		}
	}

	if ( !bFound )
	{
		printf( "Critical error -- other player not found???\n" );
		return (false);	
	}

	// Now get item...
	CItemInRoom *pItemToGet;
	bFound = false;
	for ( pItemToGet = pRoom->GetItemsInRoom()->Head(); pItemToGet; pItemToGet = pRoom->GetItemsInRoom()->Next( pItemToGet ) )
	{
		if ( pItemToGet->GetID() == itemID ) 
		{
			bFound = true;
			break;
		}
	}

	if ( !bFound )
	{
		printf( "Critical error -- item not found???\n" );
		return (false);
	}

	snprintf( szTemp, 1200, "%s%s gets %s.\n", FC_NORMAL_YELLOW, pOtherPlayer->GetName().c_str(), pItemToGet->GetName().c_str() );	
	pCur->WriteOutput( szTemp );

	if ( !pRoom->RemoveItemFromRoom( itemID ) )
	{
		printf( "Critical error -- couldn't remove item from room???\n" );
		return (false);
	}
	
	return (true);
}

bool NWCPlayerDropItemResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[880];

	// Information message for this player
	if ( !pCur->GetPlayer() )
	{
		pCur->Disconnect();
		return (false);
	}

	CRoom *pRoom = pCur->GetPlayer()->GetRoom();

	if ( !pRoom )
	{
		pCur->Disconnect();
		return (false);
	}

	if ( dataLen != 8 )
		return (false);

	uint32_t playerID = *((uint32_t*)(pData));
	uint32_t itemID = *((uint32_t*)(pData+4));

	if ( playerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	CItemInstance *pItemToDrop = pCur->GetPlayer()->GetItemFromInventoryByID( itemID );

	if ( pItemToDrop == NULL )
	{
		printf( "Critical error -- item not in inventory to drop...\n" );

		return (false);
	}

	// Remove this item from the players inventory
	if ( !pCur->GetPlayer()->RemoveItemFromInventory( pItemToDrop ) )
	{
		printf( "Critical error -- can't remove item from inventory.\n" );

		return (false);
	}

	// Create new item in room entry!
	CItemInRoom *pNewItemInRoom = new CItemInRoom( pItemToDrop->GetName(), pItemToDrop->GetItemID() );

	delete pItemToDrop;

	pRoom->GetItemsInRoom()->InsertHead( pNewItemInRoom );

	// Print player!
	snprintf( szTemp, 880, "%sYou drop %s.\n", FC_NORMAL_YELLOW, pNewItemInRoom->GetName().c_str() );
	pCur->WriteOutput( szTemp );

	return (true);
}

bool NWCOtherPlayerDropItemResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[1200];

	// Information message for this player
	if ( !pCur->GetPlayer() )
	{
		pCur->Disconnect();
		return (false);
	}

	if ( dataLen < 14 )
		return (false);

	uint32_t currentPlayerID = *((uint32_t*)(pData));
	uint32_t otherPlayerID = *((uint32_t*)(pData+4));
	uint32_t itemID = *((uint32_t*)(pData+8));
	uint16_t itemNameLength = *((uint16_t*)(pData+12));

	if ( dataLen != itemNameLength+14 )
		return (false);

	if ( currentPlayerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	CRoom *pRoom = pCur->GetPlayer()->GetRoom();

	if ( !pRoom )
		return (false);

	// Find player...
	CPlayerInRoom *pOtherPlayer;
	bool bFound = false;
	for ( pOtherPlayer = pRoom->GetPlayersInRoom()->Head(); pOtherPlayer; pOtherPlayer = pRoom->GetPlayersInRoom()->Next( pOtherPlayer ) )
	{
		if ( pOtherPlayer->GetID() == otherPlayerID )
		{
			bFound = true;
			break;
		}
	}

	if ( !bFound )
	{
		printf( "Critical error -- other player not found???\n" );
		return (false);	
	}

	// Now create item in room -- and update this player with the information...
	char *pTempData = new char[itemNameLength+1];
	memcpy( pTempData, pData+14, itemNameLength );
	pTempData[itemNameLength] = '\0';
	
	string sItemName = pTempData;
	delete pTempData;

	CItemInRoom *pNewItemInRoom = new CItemInRoom( sItemName, itemID );

	pRoom->GetItemsInRoom()->InsertHead( pNewItemInRoom );

	// Inform this player of the item being dropped by the other player
	snprintf( szTemp, 1200, "%s%s drops a %s.\n", FC_NORMAL_YELLOW, pOtherPlayer->GetName().c_str(), sItemName.c_str() );
	pCur->WriteOutput( szTemp );

	return (true);
}


bool NWCNPCArrivesResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[1024];

	// Information message for this player
	if ( !pCur->GetPlayer() )
	{
		pCur->Disconnect();
		return (false);
	}

	CRoom *pRoom;
	if ( !(pRoom = pCur->GetPlayer()->GetRoom()) )
		return (false);

	if ( dataLen < 5 )
		return (false);

	uint32_t npcID = *((uint32_t*)(pData));
	uint8_t arrivalType = pData[4];
	uint8_t nameLength = pData[5];

	if ( dataLen != 6+nameLength )
		return (false);

	char *pTempData = new char[nameLength+1];
	memcpy( pTempData, pData+6, nameLength );
	pTempData[nameLength] = '\0';

	string sNPCName = pTempData;
	delete pTempData;

	if ( arrivalType == NWC_NPC_ARRIVES_ENTERROOM )
		snprintf( szTemp, 1024, "%s%s arrives.\n", FC_NORMAL_YELLOW, sNPCName.c_str() );
	else if ( arrivalType == NWC_NPC_ARRIVES_SPAWN )
		snprintf( szTemp, 1024, "%s%s appears.\n", FC_NORMAL_YELLOW, sNPCName.c_str() );
	
	pCur->WriteOutput( szTemp );
	
	NUtil::LIST_PTR( CNPCInRoom ) pNPCsInRoomList = pRoom->GetNPCsInRoom();

	bool bAdd = true;
	for ( CNPCInRoom *pNPCInRoom = pNPCsInRoomList->Head(); pNPCInRoom; pNPCInRoom = pNPCsInRoomList->Next( pNPCInRoom ) )
	{
		if ( pNPCInRoom->GetID() == npcID )
		{
			// Already in list -- don't add (strange???)
			bAdd = false;
			break;
		}
	}

	if ( bAdd )
		pNPCsInRoomList->InsertHead( new CNPCInRoom( sNPCName, npcID ) );

	return (true);
}


bool NWCNPCLeavesResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[1024];

	if ( !pCur )
		return (false);

	if ( !pCur->GetPlayer() )
		return (false);

	CRoom *pRoom;
	
	if ( !(pRoom = pCur->GetPlayer()->GetRoom()) )
		return (false);

	if ( dataLen != 6 )
		return (false);

	uint32_t npcID = *((uint32_t*)(pData));
	uint8_t leaveType = pData[4];
	uint8_t leaveDirection = pData[5];

	char szDirection[256];
	switch ( leaveDirection )
	{
	case DIRECTION_NORTH:
		strcpy( szDirection, "north" );
		break;

	case DIRECTION_EAST:
		strcpy( szDirection, "east" );
		break;
	
	case DIRECTION_WEST:
		strcpy( szDirection, "west" );
		break;

	case DIRECTION_SOUTH:
		strcpy( szDirection, "south" );
		break;
	
	case DIRECTION_UP:
		strcpy( szDirection, "up" );
		break;

	case DIRECTION_DOWN:
		strcpy( szDirection, "down" );
		break;

	default:
		szDirection[0] = '\0';
		break;
	}

	// Find NPC in room and remove it
	NUtil::LIST_PTR( CNPCInRoom ) pNPCsInRoomList = pRoom->GetNPCsInRoom();

	string sNPCName;

	bool bRemoved = false;
	for ( CNPCInRoom *pNPCInRoom = pNPCsInRoomList->Head(); pNPCInRoom; pNPCInRoom = pNPCsInRoomList->Next( pNPCInRoom ) )
	{
		if ( pNPCInRoom->GetID() == npcID )
		{
			sNPCName = pNPCInRoom->GetName();

			delete pNPCInRoom;
			bRemoved = true;
			break;
		}
	}

	if ( !bRemoved )
		return (false);

	// OK inform user that a player has arrived in the room...
	if ( leaveType == NWC_NPC_LEAVES_KILLED )
		snprintf( szTemp, 1024, "%s%s is killed!!!\n", FC_NORMAL_RED, sNPCName.c_str() );
	else if ( leaveType == NWC_NPC_LEAVES_EXITROOM )
		snprintf( szTemp, 1024, "%s%s%s leaves %s.\n", FC_NORMAL_RED, sNPCName.c_str(), FC_NORMAL_YELLOW, szDirection );
	else if ( leaveType == NWC_NPC_LEAVES_DISAPPEAR )
		snprintf( szTemp, 1024, "%s%s%s disappears.\n", FC_NORMAL_RED, sNPCName.c_str(), FC_NORMAL_YELLOW );

	pCur->WriteOutput( szTemp );
	
	return (true);
}

bool NWCPlayerStatUpdateResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	if ( !pCur )
		return (false);

	CPlayer *pPlayer;
	if ( !(pPlayer = pCur->GetPlayer()) )
		return (false);

	if ( dataLen != 16 )
		return (false);

	uint32_t playerID = *((uint32_t*)(pData));
	uint16_t curHP = *((uint16_t*)(pData+4));
	uint16_t curCPU = *((uint16_t*)(pData+6));
	uint16_t curMemory = *((uint16_t*)(pData+8));
	
	uint16_t maxHP = *((uint16_t*)(pData+10));
	uint16_t maxCPU = *((uint16_t*)(pData+12));
	uint16_t maxMemory = *((uint16_t*)(pData+14));

	if ( playerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	// Check stats
	if ( maxHP != pPlayer->GetMaxHP() )
	{
		printf( "HP mismatch in stat update!\n" );
		pCur->Disconnect();
		return (false);
	}

	if ( maxCPU != pPlayer->GetMaxCPU() )
	{
		printf( "CPU mismatch in stat update.\n" );
		pCur->Disconnect();
		return (false);
	}

	if ( maxMemory != pPlayer->GetMaxMemory() )
	{
		printf( "Memory mismatch in stat update.\n" );
		pCur->Disconnect();
		return (false);
	}

	// Update stats
	if ( curHP > maxHP || curCPU > maxCPU || curMemory > maxMemory )
	{
		printf( "Current/MAX mismatch in stat update (%d/%d | %d/%d | %d/%d).\n", curHP, maxHP, curCPU, maxCPU, curMemory, maxMemory );
		pCur->Disconnect();
		return (false);
	}

	pPlayer->SetHP( curHP );
	pPlayer->SetCPU( curCPU );
	pPlayer->SetMemory( curMemory );

	return (true);
}

bool NWCPlayerAttackResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[900];

	if ( !pCur )
		return (false);

	CPlayer *pPlayer;
	if ( !(pPlayer = pCur->GetPlayer()) )
		return (false);

	if ( dataLen != 9 )
		return (false);

	uint32_t playerID = *((uint32_t*)(pData));
	uint32_t targetID = *((uint32_t*)(pData+4));
	uint8_t targetType = pData[8];

	if ( playerID != pPlayer->GetPlayerID() )
		return (false);

	// Response to (I am attacking something).
	if ( targetType == NWC_ATTACK_PLAYER )
	{
		if ( targetID == pCur->GetPlayer()->GetPlayerID() )
			return (false); // You can't attack yourself.
		
		CPlayerInRoom *pPlayerAttacked = NULL;
		if ( !pPlayer->AttackPlayer( targetID, pPlayerAttacked ) )
			return (false);

		if ( !pPlayerAttacked )
			return (false);
	
		snprintf( szTemp, 900, "%sYou attack %s!\n", FC_NORMAL_RED, pPlayerAttacked->GetName().c_str() );
		pCur->WriteOutput( szTemp );

		return (true);
	}
	else if ( targetType == NWC_ATTACK_NPC )
	{
		CNPCInRoom *pNPCAttacked = NULL;

		if ( !pPlayer->AttackNPC( targetID, pNPCAttacked ) )
			return (false);

		if ( !pNPCAttacked )
			return (false);

		snprintf( szTemp, 900, "%sYou attack %s!\n", FC_NORMAL_RED, pNPCAttacked->GetName().c_str() );
		pCur->WriteOutput( szTemp );

		return (true);
	}
	else
		return (false);
		

	return (true);
}

bool NWCPlayerTakesDamageResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[1400];

	if ( !pCur )
		return (false);

	CPlayer *pPlayer;
	if ( !(pPlayer = pCur->GetPlayer()) )
		return (false);

	CRoom *pRoom = pPlayer->GetRoom();
	
	if ( !pRoom )
		return (false);

	if ( dataLen < 12 )
		return (false);

	uint32_t playerID = *((uint32_t*)(pData));
	uint32_t sourceID = *((uint32_t*)(pData+4));
	uint8_t sourceType = pData[8];
	uint16_t damageAmount = *((uint16_t*)(pData+9));

	if ( playerID != pPlayer->GetPlayerID() )
		return (false);

	// Get strings
	string sDamageString;
	string sDamageSource;

	uint8_t damageStringLength;
	uint8_t damageSourceLength;

	damageStringLength = pData[11];
	
	if ( dataLen < 12+damageStringLength+1 )
		return (false);

	char *pTempData = new char[damageStringLength+1];
	memcpy( pTempData, pData+12, damageStringLength );
	pTempData[damageStringLength] = '\0';
	
	sDamageString = pTempData;
	delete pTempData;

	damageSourceLength = pData[12+damageStringLength];
	
	if ( dataLen < 12+damageStringLength+1+damageSourceLength )
		return (false);

	pTempData = new char[damageSourceLength+1];
	memcpy( pTempData, pData+12+damageStringLength+1, damageSourceLength );
	pTempData[damageSourceLength] = '\0';

	sDamageSource = pTempData;
	delete pTempData;


	if ( sourceType == DAMAGE_TYPE_PLAYER )
	{
		if ( sourceID == pCur->GetPlayer()->GetPlayerID() )
			return (false); // You can't damage yourself.
		
		CPlayerInRoom *pTarget = pRoom->GetPlayerInRoomByID( sourceID );

		if ( !pTarget )
			return (false);

		snprintf( szTemp, 1400, "%s%s's %s %s you!\n", FC_NORMAL_RED, pTarget->GetName().c_str(), sDamageSource.c_str(), sDamageString.c_str() );
	}
	else if ( sourceType == DAMAGE_TYPE_NPC )
	{
		CNPCInRoom *pTarget = pRoom->GetNPCInRoomByID( sourceID );

		if ( !pTarget )
			return (false);
	
		snprintf( szTemp, 1400, "%s%s's %s %s you!\n", FC_NORMAL_RED, pTarget->GetName().c_str(), sDamageSource.c_str(), sDamageString.c_str() );
	}
	else if ( sourceType == DAMAGE_TYPE_WORLD )
	{
		snprintf( szTemp, 1400, "%s%s %s you!\n", FC_NORMAL_RED, sDamageSource.c_str(), sDamageString.c_str() );
	}

	pCur->WriteOutput( szTemp );

	if ( pPlayer->GetCurHP() < damageAmount )
		; // Do nothing -- we will be getting a death message
	else
		pPlayer->SetHP( pPlayer->GetCurHP() - damageAmount );

	return (true);
}

bool NWCPlayerDamageResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[1400];

	if ( !pCur )
		return (false);

	CPlayer *pPlayer;
	if ( !(pPlayer = pCur->GetPlayer()) )
		return (false);

	CRoom *pRoom = pPlayer->GetRoom();
	
	if ( !pRoom )
		return (false);

	if ( dataLen < 12 )
		return (false);

	uint32_t playerID = *((uint32_t*)(pData));
	uint32_t targetID = *((uint32_t*)(pData+4));
	uint8_t targetType = pData[8];
	uint16_t damageAmount = *((uint16_t*)(pData+9));

	if ( playerID != pPlayer->GetPlayerID() )
		return (false);

	// Get strings
	string sDamageString;
	string sDamageSource;

	uint8_t damageStringLength;
	uint8_t damageSourceLength;

	damageStringLength = pData[11];
	
	if ( dataLen < 12+damageStringLength+1 )
		return (false);

	char *pTempData = new char[damageStringLength+1];
	memcpy( pTempData, pData+12, damageStringLength );
	pTempData[damageStringLength] = '\0';
	
	sDamageString = pTempData;
	delete pTempData;

	damageSourceLength = pData[12+damageStringLength];
	
	if ( dataLen < 12+damageStringLength+1+damageSourceLength )
		return (false);

	pTempData = new char[damageSourceLength+1];
	memcpy( pTempData, pData+12+damageStringLength+1, damageSourceLength );
	pTempData[damageSourceLength] = '\0';

	sDamageSource = pTempData;
	delete pTempData;

	if ( targetType == NWC_ATTACK_PLAYER )
	{
		if ( targetID == pCur->GetPlayer()->GetPlayerID() )
			return (false); // You can't damage yourself.
		
		CPlayerInRoom *pTarget = pRoom->GetPlayerInRoomByID( targetID );

		if ( !pTarget )
			return (false);
	
		snprintf( szTemp, 1400, "%sYour %s %s %s!\n", FC_NORMAL_RED, sDamageSource.c_str(), sDamageString.c_str(), pTarget->GetName().c_str() );

		pCur->WriteOutput( szTemp );

		return (true);
	}
	else if ( targetType == NWC_ATTACK_NPC )
	{
		CNPCInRoom *pTarget = pRoom->GetNPCInRoomByID( targetID );

		if ( !pTarget )
			return (false);

		snprintf( szTemp, 1400, "%sYour %s %s %s!\n", FC_NORMAL_RED, sDamageSource.c_str(), sDamageString.c_str(), pTarget->GetName().c_str() );
		
		pCur->WriteOutput( szTemp );

		return (true);
	}
	else
		return (false);

	return (true);
}

bool NWCPlayerKilledResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[1600];

	// Information message for this player
	if ( !pCur->GetPlayer() )
	{
		pCur->Disconnect();
		return (false);
	}

	if ( dataLen < 15 )
		return (false);

	uint32_t currentPlayerID = *((uint32_t*)(pData));
	uint32_t killedID = *((uint32_t*)(pData+4));
	uint8_t killedType = pData[8];
	uint32_t killerID = *((uint32_t*)(pData+9));
	uint8_t killerType = pData[13];
	uint8_t killerNameLength = pData[14];

	if ( dataLen != killerNameLength+15 )
		return (false);

	if ( currentPlayerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	CRoom *pRoom = pCur->GetPlayer()->GetRoom();
	
	if ( !pRoom )
		return (false);

	char *pTempData = new char[killerNameLength+1];
	memcpy( pTempData, pData+15, killerNameLength );
	pTempData[killerNameLength] = '\0';
	
	string sKillerName = pTempData;
	delete pTempData;

	if ( killedType == NWC_ATTACK_PLAYER )
	{
		CPlayerInRoom *pPlayer = pRoom->GetPlayerInRoomByID( killedID );

		if ( !pPlayer )
			return (false);

		if ( killerID == currentPlayerID )
			snprintf( szTemp, 1500, "%sYou terminate player %s!\n", FC_NORMAL_RED, pPlayer->GetName().c_str() );
		else

			snprintf( szTemp, 1500, "%s%s terminates player %s!\n", FC_NORMAL_RED, sKillerName.c_str(), pPlayer->GetName().c_str() );
		pCur->WriteOutput( szTemp );

		if ( pCur->GetPlayer()->IsFightingTarget( killedID, NWC_ATTACK_PLAYER ) )
			pCur->GetPlayer()->StopFightingTarget( killedID, NWC_ATTACK_PLAYER );

		delete pPlayer;
		return (true);
	}
	else if ( killedType == NWC_ATTACK_NPC )
	{
		CNPCInRoom *pNPC = pRoom->GetNPCInRoomByID( killedID );
	
		if ( !pNPC )
			return (false);

		if ( killerID == currentPlayerID )
			snprintf( szTemp, 1500, "%sYou terminate %s!\n", FC_NORMAL_RED, pNPC->GetName().c_str() );
		else
			snprintf( szTemp, 1500, "%s%s terminates %s!\n", FC_NORMAL_RED, sKillerName.c_str(), pNPC->GetName().c_str() );
		pCur->WriteOutput( szTemp );
		
		if ( pCur->GetPlayer()->IsFightingTarget( killedID, NWC_ATTACK_NPC ) )
			pCur->GetPlayer()->StopFightingTarget( killedID, NWC_ATTACK_NPC );
		
		delete pNPC;
		return (true); 
	}
	else
		return (false);

	return (true);
}

bool NWCItemToRoomResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	// Information message for this player
	if ( !pCur->GetPlayer() )
	{
		pCur->Disconnect();
		return (false);
	}

	if ( dataLen < 9 )
		return (false);

	uint32_t currentPlayerID = *((uint32_t*)(pData));
	uint32_t itemID = *((uint32_t*)(pData+4));
	uint8_t itemNameLength = pData[8];

	if ( dataLen != itemNameLength+9 )
		return (false);

	if ( currentPlayerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	CRoom *pRoom = pCur->GetPlayer()->GetRoom();

	if ( !pRoom )
		return (false);

	char *pTempData = new char[itemNameLength+1];
	memcpy( pTempData, pData+9, itemNameLength );
	pTempData[itemNameLength] = '\0';
	
	string sItemName = pTempData;
	delete pTempData;

	CItemInRoom *pNewItemInRoom = new CItemInRoom( sItemName, itemID );

	pRoom->GetItemsInRoom()->InsertHead( pNewItemInRoom );

	return (true);
}

bool NWCPlayerDeathResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[800];

	// Information message for this player
	if ( !pCur->GetPlayer() )
	{
		pCur->Disconnect();
		return (false);
	}

	if ( dataLen <= 4 )
		return (false);

	uint32_t currentPlayerID = *((uint32_t*)(pData));

	if ( currentPlayerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);
	
	CStreamData oStream;
	oStream.Write( pData+4, dataLen-4 );

	if ( !pCur->GetPlayer()->ReadPlayerData( &oStream ) )
	{
		pCur->Disconnect();
		return (false);
	}

	snprintf( szTemp, 800, "\n\n%s=============================\nYOU HAVE BEEN KILLED!!!\n%sYou respawn back at a spawn point, you notice you may have lost some items!\n", FC_NORMAL_RED, FC_NORMAL_YELLOW );
	pCur->WriteOutput( szTemp );

	return true;
}

bool NWCPlayerLevelResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	// Information message for this player
	if ( !pCur->GetPlayer() )
	{
		pCur->Disconnect();
		return (false);
	}

	CPlayer *pPlayer = pCur->GetPlayer();

	if ( dataLen != 10 ) 
		return (false);

	uint32_t currentPlayerID = *((uint32_t*)(pData));

	if ( currentPlayerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);
	
	uint8_t hackLevelGain = pData[4];
	uint8_t codeLevelGain = pData[5];
	uint8_t scriptLevelGain = pData[6];
	
	uint8_t newHackLevel = pData[7];
	uint8_t newCodeLevel = pData[8];
	uint8_t newScriptLevel = pData[9];

	char szTemp[1024] = "";

	if ( hackLevelGain > 0 )
		strcat( szTemp, "hacking " );
	
	if ( codeLevelGain > 0 )
		strcat( szTemp, "coding " );

	if ( scriptLevelGain > 0 )
		strcat( szTemp, "scripting " );

	if ( pPlayer->GetHackLevel() + hackLevelGain != newHackLevel )
	{
		printf( "Critical Error: Server mismatch? New hack level does not match!\n" );
		pCur->Disconnect();
		return (false);
	}

	if ( pPlayer->GetCodeLevel() + codeLevelGain != newCodeLevel )
	{
		printf( "Critical Error: Server mismatch? New code level does not match!\n" );
		pCur->Disconnect();
		return (false);
	}

	if ( pPlayer->GetScriptLevel() + scriptLevelGain != newScriptLevel )
	{
		printf( "Critical Error: Server mismatch? New script level does not match!\n" );
		pCur->Disconnect();
		return (false);
	}	

	pPlayer->SetHackLevel( newHackLevel );
	pPlayer->SetCodeLevel( newCodeLevel );
	pPlayer->SetScriptLevel( newScriptLevel );

	if ( hackLevelGain == 0 && codeLevelGain == 0 && scriptLevelGain == 0 )
	{
		strcpy( szTemp, "" );

		if ( pPlayer->GetHackLevel() == PLAYER_MAX_HACK_LEVEL )
			strcat( szTemp, "hack " );
		
		if ( pPlayer->GetCodeLevel() == PLAYER_MAX_CODE_LEVEL )
			strcat( szTemp, "code " );
	
		if ( pPlayer->GetScriptLevel() == PLAYER_MAX_SCRIPT_LEVEL )
			strcat( szTemp, "script " );
		
		char szMaxMessage[512];	
		sprintf( szMaxMessage, "%sYou cannot gain anymore experience as you are at your maximum %s\n", FC_BRIGHT_CYAN );
		pCur->WriteOutput( szMaxMessage );
		return (true);
	}
	
	char szLevelMessage[1024];	
	snprintf( szLevelMessage, 1024, "%sYou gain %s experience!\n", FC_BRIGHT_CYAN, szTemp );

	pCur->WriteOutput( szLevelMessage );
	
	return (true);
}

bool NWCPlayerDeletesItemResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[1024];

	// Information message for this player
	if ( !pCur->GetPlayer() )
	{
		pCur->Disconnect();
		return (false);
	}

	CPlayer *pPlayer = pCur->GetPlayer();

	if ( dataLen != 12 ) 
		return (false);

	uint32_t currentPlayerID = *((uint32_t*)(pData));
	uint32_t deletePlayerID = *((uint32_t*)(pData+4));
	uint32_t itemID = *((uint32_t*)(pData+8));

	if ( currentPlayerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	CRoom *pRoom = pPlayer->GetRoom();

	if ( !pRoom )
		return (false);	

	CItemInRoom *pItemToDelete = pRoom->GetItemInRoomByID( itemID );

	if ( pItemToDelete == NULL )
		return (true); // Item gone -- don't delete

	string sItemName = pItemToDelete->GetName();

	delete pItemToDelete;

	if ( deletePlayerID == currentPlayerID )
	{
		snprintf( szTemp, 1024, "%sYou delete item %s.\n", FC_NORMAL_YELLOW, sItemName.c_str() );
		pCur->WriteOutput( szTemp );
		
		return (true);
	}
	else
	{
		CPlayerInRoom *pOtherPlayer = pRoom->GetPlayerInRoomByID( deletePlayerID );

		if ( pOtherPlayer == NULL )
		{
			snprintf( szTemp, 1024, "%s%s is deleted!\n", FC_NORMAL_YELLOW, sItemName.c_str() );
			pCur->WriteOutput( szTemp );
			return (true);	// Player not here anymore
		}

		snprintf( szTemp, 1024, "%s%s deletes item %s.\n", FC_NORMAL_YELLOW, pOtherPlayer->GetName().c_str(), sItemName.c_str() );
		pCur->WriteOutput( szTemp );
	}

	return (true);	
}

bool NWCPlayerRunsScriptResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[768];

	// Player runs a script
	if ( !pCur->GetPlayer() )
	{
		pCur->Disconnect();
		return (false);
	}

	CPlayer *pPlayer = pCur->GetPlayer();

	if ( dataLen < 10 ) 
		return (false);

	uint32_t currentPlayerID = *((uint32_t*)(pData));
	uint8_t scriptNum = pData[4];
	uint16_t cpuUsed = *((uint16_t*)(pData+5));
	uint16_t memoryUsed = *((uint16_t*)(pData+7));
	uint8_t displayTextLength = pData[9];

	if ( currentPlayerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	if ( dataLen != displayTextLength+10 )
		return (false);

	char *pTempData = new char[displayTextLength+1];
	memcpy( pTempData, pData+10, displayTextLength );
	pTempData[displayTextLength] = '\0';

	string sDisplayText = pTempData;
	delete pTempData;

	if ( cpuUsed > pPlayer->GetCurCPU() )
		return (false);

	if ( memoryUsed > pPlayer->GetCurMemory() )
		return (false);

	pPlayer->SetCPU( pPlayer->GetCurCPU() - cpuUsed );
	pPlayer->SetMemory( pPlayer->GetCurMemory() - memoryUsed );

	snprintf( szTemp, 768, "%s%s costing you %d CPU and %d Memory.\n", FC_BRIGHT_CYAN, sDisplayText.c_str(), cpuUsed, memoryUsed );
	pCur->WriteOutput( szTemp );
	
	return (true);
}

bool NWCPlayerFingerResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	// Player runs a script
	if ( !pCur->GetPlayer() )
	{
		pCur->Disconnect();
		return (false);
	}

	CPlayer *pPlayer = pCur->GetPlayer();

	CRoom *pRoom = pPlayer->GetRoom();

	if ( !pRoom )
		return (false);

	if ( dataLen < 21 ) 
		return (false);

	uint32_t currentPlayerID = *((uint32_t*)(pData));
	uint32_t consumeItemID = *((uint32_t*)(pData+4));
	uint32_t targetID = *((uint32_t*)(pData+8));
	uint8_t targetType = pData[12];
	uint8_t targetHackLevel = pData[13];
	uint8_t targetCodeLevel = pData[14];
	uint8_t targetScriptLevel = pData[15];
	uint16_t targetCurHP = *((uint16_t*)(pData+16));
	uint16_t targetMaxHP = *((uint16_t*)(pData+18));
	uint8_t targetFingerprintLen = pData[20];

	if ( currentPlayerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	if ( dataLen != targetFingerprintLen+21 )
		return (false);

	string sTargetName;
	if ( targetType == NWC_PLAYER_FINGER_NPC )
	{
		CNPCInRoom *pNPC = pRoom->GetNPCInRoomByID( targetID );

		if ( !pNPC )
			return (false);

		sTargetName = pNPC->GetName();
	}
	else if ( targetType == NWC_PLAYER_FINGER_PLAYER )
	{
		CPlayerInRoom *pPlayer = pRoom->GetPlayerInRoomByID( targetID );

		if ( !pPlayer )
			return (false);

		sTargetName = pPlayer->GetName(); 
	}
	else
		return (false);

	CItemInstance *pRequestItem = pPlayer->GetItemFromInventoryByID( consumeItemID );

	if ( !pRequestItem )
		return (false);

	// Consume the finger print request item
	if ( !pPlayer->RemoveItemFromInventory( pRequestItem ) )
		return (false);

	// Delete the item for good (it is consumed)
	delete pRequestItem;
	
	char szResponseMessage[256] = "";
	char szFinalMessage[384] = "";
	
	if ( targetHackLevel > (pPlayer->GetHackLevel()+2) )
		strcat( szResponseMessage, "better hacker, " );
	else if ( pPlayer->GetHackLevel() > 2 && targetHackLevel < (pPlayer->GetHackLevel()-2) )
		strcat( szResponseMessage, "worse hacker, " );
	else
		strcat( szResponseMessage, "similar hacker, " );

	
	if ( targetCodeLevel > (pPlayer->GetCodeLevel()+2) )
		strcat( szResponseMessage, "better coder, " );
	else if ( pPlayer->GetCodeLevel() > 2 && targetCodeLevel < (pPlayer->GetCodeLevel()-2) )
		strcat( szResponseMessage, "worse coder, " );
	else
		strcat( szResponseMessage, "similar coder, " );
	
	if ( targetScriptLevel > (pPlayer->GetScriptLevel()+2) )
		strcat( szResponseMessage, "better scripter " );
	else if ( pPlayer->GetScriptLevel() > 2 && targetScriptLevel < (pPlayer->GetScriptLevel()-2) )
		strcat( szResponseMessage, "worse scripter " );
	else
		strcat( szResponseMessage, "similar script writing " );

	if ( targetMaxHP > (pPlayer->GetMaxHP()+1000) )
		strcat( szResponseMessage, "and you are a fly to them" );
	else if ( targetMaxHP > (pPlayer->GetMaxHP()+500) )
		strcat( szResponseMessage, "and considerably stronger than you" );
	else if ( targetMaxHP > (pPlayer->GetMaxHP()+200) )
		strcat( szResponseMessage, "and significantly stronger than you" );
	else if ( targetMaxHP > (pPlayer->GetMaxHP()+100) )
		strcat( szResponseMessage, "and a lot stronger than you" );	
	else if ( targetMaxHP > (pPlayer->GetMaxHP()+50) )
		strcat( szResponseMessage, "and a bit stronger than you" );
	else if ( targetMaxHP > (pPlayer->GetMaxHP()+10) )
		strcat( szResponseMessage, "and slightly stronger than you" );
	else if ( pPlayer->GetMaxHP() > 10 && targetMaxHP < (pPlayer->GetMaxHP()-10) )
		strcat( szResponseMessage, "and slightly weaker than you" );
	else if ( pPlayer->GetMaxHP() > 50 && targetMaxHP < (pPlayer->GetMaxHP()-50) )
		strcat( szResponseMessage, "and a bit weaker than you" );
	else if ( pPlayer->GetMaxHP() > 100 && targetMaxHP < (pPlayer->GetMaxHP()-100) )
		strcat( szResponseMessage, "and a lot weaker than you" );
	else if ( pPlayer->GetMaxHP() > 200 && targetMaxHP < (pPlayer->GetMaxHP()-200) )
		strcat( szResponseMessage, "and significantly weaker than you" );
	else if ( pPlayer->GetMaxHP() > 500 && targetMaxHP < (pPlayer->GetMaxHP()-500) )
		strcat( szResponseMessage, "and considerably weaker than you" );
	else
		strcat( szResponseMessage, "and about as strong as you" );	

	char szFingerprintString[256] = "";
	if ( targetID == currentPlayerID && targetType == NWC_PLAYER_FINGER_PLAYER )
		pPlayer->GetItemFingerprintString( szFingerprintString, 256 );	
	else
	{
		uint32_t outPos = 0;
		for ( uint32_t i = 0; i < targetFingerprintLen; i++ )
		{
			szFingerprintString[outPos++] = ToHexChar( (pData[21+i] >> 4) & 0xF );
			szFingerprintString[outPos++] = ToHexChar( (pData[21+i] & 0xF) ); 
		}
	}
		
	sprintf( szFinalMessage, "%sYour finger request returns informing you that %s is a %s.\n%s's fingerprint is: %s\n", FC_BRIGHT_CYAN, sTargetName.c_str(), szResponseMessage, sTargetName.c_str(), szFingerprintString );

	pCur->WriteOutput( szFinalMessage );

	return (true);	
}

bool NWCItemInRoomDeletedResponse( CServerConnection *pCur, uint8_t *pData, uint32_t dataLen )
{
	char szTemp[768];

	// Player runs a script
	if ( !pCur->GetPlayer() )
	{
		pCur->Disconnect();
		return (false);
	}

	CPlayer *pPlayer = pCur->GetPlayer();

	CRoom *pRoom = pPlayer->GetRoom();

	if ( !pRoom )
		return (false);

	if ( dataLen < 9 ) 
		return (false);

	uint32_t currentPlayerID = *((uint32_t*)(pData));
	uint32_t itemID = *((uint32_t*)(pData+4));
	uint8_t displayTextLength = pData[8];

	if ( currentPlayerID != pCur->GetPlayer()->GetPlayerID() )
		return (false);

	if ( dataLen != displayTextLength+9 )
		return (false);

	char *pTempData = new char[displayTextLength+1];
	memcpy( pTempData, pData+9, displayTextLength );
	pTempData[displayTextLength] = '\0';

	string sDisplayText = pTempData;
	delete pTempData;
	
	CItemInRoom *pItemToDelete = pRoom->GetItemInRoomByID( itemID );

	if ( pItemToDelete == NULL )
		return (true); // Item gone -- don't delete

	string sItemName = pItemToDelete->GetName();

	delete pItemToDelete;

	sprintf( szTemp, "%s%s %s\n", FC_BRIGHT_CYAN, sItemName.c_str(), sDisplayText.c_str() );
	pCur->WriteOutput( szTemp );
	
	return (true);
}


typedef bool (*tNWCCmdFptr)( CServerConnection *, uint8_t *, uint32_t dataLen );

tNWCCmdFptr g_cmdTable[NWC_MAX_RESPONSES] =
{
	NWCAuthChallenge,
	NWCAuthResponse,
	NWCPlayerSaysResponse,
	NWCPlayerArrivesResponse,
	NWCPlayerLeavesResponse,
	NWCPlayerWallResponse,
	NWCPlayerEnterroomResponse,
	NWCPlayerEquipItemResponse,
	NWCPlayerUnequipItemResponse,
	NWCPlayerInfoMsgResponse,
	NWCPlayerGetItemResponse,
	NWCOtherPlayerGetItemResponse,
	NWCPlayerDropItemResponse,
	NWCOtherPlayerDropItemResponse,
	NWCNPCLeavesResponse,
	NWCNPCArrivesResponse,
	NWCPlayerStatUpdateResponse,
	NWCPlayerAttackResponse,
	NWCPlayerDamageResponse,
	NWCPlayerTakesDamageResponse,
	NWCPlayerKilledResponse,
	NWCItemToRoomResponse,
	NWCPlayerDeathResponse,
	NWCPlayerLevelResponse,
	NWCPlayerDeletesItemResponse,
	NWCPlayerRunsScriptResponse,
	NWCPlayerFingerResponse,
	NWCItemInRoomDeletedResponse,
	NWCCreatePlayerResponse,
};

bool CNetworkCommands::DoNetworkCommand( CServerConnection *pCur, uint8_t cmdNumber, uint8_t *pData, uint32_t dataLen )
{
	if ( cmdNumber >= NWC_MAX_RESPONSES )
		return (false);

	// Run command
	return (*g_cmdTable[cmdNumber])( pCur, pData, dataLen );
}
