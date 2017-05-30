#ifndef __CPU_H__
#define __CPU_H__

class CSysCallHandler;

class CCPU
{
	friend class CSysCallHandler;
	friend class CRC4Coprocessor;

public:
	CCPU();
	~CCPU();

public:
	static const int REG_ZERO	= 0;
	static const int REG_AT		= 1;
	static const int REG_V0		= 2;
	static const int REG_V1		= 3;
	static const int REG_A0		= 4;
	static const int REG_A1		= 5;
	static const int REG_A2		= 6;
	static const int REG_A3		= 7;
	static const int REG_SP		= 29;
	static const int REG_RA		= 31;

	static const int DELAYSTATE_NORMAL		= 0;
	static const int DELAYSTATE_DELAYING	= 1;
	static const int DELAYSTATE_DELAYSLOT	= 2;

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


	void doException (uint16_t excCode, int mode = MEMACC_ANY, int coprocno = -1);

	void SetErrno( uint32_t newErrno );
	
	void step( void );
	void reset( void );

	bool setup( CMMU *pMMU, CSysCallHandler *pSysCallHandler, uint32_t errnoAddress, bool bigEndian, uint32_t startPC );
	bool setRegisterAndPC( uint8_t regNum, uint32_t value );

	CMMU *GetMMU( void ) { return m_pMMU; };

private:
	// Instruction Handler Functions

	// Transfer control methods
	void branch (uint32_t instr, uint32_t pc);

	// Unaligned load/store support.
	uint32_t lwr(uint32_t regval, uint32_t memval, uint8_t offset);
	uint32_t lwl(uint32_t regval, uint32_t memval, uint8_t offset);
	uint32_t swl(uint32_t regval, uint32_t memval, uint8_t offset);
	uint32_t swr(uint32_t regval, uint32_t memval, uint8_t offset);

	// Miscellaneous shared code. 
	void control_transfer(uint32_t new_pc);
	void jump(uint32_t instr, uint32_t pc);
	uint32_t calc_jump_target(uint32_t instr, uint32_t pc);
	uint32_t calc_branch_target(uint32_t instr, uint32_t pc);
	void mult64(uint32_t *hi, uint32_t *lo, uint32_t n, uint32_t m);
	void mult64s(uint32_t *hi, uint32_t *lo, int32_t n, int32_t m);
	void cop_unimpl (int coprocno, uint32_t instr, uint32_t pc);

	// Emulation of specific instructions.
	void funct_emulate(uint32_t instr, uint32_t pc);
	void regimm_emulate(uint32_t instr, uint32_t pc);
	void j_emulate(uint32_t instr, uint32_t pc);
	void jal_emulate(uint32_t instr, uint32_t pc);
	void beq_emulate(uint32_t instr, uint32_t pc);
	void bne_emulate(uint32_t instr, uint32_t pc);
	void blez_emulate(uint32_t instr, uint32_t pc);
	void bgtz_emulate(uint32_t instr, uint32_t pc);
	void addi_emulate(uint32_t instr, uint32_t pc);
	void addiu_emulate(uint32_t instr, uint32_t pc);
	void slti_emulate(uint32_t instr, uint32_t pc);
	void sltiu_emulate(uint32_t instr, uint32_t pc);
	void andi_emulate(uint32_t instr, uint32_t pc);
	void ori_emulate(uint32_t instr, uint32_t pc);
	void xori_emulate(uint32_t instr, uint32_t pc);
	void lui_emulate(uint32_t instr, uint32_t pc);
	void cpzero_emulate(uint32_t instr, uint32_t pc);
	void cpone_emulate(uint32_t instr, uint32_t pc);
	void cptwo_emulate(uint32_t instr, uint32_t pc);
	void cpthree_emulate(uint32_t instr, uint32_t pc);
	void lb_emulate(uint32_t instr, uint32_t pc);
	void lh_emulate(uint32_t instr, uint32_t pc);
	void lwl_emulate(uint32_t instr, uint32_t pc);
	void lw_emulate(uint32_t instr, uint32_t pc);
	void lbu_emulate(uint32_t instr, uint32_t pc);
	void lhu_emulate(uint32_t instr, uint32_t pc);
	void lwr_emulate(uint32_t instr, uint32_t pc);
	void sb_emulate(uint32_t instr, uint32_t pc);
	void sh_emulate(uint32_t instr, uint32_t pc);
	void swl_emulate(uint32_t instr, uint32_t pc);
	void sw_emulate(uint32_t instr, uint32_t pc);
	void swr_emulate(uint32_t instr, uint32_t pc);
	void lwc1_emulate(uint32_t instr, uint32_t pc);
	void lwc2_emulate(uint32_t instr, uint32_t pc);
	void lwc3_emulate(uint32_t instr, uint32_t pc);
	void swc1_emulate(uint32_t instr, uint32_t pc);
	void swc2_emulate(uint32_t instr, uint32_t pc);
	void swc3_emulate(uint32_t instr, uint32_t pc);
	void sll_emulate(uint32_t instr, uint32_t pc);
	void srl_emulate(uint32_t instr, uint32_t pc);
	void sra_emulate(uint32_t instr, uint32_t pc);
	void sllv_emulate(uint32_t instr, uint32_t pc);
	void srlv_emulate(uint32_t instr, uint32_t pc);
	void srav_emulate(uint32_t instr, uint32_t pc);
	void jr_emulate(uint32_t instr, uint32_t pc);
	void jalr_emulate(uint32_t instr, uint32_t pc);
	void syscall_emulate(uint32_t instr, uint32_t pc);
	void break_emulate(uint32_t instr, uint32_t pc);
	void mfhi_emulate(uint32_t instr, uint32_t pc);
	void mthi_emulate(uint32_t instr, uint32_t pc);
	void mflo_emulate(uint32_t instr, uint32_t pc);
	void mtlo_emulate(uint32_t instr, uint32_t pc);
	void mult_emulate(uint32_t instr, uint32_t pc);
	void multu_emulate(uint32_t instr, uint32_t pc);
	void div_emulate(uint32_t instr, uint32_t pc);
	void divu_emulate(uint32_t instr, uint32_t pc);
	void add_emulate(uint32_t instr, uint32_t pc);
	void addu_emulate(uint32_t instr, uint32_t pc);
	void sub_emulate(uint32_t instr, uint32_t pc);
	void subu_emulate(uint32_t instr, uint32_t pc);
	void and_emulate(uint32_t instr, uint32_t pc);
	void or_emulate(uint32_t instr, uint32_t pc);
	void xor_emulate(uint32_t instr, uint32_t pc);
	void nor_emulate(uint32_t instr, uint32_t pc);
	void slt_emulate(uint32_t instr, uint32_t pc);
	void sltu_emulate(uint32_t instr, uint32_t pc);
	void bltz_emulate(uint32_t instr, uint32_t pc);
	void bgez_emulate(uint32_t instr, uint32_t pc);
	void bltzal_emulate(uint32_t instr, uint32_t pc);
	void bgezal_emulate(uint32_t instr, uint32_t pc);
	void RI_emulate(uint32_t instr, uint32_t pc);

private:
	void advanceDelayState( void ) { m_delayState = (m_delayState << 1) & 0x03; }; // 0->1, 1->2, 2->0

private:
	static uint16_t opcode(const uint32_t i)	{ return (i >> 26) & 0x03f; }
    static uint16_t rs(const uint32_t i)		{ return (i >> 21) & 0x01f; }
    static uint16_t rt(const uint32_t i)		{ return (i >> 16) & 0x01f; }
    static uint16_t rd(const uint32_t i)		{ return (i >> 11) & 0x01f; }
    static uint16_t immed(const uint32_t i)		{ return (i & 0x0ffff); }
    static int16_t s_immed(const uint32_t i)	{ return (i & 0x0ffff); }
    static uint16_t shamt(const uint32_t i)		{ return ((i >> 6) & 0x01f); }
    static uint16_t funct(const uint32_t i)		{ return (i & 0x03f); }
    static uint32_t jumptarg(const uint32_t i)	{ return (i & 0x03ffffff); }

	void setReg( uint8_t regNo, uint32_t val ) { m_regs[regNo] = val; };
	uint32_t getReg( uint8_t regNo ) { return (m_regs[regNo]); };

private:

private:
	CMMU *m_pMMU;							// MMU for this CPU
	CSysCallHandler *m_pSysCallHandler;		// Syscall Handler for this CPU
	CRC4Coprocessor	m_cptwo;				// RC4 Coprocessor is coprocessor 2

	uint32_t m_regs[32];	// Miscellaneous registers
	uint32_t m_pc;			// Program counter
	uint32_t m_instr;		// The current instruction
	uint32_t m_hi, m_lo;	// Division and multiplication results

	uint32_t m_nextEpc;		// Next exception PC
	uint32_t m_lastEpc;		// The last exception PC

	uint32_t m_errnoAddress;	// The address of errno

	bool m_exceptionPending;
	bool m_optBigendian;

	uint32_t	m_instrCount;

	// Delay slot handling.
	int			m_delayState;	// State of delay slot
	uint32_t	m_delayPC;		// Delay slot program counter
};

#endif // __CPU_H__
