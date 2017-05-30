#include "common.h"

CStreamException::CStreamException( const char *pszReason, const char *pszFileName, uint32_t lineNumber )
	: m_sReason( pszReason ), m_sFileName( pszFileName ), m_lineNumber( lineNumber )
{

}

CStreamException::~CStreamException( )
{

}

bool CStreamException::GetExceptionString( string &sInString ) const
{
	char szExceptionText[1024];

	snprintf( szExceptionText, 1024, "StreamException::%s:%d::%s", m_sFileName.c_str(), m_lineNumber, m_sReason.c_str() );

	sInString = szExceptionText;

	return (true);
}

CStreamData::CStreamData( )
{
	for ( uint32_t i = 0; i < MAX_MEMORY_CHUNKS; i++ )
		m_memoryChunks[i] = NULL;

	m_lastChunk = 0;
	m_memoryWritePos = 0;
	m_memoryReadPos = 0;
}

CStreamData::~CStreamData( )
{
	Clear();
}

void CStreamData::Clear( void )
{
	for ( uint32_t i = 0; i < m_lastChunk; i++ )
	{
		if ( m_memoryChunks[i] )
			delete m_memoryChunks[i];

		m_memoryChunks[i] = NULL;
	}	

	m_memoryWritePos = 0;
	m_memoryReadPos = 0;
	m_lastChunk = 0;
}

uint32_t CStreamData::Write( const void* pData, uint32_t dataLen )
{
	uint32_t chunkIndex = m_memoryWritePos / MEMORY_CHUNK_SIZE;
	uint32_t chunkPos = m_memoryWritePos % MEMORY_CHUNK_SIZE;

	uint32_t writeRemaining = dataLen;
	uint32_t sourcePos = 0;

	while ( writeRemaining > 0 )
	{
		uint32_t writeAmount = writeRemaining;

		// Limit the write to inside this chunk
		if ( writeAmount > (MEMORY_CHUNK_SIZE - chunkPos) )
			writeAmount = (MEMORY_CHUNK_SIZE - chunkPos);

		if ( chunkIndex >= MAX_MEMORY_CHUNKS )
			ExceptionError( "Exception::Out of memory chunks for stream", __FILE__, __LINE__ );

		// Check if chunk is allocated...
		if ( !m_memoryChunks[chunkIndex] )
			m_memoryChunks[chunkIndex] = new uint8_t[MEMORY_CHUNK_SIZE];

		// Check memory
		if ( !m_memoryChunks[chunkIndex] )
			ExceptionError( "Exception::Out of memory for stream", __FILE__, __LINE__ );

		// Now copy in write amount
		memcpy( m_memoryChunks[chunkIndex] + chunkPos, (uint8_t*)pData + sourcePos, writeAmount );

		// Update
		writeRemaining -= writeAmount;
		sourcePos += writeAmount;

		// Next chunk
		chunkIndex++;

		// Remember last chunk
		if ( chunkIndex > m_lastChunk )
			m_lastChunk = chunkIndex;

		// Start of chunk
		chunkPos = 0;
	}

	m_memoryWritePos += dataLen;

	return (dataLen);
}

uint32_t CStreamData::Read( const void *pData, uint32_t dataLen, bool bPeak )
{
	uint32_t chunkIndex = m_memoryReadPos / MEMORY_CHUNK_SIZE;
	uint32_t chunkPos = m_memoryReadPos % MEMORY_CHUNK_SIZE;

	if ( GetReadAvailable() < dataLen )
		ExceptionError( "Exception::Insufficient data in stream", __FILE__, __LINE__ );

	uint32_t readRemaining = dataLen;
	uint32_t destPos = 0;

	while ( readRemaining > 0 )
	{
		uint32_t readAmount = readRemaining;

		if ( readAmount > (MEMORY_CHUNK_SIZE - chunkPos) )
			readAmount = (MEMORY_CHUNK_SIZE - chunkPos);

		if ( chunkIndex >= MAX_MEMORY_CHUNKS )
			ExceptionError( "Exception::Out of memory chunks for stream", __FILE__, __LINE__ );

		if ( !m_memoryChunks[chunkIndex] )
			ExceptionError( "Exception::Out of memory for stream", __FILE__, __LINE__ );

		// Copy data...
		memcpy( (uint8_t*)pData+destPos, m_memoryChunks[chunkIndex]+chunkPos, readAmount );

		// Update
		readRemaining -= readAmount;
		destPos += readAmount;

		// Next chunk
		chunkIndex++;

		// Start of chunk
		chunkPos = 0;
	}
	
	if ( !bPeak )
		m_memoryReadPos += dataLen;

	return (dataLen);
}

uint32_t CStreamData::Peak( const void *pData, uint32_t dataLen )
{
	return Read( pData, dataLen, true );
}

uint32_t CStreamData::GetReadAvailable( void )
{
	return (m_memoryWritePos - m_memoryReadPos);
}

uint32_t CStreamData::SkipForward( uint32_t dataLen )
{
	if ( GetReadAvailable() < dataLen )
		dataLen = GetReadAvailable();

	m_memoryReadPos += dataLen;

	return (dataLen);
}

void CStreamData::ExceptionError( const char *pszString, const char *pszFileName, uint32_t lineNumber )
{
	throw CStreamException( pszString, pszFileName, lineNumber );

	// TODO: Throw an exception here
	printf( "%s in %s:%d\n", pszString, pszFileName, lineNumber );
}
