#include "meb_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/evp.h>



bool CMebFile::AddHeader( uint32_t version, uint32_t info, uint32_t errno_addr )
{
	m_oHeader.magic[0] = MEBHDR_MAGIC0;
	m_oHeader.magic[1] = MEBHDR_MAGIC1;
	m_oHeader.magic[2] = MEBHDR_MAGIC2;
	m_oHeader.magic[3] = MEBHDR_MAGIC3;
	m_oHeader.version = version;
	m_oHeader.info = info;
	m_oHeader.errno_addr = errno_addr;

	m_oHeader.lsec_num = 0;
	m_oHeader.lsec_offset = 0;
	m_oHeader.rsec_num = 0;
	m_oHeader.rsec_offset = 0;
	m_oHeader.sig_offset = 0;

	return (true);
}


bool CMebFile::AddLSec( uint32_t startAddr, uint32_t memSize, uint8_t sectionType, uint8_t *pData, uint32_t dataSize )
{
	if ( pData == NULL )
		return false;

	m_oHeader.lsec_num++;
	
	Meb32_LSec oSection;

	oSection.fileOffset = 0;
	oSection.fileSize = dataSize;
	oSection.memSize = memSize;
	oSection.sectionType = sectionType;
	oSection.startAddr = startAddr; 

	m_oLSecs.push_back( oSection );

	m_oLSecData.push_back( pData );

	return (true);
}

bool CMebFile::AddRSec( uint8_t regNum, uint32_t regValue )
{
	m_oHeader.rsec_num++;

	Meb32_RSec oSection;

	oSection.regNum = regNum;
	oSection.regValue = regValue;

	m_oRSecs.push_back( oSection );

	return (true);
}

bool CMebFile::AddSignature( Crypto &oCrypto )
{
	m_pCrypto = &oCrypto;
	return (true);
}

bool CMebFile::GenerateFile( uint8_t **pOutData, uint32_t &outSize )
{
	// OK create the file first calculate the offsets...
	uint32_t LSecOffset = (sizeof(m_oHeader));
	uint32_t RSecOffset = (LSecOffset + sizeof(Meb32_LSec) * m_oHeader.lsec_num);
	uint32_t DataOffset = (RSecOffset + sizeof(Meb32_RSec) * m_oHeader.rsec_num);

	// Now set the headers appropriately
	m_oHeader.lsec_offset = LSecOffset;
	m_oHeader.rsec_offset = RSecOffset;

	// Now generate the data size!
	uint32_t fileSize = (DataOffset);

	for ( uint16_t i = 0; i < m_oHeader.lsec_num; i++ )
	{
		m_oLSecs[i].fileOffset = fileSize;
		fileSize += m_oLSecs[i].fileSize;
	}

	// Set signature offset as last item in file
	m_oHeader.sig_offset = fileSize;

	// Now update file size
	fileSize += sizeof(Meb32_Sig);

	// OK we have the file size now... allocate space!
	uint8_t *pFileData = new uint8_t[fileSize];

	// Create file
	memcpy( pFileData, &m_oHeader, sizeof(m_oHeader) );
	
	// Copy LSecs
	for ( uint16_t i = 0; i < m_oHeader.lsec_num; i++ )
	{
		memcpy( (pFileData+LSecOffset+(i*sizeof(Meb32_LSec))), &(m_oLSecs[i]), sizeof(Meb32_LSec) );
	}

	// Copy RSecs
	for ( uint16_t i = 0; i < m_oHeader.rsec_num; i++ )
	{
		memcpy( (pFileData+RSecOffset+(i*sizeof(Meb32_RSec))), &(m_oRSecs[i]), sizeof(Meb32_RSec) );
	}

	// Now copy in the data sections
	uint32_t curPos = (DataOffset);
	for ( uint16_t i = 0; i < m_oLSecData.size(); i++ )
	{
		memcpy( (pFileData+curPos), m_oLSecData[i], m_oLSecs[i].fileSize );

		curPos += m_oLSecs[i].fileSize;
	}

	// Generate signature...
	unsigned char signature[4096];
	uint32_t signatureSize;

	if ( m_pCrypto->rsaSign( (const unsigned char*)pFileData, curPos, signature, &signatureSize ) != 256 )
	{
		printf( "Failed signature.\n" );
		return (false);
	}

	if ( signatureSize != 256 )
	{
		printf( "Must be RSA-2048.\n" );
		return false;
	}

	// Set signature
	memcpy( m_oSignature.signed_hash, signature, signatureSize );
	
	// Copy signature
	memcpy( (pFileData+curPos), &m_oSignature, sizeof(Meb32_Sig) );

	(*pOutData) = pFileData;
	outSize = fileSize;

	return true;
}
