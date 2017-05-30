#include "common.h"

void CRC4Coprocessor::Emulate( CCPU *pCPU, uint32_t instr, uint32_t pc )
{
	uint32_t func_num = funct( instr );

	switch ( func_num )
	{
	case 0:
		init_emulate( pCPU, instr, pc );
		break;

	case 1:
		encrypt_emulate( pCPU, instr, pc );
		break;

	default:
		// THROW coprocessor unimplemented exception
		pCPU->doException( CCPU::EXCEPTION_CpU, MEMACC_DATALOAD );
		return;
	}
}

void CRC4Coprocessor::init_emulate( CCPU *pCPU, uint32_t instr, uint32_t pc )
{
	uint8_t keyBuf[256]; 	// Fix RSA bug

	uint8_t keySize = (pCPU->getReg( (uint8_t)rt( instr ) ) & 0x7F);
	uint32_t keyAddress = (pCPU->getReg( (uint8_t)rs( instr ) ));

	// DEBUG:
#if STACK_DEBUG
	printf( "Init: From %X copying %d bytes.\nSP: %X\n", keyAddress, keySize, &keyBuf[0] );
#endif
	// BUG:: We assumed a 32 * 8 = 256 bit key.  They can actually load up to a 128 * 8 (or 1024 bit key)
	// Now load in keyBuf...
	if ( !pCPU->m_pMMU->ReadMemory( keyAddress, keyBuf, keySize ) )
	{
		pCPU->doException( CCPU::EXCEPTION_AdEL, MEMACC_DATALOAD );
		return;
	}


	// Now Init KSA algorithm
	uint32_t dropAmount = (pCPU->getReg( (uint8_t)rd( instr ))) & 0x3FF;

	// Call Init
	InitKSA( keyBuf, keySize, dropAmount );

	// DONE
}

void CRC4Coprocessor::encrypt_emulate( CCPU *pCPU, uint32_t instr, uint32_t pc )
{
	// RC4 ENCRYPT BLOCK
	uint8_t rsaBlock[512];

	uint32_t blockSize = (pCPU->getReg( (uint8_t)rt( instr ) ) & 0x1FF);
	uint32_t blockSrcAddress = (pCPU->getReg( (uint8_t)rs( instr ) ));
	uint32_t blockDestAddress = (pCPU->getReg( (uint8_t)rd( instr ) ));

#if STACK_DEBUG
	printf( "Encr: SRC %X(%d) DEST %X(%d) copying %d(%d) bytes.\nSP: %X\n", blockSrcAddress, rs( instr ), blockDestAddress, rd(instr), blockSize, rt(instr), &rsaBlock[0] );
#endif

	if ( !pCPU->m_pMMU->ReadMemory( blockSrcAddress, rsaBlock, blockSize ) )
	{
		pCPU->doException( CCPU::EXCEPTION_AdEL, MEMACC_DATALOAD );
		return;
	}
			
	// Call Encrypt
	RC4Block( rsaBlock, blockSize );

	// WRITE BACK TO DESTINATION
	if ( !pCPU->m_pMMU->WriteMemory( blockDestAddress, rsaBlock, blockSize ) )
	{
		pCPU->doException( CCPU::EXCEPTION_AdES, MEMACC_DATALOAD );
		return;
	}

	// DONE
}

void CRC4Coprocessor::InitKSA( uint8_t *pKeyData, uint8_t keyLen, uint32_t dropAmount )
{
	unsigned int i,j=0,t; 
	   
	for (i=0; i < 256; ++i)
		m_state[i] = i; 
	
	for (i=0; i < 256; ++i) 
	{
		j = (j + m_state[i] + pKeyData[i % keyLen]) % 256; 
		t = m_state[i]; 
		m_state[i] = m_state[j]; 
		m_state[j] = t; 
	}   

	unsigned int x;
	for (x=0; x < dropAmount; ++x)  
	{
		i = (i + 1) % 256; 
		j = (j + m_state[i]) % 256; 
		t = m_state[i]; 
		m_state[i] = m_state[j]; 
		m_state[j] = t; 
	}
}

void CRC4Coprocessor::RC4Block( uint8_t *pBlock, uint8_t blockLen )
{
	int i=0,j=0,x,t; 
   
	for (x=0; x < blockLen; ++x)  
	{
		i = (i + 1) % 256; 
		j = (j + m_state[i]) % 256; 
		t = m_state[i]; 
		m_state[i] = m_state[j]; 
		m_state[j] = t; 
		pBlock[x] = m_state[(m_state[i] + m_state[j]) % 256] ^ pBlock[x];
	}   
}
