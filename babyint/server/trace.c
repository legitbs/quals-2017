#include "calltrace.h"

extern "C"
{
void __cyg_profile_func_enter( void *pFN, void *pCaller ) __attribute__((no_instrument_function));
void __cyg_profile_func_exit( void *pFn, void *pCallSite ) __attribute__((no_instrument_function));

void __cyg_profile_func_enter( void *pFN, void *pCaller )
{
        void *pSP = __builtin_frame_address(0);
	AddTrace( pSP, pFN, pCaller );
	/*

        g_callGraph[g_callPos].Set( pSP, pFN, pCaller );        

        g_callPos++;
        if ( g_callPos >= CALL_GRAPH_SIZE )
                g_callPos = 0;

        g_callLen++;
        if ( g_callLen > CALL_GRAPH_SIZE )
                g_callLen = CALL_GRAPH_SIZE;
	*/
}

void __cyg_profile_func_exit( void *pFn, void *pCallSite )
{
        // Do nothing
}
}
