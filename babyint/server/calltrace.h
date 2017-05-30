#ifndef __CALL_TRACE_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>



class CDebugCallGraph
{
public:
        CDebugCallGraph( )
        {
                m_pSP = NULL;
                m_pFN = NULL;
                m_pCaller = NULL;
        }

        CDebugCallGraph( void *pSP, void *pFN, void *pCaller )
                : m_pSP( pSP ), m_pFN( pFN ), m_pCaller( pCaller )
        {

        }

        void Set( void *pSP, void *pFN, void *pCaller ) __attribute__((no_instrument_function));

        void *GetSP( void ) const { return m_pSP; };
        void *GetFN( void ) const { return m_pFN; };
        void *GetCaller( void ) const { return m_pCaller; };

        void PrintCall( void ) __attribute__((no_instrument_function));

private:
        void *m_pSP;
        void *m_pFN;
        void *m_pCaller;
};

void AddTrace( void *pSP, void *pFN, void *pCaller ) __attribute__((no_instrument_function));
void PrintTrace( void ) __attribute__((no_instrument_function));

#endif // __CALL_TRACE_H__
