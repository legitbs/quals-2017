/*
 * mcs4.c | MCS-4 Emulation Library
 *
 * Copyright (c) 2011-2017 Alexander Taylor <ajtaylor@fuzyll.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "mcs4.h"
#include <unistd.h>

#define STDOUT STDOUT_FILENO

/*
 * Helper function to read a 4-bit value from RAM.
 */
static uint8_t _mcs4_get_ram(struct mcs4 *cpu, uint8_t idx)
{
    uint32_t loc = idx >> 1;
    if (idx & 1) {
        switch (cpu->cmram) {
            default:  //case 1:
                return cpu->ram[cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc] >> 4;
            case 2:
                return cpu->ram[0x4 | (cpu->ms >> 6)][(cpu->ms & 0x30) >> 4][loc] >> 4;
            case 4:
                return cpu->ram[0x8 | (cpu->ms >> 6)][(cpu->ms & 0x30) >> 4][loc] >> 4;
            case 3:
                return cpu->ram[0xc | (cpu->ms >> 6)][(cpu->ms & 0x30) >> 4][loc] >> 4;
        }
    } else {
        switch (cpu->cmram) {
            default:  //case 1:
                return cpu->ram[cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc] & 0xF;
            case 2:
                return cpu->ram[0x4 | (cpu->ms >> 6)][(cpu->ms & 0x30) >> 4][loc] & 0xF;
            case 4:
                return cpu->ram[0x8 | (cpu->ms >> 6)][(cpu->ms & 0x30) >> 4][loc] & 0xF;
            case 3:
                return cpu->ram[0xc | (cpu->ms >> 6)][(cpu->ms & 0x30) >> 4][loc] & 0xF;
        }
    }
}


/*
 * Helper function to write a 4-bit value to RAM.
 */
static void _mcs4_set_ram(struct mcs4 *cpu, uint8_t idx, uint8_t val)
{
    uint32_t loc = idx >> 1;
    if (idx & 1) {
        switch (cpu->cmram) {
            case 1:
                cpu->ram[cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc] &= 0x0F;
                cpu->ram[cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc] |= (val << 4);
                break;
            case 2:
                cpu->ram[0x4 | cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc] &= 0x0F;
                cpu->ram[0x4 | cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc] |= (val << 4);
                break;
            case 4:
                cpu->ram[0x8 | cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc] &= 0x0F;
                cpu->ram[0x8 | cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc] |= (val << 4);
#ifdef _DEBUG
	cpu->print(STDOUT, "%x: %x\n", &cpu->ram[0x8 | cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc], (val << 4));
#endif
                break;
            case 3:
                cpu->ram[0xC | cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc] &= 0x0F;
                cpu->ram[0xC | cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc] |= (val << 4);
                break;
        }
    } else {
        switch (cpu->cmram) {
            case 1:
                cpu->ram[cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc] &= 0xF0;
                cpu->ram[cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc] |= val;
                break;
            case 2:
                cpu->ram[0x4 | cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc] &= 0xF0;
                cpu->ram[0x4 | cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc] |= val;
                break;
            case 4:
                cpu->ram[0x8 | cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc] &= 0xF0;
                cpu->ram[0x8 | cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc] |= val;
#ifdef _DEBUG
	cpu->print(STDOUT, "%x: %x\n", &cpu->ram[0x8 | cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc], val);
#endif
                break;
            case 3:
                cpu->ram[0xC | cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc] &= 0xF0;
                cpu->ram[0xC | cpu->ms >> 6][(cpu->ms & 0x30) >> 4][loc] |= val;
                break;
        }
    }

}

/*
 * Helper function to read a 4-bit value from the scratchpad registers.
 */
static uint8_t _mcs4_get_r(struct mcs4 *cpu, uint8_t idx)
{
    uint32_t loc = idx >> 1;
    if (idx & 1) {
        return cpu->r[loc] >> 4;
    } else {
        return cpu->r[loc] & 0x0F;
    }
}


/*
 * Helper function to write a 4-bit value to the scratchpad registers.
 */
static void _mcs4_set_r(struct mcs4 *cpu, uint8_t idx, uint8_t val)
{
    uint32_t loc = idx >> 1;
    if (idx & 1) {
        cpu->r[loc] = ((val & 0xF) << 4) | (cpu->r[loc] & 0x0F);
    } else {
        cpu->r[loc] = (cpu->r[loc] & 0xF0) | (val & 0xF);
    }
}


/*
 * Places data into the specified pair of registers.
 * It is assumed that we are given register RRR0 in the "pair" argument.
 */
static void _mcs4_set_reg_pair(struct mcs4 *cpu, uint8_t pair, uint8_t data)
{
    _mcs4_set_r(cpu, 2*pair, (data >> 4));
    _mcs4_set_r(cpu, 2*pair+1, data & 0xF);
}


/*
 * Returns the data stored in a given register pair.
 * It is assumed that we are given register RRR0 in the "pair" argument.
 */
static uint8_t _mcs4_get_reg_pair(struct mcs4 *cpu, uint8_t pair)
{
    return _mcs4_get_r(cpu, 2*pair) << 4 | _mcs4_get_r(cpu, 2*pair+1);
}


/*
 * Helper function to read a 12-bit value from the program counter registers.
 */
static uint16_t _mcs4_get_pc(struct mcs4 *cpu, uint8_t idx)
{
    uint32_t loc = (idx >> 1) * 3;
    if (idx & 1) {
        return  ((cpu->pc[loc+1] & 0xF) << 8) | cpu->pc[loc+2];
    } else {
        return cpu->pc[loc] << 4 | ((cpu->pc[loc+1] >> 4) & 0xF);
    }
}


/*
 * Helper function to write a 12-bit value to the program counter registers.
 */
static void _mcs4_set_pc(struct mcs4 *cpu, uint8_t idx, uint16_t val)
{
    uint32_t loc = (idx >> 1) * 3;
    if (idx & 1) {
        cpu->pc[loc+1] = (cpu->pc[loc+1] & 0xF0) | ((val & 0xF00) >> 8);
        cpu->pc[loc+2] = val & 0xFF;
    } else {
        cpu->pc[loc] = (val & 0xFF0) >> 4;
        cpu->pc[loc+1] = (cpu->pc[loc+1] & 0x0F) | ((val & 0x00F) << 4);
    }
}


/*
 * Helper funciton to increment the program counter and handle overflow.
 */
static void _mcs4_inc_pc(struct mcs4 *cpu)
{
	uint16_t current = _mcs4_get_pc(cpu, 0);
    _mcs4_set_pc(cpu, 0, (current & 0xF00) | ((current + 1) & 0xFF));
    cpu->cycles++;
}


/*
 * Helper function to fetch a new 8-bit instruction from the PROM and return it.
 */
static uint8_t _mcs4_fetch(struct mcs4 *cpu)
{
    return cpu->prom[_mcs4_get_pc(cpu, 0) & (sizeof (cpu->prom) - 1)];
}


/*
 * Initializes the virtual CPU, RAM, and ROM.
 */
void mcs4_init(struct mcs4 *cpu, uint8_t *prom, int (*print)(const int, const char *, ...))
{
    // array of function pointers for executing an instruction regardless of OPA's value
	void (*ftable[256])(struct mcs4 *) = {
		&mcs4_op_nop, &mcs4_op_inv, &mcs4_op_inv, &mcs4_op_inv,
		&mcs4_op_inv, &mcs4_op_inv, &mcs4_op_inv, &mcs4_op_inv,
		&mcs4_op_inv, &mcs4_op_inv, &mcs4_op_inv, &mcs4_op_inv,
		&mcs4_op_inv, &mcs4_op_inv, &mcs4_op_inv, &mcs4_op_inv,
		&mcs4_op_jcn, &mcs4_op_jcn, &mcs4_op_jcn, &mcs4_op_jcn,
		&mcs4_op_jcn, &mcs4_op_jcn, &mcs4_op_jcn, &mcs4_op_jcn,
		&mcs4_op_jcn, &mcs4_op_jcn, &mcs4_op_jcn, &mcs4_op_jcn,
		&mcs4_op_jcn, &mcs4_op_jcn, &mcs4_op_jcn, &mcs4_op_jcn,
		&mcs4_op_fim, &mcs4_op_src, &mcs4_op_fim, &mcs4_op_src,
		&mcs4_op_fim, &mcs4_op_src, &mcs4_op_fim, &mcs4_op_src,
		&mcs4_op_fim, &mcs4_op_src, &mcs4_op_fim, &mcs4_op_src,
		&mcs4_op_fim, &mcs4_op_src, &mcs4_op_fim, &mcs4_op_src,
		&mcs4_op_fin, &mcs4_op_jin, &mcs4_op_fin, &mcs4_op_jin,
		&mcs4_op_fin, &mcs4_op_jin, &mcs4_op_fin, &mcs4_op_jin,
		&mcs4_op_fin, &mcs4_op_jin, &mcs4_op_fin, &mcs4_op_jin,
		&mcs4_op_fin, &mcs4_op_jin, &mcs4_op_fin, &mcs4_op_jin,
		&mcs4_op_jun, &mcs4_op_jun, &mcs4_op_jun, &mcs4_op_jun,
		&mcs4_op_jun, &mcs4_op_jun, &mcs4_op_jun, &mcs4_op_jun,
		&mcs4_op_jun, &mcs4_op_jun, &mcs4_op_jun, &mcs4_op_jun,
		&mcs4_op_jun, &mcs4_op_jun, &mcs4_op_jun, &mcs4_op_jun,
		&mcs4_op_jms, &mcs4_op_jms, &mcs4_op_jms, &mcs4_op_jms,
		&mcs4_op_jms, &mcs4_op_jms, &mcs4_op_jms, &mcs4_op_jms,
		&mcs4_op_jms, &mcs4_op_jms, &mcs4_op_jms, &mcs4_op_jms,
		&mcs4_op_jms, &mcs4_op_jms, &mcs4_op_jms, &mcs4_op_jms,
		&mcs4_op_inc, &mcs4_op_inc, &mcs4_op_inc, &mcs4_op_inc,
		&mcs4_op_inc, &mcs4_op_inc, &mcs4_op_inc, &mcs4_op_inc,
		&mcs4_op_inc, &mcs4_op_inc, &mcs4_op_inc, &mcs4_op_inc,
		&mcs4_op_inc, &mcs4_op_inc, &mcs4_op_inc, &mcs4_op_inc,
		&mcs4_op_isz, &mcs4_op_isz, &mcs4_op_isz, &mcs4_op_isz,
		&mcs4_op_isz, &mcs4_op_isz, &mcs4_op_isz, &mcs4_op_isz,
		&mcs4_op_isz, &mcs4_op_isz, &mcs4_op_isz, &mcs4_op_isz,
		&mcs4_op_isz, &mcs4_op_isz, &mcs4_op_isz, &mcs4_op_isz,
		&mcs4_op_add, &mcs4_op_add, &mcs4_op_add, &mcs4_op_add,
		&mcs4_op_add, &mcs4_op_add, &mcs4_op_add, &mcs4_op_add,
		&mcs4_op_add, &mcs4_op_add, &mcs4_op_add, &mcs4_op_add,
		&mcs4_op_add, &mcs4_op_add, &mcs4_op_add, &mcs4_op_add,
		&mcs4_op_sub, &mcs4_op_sub, &mcs4_op_sub, &mcs4_op_sub,
		&mcs4_op_sub, &mcs4_op_sub, &mcs4_op_sub, &mcs4_op_sub,
		&mcs4_op_sub, &mcs4_op_sub, &mcs4_op_sub, &mcs4_op_sub,
		&mcs4_op_sub, &mcs4_op_sub, &mcs4_op_sub, &mcs4_op_sub,
		&mcs4_op_ld,  &mcs4_op_ld,  &mcs4_op_ld,  &mcs4_op_ld,
		&mcs4_op_ld,  &mcs4_op_ld,  &mcs4_op_ld,  &mcs4_op_ld,
		&mcs4_op_ld,  &mcs4_op_ld,  &mcs4_op_ld,  &mcs4_op_ld,
		&mcs4_op_ld,  &mcs4_op_ld,  &mcs4_op_ld,  &mcs4_op_ld,
		&mcs4_op_xch, &mcs4_op_xch, &mcs4_op_xch, &mcs4_op_xch,
		&mcs4_op_xch, &mcs4_op_xch, &mcs4_op_xch, &mcs4_op_xch,
		&mcs4_op_xch, &mcs4_op_xch, &mcs4_op_xch, &mcs4_op_xch,
		&mcs4_op_xch, &mcs4_op_xch, &mcs4_op_xch, &mcs4_op_xch,
		&mcs4_op_bbl, &mcs4_op_bbl, &mcs4_op_bbl, &mcs4_op_bbl,
		&mcs4_op_bbl, &mcs4_op_bbl, &mcs4_op_bbl, &mcs4_op_bbl,
		&mcs4_op_bbl, &mcs4_op_bbl, &mcs4_op_bbl, &mcs4_op_bbl,
		&mcs4_op_bbl, &mcs4_op_bbl, &mcs4_op_bbl, &mcs4_op_bbl,
		&mcs4_op_ldm, &mcs4_op_ldm, &mcs4_op_ldm, &mcs4_op_ldm,
		&mcs4_op_ldm, &mcs4_op_ldm, &mcs4_op_ldm, &mcs4_op_ldm,
		&mcs4_op_ldm, &mcs4_op_ldm, &mcs4_op_ldm, &mcs4_op_ldm,
		&mcs4_op_ldm, &mcs4_op_ldm, &mcs4_op_ldm, &mcs4_op_ldm,
		&mcs4_op_wrm, &mcs4_op_wmp, &mcs4_op_wrr, &mcs4_op_inv,
		&mcs4_op_wr,  &mcs4_op_wr,  &mcs4_op_wr,  &mcs4_op_wr,
		&mcs4_op_sbm, &mcs4_op_rdm, &mcs4_op_rdr, &mcs4_op_adm,
		&mcs4_op_rd,  &mcs4_op_rd,  &mcs4_op_rd,  &mcs4_op_rd,
		&mcs4_op_clb, &mcs4_op_clc, &mcs4_op_iac, &mcs4_op_cmc,
		&mcs4_op_cma, &mcs4_op_ral, &mcs4_op_rar, &mcs4_op_tcc,
		&mcs4_op_dac, &mcs4_op_tcs, &mcs4_op_stc, &mcs4_op_daa,
		&mcs4_op_kbp, &mcs4_op_dcl, &mcs4_op_inv, &mcs4_op_inv
	};

    // initialize Intel 4001 PROM Chips
    for (uint32_t i = 0; i < sizeof (cpu->prom); i++) {
        cpu->prom[i] = prom[i];
    }
    for (uint32_t i = 0; i < 4; i++) {
        cpu->promio[i] = 0;
    }

    // initialize Intel 4002 RAM Chips
    for (uint32_t i = 0; i < 8; i++) {
        for (uint32_t j = 0; j < 4; j++) {
            for (uint32_t k = 0; k < 10; k++) {
                cpu->ram[i][j][k] = 0;
            }
        }
    }
    for (uint32_t i = 0; i < 8; i++) {
        cpu->ramout[i] = 0;
    }

    // initialize Intel 4004 CPU Chip
    for (uint32_t i = 0; i < 6; i++) {
        cpu->pc[i] = 0;
    }
    cpu->sp = 0;
    cpu->ms = 0;
    cpu->tmp = 0;
    cpu->a = 0;
    cpu->c = 0;
    cpu->t = 0;
    for (uint32_t i = 0; i < 8; i++) {
        cpu->r[i] = 0;
    }
    cpu->cmram = 0;
    cpu->cycles = 0;

    // initialize array of function pointers to instruction handlers
	cpu->print = print;
	for (uint32_t i = 0; i < 256; i++) {
		cpu->exec[i] = ftable[i];
	}

    return;
}


/*
 * Advances the state of the system by one instruction.
 */
void mcs4_step(struct mcs4 *cpu)
{
    uint8_t instruction = _mcs4_fetch(cpu);
#ifdef _DEBUG
    cpu->print(STDOUT, "%04x %03x %02x %s %x [%02x %02x %02x %02x %02x %02x %02x %02x] %x [%03x %03x %03x] %x %x\n",
            cpu->cycles, _mcs4_get_pc(cpu, 0), instruction, _mcs4_mnemonic[instruction], cpu->ms,
            cpu->r[0], cpu->r[1], cpu->r[2], cpu->r[3], cpu->r[4], cpu->r[5], cpu->r[6], cpu->r[7],
            cpu->sp, _mcs4_get_pc(cpu, 1), _mcs4_get_pc(cpu, 2), _mcs4_get_pc(cpu, 3), cpu->a, cpu->c);
#endif
    (*cpu->exec[instruction])(cpu);
}


/*
 * The Intel 4004 microprocessor has a word size of 4 bits. Thus, each opcode
 * is 4 bits long. Instructions are 8 or 16 bits in length, consisting of
 * 1-2 4-bit opcodes (OPR) and 1-2 4-bit modifiers (OPA). There are 4 types of
 * modifiers and 45 instructions in total. Addresses are 12-bits long.
 *
 * The following lists all modifiers in the Intel 4004 Instruction Set:
 *
 * SYMBOL   MODIFIER
 * ----------------
 * C        Condition
 * A        Address
 * R        Register
 * D        Data
 *
 * The following lists all instructions in the Intel 4004 Instruction Set:
 *
 * OPR OPA OPR OPA  MNEMONIC    INSTRUCTION
 * ------------------------------------------------     Machine Instructions
 * 00000000         NOP         No Operation
 * 0001CCCCAAAAAAAA JCN         Jump Conditional
 * 0010RRR0DDDDDDDD FIM         Fetch Immediate
 * 0010RRR1         SRC         Send Register Control
 * 0011RRR0         FIN         Fetch Indirect
 * 0011RRR1         JIN         Jump Indirect
 * 0100AAAAAAAAAAAA JUN         Jump Unconditional
 * 0101AAAAAAAAAAAA JMS         Jump to Subroutine
 * 0110RRRR         INC         Increment
 * 0111RRRRAAAAAAAA ISZ         Increment and Skip
 * 1000RRRR         ADD         Add
 * 1001RRRR         SUB         Subtract
 * 1010RRRR         LD          Load
 * 1011RRRR         XCH         Exchange
 * 1100DDDD         BBL         Branch Back and Load
 * 1101DDDD         LDM         Load Immediate
 * ------------------------------------------------     I/O Instructions
 * 11100000         WRM         Write Main Memory
 * 11100001         WMP         Write RAM Port
 * 11100010         WRR         Write ROM Port
 * 11100100         WR0         Write Status Char 0
 * 11100101         WR1         Write Status Char 1
 * 11100110         WR2         Write Status Char 2
 * 11100111         WR3         Write Status Char 3
 * 11101000         SBM         Subtract Main Memory
 * 11101001         RDM         Read Main Memory
 * 11101010         RDR         Read ROM Port
 * 11101011         ADM         Add Main Memory
 * 11101100         RD0         Read Status Char 0
 * 11101101         RD1         Read Status Char 1
 * 11101110         RD2         Read Status Char 2
 * 11101111         RD3         Read Status Char 3
 * ------------------------------------------------     Accumulator Instructions
 * 11110000         CLB         Clear Both
 * 11110001         CLC         Clear Carry
 * 11110010         IAC         Increment Accumulator
 * 11110011         CMC         Complement Carry
 * 11110100         CMA         Complement Accumulator
 * 11110101         RAL         Rotate Left
 * 11110110         RAR         Rotate Right
 * 11110111         TCC         Transmit Carry and Clear
 * 11111000         DAC         Decrement Accumulator
 * 11111001         TCS         Transfer Carry Subtract
 * 11111010         STC         Set Carry
 * 11111011         DAA         Decimal Adjust Accumulator
 * 11111100         KBP         Keyboard Process
 * 11111101         DCL         Designate Command Line
 */


/*
 * No Operation
 * NOP | 0000 0000
 * No operation performed.
 */
void mcs4_op_nop(struct mcs4 *cpu)
{
    _mcs4_inc_pc(cpu);
}


/*
 * Jump Conditional
 * JCN | 0001 CCCC AAAA AAAA
 * Jump to the instruction located at the 8-bit address AAAA AAAA based on the
 * 4-bit condition CCCC. The condition bits are interpreted as follows:
 *
 * If 1XXX, invert the jump condition.
 * If X1XX, jump if the accumulator is zero.
 * If XX1X, jump if the carry flag is set.
 * If XXX1, jump if the test flag is unset.
 */
void mcs4_op_jcn(struct mcs4 *cpu)
{
    uint8_t page = _mcs4_get_pc(cpu, 0) & 0xF00;  // must stay on same ROM chip
    cpu->tmp = _mcs4_fetch(cpu);  // condition
    _mcs4_inc_pc(cpu);

    if ((cpu->tmp & 0x4) && ((!cpu->a) ^ (cpu->tmp & 0x8))) {
        _mcs4_set_pc(cpu, 0, page | _mcs4_fetch(cpu));  // jump if accumulator is zero
    } else if ((cpu->tmp & 0x2) && ((cpu->c) ^ (cpu->tmp & 0x8))) {
        _mcs4_set_pc(cpu, 0, page | _mcs4_fetch(cpu));  // jump if carry is not zero
    } else if ((cpu->tmp & 0x1) && ((!cpu->t) ^ (cpu->tmp & 0x8))) {
        _mcs4_set_pc(cpu, 0, page | _mcs4_fetch(cpu));  // jump if test is zero
    } else {
        _mcs4_inc_pc(cpu);
    }
}


/*
 * Fetch Immediate
 * FIM | 0010 RRR0 DDDD DDDD
 * Loads first DDDD into RRR1 and second DDDD into RRR0.
 */
void mcs4_op_fim(struct mcs4 *cpu)
{
    cpu->tmp = (_mcs4_fetch(cpu) & 0xF) >> 1;  // register pair
    _mcs4_inc_pc(cpu);
    _mcs4_set_reg_pair(cpu, cpu->tmp, _mcs4_fetch(cpu));
    _mcs4_inc_pc(cpu);
}


/*
 * Send Register Control
 * SRC | 0010 RRR1
 * The data in RRR0 RRR1 is sent to the memory select register.
 */
void mcs4_op_src(struct mcs4 *cpu)
{
    cpu->ms = _mcs4_get_reg_pair(cpu, (_mcs4_fetch(cpu) & 0xF) >> 1);
    _mcs4_inc_pc(cpu);
}


/*
 * Fetch Indirect
 * FIN | 0011 RRR0
 * The data in RRR0 RRR1 is interpreted as the address of data that should be
 * loaded into those registers.
 */
void mcs4_op_fin(struct mcs4 *cpu)
{
    cpu->tmp = (_mcs4_fetch(cpu) & 0xF) >> 1;  // register pair
    _mcs4_set_reg_pair(cpu, cpu->tmp, (_mcs4_fetch(cpu) & 0xF00) | _mcs4_get_reg_pair(cpu, cpu->tmp));
    _mcs4_inc_pc(cpu);
}


/*
 * Jump Indirect
 * JIN | 0011 RRR1
 * The data in RRR0 RRR1 is interpreted as the address of the next instruction
 * that should be executed by the microprocessor.
 */
void mcs4_op_jin(struct mcs4 *cpu)
{
    _mcs4_set_pc(cpu, 0, (_mcs4_get_pc(cpu, 0) & 0xF00) | _mcs4_get_reg_pair(cpu, (_mcs4_get_pc(cpu, 0) & 0xF) >> 1));
}


/*
 * Jump Unconditional
 * JUN | 0100 AAAA AAAA AAAA
 * Program control is unconditionally transferred to the instruction located
 * at address AAAA AAAA AAAA.
 */
void mcs4_op_jun(struct mcs4 *cpu)
{
    cpu->tmp = _mcs4_get_pc(cpu, 0) & 0xF;  // highest 4-bits of address
    _mcs4_inc_pc(cpu);
    _mcs4_set_pc(cpu, 0, (cpu->tmp << 8) | _mcs4_get_pc(cpu, 0));
}


/*
 * Jump to Subroutine
 * JMS | 0101 AAAA AAAA AAAA
 * Program control is unconditionally transferred to the instruction located
 * at address AAAA AAAA AAAA. The previous value of PC is saved and pushed
 * onto the PC stack.
 */
void mcs4_op_jms(struct mcs4 *cpu)
{
    cpu->tmp = _mcs4_fetch(cpu) & 0xF;  // highest 4-bits of address
    _mcs4_inc_pc(cpu);
    uint16_t next = (cpu->tmp << 8) | _mcs4_fetch(cpu);
    _mcs4_inc_pc(cpu);  // increment PC here so we'll jump back to correct instruction
    cpu->sp = (cpu->sp + 1) & 0x3;
    for (uint32_t i = cpu->sp; i > 0; i--) {
        _mcs4_set_pc(cpu, i, _mcs4_get_pc(cpu, i-1));
    }
    _mcs4_set_pc(cpu, 0, next);
}


/*
 * Increment
 * INC | 0110 RRRR
 * Register RRRR is incremented by 1 (RRRR is set to 0 if it overflows).
 */
void mcs4_op_inc(struct mcs4 *cpu)
{
    cpu->tmp = _mcs4_fetch(cpu) & 0xF;  // selected register
    _mcs4_set_r(cpu, cpu->tmp, _mcs4_get_r(cpu, cpu->tmp) + 1);
    _mcs4_inc_pc(cpu);
}


/*
 * Increment and Skip
 * ISZ | 0111 RRRR AAAA AAAA
 * Register RRRR is incremented by 1. If the result is zero, the next
 * instruction after ISZ is executed. Otherwise, program control is transferred
 * to the instruction located at address AAAA AAAA on the same ROM.
 */
void mcs4_op_isz(struct mcs4 *cpu)
{
    cpu->tmp = _mcs4_fetch(cpu) & 0xF;  // selected register
    _mcs4_inc_pc(cpu);
    _mcs4_set_r(cpu, cpu->tmp, _mcs4_get_r(cpu, cpu->tmp) + 1);
    if (_mcs4_get_r(cpu, cpu->tmp)) {
        _mcs4_set_pc(cpu, 0, (_mcs4_get_pc(cpu, 0) & 0xF00) | _mcs4_fetch(cpu));  // stay on the same ROM
    } else {
        _mcs4_inc_pc(cpu);
	}
}


/*
 * Add
 * ADD | 1000 RRRR
 * Register RRRR is added to the accumulator with carry. The result is stored
 * in the accumulator. If a carry is generated, the carry flag is set
 * (otherwise, it is unset).
 */
void mcs4_op_add(struct mcs4 *cpu)
{
	uint8_t old = cpu->a;
    cpu->tmp = _mcs4_fetch(cpu) & 0xF;  // selected register
    cpu->a += _mcs4_get_r(cpu, cpu->tmp) + cpu->c;
    if (old > cpu->a) {
        cpu->c = 1;
	} else if (old == cpu->a && cpu->c) {
		cpu->c = 1;
    } else {
		cpu->c = 0;
	}
    _mcs4_inc_pc(cpu);
}


/*
 * Subtract
 * SUB | 1001 RRRR
 * Register RRRR is complemented (1s complement) and added to the accumulator
 * with borrow. The result is stored in the accumulator. If a borrow is
 * generated, the carry flag is left unset. If no borrow is generated, the
 * carry flag is set.
 */
void mcs4_op_sub(struct mcs4 *cpu)
{
    uint8_t old = cpu->a;
    cpu->tmp = _mcs4_fetch(cpu) & 0xF;  // selected register
    cpu->a += ((~_mcs4_get_r(cpu, cpu->tmp)) + (~cpu->c));
    if (old > cpu->a) {
        cpu->c = 1;
	} else if (old == cpu->a && ~cpu->c) {
		cpu->c = 1;
    } else {
		cpu->c = 0;
	}
    _mcs4_inc_pc(cpu);
}


/*
 * Load
 * LD | 1010 RRRR
 * Loads the contents of register RRRR into the accumulator.
 */
void mcs4_op_ld(struct mcs4 *cpu)
{
    cpu->tmp = _mcs4_fetch(cpu) & 0xF;  // selected register
    cpu->a = _mcs4_get_r(cpu, cpu->tmp);
    _mcs4_inc_pc(cpu);
}


/*
 * Exchange
 * XCH | 1011 RRRR
 * Loads the contents of register RRRR into the accumulator. The previous
 * contents of the accumulator are loaded into register RRRR.
 */
void mcs4_op_xch(struct mcs4 *cpu)
{
    uint8_t reg = _mcs4_fetch(cpu) & 0xF;
    cpu->tmp = cpu->a;
    cpu->a = _mcs4_get_r(cpu, reg);
    _mcs4_set_r(cpu, reg, cpu->tmp);
    _mcs4_inc_pc(cpu);
}


/*
 * Branch Back and Load
 * BBL | 1100 DDDD
 * The program counter is pushed down a level and program control transfers
 * to the next instruction following the last JMS. The data DDDD is then loaded
 * into the accumulator.
 */
void mcs4_op_bbl(struct mcs4 *cpu)
{
    cpu->tmp = _mcs4_fetch(cpu) & 0xF;  // data
    if (cpu->sp > 0) {
        for (uint32_t i = 0; i < cpu->sp; i++) {
            _mcs4_set_pc(cpu, i, _mcs4_get_pc(cpu, i+1));
        }
        _mcs4_set_pc(cpu, cpu->sp, 0);
        cpu->sp--;
        cpu->a = cpu->tmp;
    } else {
        _mcs4_inc_pc(cpu);  // handle a stack underflow by simply doing nothing
    }
}


/*
 * Load Immediate
 * LDM | 1101 DDDD
 * Loads data DDDD into the accumulator.
 */
void mcs4_op_ldm(struct mcs4 *cpu)
{
    cpu->tmp = _mcs4_fetch(cpu) & 0xF;  // data
    cpu->a = cpu->tmp;
    _mcs4_inc_pc(cpu);
}


/*
 * Write Main Memory
 * WRM | 1110 0000
 * Data in the accumulator is written to the selected RAM location.
 */
void mcs4_op_wrm(struct mcs4 *cpu)
{
    _mcs4_set_ram(cpu, cpu->ms & 0xF, cpu->a);
    _mcs4_inc_pc(cpu);
}


/*
 * Write RAM Port
 * WMP | 1110 0001
 * Data in the accumulator is written to the selected RAM output.
 */
void mcs4_op_wmp(struct mcs4 *cpu)
{
    switch (cpu->cmram) {
        case 1:
            if (cpu->ms & 0x40) {
                cpu->ramout[cpu->ms >> 7] = cpu->a;
            } else {
                cpu->ramout[cpu->ms >> 7] = cpu->a << 4;
            }
            break;
        case 2:
            if (cpu->ms & 0x40) {
                cpu->ramout[0x2 | (cpu->ms >> 7)] = cpu->a;
            } else {
                cpu->ramout[0x2 | (cpu->ms >> 7)] = cpu->a << 4;
            }
            break;
        case 4:
            if (cpu->ms & 0x40) {
                cpu->ramout[0x4 | (cpu->ms >> 7)] = cpu->a;
            } else {
                cpu->ramout[0x4 | (cpu->ms >> 7)] = cpu->a << 4;
            }
            break;
        case 3:
            if (cpu->ms & 0x40) {
                cpu->ramout[0x6 | (cpu->ms >> 7)] = cpu->a;
            } else {
                cpu->ramout[0x6 | (cpu->ms >> 7)] = cpu->a << 4;
            }
            break;
    }
    _mcs4_inc_pc(cpu);
}


/*
 * Write ROM Port
 * WRR | 1110 0010
 * Places the contents of the accumulator on the PROM's I/O lines.
 */
void mcs4_op_wrr(struct mcs4 *cpu)
{
    cpu->tmp = (_mcs4_get_pc(cpu, 0) >> 8);  // high bits indicate which ROM we're on
    if (cpu->tmp & 1) {
        cpu->promio[cpu->tmp/2] &= 0x0F;
        cpu->promio[cpu->tmp/2] |= cpu->a << 4;
    } else {
        cpu->promio[cpu->tmp/2] &= 0xF0;
        cpu->promio[cpu->tmp/2] |= cpu->a;
    }
	cpu->print(STDOUT, "%x%x%x%x\n", cpu->promio[0], cpu->promio[1], cpu->promio[2], cpu->promio[3]);
    _mcs4_inc_pc(cpu);
}


/*
 * Write Status Char
 * WRx | 111001xx
 * Data in the accumulator is written to the selected RAM location's status
 * area.
 */
void mcs4_op_wr(struct mcs4 *cpu)
{
    cpu->tmp = _mcs4_fetch(cpu) & 0x3;  // lower 2 bits are index
    _mcs4_set_ram(cpu, 0xF + cpu->tmp, cpu->a);
    _mcs4_inc_pc(cpu);
}


/*
 * Subtract Main Memory
 * SBM | 1110 1000
 * The previously selected RAM location's data is subtracted from the
 * accumulator with borrow and stored in the accumulator. If a borrow is
 * generated, the carry flag is left unset. If no borrow is generated, the
 * carry flag is set.
 */
void mcs4_op_sbm(struct mcs4 *cpu)
{
    cpu->tmp = cpu->a;
    cpu->a += (~_mcs4_get_ram(cpu, cpu->ms & 0xF)) + (~cpu->c);
    if (cpu->tmp > cpu->a) {
        cpu->c = 1;
	} else if (cpu->tmp == cpu->a && ~cpu->c) {
		cpu->c = 1;
    } else {
		cpu->c = 0;
	}
    _mcs4_inc_pc(cpu);
}


/*
 * Read Main Memory
 * RDM | 1110 1001
 * Data in the selected RAM location is loaded into the accumulator.
 */
void mcs4_op_rdm(struct mcs4 *cpu)
{
    cpu->a = _mcs4_get_ram(cpu, cpu->ms & 0xF);
    _mcs4_inc_pc(cpu);
}


/*
 * Read ROM Port
 * RDR | 1110 1010
 * Places the output of the PROM's I/O port in the accumulator.
 */
void mcs4_op_rdr(struct mcs4 *cpu)
{
    cpu->tmp = (_mcs4_get_pc(cpu, 0) >> 8);  // high bits indicate which ROM we're on
    if (cpu->tmp & 1) {
        cpu->a = cpu->promio[cpu->tmp/2] >> 4;
    } else {
        cpu->a = cpu->promio[cpu->tmp/2] & 0x0F;
    }
    _mcs4_inc_pc(cpu);
}


/*
 * Add Main Memory
 * ADM | 1110 1011
 * The previously selected RAM location's data is subtracted from the
 * accumulator with borrow and stored in the accumulator. If a carry is
 * generated, the carry flag is set (otherwise it is unset).
 */
void mcs4_op_adm(struct mcs4 *cpu)
{
	cpu->tmp = cpu->a;
    cpu->a += _mcs4_get_ram(cpu, cpu->ms & 0xF) + cpu->c;
    if (cpu->tmp > cpu->a) {
        cpu->c = 1;
	} else if (cpu->tmp == cpu->a && cpu->c) {
		cpu->c = 1;
    } else {
		cpu->c = 0;
	}
    _mcs4_inc_pc(cpu);
}


/*
 * Read Status Character
 * RDx | 111011xx
 * Data in the selected RAM location's status area is written to the
 * accumulator.
 */
void mcs4_op_rd(struct mcs4 *cpu)
{
    cpu->tmp = _mcs4_fetch(cpu) & 0x3;  // lower 2 bits are index
    cpu->a = _mcs4_get_ram(cpu, 0xF + cpu->tmp);
    _mcs4_inc_pc(cpu);
}


/*
 * Clear Both
 * CLB | 1111 0000
 * Clears the accumulator and the carry flag.
 */
void mcs4_op_clb(struct mcs4 *cpu)
{
    cpu->a = 0;
    cpu->c = 0;
    _mcs4_inc_pc(cpu);
}


/*
 * Clear Carry
 * CLC | 1111 0001
 * Clears the carry flag.
 */
void mcs4_op_clc(struct mcs4 *cpu)
{
    cpu->c = 0;
    _mcs4_inc_pc(cpu);
}


/*
 * Increment Accumulator
 * IAC | 1111 0010
 * Increments the accumulator and sets the carry flag if overflow occurs.
 */
void mcs4_op_iac(struct mcs4 *cpu)
{
	cpu->tmp = cpu->a;
    cpu->a++;
    if (cpu->tmp > cpu->a) {
        cpu->c = 1;
    } else {
		cpu->c = 0;
	}
    _mcs4_inc_pc(cpu);
}


/*
 * Complement Carry
 * CMC | 1111 0011
 * Complements the carry flag.
 */
void mcs4_op_cmc(struct mcs4 *cpu)
{
    cpu->c = (cpu->c == 1) ? 0 : 1;
    _mcs4_inc_pc(cpu);
}


/*
 * Complement Accumulator
 * CMA | 1111 0100
 * Complements the accumulator.
 */
void mcs4_op_cma(struct mcs4 *cpu)
{
    cpu->a = ~cpu->a;
    _mcs4_inc_pc(cpu);
}


/*
 * Rotate Left
 * RAL | 1111 0101
 * The accumulator and carry flag are rotated to the left.
 */
void mcs4_op_ral(struct mcs4 *cpu)
{
	cpu->tmp = cpu->a;
    cpu->a = (cpu->a << 1) | cpu->c;
    if (cpu->tmp >= cpu->a && cpu->c) {
        cpu->c = 1;
    } else {
		cpu->c = 0;
	}
    _mcs4_inc_pc(cpu);
}


/*
 * Rotate Right
 * RAR | 1111 0110
 * The accumulator and carry flag are rotated to the right.
 */
void mcs4_op_rar(struct mcs4 *cpu)
{
    cpu->tmp = cpu->a & 1;
    cpu->a = (cpu->a >> 1) | (cpu->c << 3);
    cpu->c = cpu->tmp;
    _mcs4_inc_pc(cpu);
}


/*
 * Transmit Carry and Clear
 * TCC | 1111 0111
 * The accumulator is cleared and set to the value of the carry flag. The carry
 * flag is then also cleared.
 */
void mcs4_op_tcc(struct mcs4 *cpu)
{
    cpu->a = cpu->c & 1;
    cpu->c = 0;
    _mcs4_inc_pc(cpu);
}


/*
 * Decrement Accumulator
 * DAC | 1111 1000
 * The accumulator is decremented by 1. The carry flag is set if no borrow
 * occurs.
 */
void mcs4_op_dac(struct mcs4 *cpu)
{
	cpu->tmp = cpu->a;
    cpu->a += 0xF;
    if (cpu->a > cpu->tmp) {
        cpu->c = 0;
    } else {
		cpu->c = 1;
	}
    _mcs4_inc_pc(cpu);
}


/*
 * Transfer Carry Subtract
 * TCS | 1111 1001
 * The accumulator is set to 9 if the carry flag is not set, otherwise it is
 * set to 10. The carry flag is unset.
 */
void mcs4_op_tcs(struct mcs4 *cpu)
{
    cpu->a = 9 + cpu->c;
    cpu->c = 0;
    _mcs4_inc_pc(cpu);
}


/*
 * Set Carry
 * STC | 1111 1010
 * The carry flag is set.
 */
void mcs4_op_stc(struct mcs4 *cpu)
{
    cpu->c = 1;
    _mcs4_inc_pc(cpu);
}


/*
 * Decimal Adjust Accumulator
 * DAA | 1111 1011
 * The accumulator is incremented by 6 if the carry flag is set or if the
 * accumulator is greater than 9. The carry flag is set if the result
 * generates a carry.
 */
void mcs4_op_daa(struct mcs4 *cpu)
{
	cpu->tmp = cpu->a;
    if (cpu->a > 9 || cpu->c) {
        cpu->a += 6;
	}
    if (cpu->tmp > cpu->a) {
        cpu->c = 1;
    } else {
		cpu->c = 0;
	}
    _mcs4_inc_pc(cpu);
}


/*
 * Keyboard Process
 * KBP | 1111 1100
 * If the accumulator has more than 1 bit set, the accumulator will be set to
 * 0xf (to indicate an error has occurred).
 */
void mcs4_op_kbp(struct mcs4 *cpu)
{
    switch (cpu->a) {
        case 0x0:
            cpu->a = 0x0;
            break;
        case 0x1:
            cpu->a = 0x1;
            break;
        case 0x2:
            cpu->a = 0x2;
            break;
        case 0x4:
            cpu->a = 0x3;
            break;
        case 0x8:
            cpu->a = 0x4;
            break;
        default:
            cpu->a = 0xF;
            break;
    }
    _mcs4_inc_pc(cpu);
}


/*
 * Designate Command Line
 * DCL | 1111 1101
 * Transfers the data in the accumulator to the CMRAM register, which selects
 * a new RAM bank for use according to this table:
 *
 * A    Bank    CMRAM Enabled
 * ----------------------------
 * X000 Bank 0  CMRAM 0
 * X001 Bank 1  CMRAM 1
 * X010 Bank 2  CMRAM 2
 * X100 Bank 3  CMRAM 3
 * X011 Bank 4  CMRAM 1, CMRAM 2
 * X101 Bank 5  CMRAM 1, CMRAM 3
 * X110 Bank 6  CMRAM 2, CMRAM 3
 * X111 Bank 7  CMRAM 1, CMRAM 2, CMRAM 3
 */
void mcs4_op_dcl(struct mcs4 *cpu)
{
    switch (cpu->a & 0x7) {
        case 0x0:
            cpu->cmram = 1;
            break;
        case 0x1:
            cpu->cmram = 2;
            break;
        case 0x2:
            cpu->cmram = 4;
            break;
        case 0x3:
            cpu->cmram = 3;
            break;
        case 0x4:
            cpu->cmram = 8;
            break;
        case 0x5:
            cpu->cmram = 10;
            break;
        case 0x6:
            cpu->cmram = 12;
            break;
        case 0x7:
            cpu->cmram = 14;
            break;
    }
    _mcs4_inc_pc(cpu);
}


/*
 * Invalid opcodes are, by design, treated as a NOP.
 */
void mcs4_op_inv(struct mcs4 *cpu)
{
    _mcs4_inc_pc(cpu);
}
