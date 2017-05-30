#ifndef __COMMANDS_H__
#define __COMMANDS_H__



class CCommandHandler
{
public:
	CCommandHandler();
	~CCommandHandler();
	void DoCommand( CServerConnection *pCur, char *szCommandLine );

private:
	typedef void (*tCmdHandler) ( CServerConnection *pCur, vector<string> &args );
	std::map<std::string, tCmdHandler> m_cmdMap;
};

void DoPrompt( CServerConnection *pCur );

#endif // __COMMANDS_H__
