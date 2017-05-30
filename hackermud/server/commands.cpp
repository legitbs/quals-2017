#include "common.h"

void DoNorth( CPlayer *pPlayer, vector<string> &sArgs )
{
	printf( "NORTH\n" );
}

void DoSouth( CPlayer *pPlayer, vector<string> &sArgs )
{
	printf( "SOUTH\n" );
}

void DoEast( CPlayer *pPlayer, vector<string> &sArgs )
{
	printf( "EAST\n" );
}

void DoWest( CPlayer *pPlayer, vector<string> &sArgs )
{
	printf( "WEST\n" );
}

void DoUp( CPlayer *pPlayer, vector<string> &sArgs )
{
	printf( "UP\n" );
}

void DoDown( CPlayer *pPlayer, vector<string> &sArgs )
{
	printf( "DOWN\n" );
}

void DoInfo( CPlayer *pPlayer, vector<string> &sArgs )
{
	printf( "INFO\n" );
}

void DoStat( CPlayer *pPlayer, vector<string> &sArgs )
{
	printf( "STAT\n" );
}



CCommandHandler::CCommandHandler( )
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
	m_cmdMap["info"] = DoInfo;
	m_cmdMap["stat"] = DoStat;
}

CCommandHandler::~CCommandHandler( )
{

}

void CCommandHandler::DoCommand( CPlayer *pPlayer, string sCommandLine )
{

	printf( "command is: %s\n", sCommandLine.c_str() );

	vector<string> sArgs = SplitString( sCommandLine );	

        // Do room procs...
	/*
        if ( pPlayer->GetRoom() )
        {
                if ( pPlayer->GetRoom()->DoRoomProc( command, p, sArgs ) )
                {
                        // If we handled the command, exit!
                        return;
                }

                // Do item procs...
                tItemRoomList *pItemRoomList = p->GetRoom()->GetItemListInRoom();

                tItemInstance *pCurItem;

                for ( pCurItem = pItemRoomList->Head(); pCurItem; pCurItem = pItemRoomList->Next( pCurItem ) )
                {
                        if ( pCurItem->DoItemProc( command, p, sArgs ) )
                        {
                                // If we handled the command, exit!
                                return;
                        }
                }
        }
	*/


	map<string, tCmdHandler>::const_iterator command_iter = m_cmdMap.find ( sArgs[0] );
	if ( command_iter == m_cmdMap.end () )
	{
		printf( "%s%s command not found.\n%s", FC_NORMAL_RED, sArgs[0].c_str(), FC_NORMAL_WHITE );
	}
	else
	{
		command_iter->second( pPlayer, sArgs );
	}
}
