#ifndef __COMMANDS_H__
#define __COMMANDS_H__



class CCommandHandler
{
public:
	CCommandHandler();
	~CCommandHandler();

	void DoCommand( CPlayer *pPlayer, string sCommandLine );

private:
	typedef void (*tCmdHandler) ( CPlayer *pPlayer, vector<string> &args );
	std::map<std::string, tCmdHandler> m_cmdMap;
};

#endif // __COMMANDS_H__
