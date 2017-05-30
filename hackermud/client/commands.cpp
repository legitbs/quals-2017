#include "common.h"

void DoNorth( CServerConnection *pCur, vector<string> &sArgs )
{
	char szBuffer[512];

	// NORTH
	if ( !pCur || !pCur->GetPlayer() || !pCur->GetPlayer()->GetRoom() )
		return;
	
	CPlayer *pPlayer = pCur->GetPlayer();	
	if ( pPlayer->IsFighting() )
	{
		sprintf( szBuffer, "%sYou can't do that while fighting.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szBuffer ); 
		return;
	}

	if ( pCur->GetPlayer()->GetRoom()->GetNorth().GetVnum() == NO_ROOM_VNUM )
	{
		sprintf( szBuffer, "%sCan't go that way.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szBuffer ); 
		return;
	}

	if ( !CanMoveCPUCheck( pCur->GetPlayer() ) )
	{
		sprintf( szBuffer, "%sInsufficient CPU to move!\n", FC_NORMAL_RED );
		pCur->WriteOutput( szBuffer ); 
		return;
	}


	NWCDirectionRequest( pCur, DIRECTION_NORTH );
}

void DoSouth( CServerConnection *pCur, vector<string> &sArgs )
{
	char szBuffer[512];

	// SOUTH
	if ( !pCur || !pCur->GetPlayer() || !pCur->GetPlayer()->GetRoom() )
		return;

	CPlayer *pPlayer = pCur->GetPlayer();	
	if ( pPlayer->IsFighting() )
	{
		sprintf( szBuffer, "%sYou can't do that while fighting.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szBuffer ); 
		return;
	}

	if ( pCur->GetPlayer()->GetRoom()->GetSouth().GetVnum() == NO_ROOM_VNUM )
	{
		sprintf( szBuffer, "%sCan't go that way.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szBuffer ); 
		return;
	}

	if ( !CanMoveCPUCheck( pCur->GetPlayer() ) )
	{
		sprintf( szBuffer, "%sInsufficient CPU to move!\n", FC_NORMAL_RED );
		pCur->WriteOutput( szBuffer ); 
		return;
	}
 
	NWCDirectionRequest( pCur, DIRECTION_SOUTH );
}

void DoEast( CServerConnection *pCur, vector<string> &sArgs )
{
	char szBuffer[512];
	// EAST
	if ( !pCur || !pCur->GetPlayer() || !pCur->GetPlayer()->GetRoom() )
		return;

	CPlayer *pPlayer = pCur->GetPlayer();	
	if ( pPlayer->IsFighting() )
	{
		sprintf( szBuffer, "%sYou can't do that while fighting.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szBuffer );
		return;
	}

	if ( pCur->GetPlayer()->GetRoom()->GetEast().GetVnum() == NO_ROOM_VNUM )
	{
		sprintf( szBuffer, "%sCan't go that way.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szBuffer );
		return;
	}

	if ( !CanMoveCPUCheck( pCur->GetPlayer() ) )
	{
		sprintf( szBuffer, "%sInsufficient CPU to move!\n", FC_NORMAL_RED );
		pCur->WriteOutput( szBuffer );
		return;
	}
 
	NWCDirectionRequest( pCur, DIRECTION_EAST );
}

void DoWest( CServerConnection *pCur, vector<string> &sArgs )
{
	char szBuffer[512];

	// WEST
	if ( !pCur || !pCur->GetPlayer() || !pCur->GetPlayer()->GetRoom() )
		return;

	CPlayer *pPlayer = pCur->GetPlayer();	
	if ( pPlayer->IsFighting() )
	{
		sprintf( szBuffer, "%sYou can't do that while fighting.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szBuffer );
		return;
	}

	if ( pCur->GetPlayer()->GetRoom()->GetWest().GetVnum() == NO_ROOM_VNUM )
	{
		sprintf( szBuffer,"%sCan't go that way.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szBuffer );
		return;
	}
	
	if ( !CanMoveCPUCheck( pCur->GetPlayer() ) )
	{
		sprintf( szBuffer, "%sInsufficient CPU to move!\n", FC_NORMAL_RED );
		pCur->WriteOutput( szBuffer );
		return;
	}

	NWCDirectionRequest( pCur, DIRECTION_WEST );
}

void DoUp( CServerConnection *pCur, vector<string> &sArgs )
{
	char szBuffer[512];

	// UP
	if ( !pCur || !pCur->GetPlayer() || !pCur->GetPlayer()->GetRoom() )
		return;

	CPlayer *pPlayer = pCur->GetPlayer();	
	if ( pPlayer->IsFighting() )
	{
		sprintf( szBuffer, "%sYou can't do that while fighting.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szBuffer );
		return;
	}

	if ( pCur->GetPlayer()->GetRoom()->GetUp().GetVnum() == NO_ROOM_VNUM )
	{
		sprintf( szBuffer, "%sCan't go that way.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szBuffer );
		return;
	}
	
	if ( !CanMoveCPUCheck( pCur->GetPlayer() ) )
	{
		sprintf( szBuffer, "%sInsufficient CPU to move!\n", FC_NORMAL_RED );
		pCur->WriteOutput( szBuffer );
		return;
	}

	NWCDirectionRequest( pCur, DIRECTION_UP );
}

void DoDown( CServerConnection *pCur, vector<string> &sArgs )
{
	char szBuffer[512];

	// UP
	if ( !pCur || !pCur->GetPlayer() || !pCur->GetPlayer()->GetRoom() )
		return;

	CPlayer *pPlayer = pCur->GetPlayer();	
	if ( pPlayer->IsFighting() )
	{
		sprintf( szBuffer, "%sYou can't do that while fighting.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szBuffer );
		return;
	}

	if ( pCur->GetPlayer()->GetRoom()->GetDown().GetVnum() == NO_ROOM_VNUM )
	{
		sprintf( szBuffer, "%sCan't go that way.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szBuffer );
		return;
	}
	
	if ( !CanMoveCPUCheck( pCur->GetPlayer() ) )
	{
		sprintf( szBuffer, "%sInsufficient CPU to move!\n", FC_NORMAL_RED );
		pCur->WriteOutput( szBuffer );
		return;
	}

	NWCDirectionRequest( pCur, DIRECTION_DOWN );
}

void DoFinger( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[1024+0xD0];

	// Info on players in room, items in room, or items in inventory
	CPlayer *pPlayer = pCur->GetPlayer();

	if ( !pPlayer )
		return;

	if ( sArgs.size() < 2 )
	{
		sprintf( szTemp, "%sFinger who?\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}

	CRoom *pRoom;

	if ( !(pRoom = pPlayer->GetRoom()) )
		return;

	// Find item
	CItemInstance *pRequestItem = NULL;
        bool bFound = false;

        for ( pRequestItem = pPlayer->GetItemsOnPlayer()->Head(); pRequestItem; pRequestItem = pPlayer->GetItemsOnPlayer()->Next( pRequestItem ) )
        {
                if ( pRequestItem->HasFlagFingerRequest() )
                {
                        bFound = true;
                        break;
                }
        }

        if ( !bFound )
        {
		sprintf( szTemp, "%sYou need a finger request script item on you to do that.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
        }

	// Get by item name
	std::ostringstream result_stream;
	ostream_iterator< string > oit( result_stream, " " );
	copy( sArgs.begin()+1, sArgs.end(), oit );

	string sTargetName = result_stream.str();
	sTargetName = rtrim( sTargetName );

	bFound = false;

	CPlayerInRoom *pPlayerInRoom = NULL;
	for ( pPlayerInRoom = pRoom->GetPlayersInRoom()->Head(); pPlayerInRoom; pPlayerInRoom = pRoom->GetPlayersInRoom()->Next( pPlayerInRoom ) )
	{
		if ( strncasecmp( sTargetName.c_str(), pPlayerInRoom->GetName().c_str(), sTargetName.length() ) == 0 )
		{
			bFound = true;
			break;
		}	
	}

	if ( bFound )
	{
		// Send command for player
		NWCPlayerFingerRequest( pCur, pPlayerInRoom->GetID(), NWC_PLAYER_FINGER_PLAYER, pRequestItem->GetItemID() );
		return;
	}

	CNPCInRoom *pNPCInRoom = NULL;
	for ( pNPCInRoom = pRoom->GetNPCsInRoom()->Head(); pNPCInRoom; pNPCInRoom = pRoom->GetNPCsInRoom()->Next( pNPCInRoom ) )
	{
		if ( strncasecmp( sTargetName.c_str(), pNPCInRoom->GetName().c_str(), sTargetName.length() ) == 0 )
		{
			bFound = true;
			break;
		}
	}

	if ( bFound )
	{
		// Send command for NPC
		NWCPlayerFingerRequest( pCur, pNPCInRoom->GetID(), NWC_PLAYER_FINGER_NPC, pRequestItem->GetItemID() );
		return;
	}

	sprintf( szTemp, "%sNothing here by that name.\n", FC_NORMAL_RED );
	pCur->WriteOutput( szTemp );

	return;
}

void DoStat( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[1024];

	// Get your player stats here
	CPlayer *pPlayer = pCur->GetPlayer();

	if ( !pPlayer )
		return;

	// Print stats
	sprintf( szTemp, "%sName: %s[HIDDEN]\n", FC_NORMAL_CYAN, FC_NORMAL_WHITE );
	pCur->WriteOutput( szTemp );
	sprintf( szTemp, "%sDescription: %s%s\n", FC_NORMAL_CYAN, FC_NORMAL_WHITE, pPlayer->GetDescription().c_str() );
	pCur->WriteOutput( szTemp );
	sprintf( szTemp, "%sHP: %s%d/%d\n", FC_NORMAL_CYAN, FC_NORMAL_WHITE, pPlayer->GetCurHP(), pPlayer->GetMaxHP() );
	pCur->WriteOutput( szTemp );
	sprintf( szTemp, "%sCPU: %s%d/%d\n", FC_NORMAL_CYAN, FC_NORMAL_WHITE, pPlayer->GetCurCPU(), pPlayer->GetMaxCPU() );
	pCur->WriteOutput( szTemp );
	sprintf( szTemp, "%sMEM: %s%d/%d\n", FC_NORMAL_CYAN, FC_NORMAL_WHITE, pPlayer->GetCurMemory(), pPlayer->GetMaxMemory() );
	pCur->WriteOutput( szTemp );
	sprintf( szTemp, "%sScript Lvl: %s%d\n", FC_NORMAL_CYAN, FC_NORMAL_WHITE, pPlayer->GetScriptLevel() );
	pCur->WriteOutput( szTemp );
	sprintf( szTemp, "%sCode Lvl: %s%d\n", FC_NORMAL_CYAN, FC_NORMAL_WHITE, pPlayer->GetCodeLevel() );
	pCur->WriteOutput( szTemp );
	sprintf( szTemp, "%sHack Lvl: %s%d\n", FC_NORMAL_CYAN, FC_NORMAL_WHITE, pPlayer->GetHackLevel() );
	pCur->WriteOutput( szTemp );
	sprintf( szTemp, "%sDisk Space: %s%d/%dKB\n\n", FC_NORMAL_CYAN, FC_NORMAL_WHITE, pPlayer->GetCurDiskSpace(), pPlayer->GetMaxDiskSpace() );
	pCur->WriteOutput( szTemp );

	string sTempString;
	sprintf( szTemp, "%sInstalled Items:\n           CPU: %s%s\n", FC_NORMAL_CYAN, FC_NORMAL_WHITE, pPlayer->GetInstalledItemName( eItemEquipProcessor, sTempString ).c_str() );
	pCur->WriteOutput( szTemp );
	sprintf( szTemp, "%s        Memory: %s%s\n", FC_NORMAL_CYAN, FC_NORMAL_WHITE, pPlayer->GetInstalledItemName( eItemEquipMemory, sTempString ).c_str() );
	pCur->WriteOutput( szTemp );
	sprintf( szTemp, "%s   Coprocessor: %s%s\n", FC_NORMAL_CYAN, FC_NORMAL_WHITE, pPlayer->GetInstalledItemName( eItemEquipCoprocessor, sTempString ).c_str() );
	pCur->WriteOutput( szTemp );
	sprintf( szTemp, "%s         Card1: %s%s\n", FC_NORMAL_CYAN, FC_NORMAL_WHITE, pPlayer->GetInstalledItemName( eItemEquipCard1, sTempString ).c_str() );
	pCur->WriteOutput( szTemp );
	sprintf( szTemp, "%s         Card2: %s%s\n", FC_NORMAL_CYAN, FC_NORMAL_WHITE, pPlayer->GetInstalledItemName( eItemEquipCard2, sTempString ).c_str() );
	pCur->WriteOutput( szTemp );
	sprintf( szTemp, "%s          Disk: %s%s\n", FC_NORMAL_CYAN, FC_NORMAL_WHITE, pPlayer->GetInstalledItemName( eItemEquipDisk, sTempString ).c_str() );
	pCur->WriteOutput( szTemp );
	sprintf( szTemp, "%s   Accelerator: %s%s\n", FC_NORMAL_CYAN, FC_NORMAL_WHITE, pPlayer->GetInstalledItemName( eItemEquipAccelerator, sTempString ).c_str() );
	pCur->WriteOutput( szTemp );

	sprintf( szTemp, "\n%sItems on you (%d of %d available to carry):\n", FC_NORMAL_CYAN, pPlayer->GetCurDiskItems(), pPlayer->GetMaxDiskItems() );
	pCur->WriteOutput( szTemp );
	
	// Print items on player
	CItemInstance *pItem;
	for ( pItem = pPlayer->GetItemsOnPlayer()->Head(); pItem; pItem = pPlayer->GetItemsOnPlayer()->Next( pItem ) )
	{
		sprintf( szTemp, "  %s%s\n", FC_NORMAL_WHITE, pItem->GetName().c_str() );
		pCur->WriteOutput( szTemp );
	}
}

void DoLS( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[512];

	CPlayer *pPlayer = pCur->GetPlayer();
	
	if ( !pPlayer )
		return;

	CRoom *pRoom = pPlayer->GetRoom();
	if ( !pRoom )
	{
		printf( "Not in a room, critical error.\n" );
		pCur->Disconnect();

		return;
	}

	// Check for PK -- inform them!
	if ( pRoom->IsPKAllowed() )
	{
		// If PK is allowed LET THEM KNOW
		sprintf( szTemp, "%s[PLAYERKILL ALLOWED]\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
	}

	sprintf( szTemp, "%s%s\n", FC_NORMAL_WHITE, pRoom->GetName().c_str() );
	pCur->WriteOutput( szTemp );
	sprintf( szTemp, "%s%s\n", FC_NORMAL_BLUE, pRoom->GetDescription().c_str() );
	pCur->WriteOutput( szTemp );
	
	string sRoomExitList;
	pRoom->GetExitList( sRoomExitList );
	sprintf( szTemp, "%sExits: %s%s\n", FC_NORMAL_BLUE, FC_NORMAL_WHITE, sRoomExitList.c_str() );
	pCur->WriteOutput( szTemp );

	NUtil::LIST_PTR( CPlayerInRoom ) pPlayersInRoomList = pRoom->GetPlayersInRoom();

	for ( CPlayerInRoom *pPlayerInRoom = pPlayersInRoomList->Head(); pPlayerInRoom; pPlayerInRoom = pPlayersInRoomList->Next( pPlayerInRoom ) )
	{
		// Skip ourselves
		if ( pPlayerInRoom->GetID() != pPlayer->GetPlayerID() )
		{
			sprintf( szTemp, "   %s%s\n", FC_NORMAL_RED, pPlayerInRoom->GetName().c_str() );
			pCur->WriteOutput( szTemp );
		}
	}

	NUtil::LIST_PTR( CNPCInRoom ) pNPCsInRoomList = pRoom->GetNPCsInRoom();
	
	for ( CNPCInRoom *pNPCInRoom = pNPCsInRoomList->Head(); pNPCInRoom; pNPCInRoom = pNPCsInRoomList->Next( pNPCInRoom ) ) 
	{
		sprintf( szTemp, "   %s%s\n", FC_NORMAL_RED, pNPCInRoom->GetName().c_str() );
		pCur->WriteOutput( szTemp );
	}	

	NUtil::LIST_PTR( CItemInRoom ) pItemsInRoomList = pRoom->GetItemsInRoom();

	for ( CItemInRoom *pItemInRoom = pItemsInRoomList->Head(); pItemInRoom; pItemInRoom = pItemsInRoomList->Next( pItemInRoom ) )
	{
		sprintf( szTemp, "   %s%s\n", FC_NORMAL_YELLOW, pItemInRoom->GetName().c_str() );
		pCur->WriteOutput( szTemp );
	}
	
	pCur->WriteOutput( "\n" );
}

void DoExit( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[256];

	if ( !pCur )
		return;

	CPlayer *pPlayer;
	if ( !(pPlayer = pCur->GetPlayer()) )
		return;

	if ( pPlayer->IsFighting() )
	{
		sprintf( szTemp, "%sYou can't do that while fighting.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}

	sprintf( szTemp, "%sExiting!\n", FC_BRIGHT_CYAN );
	pCur->WriteOutput( szTemp );

	pCur->Disconnect();
}

void DoPrompt( CServerConnection *pCur )
{
	CPlayer *pPlayer = pCur->GetPlayer();

	if ( !pPlayer )
		return;

	printf( "%s[HP:%s%d%s CPU:%s%d%s MEM:%s%d%s]: %s", FC_BRIGHT_CYAN, FC_NORMAL_WHITE, pPlayer->GetCurHP(), FC_BRIGHT_CYAN, FC_NORMAL_WHITE, pPlayer->GetCurCPU(), FC_BRIGHT_CYAN, FC_NORMAL_WHITE, pPlayer->GetCurMemory(), FC_BRIGHT_CYAN, FC_NORMAL_BLUE );

	fflush( stdout );
}

void DoSay( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[128];

	// Build say string
	std::ostringstream result_stream;
	ostream_iterator< string > oit( result_stream, " " );
	copy( sArgs.begin()+1, sArgs.end(), oit );

	string sText = result_stream.str();
	sText = rtrim( sText );

	if ( sText.length() == 0 )
	{
		sprintf( szTemp, "%sSay what?\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}

	NWCSayRequest( pCur, sText );	
}

void DoWall( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[128];

	// Build say string
	std::ostringstream result_stream;
	ostream_iterator< string > oit( result_stream, " " );
	copy( sArgs.begin()+1, sArgs.end(), oit );

	string sText = result_stream.str();
	sText = rtrim( sText );

	if ( sText.length() == 0 )
	{
		sprintf( szTemp, "%sWall what?\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}

	NWCWallRequest( pCur, sText );	
}

void DoInstall( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[384];

	// Installs an item into an equipment slot
	CPlayer *pPlayer = pCur->GetPlayer();

	if ( !pPlayer )
		return;

	if ( pPlayer->IsFighting() )
	{
		sprintf( szTemp, "%sYou can't do that while fighting.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}

	if ( sArgs.size() < 3 )
	{
		sprintf( szTemp, "%sUsage: install <slot> <item name>\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}

	// Get slot....
	string sSlot = sArgs[1];
	tItemEquipType equipSlot;
	
	if ( !CItemInstance::GetEquipTypeFromString( sSlot, equipSlot ) )
	{
		sprintf( szTemp, "%sInvalid slot specified for install\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}

	if ( pPlayer->GetInstallItem( equipSlot ) )
	{
		sprintf( szTemp, "%sItem already installed in that slot, uninstall it first\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}
	
	std::ostringstream result_stream;
	ostream_iterator< string > oit( result_stream, " " );
	copy( sArgs.begin()+2, sArgs.end(), oit );

	string sText = result_stream.str();
	sText = rtrim( sText );

	// Find item in inventory list...
	CItemInstance *pItem;
	CItemInstance *pMatch = NULL;

	for ( pItem = pPlayer->GetItemsOnPlayer()->Head(); pItem; pItem = pPlayer->GetItemsOnPlayer()->Next( pItem ) )
	{
		// Find closest match...
		if ( strncasecmp( pItem->GetName().c_str(), sText.c_str(), sText.length() ) == 0 )
		{
			pMatch = pItem;
			break;
		}
	}

	if ( pMatch == NULL )
	{
		sprintf( szTemp, "%sInstall what? item not found\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}

	if ( pMatch->GetItemType() != eItemEquip )
	{
		sprintf( szTemp, "%sThat item cannot be equipped\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}

	// Send install command to server to process!
	NWCEquipItemRequest( pCur, pMatch, equipSlot );
}

void DoUninstall( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[384];

	// Uninstalls (removes) an item from an equipment slot
	// Installs an item into an equipment slot
	CPlayer *pPlayer = pCur->GetPlayer();

	if ( !pPlayer )
		return;
	
	if ( pPlayer->IsFighting() )
	{
		sprintf( szTemp, "%sYou can't do that while fighting.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}

	if ( sArgs.size() < 2 )
	{
		sprintf( szTemp, "%sUsage: uninstall <slot>\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}

	// Get slot....
	string sSlot = sArgs[1];
	tItemEquipType equipSlot;
	
	if ( !CItemInstance::GetEquipTypeFromString( sSlot, equipSlot ) )
	{
		sprintf( szTemp, "%sInvalid slot specified for uninstall\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}

	CItemInstance *pItem;
	if ( !(pItem = pPlayer->GetInstallItem( equipSlot )) )
	{
		sprintf( szTemp, "%sNo item in slot to uninstall\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}
	
	// Send install command to server to process!
	NWCUnequipItemRequest( pCur, pItem, equipSlot );
}

void DoGet( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[384];

	// Uninstalls (removes) an item from an equipment slot
	// Installs an item into an equipment slot
	CPlayer *pPlayer = pCur->GetPlayer();

	if ( !pPlayer )
		return;

	if ( !pPlayer->GetRoom() )
		return;

	if ( sArgs.size() < 2 )
	{
		sprintf( szTemp, "%sGet what?\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}

	// Get by item name
	std::ostringstream result_stream;
	ostream_iterator< string > oit( result_stream, " " );
	copy( sArgs.begin()+1, sArgs.end(), oit );

	string sItemName = result_stream.str();
	sItemName = rtrim( sItemName );

	// Find item in room!
	bool bFound = false;
	CItemInRoom *pItem = NULL;
	for ( pItem = pPlayer->GetRoom()->GetItemsInRoom()->Head(); pItem; pItem = pPlayer->GetRoom()->GetItemsInRoom()->Next( pItem ) )
	{
		if ( strncasecmp( sItemName.c_str(), pItem->GetName().c_str(), sItemName.length() ) == 0 )
		{
			bFound = true;
			break;
		}
	}

	if ( !bFound )
	{
		sprintf( szTemp, "%sNo item by that name here.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}
	
	// Attempt to get this item!
	NWCGetItemRequest( pCur, pItem );
}

void DoDrop( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[256];

	// Uninstalls (removes) an item from an equipment slot
	// Installs an item into an equipment slot
	CPlayer *pPlayer = pCur->GetPlayer();

	if ( !pPlayer )
		return;

	if ( sArgs.size() < 2 )
	{
		sprintf( szTemp, "%sDrop what?\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}

	// Get by item name
	std::ostringstream result_stream;
	ostream_iterator< string > oit( result_stream, " " );
	copy( sArgs.begin()+1, sArgs.end(), oit );

	string sItemName = result_stream.str();
	sItemName = rtrim( sItemName );

	bool bFound = false;
	CItemInstance *pItem = NULL;
	for ( pItem = pPlayer->GetItemsOnPlayer()->Head(); pItem; pItem = pPlayer->GetItemsOnPlayer()->Next( pItem ) )
	{
		if ( strncasecmp( sItemName.c_str(), pItem->GetName().c_str(), sItemName.length() ) == 0 )
		{
			bFound = true;
			break;
		}
	}

	if ( !bFound )
	{
		sprintf( szTemp, "%sNo item on you to drop by that name.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}

	// OK send drop request
	NWCDropItemRequest( pCur, pItem );
}

void DoAttack( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[256];

	// Uninstalls (removes) an item from an equipment slot
	// Installs an item into an equipment slot
	CPlayer *pPlayer = pCur->GetPlayer();

	if ( !pPlayer )
		return;

	if ( sArgs.size() < 2 )
	{
		sprintf( szTemp, "%sAttack what?\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}

	CRoom *pRoom;

	if ( !(pRoom = pPlayer->GetRoom()) )
		return;

	// Get by item name
	std::ostringstream result_stream;
	ostream_iterator< string > oit( result_stream, " " );
	copy( sArgs.begin()+1, sArgs.end(), oit );

	string sTargetName = result_stream.str();
	sTargetName = rtrim( sTargetName );

	bool bFound = false;

	CPlayerInRoom *pPlayerInRoom = NULL;
	for ( pPlayerInRoom = pRoom->GetPlayersInRoom()->Head(); pPlayerInRoom; pPlayerInRoom = pRoom->GetPlayersInRoom()->Next( pPlayerInRoom ) )
	{
		if ( strncasecmp( sTargetName.c_str(), pPlayerInRoom->GetName().c_str(), sTargetName.length() ) == 0 )
		{
			bFound = true;
			break;
		}	
	}

	if ( bFound )
	{
		// Attacking player
		if ( !pRoom->IsPKAllowed() )
		{
			sprintf( szTemp, "%sYou can't attack another player in a non PK area.\n", FC_NORMAL_RED );
			pCur->WriteOutput( szTemp );
			return;
		}

		// Request to attack player
		NWCAttackRequest( pCur, pPlayerInRoom->GetID(), NWC_ATTACK_PLAYER );
		return;
	}

	CNPCInRoom *pNPCInRoom = NULL;
	for ( pNPCInRoom = pRoom->GetNPCsInRoom()->Head(); pNPCInRoom; pNPCInRoom = pRoom->GetNPCsInRoom()->Next( pNPCInRoom ) )
	{
		if ( strncasecmp( sTargetName.c_str(), pNPCInRoom->GetName().c_str(), sTargetName.length() ) == 0 )
		{
			bFound = true;
			break;
		}
	}

	if ( bFound )
	{
		NWCAttackRequest( pCur, pNPCInRoom->GetID(), NWC_ATTACK_NPC );
		return;
	}

	sprintf( szTemp, "%sAttack what?\n", FC_NORMAL_RED );
	pCur->WriteOutput( szTemp );

	return;
}

void DoFlee( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[256];

	// Uninstalls (removes) an item from an equipment slot
	// Installs an item into an equipment slot
	CPlayer *pPlayer = pCur->GetPlayer();

	if ( !pPlayer )
		return;

	CRoom *pRoom;

	if ( !(pRoom = pPlayer->GetRoom()) )
		return;

	if ( pPlayer->GetCurCPU() < (pPlayer->GetMaxCPU() * FLEE_CPU_THRESHOLD_PERCENTAGE) )
	{
		sprintf( szTemp, "%sInsufficient CPU available to flee!!!\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}
		
	sprintf( szTemp, "%sYou attempt to flee!\n", FC_NORMAL_RED );
	pCur->WriteOutput( szTemp );

	// Attempt to flee!
	NWCPlayerFleeRequest( pCur );

	return;
}

void DoRun( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[256];

	// Uninstalls (removes) an item from an equipment slot
	// Installs an item into an equipment slot
	CPlayer *pPlayer = pCur->GetPlayer();

	if ( !pPlayer )
		return;

	CRoom *pRoom;

	if ( !(pRoom = pPlayer->GetRoom()) )
		return;

	if ( sArgs.size() < 2 )
	{
		sprintf( szTemp, "%sRun what script?\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}
	
	vector<string> sRemainingArgs;

	for(std::vector<string>::iterator it = sArgs.begin()+2; it != sArgs.end(); ++it )
	{
		sRemainingArgs.push_back( *it );
	}

	RunScript( pCur, sArgs[1], sRemainingArgs );
	
	return;
}

void DoInfo( CServerConnection *pCur, vector<string> &sArgs )
{
	char szTemp[768];

	// Prints stats on an item in inventory
	CPlayer *pPlayer = pCur->GetPlayer();

	if ( !pPlayer )
		return;

	if ( sArgs.size() < 2 )
	{
		sprintf( szTemp, "%sInfo what?\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}

	// Get by item name
	std::ostringstream result_stream;
	ostream_iterator< string > oit( result_stream, " " );
	copy( sArgs.begin()+1, sArgs.end(), oit );

	string sItemName = result_stream.str();
	sItemName = rtrim( sItemName );

	bool bFound = false;
	CItemInstance *pItem = NULL;
	for ( pItem = pPlayer->GetItemsOnPlayer()->Head(); pItem; pItem = pPlayer->GetItemsOnPlayer()->Next( pItem ) )
	{
		if ( strncasecmp( sItemName.c_str(), pItem->GetName().c_str(), sItemName.length() ) == 0 )
		{
			bFound = true;
			break;
		}
	}

	if ( !bFound )
	{
		sprintf( szTemp, "%sNo item on you to display info for.\n", FC_NORMAL_RED );
		pCur->WriteOutput( szTemp );
		return;
	}

	// OK send drop request
	// Print information for item...
	sprintf( szTemp, "%sName: %s%s\n", FC_BRIGHT_CYAN, FC_BRIGHT_WHITE, pItem->GetName().c_str() );
	pCur->WriteOutput( szTemp );
	sprintf( szTemp, "%sDescription: %s%s\n", FC_BRIGHT_CYAN, FC_BRIGHT_WHITE, pItem->GetDescription().c_str() );
	pCur->WriteOutput( szTemp );

	if ( pItem->GetItemType() == eItemEquip )
	{
		string sEquipSlot;
		CItemInstance::GetEquipStringFromType( pItem->GetEquipType(), sEquipSlot );
		sprintf( szTemp, "%sEquippable in %s%s%s slot.\n", FC_BRIGHT_CYAN, FC_BRIGHT_WHITE, sEquipSlot.c_str(), FC_BRIGHT_CYAN ); 
	}
	else if ( pItem->GetItemType() == eItemScript )
		sprintf( szTemp, "%sItem is consumable script.\n", FC_BRIGHT_CYAN );
	pCur->WriteOutput( szTemp );

	sprintf( szTemp, "%sCode Level: %s%d\n", FC_BRIGHT_CYAN, FC_BRIGHT_WHITE, pItem->GetCodeLevel() );
	pCur->WriteOutput( szTemp );
	sprintf( szTemp, "%sHack Level: %s%d\n", FC_BRIGHT_CYAN, FC_BRIGHT_WHITE, pItem->GetHackLevel() );
	pCur->WriteOutput( szTemp );
	sprintf( szTemp, "%sScript Level: %s%d\n", FC_BRIGHT_CYAN, FC_BRIGHT_WHITE, pItem->GetScriptLevel() );
	pCur->WriteOutput( szTemp );

	string sItemFlags;
	pItem->GetItemFlagsString( sItemFlags );
	sprintf( szTemp, "%sFlags: %s%s\n", FC_BRIGHT_CYAN, FC_BRIGHT_WHITE, sItemFlags.c_str() );
	pCur->WriteOutput( szTemp );

	sprintf( szTemp, "%sHP Modifier: %s%d\n%sCPU Modifier: %s%d\n%sMemory Modifier: %s%d\n", FC_BRIGHT_CYAN, FC_BRIGHT_WHITE, pItem->GetHPModifier(), FC_BRIGHT_CYAN, FC_BRIGHT_WHITE, pItem->GetCPUModifier(), FC_BRIGHT_CYAN, FC_BRIGHT_WHITE, pItem->GetMemoryModifier() );
	pCur->WriteOutput( szTemp );	

	sprintf( szTemp, "%sDisk Usage: %s%d\n", FC_BRIGHT_CYAN, FC_BRIGHT_WHITE, pItem->GetDiskUsage() );
	pCur->WriteOutput( szTemp );

	return;
}

CCommandHandler::CCommandHandler()
{
	m_cmdMap["north"] = DoNorth;
	m_cmdMap["n"] = DoNorth;
	m_cmdMap["south"] = DoSouth;
	m_cmdMap["s"] = DoSouth;
	m_cmdMap["east"] = DoEast;
	m_cmdMap["e"] = DoEast;
	m_cmdMap["west"] = DoWest;
	m_cmdMap["w"] = DoWest;
	m_cmdMap["up"] = DoUp;
	m_cmdMap["u"] = DoUp;
	m_cmdMap["down"] = DoDown;
	m_cmdMap["d"] = DoDown;
	m_cmdMap["finger"] = DoFinger;
	m_cmdMap["say"] = DoSay;
	m_cmdMap["stat"] = DoStat;
	m_cmdMap["ls"] = DoLS;
	m_cmdMap["exit"] = DoExit;
	m_cmdMap["wall"] = DoWall;
	m_cmdMap["install"] = DoInstall;
	m_cmdMap["uninstall"] = DoUninstall;
	m_cmdMap["get"] = DoGet;
	m_cmdMap["drop"] = DoDrop;
	m_cmdMap["attack"] = DoAttack;
	m_cmdMap["flee"] = DoFlee;
	m_cmdMap["run"] = DoRun;
	m_cmdMap["info"] = DoInfo;
}

CCommandHandler::~CCommandHandler()
{

}

void CCommandHandler::DoCommand( CServerConnection *pCur, char *szCommandLine )
{

	// printf( "command is: %s\n", sCommandLine.c_str() );

	vector<string> sArgs = SplitString( szCommandLine );	

	map<string, tCmdHandler>::const_iterator command_iter = m_cmdMap.find ( sArgs[0] );
	if ( command_iter == m_cmdMap.end () )
	{
		char szTemp[128];
		sprintf( szTemp, "%sCommand not found.\n", FC_NORMAL_RED );

		pCur->WriteOutput( szTemp );
	}
	else
	{
		command_iter->second( pCur, sArgs );
	}
}
