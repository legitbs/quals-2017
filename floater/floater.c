/*
 * floater.c | Floating-point Shellcoding Challenge
 *
 * Copyright (c) 2017 Alexander Taylor <ajtaylor@fuzyll.com>
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

#include <seccomp.h>
#include <sys/mman.h>
#include <math.h>
#include "ctf.h"

#define NUM_NUMS 25
#define BUF_LEN 100

#ifdef _ALARM
// function to handle the alarm going off
static void alarm_handler(int signal)
{
    if (signal == SIGALRM) { exit(-1); }
}
#endif

// function to round a float
static float fround(float n, unsigned int d)
{
    return floor(n * pow(10.0, d) + .5) / pow(10.0, d);
}

// main function
int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    scmp_filter_ctx ctx;
    float *data;
    char buf[BUF_LEN] = { 0 };

#ifdef _ALARM
    // set up alarm
    signal(SIGALRM, alarm_handler);
    alarm(4);
#endif

    // allocate memory and mark it writable
    data = mmap(NULL, sizeof(float) * NUM_NUMS * 2, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if (data == NULL) {
        ctf_writes(STDOUT_FILENO, "Could not allocate memory. Contact an administrator.\n");
    }

    // read user input into dblary
    for (int i = 0; i < NUM_NUMS * 2; i+=2) {
        ctf_readsn(STDIN_FILENO, buf, BUF_LEN);
        data[i] = fround(strtof(buf, NULL), 3);
    }

#ifdef _DEBUG
    // print out the data that we got
    ctf_writes(STDOUT_FILENO, "DEBUG: Got the following from the user:\n");
    for (int i = 0; i < NUM_NUMS * 2; i++) {
        ctf_writef(STDOUT_FILENO, "0x%x - %f\n", *(unsigned int *)(&data[i]), data[i]);
    }
#endif

    // mark memory executable
    if (mprotect(data, sizeof(float) * NUM_NUMS * 2, PROT_EXEC)) {
        ctf_writes(STDOUT_FILENO, "Could not make memory executable. Contact an administrator.\n");
    }

    // turn on sandbox
    ctx = seccomp_init(SCMP_ACT_KILL);
    if (ctx == NULL ||
        seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(open), 0) < 0 ||
        seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0) < 0 ||
        seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0) < 0 ||
        seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(close), 0) < 0 ||
        seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0) < 0 ||
        seccomp_load(ctx) < 0) {
            ctf_writes(STDOUT_FILENO, "Could not initialize sandbox. Contact an administrator.\n");
            seccomp_release(ctx);
            exit(-1);
    }
    seccomp_arch_add(ctx, SCMP_ARCH_X86_64);

    // execute user payload
    ((void (*)(void))data)();

    // clean up
    munmap(data, sizeof(float) * NUM_NUMS * 2);

    return 0;
}
