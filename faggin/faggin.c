/*
 * faggin | MCS-4 Emulation Service
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

#include "ctf.h"
#include "mcs4.h"

#define STDIN STDIN_FILENO
#define STDOUT STDOUT_FILENO
#define MAXCYCLES 4096
#define ALARM 5

#if !defined(_DEBUG) && defined(_ALARM)
/*
 * Handler for alarm signals.
 */
static void alarm_handler(int signal)
{
    if (signal == SIGALRM) {
        exit(-1);
    }
}
#endif


/*
 * Service main function.
 */
int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    unsigned char prom[2048] = {0};
    struct mcs4 cpu;

#ifdef _DEBUG
    warnx("Service was compiled with debug support");
#endif

#if !defined(_DEBUG) && defined(_ALARM)
    // set up alarm
    signal(SIGALRM, (sighandler_t)alarm_handler);
    alarm(ALARM);
#endif

    // place stdout into non-buffered mode
    setvbuf(stdout, NULL, _IONBF, 0);

    // initialize the CPU
    ctf_readn(STDIN, prom, 2048);
    mcs4_init(&cpu, prom, &ctf_writef);

#ifdef _DEBUG
    ctf_writes(STDOUT, "Starting contents of accessible RAM:\n");
    for (uint32_t i = 0; i < 16; i++) {
        ctf_writef(STDOUT, "%08x %02d: ", &cpu.ram[i][0][0], i);
        for (uint32_t j = 0; j < 4; j++) {
            for (uint32_t k = 0; k < 10; k++) {
                ctf_writef(STDOUT, "%02x ", cpu.ram[i][j][k]);
            }
            ctf_writes(STDOUT, "-- ");
        }
        ctf_writes(STDOUT, "\n");
    }
    
    ctf_writes(STDOUT, "");
#endif

    // execute the user's program
    while (cpu.cycles < MAXCYCLES) {
        mcs4_step(&cpu);
    }

    return 0;
}
