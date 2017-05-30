
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>
#include "common.h"
#include "calltrace.h"
#include <unistd.h>
#include <dlfcn.h>

#define MAX_IDLE_SECS	(600)


void sig_alarm_handler( int signum )
{
        LogDebug( LOG_PRIORITY_LOW, "Connection timed out, closing!\n" );

        exit( 1 );
}

void sig_abort_handler( int signum )
{
	exit( -1 );
}

#if 0
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

	void Set( void *pSP, void *pFN, void *pCaller )
	{
		m_pSP = pSP;
		m_pFN = pFN;
		m_pCaller = pCaller;
	}

	void *GetSP( void ) const { return m_pSP; };
	void *GetFN( void ) const { return m_pFN; };
	void *GetCaller( void ) const { return m_pCaller; };

	void PrintCall( void )
	{
		printf( "[%X called by %x][SP: %X]\n", m_pFN, m_pCaller, m_pSP );
	}

private:
	void *m_pSP;
	void *m_pFN;
	void *m_pCaller;
};

#define CALL_GRAPH_SIZE	(64)
uint32_t g_callPos = 0;
uint32_t g_callLen = 0;

CDebugCallGraph g_callGraph[CALL_GRAPH_SIZE];
#endif

#if 0
void __cyg_profile_func_enter( void *pFN, void *pCaller )
{
	/*
	void *pSP = __builtin_frame_address(0);

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
#endif

// Hook normal exists and print the call graph on error
/*
void exit( int exit_num )
{
	void (*pExitFptr)( int );
	
	pExitFptr = (void (*)(int ))dlsym(RTLD_NEXT, "exit");

	if ( exit_num < 0 )
	{
		uint32_t curPos;
		uint32_t curCount;

		printf( "Exiting with, error, debug call graph:\n" );	
		if ( g_callLen >= CALL_GRAPH_SIZE )
			curPos = g_callPos;
		else
			curPos = 0;

		for ( curCount; curCount < g_callLen; curCount++ )
		{
			g_callGraph[curPos++].PrintCall( );

			if ( curPos >= CALL_GRAPH_SIZE )
				curPos = 0;
		} 
	}
	// Exit normally no print

	(*pExitFptr)( exit_num );
}
*/

class CDataSegment : public CDoubleLink
{
private:
	uint16_t m_offset;
	uint8_t m_length;
	uint8_t *m_pData;

public:
	CDataSegment( uint16_t offset, uint8_t *pData, uint8_t dataLen )
		: CDoubleLink( )
	{
		m_pData = new uint8_t[(uint32_t)dataLen+1];

		memcpy( m_pData, pData, dataLen );

		m_offset = offset;
		m_length = dataLen;
	}

	~CDataSegment( )
	{
		delete [] m_pData;
	}

	uint16_t GetOffset( void ) const { return m_offset; };
	uint8_t GetLength( void ) const { return m_length; };
	const uint8_t *GetData( void ) const { return m_pData; };
};

class CDataSequence
{
private:
	int16_t m_totalLen; 	// THE BUG integer overflow

	CDoubleList m_dataSegments;	// Linked list of data segments

	uint8_t m_sequenceNumber;

public:
	CDataSequence( uint8_t sequenceNumber )
		: m_totalLen( 0 ), m_sequenceNumber( sequenceNumber )
	{

	}

	~CDataSequence( )
	{

	}

	void AddSegment( uint16_t offset, uint8_t *pData, uint8_t dataLen )
	{
		// Check for an existing segment -- and overwrite
		CDataSegment *pFirst = NULL;
		CDataSegment *pNext = (CDataSegment *)m_dataSegments.GetFirst();

		bool bReplace = false;
#if 0
		while ( (pFirst = pNext) )
		{
			pNext = (CDataSegment *)m_dataSegments.GetNext( pFirst );

			if ( pFirst->GetOffset() == offset && pFirst->GetLength() == dataLen )
			{
				delete pFirst;
				bReplace = true;
				break;
			}
		}
#endif

#if 0
		if ( !bReplace )
		{
			if ( offset+dataLen > m_totalLen )
				m_totalLen = offset+dataLen;

			m_totalLen += dataLen;
		}
#endif
		m_totalLen += dataLen;

		m_dataSegments.AddLast( new CDataSegment( offset, pData, dataLen ) );
	}

	void Reset( void )
	{
		m_dataSegments.DeleteAll();
	
		m_totalLen = 0;
	}

	uint8_t *AssembleSegments( uint32_t &finalLength )
	{
		// Allocate!
		if ( m_totalLen == 0 )
		{
			finalLength = 0;
			return (NULL);
		}

		uint8_t *pFinalBuffer = new uint8_t[m_totalLen];

		CDataSegment *pCur;

		for ( pCur = (CDataSegment *)m_dataSegments.GetFirst(); pCur; pCur = (CDataSegment *)m_dataSegments.GetNext( pCur ) )
			memcpy( (pFinalBuffer+pCur->GetOffset()), pCur->GetData(), pCur->GetLength() );

		finalLength = m_totalLen;

		return (pFinalBuffer);
	}	
};

uint8_t AsciiToHex( char c )
{
	if ( c >= '0' && c <= '9' )
		return (c - '0');
	else if ( c >= 'a' && c <= 'f' )
		return ((c - 'a') + 10);
	else if ( c >= 'A' && c <= 'F' )
		return ((c - 'A') + 10);
	else
		return 0;
}

uint8_t *ConvertHexStringToData( const char *pszStr, uint32_t &dataLen )
{
	uint8_t *pTemp = new uint8_t[strlen(pszStr)/2];

	uint32_t pos = 0;
	while ( *pszStr != '\0' && *pszStr != '\n' )
	{
		if ( pos%2 == 0 )
			pTemp[pos/2] = AsciiToHex( *pszStr++ );
		else
			pTemp[pos/2] = (pTemp[pos/2] << 4) | AsciiToHex( *pszStr++ );

		pos++;	
	}

	if ( pos%2 == 0 )
		dataLen = (pos/2);
	else
		dataLen = (pos/2)+1;

	return pTemp;
}

CDataSequence *g_pSequences[256];

void RunReceiver( void )
{
	uint32_t i;
	char szLine[1024];
	bool bDone = false;
	int16_t totalLen;


	while ( !bDone )
	{
		uint32_t seqNum;
		uint32_t offset;
		bool lsfFlag = false;

		printf( "SEQ #: " );
		
		if ( fgets( szLine, 255, stdin ) == NULL )
			goto exit;

		seqNum = atol( szLine );

		if ( seqNum > 255 )
			goto exit;

		
		printf( "Offset: " );
		
		if ( fgets( szLine, 255, stdin ) == NULL )
			goto exit;

		offset = atol( szLine );

		if ( offset > 65535 )
			goto exit;

		printf( "Data: " );
		
		if ( fgets( szLine, 768, stdin ) == NULL )
			goto exit;

		uint32_t dataLen = 0;
		uint8_t *pData = ConvertHexStringToData( szLine, dataLen );

		if ( dataLen == 0 || pData == NULL )
			goto exit;

		if ( dataLen > 255 )
		{
			printf( "Invalid data\n" );
			PrintTrace();

			continue;
		}
		
		printf( "LSF Yes/No: " );

		if ( fgets( szLine, 768, stdin ) == NULL )
			goto exit;

		if ( szLine[0] == 'Y' && szLine[1] == 'e' && szLine[2] == 's' )
			lsfFlag = true;
		else if ( szLine[0] == 'N' && szLine[1] == 'o' )
			lsfFlag = false;
		else
			goto exit;
	
	
		printf( "RX PDU [%d] [len=%d]\n", seqNum, dataLen );
	
		g_pSequences[seqNum]->AddSegment( offset, pData, dataLen );

		delete pData;

		if ( lsfFlag )
		{
			uint32_t finalLength;
			uint8_t *pAssembledData;

			pAssembledData = g_pSequences[seqNum]->AssembleSegments( finalLength );

			if ( pAssembledData == NULL )
				goto exit;

			g_pSequences[seqNum]->Reset();
			printf( "Assembled [seq: %u]: ", seqNum );
			for ( i = 0; i < finalLength; i++ )
			{
				printf( "%02x", pAssembledData[i] );
			}
			printf ( "\n\n" );

			//g_pSequences[seqNum]->Reset();

			delete pAssembledData;
		}

	}

exit:
	return;
}

int main ( void )
{
	for ( uint32_t i = 0; i < 256; i++ )
		g_pSequences[i] = new CDataSequence( i );

	 // Setup sig alarm handler
        signal( SIGALRM, sig_alarm_handler );
        alarm( MAX_IDLE_SECS );

	signal( SIGABRT, sig_abort_handler );

        // XINETD service
        setvbuf( stdout, NULL, _IONBF, 0 );

	OpenDebugLog( DEBUG_LOG_FILENAME );

	LogDebug( LOG_PRIORITY_LOW, "Service started:\n" );

	RunReceiver();

	CloseDebugLog();

	for ( uint32_t i = 0; i < 256; i++ )
		delete g_pSequences[i];	

	exit( -1 );
	return 0;
}
