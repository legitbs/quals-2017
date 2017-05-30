#ifndef __STREAM_DATA_H__
#define __STREAM_DATA_H__

class CStreamException
{
public:
	CStreamException( const char *pszReason, const char *pszFileName, uint32_t lineNumber );
	~CStreamException( );

	bool GetExceptionString( string &sInString ) const;
	
	string &GetReason( void ) { return m_sReason; };
	string &GetFileName( void ) { return m_sFileName; };
	uint32_t GetLineNumber( void ) { return m_lineNumber; };

private:
	string m_sReason;
	string m_sFileName;
	uint32_t m_lineNumber;
};

class CStreamData
{
public:
	static const uint32_t MAX_MEMORY_CHUNKS = (420);
	static const uint32_t MEMORY_CHUNK_SIZE	= (256);

public:
	CStreamData();
	~CStreamData();

	void Clear( void );	// Clear any current data
	
	uint32_t Write( const void* pData, uint32_t dataLen );
	uint32_t Read( const void* pData, uint32_t dataLen, bool bPeak = false );
	uint32_t Peak( const void* pData, uint32_t dataLen );

	uint32_t GetReadAvailable( void );
	uint32_t SkipForward( uint32_t dataLen );

private:
	void ExceptionError( const char *pszString, const char *pszFileName, uint32_t lineNumber );

private:
	uint8_t *m_memoryChunks[MAX_MEMORY_CHUNKS];
	
	uint32_t m_memoryWritePos;
	uint32_t m_memoryReadPos;

	uint32_t m_lastChunk;
};

#endif // __STREAM_DATA_H__
