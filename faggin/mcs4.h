/*
 * mcs4.h | MCS-4 Emulation Library
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

#ifndef __MCS4_H__
#define __MCS4_H__

#include <stdint.h>

// MCS4 system structure
struct __attribute__((packed)) mcs4 {
    uint8_t prom[2048];     // 8 Intel 4001 PROM Chips (8x256 bytes)
    uint8_t promio[4];      // 8 Intel 4001 I/O Port (8 pins, 4 bits each)
    uint8_t ram[8][4][10];  // 8 Intel 4002 RAM Chips (8x4x20 4-bit words)
    uint8_t ramout[4];      // 8 Intel 4002 Output Ports (8 ports, 4 bits each)
    uint8_t r[8];           // Intel 4004 Index Registers (16 registers, 4 bits each)
    uint16_t ms;            // Intel 4004 Memory Select (16 bits)
    uint8_t cmram:4;        // Intel 4004 RAM Control (4 pins)
    uint8_t sp:4;           // Intel 4004 Stack Pointer (4 bits)
    uint8_t tmp:4;          // Intel 4004 Temporary Register (4 bits)
    uint8_t a:4;            // Intel 4004 Accumulator (4 bits)
    uint8_t c:1;            // Intel 4004 Carry Flag (1 bit)
    uint8_t t:1;            // Intel 4004 Test Flag (1 bit)
    uint8_t unused:2;       // padding for the structure
    uint8_t pc[6];          // Intel 4004 Program Counter Registers (4 registers, 12 bits each)
    uint32_t cycles;        // cycle count for Intel 4004
    int (*print)(const int, const char *, ...);  // function pointer to I/O handler
    void (*exec[256])(struct mcs4 *);  // function pointers for instruction handlers
    
};

// helper functions
void mcs4_init(struct mcs4 *cpu, uint8_t *prom, int (*print)(const int, const char *, ...));  // initializes structure
void mcs4_step(struct mcs4 *cpu);  // advances the state of the system

// machine instructions
void mcs4_op_nop();  // no operation
void mcs4_op_jcn();  // jump conditional
void mcs4_op_fim();  // fetch immediate
void mcs4_op_src();  // send register control
void mcs4_op_fin();  // fetch indirect
void mcs4_op_jin();  // jump indirect
void mcs4_op_jun();  // jump unconditional
void mcs4_op_jms();  // jump to subroutine
void mcs4_op_inc();  // increment
void mcs4_op_isz();  // increment and skip
void mcs4_op_add();  // add
void mcs4_op_sub();  // subtract
void mcs4_op_ld();   // load
void mcs4_op_xch();  // exchange
void mcs4_op_bbl();  // branch back and load
void mcs4_op_ldm();  // load immediate
// i/o instructions
void mcs4_op_wrm();  // write main memory
void mcs4_op_wmp();  // write RAM port
void mcs4_op_wrr();  // write ROM port
void mcs4_op_wr();   // write status char
void mcs4_op_sbm();  // subtract main memory
void mcs4_op_rdm();  // read main memory
void mcs4_op_rdr();  // read ROM port
void mcs4_op_adm();  // add main memory
void mcs4_op_rd();   // read status char
// accumulator instructions
void mcs4_op_clb();  // clear both
void mcs4_op_clc();  // clear carry
void mcs4_op_iac();  // increment accumulator
void mcs4_op_cmc();  // complement carry
void mcs4_op_cma();  // complement accumulator
void mcs4_op_ral();  // rotate left
void mcs4_op_rar();  // rotate right
void mcs4_op_tcc();  // transmit carry and clear
void mcs4_op_dac();  // decrement accumulator
void mcs4_op_tcs();  // transfer carry subtract
void mcs4_op_stc();  // set carry
void mcs4_op_daa();  // decimal adjust accumulator
void mcs4_op_kbp();  // keyboard process
void mcs4_op_dcl();  // designate command line
// helper instructions
void mcs4_op_inv();  // invalid instruction

// array of function pointers for executing an instruction regardless of OPA's value
static void (*_mcs4_ftable[256])(struct mcs4 *) = {
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

#ifdef _DEBUG
// array holding all mnemonics for displaying in DEBUG mode
static char _mcs4_mnemonic[256][4] = {
    "NOP", "INV", "INV", "INV", "INV", "INV", "INV", "INV",
    "INV", "INV", "INV", "INV", "INV", "INV", "INV", "INV",
    "JCN", "JCN", "JCN", "JCN", "JCN", "JCN", "JCN", "JCN",
    "JCN", "JCN", "JCN", "JCN", "JCN", "JCN", "JCN", "JCN",
    "FIM", "SRC", "FIM", "SRC", "FIM", "SRC", "FIM", "SRC",
    "FIM", "SRC", "FIM", "SRC", "FIM", "SRC", "FIM", "SRC",
    "FIN", "JIN", "FIN", "JIN", "FIN", "JIN", "FIN", "JIN",
    "FIN", "JIN", "FIN", "JIN", "FIN", "JIN", "FIN", "JIN",
    "JUN", "JUN", "JUN", "JUN", "JUN", "JUN", "JUN", "JUN",
    "JUN", "JUN", "JUN", "JUN", "JUN", "JUN", "JUN", "JUN",
    "JMS", "JMS", "JMS", "JMS", "JMS", "JMS", "JMS", "JMS",
    "JMS", "JMS", "JMS", "JMS", "JMS", "JMS", "JMS", "JMS",
    "INC", "INC", "INC", "INC", "INC", "INC", "INC", "INC",
    "INC", "INC", "INC", "INC", "INC", "INC", "INC", "INC",
    "ISZ", "ISZ", "ISZ", "ISZ", "ISZ", "ISZ", "ISZ", "ISZ",
    "ISZ", "ISZ", "ISZ", "ISZ", "ISZ", "ISZ", "ISZ", "ISZ",
    "ADD", "ADD", "ADD", "ADD", "ADD", "ADD", "ADD", "ADD",
    "ADD", "ADD", "ADD", "ADD", "ADD", "ADD", "ADD", "ADD",
    "SUB", "SUB", "SUB", "SUB", "SUB", "SUB", "SUB", "SUB",
    "SUB", "SUB", "SUB", "SUB", "SUB", "SUB", "SUB", "SUB",
    "LD ", "LD ", "LD ", "LD ", "LD ", "LD ", "LD ", "LD ",
    "LD ", "LD ", "LD ", "LD ", "LD ", "LD ", "LD ", "LD ",
    "XCH", "XCH", "XCH", "XCH", "XCH", "XCH", "XCH", "XCH",
    "XCH", "XCH", "XCH", "XCH", "XCH", "XCH", "XCH", "XCH",
    "BBL", "BBL", "BBL", "BBL", "BBL", "BBL", "BBL", "BBL",
    "BBL", "BBL", "BBL", "BBL", "BBL", "BBL", "BBL", "BBL",
    "LDM", "LDM", "LDM", "LDM", "LDM", "LDM", "LDM", "LDM",
    "LDM", "LDM", "LDM", "LDM", "LDM", "LDM", "LDM", "LDM",
    "WRM", "WMP", "WRR", "INV", "WR ", "WR ", "WR ", "WR ",
    "SBM", "RDM", "RDR", "ADM", "RD ", "RD ", "RD ", "RD ",
    "CLB", "CLC", "IAC", "CMC", "CMA", "RAL", "RAR", "TCC",
    "DAC", "TCS", "STC", "DAA", "KBP", "DCL", "INV", "INV"
};
#endif

#endif
