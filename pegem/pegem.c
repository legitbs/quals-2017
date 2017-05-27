#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include "pegem.h"

#define IO (0xFFFF)
#define DEBUG (0)
// Sample exploit takes ~500k instructions.  So, 2M should be more than enough.
#define MAX_INSTRUCTIONS (20000000)

/*
void Logit(const char *format, ...) {
	va_list args;
	char buf[1000];
        
	if (!DEBUG) {
		return;
	}
	va_start(args, format);
	vsprintf(buf, format, args);
	write(2, buf, strlen(buf));
	va_end(args);

	return;
}
*/

void TooSlow(int x) {
	printf("Too Slow\n");
	exit(0);
}

void SubleqExecute(short *PROG, unsigned short ProgLen) {
	unsigned short PC = 0;
	unsigned short A,B,C;
	int i;
	unsigned int InstructionsExecuted = 0;

	alarm(5);
	signal(SIGALRM, TooSlow);

	while (1) {
		if (InstructionsExecuted > MAX_INSTRUCTIONS) {
			printf("Done\n");
			return;
		}
			
		if (PC > ProgLen) {
			printf("Done\n");
			return;
		}
		A = PROG[PC];
		B = PROG[PC+1];
		C = PROG[PC+2];

		if (C > ProgLen) {
			printf("Done\n");
			return;
		}
		
		// Handle Input
		if (A == IO && B == IO) {
			printf("IO Fault\n");
			return;
		}
		if (A == IO) {
			if (B > ProgLen) {
				printf("Invalid address\n");
				return;
			}
			PROG[B] = fgetc(stdin);
			alarm(5);
			if (PROG[B] <= 0) {
				PC = C;
			} else {
				PC += 3;
			}
			InstructionsExecuted++;
			continue;
		}
		
		// Handle Output
		if (B == IO) {
			if (A > ProgLen) {
				printf("Invalid address\n");
				return;
			}
			putchar(PROG[A]);
			fflush(stdout);
			PC += 3;
			InstructionsExecuted++;
			continue;
		}

		if (A > ProgLen || B > ProgLen) {
			printf("Invalid address\n");
			return;
		}

		PROG[B] = PROG[B] - PROG[A];
		if (PROG[B] <= 0) {
			PC = C;
		} else {
			PC += 3;
		}
		InstructionsExecuted++;
	}
}


int main(int argc, char **argv) {

	SubleqExecute(PROG, ProgLen);

	exit(0);
}
