#include "common.h"

#include <stdio.h>
#include <stdlib.h>

typedef void (CCPU::*tEmulateFptr)( uint32_t instr, uint32_t pc );

CCPU::CCPU() : m_pMMU( NULL )
{
	reset();
}

CCPU::~CCPU()
{

}

void CCPU::step( void )
{
	// Table of emulation functions.
	static const tEmulateFptr opcodeJumpTable[] = 
	{
        &CCPU::funct_emulate, &CCPU::regimm_emulate,  &CCPU::j_emulate,
        &CCPU::jal_emulate,   &CCPU::beq_emulate,     &CCPU::bne_emulate,
        &CCPU::blez_emulate,  &CCPU::bgtz_emulate,    &CCPU::addi_emulate,
        &CCPU::addiu_emulate, &CCPU::slti_emulate,    &CCPU::sltiu_emulate,
        &CCPU::andi_emulate,  &CCPU::ori_emulate,     &CCPU::xori_emulate,
        &CCPU::lui_emulate,   &CCPU::cpzero_emulate,  &CCPU::cpone_emulate,
        &CCPU::cptwo_emulate, &CCPU::cpthree_emulate, &CCPU::RI_emulate,
        &CCPU::RI_emulate,    &CCPU::RI_emulate,      &CCPU::RI_emulate,
        &CCPU::RI_emulate,    &CCPU::RI_emulate,      &CCPU::RI_emulate,
        &CCPU::RI_emulate,    &CCPU::RI_emulate,      &CCPU::RI_emulate,
        &CCPU::RI_emulate,    &CCPU::RI_emulate,      &CCPU::lb_emulate,
        &CCPU::lh_emulate,    &CCPU::lwl_emulate,     &CCPU::lw_emulate,
        &CCPU::lbu_emulate,   &CCPU::lhu_emulate,     &CCPU::lwr_emulate,
        &CCPU::RI_emulate,    &CCPU::sb_emulate,      &CCPU::sh_emulate,
        &CCPU::swl_emulate,   &CCPU::sw_emulate,      &CCPU::RI_emulate,
        &CCPU::RI_emulate,    &CCPU::swr_emulate,     &CCPU::RI_emulate,
        &CCPU::RI_emulate,    &CCPU::lwc1_emulate,    &CCPU::lwc2_emulate,
        &CCPU::lwc3_emulate,  &CCPU::RI_emulate,      &CCPU::RI_emulate,
        &CCPU::RI_emulate,    &CCPU::RI_emulate,      &CCPU::RI_emulate,
        &CCPU::swc1_emulate,  &CCPU::swc2_emulate,    &CCPU::swc3_emulate,
        &CCPU::RI_emulate,    &CCPU::RI_emulate,      &CCPU::RI_emulate,
        &CCPU::RI_emulate
    };

	// Clear exception_pending flag if it was set by a prior instruction.
	m_exceptionPending = false;

	// Decrement Random register every clock cycle.
	// cpzero->adjust_random();

	// Save address of instruction responsible for exceptions which may occur.
	if (m_delayState != DELAYSTATE_DELAYSLOT)
		m_nextEpc = m_pc;

	
	// Fetch next instruction.
	uint32_t curInstr = m_pMMU->fetchWord( m_pc, MEMACC_INSTFETCH, this );
	if (m_exceptionPending) 
	{
		goto out;
	}

#if DEBUG_INSTR_TRACING
	printf( "PC(%X) INSTR(%x) GP(%08x)\n", m_pc, curInstr, m_regs[28] );
#endif

	// Emulate the instruction by jumping to the appropriate emulation method.
	(this->*opcodeJumpTable[opcode(curInstr)])(curInstr, m_pc);

	// Update instruction counter
	m_instrCount++;

	if ( m_instrCount > MAX_CPU_INSTRUCTION_COUNT )
	{
		doException( CCPU::EXCEPTION_MAXINSTR );
	}

out:
	// Force register zero to contain zero.
	m_regs[REG_ZERO] = 0;

	// If an exception is pending, then the PC has already been changed to
    // contain the exception vector.  Return now, so that we don't clobber it.
	if (m_exceptionPending) 
	{
		// Instruction at beginning of exception handler is NOT in delay slot,
        // no matter what the last instruction was.
		m_delayState = DELAYSTATE_NORMAL;
		return;
	}

    // Recall the delay_state values: 0=NORMAL, 1=DELAYING, 2=DELAYSLOT.
    // This is what the delay_state values mean (at this point in the code):
	// DELAYING: The last instruction caused a branch to be taken.
	//  The next instruction is in the delay slot.
	//  The next instruction EPC will be PC - 4.
	// DELAYSLOT: The last instruction was executed in a delay slot.
	//  The next instruction is on the other end of the branch.
	//  The next instruction EPC will be PC.
	// NORMAL: No branch was executed; next instruction is at PC + 4.
	//  Next instruction EPC is PC.

    // Update the pc and delay_state values.
    m_pc += 4;
    if (m_delayState == DELAYSTATE_DELAYSLOT)
		m_pc = m_delayPC;

	// Move delay state
    advanceDelayState();
}

void CCPU::reset( void )
{
	for ( uint8_t i = 0; i < 32; i++ )
		m_regs[i] = 0;

	m_pc = 0;
	m_instr = 0;
	m_hi = 0;
	m_lo = 0;

	m_nextEpc = 0;
	m_lastEpc = 0;

	m_delayState = DELAYSTATE_NORMAL;
	m_delayPC = 0;

	// Reset instruction counter
	m_instrCount = 0;

	m_exceptionPending = false;
}

bool CCPU::setup( CMMU *pMMU, CSysCallHandler *pSysCallHandler, uint32_t errnoAddress, bool bigEndian, uint32_t startPC )
{
	m_pMMU = pMMU;
	m_pSysCallHandler = pSysCallHandler;

	if ( m_pMMU == NULL || m_pSysCallHandler == NULL )
		return (false);

	// Reset cpu
	reset();

	// Setup exception handler address and endianness
	m_errnoAddress = errnoAddress;
	m_optBigendian = bigEndian;

	// Set PC
	m_pc = startPC;

	return (true);
}

bool CCPU::setRegisterAndPC( uint8_t regNum, uint32_t value )
{
	if ( regNum == 32 )
	{
		m_pc = value;
		return (true);
	}
	else if ( regNum < 32 )
	{
		m_regs[regNum] = value;
		return true;
	}
	else
		return false;
}

void CCPU::funct_emulate(uint32_t instr, uint32_t pc)
{
	static const tEmulateFptr functJumpTable[] = {
		&CCPU::sll_emulate,     &CCPU::RI_emulate,
		&CCPU::srl_emulate,     &CCPU::sra_emulate,
		&CCPU::sllv_emulate,    &CCPU::RI_emulate,
		&CCPU::srlv_emulate,    &CCPU::srav_emulate,
		&CCPU::jr_emulate,      &CCPU::jalr_emulate,
		&CCPU::RI_emulate,      &CCPU::RI_emulate,
		&CCPU::syscall_emulate, &CCPU::break_emulate,
		&CCPU::RI_emulate,      &CCPU::RI_emulate,
		&CCPU::mfhi_emulate,    &CCPU::mthi_emulate,
		&CCPU::mflo_emulate,    &CCPU::mtlo_emulate,
		&CCPU::RI_emulate,      &CCPU::RI_emulate,
		&CCPU::RI_emulate,      &CCPU::RI_emulate,
		&CCPU::mult_emulate,    &CCPU::multu_emulate,
		&CCPU::div_emulate,     &CCPU::divu_emulate,
		&CCPU::RI_emulate,      &CCPU::RI_emulate,
		&CCPU::RI_emulate,      &CCPU::RI_emulate,
		&CCPU::add_emulate,     &CCPU::addu_emulate,
		&CCPU::sub_emulate,     &CCPU::subu_emulate,
		&CCPU::and_emulate,     &CCPU::or_emulate,
		&CCPU::xor_emulate,     &CCPU::nor_emulate,
		&CCPU::RI_emulate,      &CCPU::RI_emulate,
		&CCPU::slt_emulate,     &CCPU::sltu_emulate,
		&CCPU::RI_emulate,      &CCPU::RI_emulate,
		&CCPU::RI_emulate,      &CCPU::RI_emulate,
		&CCPU::RI_emulate,      &CCPU::RI_emulate,
		&CCPU::RI_emulate,      &CCPU::RI_emulate,
		&CCPU::RI_emulate,      &CCPU::RI_emulate,
		&CCPU::RI_emulate,      &CCPU::RI_emulate,
		&CCPU::RI_emulate,      &CCPU::RI_emulate,
		&CCPU::RI_emulate,      &CCPU::RI_emulate,
		&CCPU::RI_emulate,      &CCPU::RI_emulate,
		&CCPU::RI_emulate,      &CCPU::RI_emulate
	};
	(this->*functJumpTable[funct(instr)])(instr, pc);
}

void CCPU::regimm_emulate(uint32_t instr, uint32_t pc)
{
	switch(rt(instr))
	{
		case 0: 
			bltz_emulate(instr, pc); 
			break;

		case 1: 
			bgez_emulate(instr, pc); 
			break;

		case 16: 
			bltzal_emulate(instr, pc); 
			break;

		case 17: 
			bgezal_emulate(instr, pc); 
			break;

		default: 
			doException(EXCEPTION_RI); 
			break; // reserved instruction
	}
}

void CCPU::doException(uint16_t excCode, int mode /* = ANY */, int coprocno /* = -1 */)
{
	uint32_t epc;
	bool delaying = (m_delayState == DELAYSTATE_DELAYSLOT);

	typedef struct
	{
		const char *pszExceptionString;
		unsigned int exceptionNumber;
	} tExceptionNameTable;

	/*
	static const int EXCEPTION_INT = 0;
	static const int EXCEPTION_MOD = 1;
	static const int EXCEPTION_TLBL = 2;
	static const int EXCEPTION_TLBS = 3;
	static const int EXCEPTION_AdEL = 4;
	static const int EXCEPTION_AdES = 5;
	static const int EXCEPTION_IBE = 6;
	static const int EXCEPTION_DBE = 7;
	static const int EXCEPTION_Sys = 8;
	static const int EXCEPTION_Bp = 9;
	static const int EXCEPTION_RI = 10;
	static const int EXCEPTION_CpU = 11;
	static const int EXCEPTION_Ov = 12;
	static const int EXCEPTION_Tr = 13;
	static const int EXCEPTION_RESERVED1 = 14;
	static const int EXCEPTION_FPE = 15;
	static const int EXCEPTION_C2E = 18;
	static const int EXCEPTION_MDMX = 22;
	static const int EXCEPTION_WATCH = 23;
	static const int EXCEPTION_MCheck = 24;
	static const int EXCEPTION_MAXINSTR	= 25;
	static const int EXCEPTION_HSC = 26;			// Halt and Spontaneously Combust
	static const int EXCEPTION_CacheErr = 30;
	*/

	static tExceptionNameTable oExceptionNameTable[] = {
		{ "Integer overflow",			0 },
		{ "Modulus",					1 },
		{ "TLBL",						2 },
		{ "TLBS",						3 },
		{ "Invalid address on Load",	4 },
		{ "Invalid address on store",	5 },
		{ "IBE",						6 },
		{ "DBE",						7 },
		{ "Sys",						8 },
		{ "Bp",							9 },
		{ "RI",							10 },
		{ "Coprocessor Unimplemented",	11 },
		{ "Ov",							12 },
		{ "Tr",							13 },
		{ "RESERVED",					14 },
		{ "FPE",						15 },
		{ "C2E",						18 },
		{ "MDMX",						22 },
		{ "WATCH",						23 },
		{ "MCheck",						24 },
		{ "Max instructions executed",	25 },
		{ "Halt and Spontaneously Combust", 26 },
		{ "Cache Error",				30 },
		{ "",							100 }
	};
	
	bool bFound = false;
	unsigned int idx = 0;
	for ( idx = 0; ; idx++ )
	{
		if ( oExceptionNameTable[idx].exceptionNumber == 100 )
		{
			bFound = false;
			break;
		}

		if ( oExceptionNameTable[idx].exceptionNumber == excCode )
		{
			bFound = true;
			break;
		}
	}

	if ( bFound )
	{
		printf( "Exception: %s\nPC: %X\n", oExceptionNameTable[idx].pszExceptionString, m_nextEpc );
	}
	else
	{
		printf( "Exception: Unknown\nPC: %X\n", m_nextEpc );
		return;
	}

	exit(1);
	// step() ensures that next_epc will always contain the correct
	// EPC whenever exception() is called.
	//
	epc = m_nextEpc;

	// Do not prioritize exceptions
	m_lastEpc = epc;

	// Set processor to Kernel mode, disable interrupts, and save 
	// exception PC.
	//
#if 0
	cpzero->enter_exception(epc,excCode,coprocno,delaying);
#endif 

	m_exceptionPending = true;
}

void CCPU::SetErrno( uint32_t newErrno )
{
	if ( !m_pMMU->WriteMemory( m_errnoAddress, (uint8_t*)&newErrno, sizeof(errno) ) )
	{
		doException( EXCEPTION_AdES );
		return;
	}
}

/* emulation of instructions */
void CCPU::cpzero_emulate(uint32_t instr, uint32_t pc)
{
#if 0
	cpzero->cpzero_emulate(instr, pc);
#endif 
}

// Called when the program wants to use coprocessor COPROCNO, and there
// isn't any implementation for that coprocessor.
// Results in a Coprocessor Unusable exception, along with an error
// message being printed if the coprocessor is marked usable in the
// CP0 Status register.
//
void CCPU::cop_unimpl (int coprocno, uint32_t instr, uint32_t pc)
{
	doException (EXCEPTION_CpU, MEMACC_ANY, coprocno);
}

void CCPU::cpone_emulate(uint32_t instr, uint32_t pc)
{
	cop_unimpl (1, instr, pc);
}

void CCPU::cptwo_emulate(uint32_t instr, uint32_t pc)
{
	m_cptwo.Emulate( this, instr, pc );
}

void CCPU::cpthree_emulate(uint32_t instr, uint32_t pc)
{
	cop_unimpl (3, instr, pc);
}


void CCPU::control_transfer( uint32_t newPC )
{
	m_delayState = DELAYSTATE_DELAYING;
	m_delayPC = newPC;
}

/// calc_jump_target - Calculate the address to jump to as a result of
/// the J-format (jump) instruction INSTR at address PC.  (PC is the address
/// of the jump instruction, and INSTR is the jump instruction word.)
///
uint32_t CCPU::calc_jump_target (uint32_t instr, uint32_t pc)
{
    // Must use address of delay slot (pc + 4) to calculate.
	return ((pc + 4) & 0xf0000000) | (jumptarg(instr) << 2);
}

void CCPU::jump(uint32_t instr, uint32_t pc)
{
    control_transfer (calc_jump_target (instr, pc));
}

void CCPU::j_emulate(uint32_t instr, uint32_t pc)
{
	jump (instr, pc);
}

void CCPU::jal_emulate(uint32_t instr, uint32_t pc)
{
    jump (instr, pc);
	// RA gets addr of instr after delay slot (2 words after this one).
	m_regs[REG_RA] = pc + 8;
}

/// calc_branch_target - Calculate the address to jump to for the
/// PC-relative branch for which the offset is specified by the immediate field
/// of the branch instruction word INSTR, with the program counter equal to PC.
/// 
uint32_t CCPU::calc_branch_target(uint32_t instr, uint32_t pc)
{
	return (pc + 4) + (s_immed(instr) << 2);
}

void CCPU::branch(uint32_t instr, uint32_t pc)
{
    control_transfer (calc_branch_target (instr, pc));
}

void CCPU::beq_emulate(uint32_t instr, uint32_t pc)
{
	if (m_regs[rs(instr)] == m_regs[rt(instr)])
        branch (instr, pc);
}

void CCPU::bne_emulate(uint32_t instr, uint32_t pc)
{
	if (m_regs[rs(instr)] != m_regs[rt(instr)])
        branch (instr, pc);
}

void CCPU::blez_emulate(uint32_t instr, uint32_t pc)
{
	if (rt(instr) != 0) 
	{
		doException(EXCEPTION_RI);
		return;
    }

	if (m_regs[rs(instr)] == 0 || (m_regs[rs(instr)] & 0x80000000))
		branch(instr, pc);
}

void CCPU::bgtz_emulate(uint32_t instr, uint32_t pc)
{
	if (rt(instr) != 0) 
	{
		doException(EXCEPTION_RI);
		return;
	}

	if (m_regs[rs(instr)] != 0 && (m_regs[rs(instr)] & 0x80000000) == 0)
		branch(instr, pc);
}

void CCPU::addi_emulate(uint32_t instr, uint32_t pc)
{
	int32_t a, b, sum;

	a = (int32_t)m_regs[rs(instr)];
	b = s_immed(instr);
	sum = a + b;

	if ((a < 0 && b < 0 && !(sum < 0)) || (a >= 0 && b >= 0 && !(sum >= 0))) 
	{
		doException(EXCEPTION_Ov);
		return;
	} 
	else 
	{
		m_regs[rt(instr)] = (uint32_t)sum;
	}
}

void CCPU::addiu_emulate(uint32_t instr, uint32_t pc)
{
	int32_t a, b, sum;

	a = (int32_t)m_regs[rs(instr)];
	b = s_immed(instr);
	sum = a + b;
	m_regs[rt(instr)] = (uint32_t)sum;
}

void CCPU::slti_emulate(uint32_t instr, uint32_t pc)
{
	int32_t s_rs = m_regs[rs(instr)];

	if (s_rs < s_immed(instr)) 
	{
		m_regs[rt(instr)] = 1;
	} 
	else 
	{
		m_regs[rt(instr)] = 0;
	}
}

void CCPU::sltiu_emulate(uint32_t instr, uint32_t pc)
{
	if (m_regs[rs(instr)] < (uint32_t)(int32_t)s_immed(instr)) 
	{
		m_regs[rt(instr)] = 1;
	} 
	else 
	{
		m_regs[rt(instr)] = 0;
	}
}

void CCPU::andi_emulate(uint32_t instr, uint32_t pc)
{
	m_regs[rt(instr)] = (m_regs[rs(instr)] & 0x0ffff) & immed(instr);
}

void CCPU::ori_emulate(uint32_t instr, uint32_t pc)
{
	m_regs[rt(instr)] = m_regs[rs(instr)] | immed(instr);
}

void CCPU::xori_emulate(uint32_t instr, uint32_t pc)
{
	m_regs[rt(instr)] = m_regs[rs(instr)] ^ immed(instr);
}

void CCPU::lui_emulate(uint32_t instr, uint32_t pc)
{
	m_regs[rt(instr)] = immed(instr) << 16;
}

void CCPU::lb_emulate(uint32_t instr, uint32_t pc)
{
	uint32_t addr, base;
	int8_t byte;
	int32_t offset;

	// Calculate address
	base = m_regs[rs(instr)];
	offset = s_immed(instr);
	addr = base + offset;

	// Fetch byte.
	// Because it is assigned to a signed variable (int32 byte)
	// it will be sign-extended.
	byte = m_pMMU->fetchByte(addr, MEMACC_DATALOAD, this);
	if (m_exceptionPending) 
		return;

	/* Load target register with data. */
	m_regs[rt(instr)] = byte;
}

void CCPU::lh_emulate(uint32_t instr, uint32_t pc)
{
	uint32_t addr, base;
	int16_t halfword;
	int32_t offset;

	// Calculate virtual address.
	base = m_regs[rs(instr)];
	offset = s_immed(instr);
	addr = base + offset;

	/* This virtual address must be halfword-aligned. */
	if ( (addr % 2) != 0) 
	{
		doException(EXCEPTION_AdEL,MEMACC_DATALOAD);
		return;
	}

	/* Fetch halfword.
	 * Because it is assigned to a signed variable (int32 halfword)
	 * it will be sign-extended.
	 */
	halfword = m_pMMU->fetchHalfword(addr, MEMACC_DATALOAD, this);
	if (m_exceptionPending) 
		return;

	/* Load target register with data. */
	m_regs[rt(instr)] = halfword;
}

/* The lwr and lwl algorithms here are taken from SPIM 6.0,
 * since I didn't manage to come up with a better way to write them.
 * Improvements are welcome.
 */
uint32_t CCPU::lwr(uint32_t regval, uint32_t memval, uint8_t offset)
{
	if (m_optBigendian) 
	{
		switch (offset)
		{
			case 0: return (regval & 0xffffff00) |
						((unsigned)(memval & 0xff000000) >> 24);
			case 1: return (regval & 0xffff0000) |
						((unsigned)(memval & 0xffff0000) >> 16);
			case 2: return (regval & 0xff000000) |
						((unsigned)(memval & 0xffffff00) >> 8);
			case 3: return memval;
		}
	} 
	else // if MIPS target is little endian
	{
		switch (offset)
		{
			/* The SPIM source claims that "The description of the
			 * little-endian case in Kane is totally wrong." The fact
			 * that I ripped off the LWR algorithm from them could be
			 * viewed as a sort of passive assumption that their claim
			 * is correct.
			 */
			case 0: /* 3 in book */
				return memval;
			case 1: /* 0 in book */
				return (regval & 0xff000000) | ((memval & 0xffffff00) >> 8);
			case 2: /* 1 in book */
				return (regval & 0xffff0000) | ((memval & 0xffff0000) >> 16);
			case 3: /* 2 in book */
				return (regval & 0xffffff00) | ((memval & 0xff000000) >> 24);
		}
	}

	// SHOULD NEVER GET HERE
	return (0);
}

uint32_t CCPU::lwl(uint32_t regval, uint32_t memval, uint8_t offset)
{
	if (m_optBigendian) 
	{
		switch (offset)
		{
			case 0: return memval;
			case 1: return (memval & 0xffffff) << 8 | (regval & 0xff);
			case 2: return (memval & 0xffff) << 16 | (regval & 0xffff);
			case 3: return (memval & 0xff) << 24 | (regval & 0xffffff);
		}
	} 
	else // if MIPS target is little endian 
	{
		switch (offset)
		{
			case 0: return (memval & 0xff) << 24 | (regval & 0xffffff);
			case 1: return (memval & 0xffff) << 16 | (regval & 0xffff);
			case 2: return (memval & 0xffffff) << 8 | (regval & 0xff);
			case 3: return memval;
		}
	}

	// SHOULD NEVER GET HERE
	return (0);
}

void CCPU::lwl_emulate(uint32_t instr, uint32_t pc)
{
	uint32_t addr, wordaddr, base, memword;
	uint8_t which_byte;
	int32_t offset;

	/* Calculate virtual address. */
	base = m_regs[rs(instr)];
	offset = s_immed(instr);
	addr = base + offset;
	/* We request the word containing the byte-address requested. */
	wordaddr = addr & ~0x03UL;

	/* Fetch word. */
	memword = m_pMMU->fetchWord(wordaddr, MEMACC_DATALOAD, this);
	if (m_exceptionPending) 
		return;
	
	/* Insert bytes into the left side of the register. */
	which_byte = addr & 0x03;
	m_regs[rt(instr)] = lwl(m_regs[rt(instr)], memword, which_byte);
}

void
CCPU::lw_emulate(uint32_t instr, uint32_t pc)
{
	uint32_t addr, base, word;
	int32_t offset;

	/* Calculate virtual address. */
	base = m_regs[rs(instr)];
	offset = s_immed(instr);
	addr = base + offset;

	/* This virtual address must be word-aligned. */
	if (addr % 4 != 0) 
	{
		doException(EXCEPTION_AdEL, MEMACC_DATALOAD);
		return;
	}

	/* Fetch word. */
	word = m_pMMU->fetchWord(addr, MEMACC_DATALOAD, this);
	if (m_exceptionPending) 
		return;

	/* Load target register with data. */
	m_regs[rt(instr)] = word;
}

void
CCPU::lbu_emulate(uint32_t instr, uint32_t pc)
{
	uint32_t addr, base, byte;
	int32_t offset;

	/* Calculate virtual address. */
	base = m_regs[rs(instr)];
	offset = s_immed(instr);
	addr = base + offset;

	/* Fetch byte.  */
	byte = m_pMMU->fetchByte(addr, MEMACC_DATALOAD, this) & 0x000000ff;
	if (m_exceptionPending) 
		return;
	
	/* Load target register with data. */
	m_regs[rt(instr)] = byte;
}

void
CCPU::lhu_emulate(uint32_t instr, uint32_t pc)
{
	uint32_t addr, base, halfword;
	int32_t offset;

	/* Calculate virtual address. */
	base = m_regs[rs(instr)];
	offset = s_immed(instr);
	addr = base + offset;

	/* This virtual address must be halfword-aligned. */
	if (addr % 2 != 0) 
	{
		doException(EXCEPTION_AdEL,MEMACC_DATALOAD);
		return;
	}

	/* Fetch halfword.  */
	halfword = m_pMMU->fetchHalfword(addr, MEMACC_DATALOAD, this) & 0x0000ffff;
	if (m_exceptionPending) 
		return;

	/* Load target register with data. */
	m_regs[rt(instr)] = halfword;
}

void
CCPU::lwr_emulate(uint32_t instr, uint32_t pc)
{
	uint32_t addr, wordaddr, base, memword;
	uint8_t which_byte;
	int32_t offset;

	/* Calculate virtual address. */
	base = m_regs[rs(instr)];
	offset = s_immed(instr);
	addr = base + offset;
	/* We request the word containing the byte-address requested. */
	wordaddr = addr & ~0x03UL;

	/* Fetch word. */
	memword = m_pMMU->fetchWord(wordaddr, MEMACC_DATALOAD, this);
	if (m_exceptionPending) 
		return;
	
	/* Insert bytes into the left side of the register. */
	which_byte = addr & 0x03;
	m_regs[rt(instr)] = lwr(m_regs[rt(instr)], memword, which_byte);
}

void
CCPU::sb_emulate(uint32_t instr, uint32_t pc)
{
	uint32_t addr, base;
	uint8_t data;
	int32_t offset;

	// Load data from register
	data = m_regs[rt(instr)] & 0x0ff;

	// Calculate address
	base = m_regs[rs(instr)];
	offset = s_immed(instr);
	addr = base + offset;

	// Store byte
	m_pMMU->storeByte(addr, data, this);
}

void
CCPU::sh_emulate(uint32_t instr, uint32_t pc)
{
	uint32_t addr, base;
	uint16_t data;
	int32_t offset;

	/* Load data from register. */
	data = m_regs[rt(instr)] & 0x0ffff;

	/* Calculate virtual address. */
	base = m_regs[rs(instr)];
	offset = s_immed(instr);
	addr = base + offset;

	// This virtual address must be halfword-aligned.
	if (addr % 2 != 0) 
	{
		doException(EXCEPTION_AdES, MEMACC_DATASTORE);
		return;
	}

	// Store halfword
	m_pMMU->storeHalfword(addr, data, this);
}

uint32_t CCPU::swl(uint32_t regval, uint32_t memval, uint8_t offset)
{
	if ( m_optBigendian ) 
	{
		switch (offset) 
		{
			case 0: return regval; 
			case 1: return (memval & 0xff000000) | (regval >> 8 & 0xffffff); 
			case 2: return (memval & 0xffff0000) | (regval >> 16 & 0xffff); 
			case 3: return (memval & 0xffffff00) | (regval >> 24 & 0xff); 
		}
	} 
	else // MIPS target is little endian
	{
		switch (offset) 
		{
			case 0: return (memval & 0xffffff00) | (regval >> 24 & 0xff); 
			case 1: return (memval & 0xffff0000) | (regval >> 16 & 0xffff); 
			case 2: return (memval & 0xff000000) | (regval >> 8 & 0xffffff); 
			case 3: return regval; 
		}
	}

	// SHOULD NOT GET HERE
	return (0);
}

uint32_t CCPU::swr(uint32_t regval, uint32_t memval, uint8_t offset)
{
	if ( m_optBigendian ) 
	{
		switch (offset) 
		{
			case 0: return ((regval << 24) & 0xff000000) | (memval & 0xffffff); 
			case 1: return ((regval << 16) & 0xffff0000) | (memval & 0xffff); 
			case 2: return ((regval << 8) & 0xffffff00) | (memval & 0xff); 
			case 3: return regval; 
		}
	} 
	else // MIPS target is little endian
	{
		switch (offset) 
		{
			case 0: return regval; 
			case 1: return ((regval << 8) & 0xffffff00) | (memval & 0xff); 
			case 2: return ((regval << 16) & 0xffff0000) | (memval & 0xffff); 
			case 3: return ((regval << 24) & 0xff000000) | (memval & 0xffffff); 
		}
	}

	// SHOULD NOT GET HERE
	return (0);
}

void CCPU::swl_emulate(uint32_t instr, uint32_t pc)
{
	uint32_t addr, wordaddr, base, regdata, memdata;
	int32_t offset;
	uint8_t which_byte;

	/* Load data from register. */
	regdata = m_regs[rt(instr)];

	/* Calculate virtual address. */
	base = m_regs[rs(instr)];
	offset = s_immed(instr);
	addr = base + offset;
	/* We request the word containing the byte-address requested. */
	wordaddr = addr & ~0x03UL;

	/* Read data from memory. */
	memdata = m_pMMU->fetchWord(wordaddr, MEMACC_DATASTORE, this);
	if (m_exceptionPending) 
		return;

	/* Write back the left side of the register. */
	which_byte = addr & 0x03UL;
	m_pMMU->storeByte(addr, swl(regdata, memdata, which_byte), this);
}

void CCPU::sw_emulate(uint32_t instr, uint32_t pc)
{
	uint32_t addr, base, data;
	int32_t offset; 

	/* Load data from register. */
	data = m_regs[rt(instr)];

	/* Calculate virtual address. */
	base = m_regs[rs(instr)];
	offset = s_immed(instr);
	addr = base + offset;

	/* This virtual address must be word-aligned. */
	if (addr % 4 != 0) 
	{
		doException(EXCEPTION_AdES, MEMACC_DATASTORE);
		return;
	}

	/* Store word. */
	m_pMMU->storeWord(addr, data, this);
}

void CCPU::swr_emulate(uint32_t instr, uint32_t pc)
{
	uint32_t addr, wordaddr, base, regdata, memdata;
	int32_t offset;
	uint8_t which_byte;

	/* Load data from register. */
	regdata = m_regs[rt(instr)];

	/* Calculate virtual address. */
	base = m_regs[rs(instr)];
	offset = s_immed(instr);
	addr = base + offset;
	/* We request the word containing the byte-address requested. */
	wordaddr = addr & ~0x03UL;

	/* Read data from memory. */
	memdata = m_pMMU->fetchWord(addr, MEMACC_DATASTORE, this);
	if (m_exceptionPending) 
		return;

	/* Write back the right side of the register. */
	which_byte = addr & 0x03UL;
	m_pMMU->storeWord(addr, swr(regdata, memdata, which_byte), this);
}

void CCPU::lwc1_emulate(uint32_t instr, uint32_t pc)
{
#if HAS_FPU
		uint32_t phys, virt, base, word;
		int32_t offset;
		bool cacheable;

		/* Calculate virtual address. */
		base = m_regs[rs(instr)];
		offset = s_immed(instr);
		virt = base + offset;

		/* This virtual address must be word-aligned. */
		if (virt % 4 != 0) {
			exception(AdEL,DATALOAD);
			return;
		}

		/* Translate virtual address to physical address. */
		phys = cpzero->address_trans(virt, DATALOAD, &cacheable, this);
		if (exception_pending) return;

		/* Fetch word. */
		word = mem->fetch_word(phys, DATALOAD, cacheable, this);
		if (exception_pending) return;

		/* Load target register with data. */
		fpu->write_reg (rt (instr), word);
#else
	cop_unimpl (1, instr, pc);
#endif
}

void CCPU::lwc2_emulate(uint32_t instr, uint32_t pc)
{
	cop_unimpl (2, instr, pc);
}

void CCPU::lwc3_emulate(uint32_t instr, uint32_t pc)
{
	if ( (instr & 0x3ffffff) == 0x3e00000 )
	{
		if ( m_delayState == DELAYSTATE_NORMAL )
		{
			// Set RA = pc;
			m_regs[31] = pc;

			// Throw exception
			doException( EXCEPTION_HSC );
		}
		else
		{
			// Set RA = pc
			m_regs[31] = pc;

			#pragma OPTIMIZE OFF
			volatile uint32_t test[2];

			// BUG:: This will overflow the stack.
			for ( unsigned int i = 0; i < 32; i++ )
			{
				test[i] = m_regs[i];
			}

			#pragma OPTIMIZE ON
			
			// Throw exception
			doException( test[20] );
		}
		
	}
	else
		cop_unimpl (3, instr, pc);
}

void CCPU::swc1_emulate(uint32_t instr, uint32_t pc)
{
#if HAS_FPU
		uint32_t phys, virt, base, data;
		int32_t offset;
		bool cacheable;

		/* Load data from register. */
		data = fpu->read_reg (rt (instr));

		/* Calculate virtual address. */
		base = m_regs[rs(instr)];
		offset = s_immed(instr);
		virt = base + offset;

		/* This virtual address must be word-aligned. */
		if (virt % 4 != 0) {
			exception(AdES,DATASTORE);
			return;
		}

		/* Translate virtual address to physical address. */
		phys = cpzero->address_trans(virt, DATASTORE, &cacheable, this);
		if (exception_pending) return;

		/* Store word. */
		mem->store_word(phys, data, cacheable, this);
#else
		cop_unimpl (1, instr, pc);
#endif
}

void
CCPU::swc2_emulate(uint32_t instr, uint32_t pc)
{
	cop_unimpl (2, instr, pc);
}

void
CCPU::swc3_emulate(uint32_t instr, uint32_t pc)
{
	cop_unimpl (3, instr, pc);
}

void
CCPU::sll_emulate(uint32_t instr, uint32_t pc)
{
	m_regs[rd(instr)] = m_regs[rt(instr)] << shamt(instr);
}

int32_t srl(int32_t a, int32_t b)
{
	if (b == 0) 
	{
		return a;
	} 
	else if (b == 32) 
	{
		return 0;
	} 
	else 
	{
		return (a >> b) & ((1 << (32 - b)) - 1);
	}
}

int32_t sra(int32_t a, int32_t b)
{
	if (b == 0) 
	{
		return a;
	} 
	else
	{
		return (a >> b) | (((a >> 31) & 0x01) * (((1 << b) - 1) << (32 - b)));
	}
}

void CCPU::srl_emulate(uint32_t instr, uint32_t pc)
{
	m_regs[rd(instr)] = srl(m_regs[rt(instr)], shamt(instr));
}

void CCPU::sra_emulate(uint32_t instr, uint32_t pc)
{
	m_regs[rd(instr)] = sra(m_regs[rt(instr)], shamt(instr));
}

void CCPU::sllv_emulate(uint32_t instr, uint32_t pc)
{
	m_regs[rd(instr)] = m_regs[rt(instr)] << (m_regs[rs(instr)] & 0x01f);
}

void CCPU::srlv_emulate(uint32_t instr, uint32_t pc)
{
	m_regs[rd(instr)] = srl(m_regs[rt(instr)], m_regs[rs(instr)] & 0x01f);
}

void CCPU::srav_emulate(uint32_t instr, uint32_t pc)
{
	m_regs[rd(instr)] = sra(m_regs[rt(instr)], m_regs[rs(instr)] & 0x01f);
}

void CCPU::jr_emulate(uint32_t instr, uint32_t pc)
{
	if (m_regs[rd(instr)] != 0) 
	{
		doException(EXCEPTION_RI);
		return;
	}
	control_transfer (m_regs[rs(instr)]);
}

void CCPU::jalr_emulate(uint32_t instr, uint32_t pc)
{
	control_transfer (m_regs[rs(instr)]);
	// RA gets addr of instr after delay slot (2 words after this one).
	m_regs[rd(instr)] = pc + 8;
}

void CCPU::syscall_emulate(uint32_t instr, uint32_t pc)
{
	m_pSysCallHandler->DoSyscall( this );
	//doException(EXCEPTION_Sys);
}

void CCPU::break_emulate(uint32_t instr, uint32_t pc)
{
	doException(EXCEPTION_Bp);
}

void CCPU::mfhi_emulate(uint32_t instr, uint32_t pc)
{
	m_regs[rd(instr)] = m_hi;
}

void CCPU::mthi_emulate(uint32_t instr, uint32_t pc)
{
	if (rd(instr) != 0) 
	{
		doException(EXCEPTION_RI);
		return;
	}
	m_hi = m_regs[rs(instr)];
}

void CCPU::mflo_emulate(uint32_t instr, uint32_t pc)
{
	m_regs[rd(instr)] = m_lo;
}

void CCPU::mtlo_emulate(uint32_t instr, uint32_t pc)
{
	if (rd(instr) != 0) 
	{
		doException(EXCEPTION_RI);
		return;
	}
	m_lo = m_regs[rs(instr)];
}

void CCPU::mult_emulate(uint32_t instr, uint32_t pc)
{
	if (rd(instr) != 0) 
	{
		doException(EXCEPTION_RI);
		return;
	}
	mult64s(&m_hi, &m_lo, m_regs[rs(instr)], m_regs[rt(instr)]);
}

void CCPU::mult64(uint32_t *hi, uint32_t *lo, uint32_t n, uint32_t m)
{
	uint64_t result;
	result = ((uint64_t)n) * ((uint64_t)m);
	*hi = (uint32_t) (result >> 32);
	*lo = (uint32_t) result;
}

void CCPU::mult64s(uint32_t *hi, uint32_t *lo, int32_t n, int32_t m)
{
	int64_t result;
	result = ((int64_t)n) * ((int64_t)m);
	*hi = (uint32_t) (result >> 32);
	*lo = (uint32_t) result;
}

void CCPU::multu_emulate(uint32_t instr, uint32_t pc)
{
	if (rd(instr) != 0) 
	{
		doException(EXCEPTION_RI);
		return;
	}

	mult64(&m_hi, &m_lo, m_regs[rs(instr)], m_regs[rt(instr)]);
}

void CCPU::div_emulate(uint32_t instr, uint32_t pc)
{
	int32_t signed_rs = (int32_t)m_regs[rs(instr)];
	int32_t signed_rt = (int32_t)m_regs[rt(instr)];
	m_lo = signed_rs / signed_rt;
	m_hi = signed_rs % signed_rt;
}

void CCPU::divu_emulate(uint32_t instr, uint32_t pc)
{
	m_lo = m_regs[rs(instr)] / m_regs[rt(instr)];
	m_hi = m_regs[rs(instr)] % m_regs[rt(instr)];
}

void CCPU::add_emulate(uint32_t instr, uint32_t pc)
{
	int32_t a, b, sum;
	a = (int32_t)m_regs[rs(instr)];
	b = (int32_t)m_regs[rt(instr)];
	sum = a + b;

	if ((a < 0 && b < 0 && !(sum < 0)) || (a >= 0 && b >= 0 && !(sum >= 0))) 
	{
		doException(EXCEPTION_Ov);
		return;
	} 
	else 
	{
		m_regs[rd(instr)] = (uint32_t)sum;
	}
}

void CCPU::addu_emulate(uint32_t instr, uint32_t pc)
{
	int32_t a, b, sum;
	a = (int32_t)m_regs[rs(instr)];
	b = (int32_t)m_regs[rt(instr)];
	sum = a + b;
	m_regs[rd(instr)] = (uint32_t)sum;
}

void CCPU::sub_emulate(uint32_t instr, uint32_t pc)
{
	int32_t a, b, diff;
	a = (int32_t)m_regs[rs(instr)];
	b = (int32_t)m_regs[rt(instr)];
	diff = a - b;
	
	if ((a < 0 && !(b < 0) && !(diff < 0)) || (!(a < 0) && b < 0 && diff < 0)) 
	{
		doException(EXCEPTION_Ov);
		return;
	} 
	else 
	{
		m_regs[rd(instr)] = (uint32_t)diff;
	}
}

void CCPU::subu_emulate(uint32_t instr, uint32_t pc)
{
	int32_t a, b, diff;
	a = (int32_t)m_regs[rs(instr)];
	b = (int32_t)m_regs[rt(instr)];
	diff = a - b;
	m_regs[rd(instr)] = (uint32_t)diff;
}

void CCPU::and_emulate(uint32_t instr, uint32_t pc)
{
	m_regs[rd(instr)] = m_regs[rs(instr)] & m_regs[rt(instr)];
}

void CCPU::or_emulate(uint32_t instr, uint32_t pc)
{
	m_regs[rd(instr)] = m_regs[rs(instr)] | m_regs[rt(instr)];
}

void CCPU::xor_emulate(uint32_t instr, uint32_t pc)
{
	m_regs[rd(instr)] = m_regs[rs(instr)] ^ m_regs[rt(instr)];
}

void CCPU::nor_emulate(uint32_t instr, uint32_t pc)
{
	m_regs[rd(instr)] = ~(m_regs[rs(instr)] | m_regs[rt(instr)]);
}

void CCPU::slt_emulate(uint32_t instr, uint32_t pc)
{
	int32_t s_rs = (int32_t)m_regs[rs(instr)];
	int32_t s_rt = (int32_t)m_regs[rt(instr)];

	if (s_rs < s_rt) 
	{
		m_regs[rd(instr)] = 1;
	} 
	else 
	{
		m_regs[rd(instr)] = 0;
	}
}

void CCPU::sltu_emulate(uint32_t instr, uint32_t pc)
{
	if (m_regs[rs(instr)] < m_regs[rt(instr)]) 
	{
		m_regs[rd(instr)] = 1;
	} 
	else 
	{
		m_regs[rd(instr)] = 0;
	}
}

void CCPU::bltz_emulate(uint32_t instr, uint32_t pc)
{
	if ((int32_t)m_regs[rs(instr)] < 0)
		branch(instr, pc);
}

void CCPU::bgez_emulate(uint32_t instr, uint32_t pc)
{
	if ((int32_t)m_regs[rs(instr)] >= 0)
		branch(instr, pc);
}

/* As with JAL, BLTZAL and BGEZAL cause RA to get the address of the
 * instruction two words after the current one (pc + 8).
 */
void CCPU::bltzal_emulate(uint32_t instr, uint32_t pc)
{
	m_regs[REG_RA] = pc + 8;
	if ((int32_t)m_regs[rs(instr)] < 0)
		branch(instr, pc);
}

void CCPU::bgezal_emulate(uint32_t instr, uint32_t pc)
{
	m_regs[REG_RA] = pc + 8;
	if ((int32_t)m_regs[rs(instr)] >= 0)
		branch(instr, pc);
}

/* reserved instruction */
void CCPU::RI_emulate(uint32_t instr, uint32_t pc)
{
	doException(EXCEPTION_RI);
}
