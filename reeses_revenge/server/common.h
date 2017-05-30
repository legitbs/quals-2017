#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <unistd.h>
#include <signal.h>

#include "crypto.h"
#include "RC4Coprocessor.h"
#include "mmu.h"
#include "cpu.h"
#include "syscall_handler.h"
#include "mebloader.h"


#define INSTR_TRACING	0
#define ECEPTION_MSG	1
#define HAS_FPU			0
//#define MMU_DEBUG	1
//#define DEBUG_INSTR_TRACING	1
//#define STACK_DEBUG	1

#define MAX_CPU_INSTRUCTION_COUNT 50000000

//void LOG_ERROR( char *szError, ... ) printf( szError, ... );

#endif // __COMMON_H__
