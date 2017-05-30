#ifndef __NETWORK_COMMANDS_H__
#define __NETWORK_COMMANDS_H__

#define KILLER_TYPE_PLAYER	(1)
#define KILLER_TYPE_NPC		(2)
#define KILLER_TYPE_WORLD	(3)

#define DAMAGE_TYPE_PLAYER	(1)
#define DAMAGE_TYPE_NPC		(2)
#define DAMAGE_TYPE_WORLD	(3)

enum
{
	NWC_AUTH_REQUEST = 0x0,
	NWC_SAY_REQUEST,
	NWC_WALL_REQUEST,
	NWC_DIRECTION_REQUEST,
	NWC_GETITEM_REQUEST,
	NWC_DROPITEM_REQUEST,
	NWC_LOGOUT_REQUEST,
	NWC_EQUIPITEM_REQUEST,
	NWC_UNEQUIPITEM_REQUEST,
	NWC_RUNSCRIPT_REQUEST,
	NWC_ATTACK_REQUEST,
	NWC_FLEE_REQUEST,
	NWC_RUN_SCRIPT_REQUEST,
	NWC_PLAYER_FINGER_REQUEST,
	NWC_CREATE_PLAYER,
	
	NWC_MAX_REQUESTS 
} eNetworkClientCommands;

enum
{
	NWC_AUTH_CHALLENGE,			// Challenge data
	NWC_AUTH_RESPONSE,			// Response to auth (success or not)
	NWC_PLAYER_SAYS_RESPONSE,		// Player says something in room
	NWC_PLAYER_ARRIVES_RESPONSE,		// Player arrives in room (login, enter room)
	NWC_PLAYER_LEAVES_RESPONSE,		// Player leaves room (logout, exit room)
	NWC_PLAYER_WALL_RESPONSE,		// Player walls all players
	NWC_PLAYER_ENTERROOM_RESPONSE,		// This player enters a room
	NWC_PLAYER_EQUIPITEM_RESPONSE,		// Player equips item
	NWC_PLAYER_UNEQUIPITEM_RESPONSE,	// Player unequips item	
	NWC_PLAYER_INFOMSG_RESPONSE,		// Player information (other actions)
	NWC_PLAYER_GETITEM_RESPONSE,            // Player gets an item  
        NWC_OTHERPLAYER_GETITEM_RESPONSE,       // Other player in room gets an item    
	NWC_PLAYER_DROPITEM_RESPONSE,           // Player drops an item
        NWC_OTHERPLAYER_DROPITEM_RESPONSE,      // Other player in room drops an item
	NWC_NPC_LEAVES_RESPONSE,                // NPC leaves
        NWC_NPC_ARRIVES_RESPONSE,               // NPC arrives
	NWC_PLAYER_STAT_UPDATE_RESPONSE,	// Player stat update
	NWC_PLAYER_ATTACK_RESPONSE,		// A player (or NPC) attacks something!
	NWC_PLAYER_DAMAGE_RESPONSE,		// A player (or NPC) (who is alreadying attacking something, damages it)
	NWC_PLAYER_TAKES_DAMAGE_RESPONSE,	// A player is taking damage
	NWC_PLAYER_KILLED_RESPONSE,             // A plyaer (or NPC) is killed in your room
	NWC_ITEM_TO_ROOM,			// An item goes to this room
	NWC_PLAYER_DEATH,			// A player dies
	NWC_PLAYER_LEVEL,			// A player levels up!!!
	NWC_PLAYER_DELETES_ITEM,		// A player deletes an item
	NWC_PLAYER_RUNS_SCRIPT,			// A player runs a script
	NWC_PLAYER_FINGER_RESPONSE,		// A player fingers another a player
	NWC_ITEM_IN_ROOM_DELETED,		// An item in the room is deleted (by something)
	NWC_CREATE_PLAYER_RESPONSE,		// A response for creating a new player
	NWC_AUTH_PROOF,				// A proof of work request

	NWC_MAX_RESPONSES
} eNetworkServerCommands;

enum
{
	NWC_PLAYER_ARRIVAL_LOGIN,
	NWC_PLAYER_ARRIVAL_ENTERROOM,
	NWC_PLAYER_ARRIVAL_RESPAWN
} eNWCPlayerArrivalType;

enum
{
        NWC_PLAYER_LEAVES_LOGOUT,
        NWC_PLAYER_LEAVES_EXITROOM,
	NWC_PLAYER_LEAVES_FLEE
} eNWCPlayerLeaveTypes;

enum
{
        NWC_NPC_LEAVES_EXITROOM,
        NWC_NPC_LEAVES_DISAPPEAR,
        NWC_NPC_LEAVES_KILLED
} eNWCNPCLeaveTypes;

enum
{
        NWC_NPC_ARRIVES_ENTERROOM,
        NWC_NPC_ARRIVES_SPAWN,
        NWC_NPC_ARRIVES_HIDDEN
} eNWCNPCArriveTypes;

enum
{
	NWC_ATTACK_NPC,
	NWC_ATTACK_PLAYER
} eNWCAttackTargetTypes;

enum
{
	NWC_PLAYER_FINGER_NPC,
	NWC_PLAYER_FINGER_PLAYER
} eNWCPlayerFingerType;

enum
{
        NWC_CREATEPLAYER_PLAYER_EXISTS,
        NWC_CREATEPLAYER_INVALID_DATA,
        NWC_CREATEPLAYER_SUCCESS
} eNWCCreatePlayerResponseType;

class CNetworkCommands
{
public:
	static bool DoNetworkCommand( CServerConnection *pConnection, uint8_t cmdNumber, uint8_t *pCmdData, uint32_t dataLen );
};

void NWCPlayerFingerRequest( CServerConnection *pCur, uint32_t targetID, uint8_t targetType, uint32_t requestItemID );
void NWCRunScriptRequest( CServerConnection *pCur, uint8_t scriptID, CStreamData &oExtraData );
void NWCDeleteItemRequest( CServerConnection *pCur, uint32_t itemID );
void NWCPlayerFleeRequest( CServerConnection *pCur );
void NWCSayRequest( CServerConnection *pCur, string sText );
void NWCWallRequest( CServerConnection *pCur, string sText );
void NWCDirectionRequest( CServerConnection *pCur, uint8_t direction );
void NWCEquipItemRequest( CServerConnection *pCur, CItemInstance *pItem, tItemEquipType slot );
void NWCUnequipItemRequest( CServerConnection *pCur, CItemInstance *pItem, tItemEquipType slot );
void NWCGetItemRequest( CServerConnection *pCur, CItemInRoom *pItem );
void NWCDropItemRequest( CServerConnection *pCur, CItemInstance *pItem );
void NWCAttackRequest( CServerConnection *pCur, uint32_t targetID, uint8_t targetType );

#endif // __NETWORK_COMMANDS_H__
