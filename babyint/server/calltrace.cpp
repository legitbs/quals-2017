#include "calltrace.h"

void CDebugCallGraph::Set( void *pSP, void *pFN, void *pCaller )
{
	m_pSP = pSP;
	m_pFN = pFN;
	m_pCaller = pCaller;
}

void CDebugCallGraph::PrintCall( void )
{
	printf( "[%016llX called %016llx]%016llX]\n", m_pFN, m_pCaller, m_pSP );
}



#define CALL_GRAPH_SIZE (64)
uint32_t g_callPos = 0;
uint32_t g_callLen = 0;

CDebugCallGraph g_callGraph[CALL_GRAPH_SIZE];

void exit( int exit_num )
{
        void (*pExitFptr)( int );

        pExitFptr = (void (*)(int ))dlsym(RTLD_NEXT, "exit");

        if ( exit_num < 0 )
		PrintTrace( );

        // Exit normally no print

        (*pExitFptr)( exit_num );
}

void PrintTrace( void )
{
	uint32_t curPos;
	uint32_t curCount;

	if ( g_callLen >= CALL_GRAPH_SIZE )
		curPos = g_callPos;
	else
		curPos = 0;

	printf( "Error, call graph (%d, %d):\n", curPos, g_callLen );
	for ( curCount = 0; curCount < g_callLen; curCount++ )
	{
		g_callGraph[curPos++].PrintCall( );

		if ( curPos >= CALL_GRAPH_SIZE )
			curPos = 0;
	}

}

void AddTrace( void *pSP, void *pFN, void *pCaller )
{
        g_callGraph[g_callPos].Set( pSP, pFN, pCaller );        

        g_callPos++;
        if ( g_callPos >= CALL_GRAPH_SIZE )
                g_callPos = 0;

        g_callLen++;
        if ( g_callLen > CALL_GRAPH_SIZE )
                g_callLen = CALL_GRAPH_SIZE;
}

#if 0
extern "C"
{
void __cyg_profile_func_enter( void *pFN, void *pCaller ) __attribute__((no_instrument_function));
void __cyg_profile_func_exit( void *pFn, void *pCallSite ) __attribute__((no_instrument_function));

void __cyg_profile_func_enter( void *pFN, void *pCaller )
{
        void *pSP = __builtin_frame_address(0);

        g_callGraph[g_callPos].Set( pSP, pFN, pCaller );        

        g_callPos++;
        if ( g_callPos >= CALL_GRAPH_SIZE )
                g_callPos = 0;

        g_callLen++;
        if ( g_callLen > CALL_GRAPH_SIZE )
                g_callLen = CALL_GRAPH_SIZE;
}

void __cyg_profile_func_exit( void *pFn, void *pCallSite )
{
        // Do nothing
}
}
#endif
