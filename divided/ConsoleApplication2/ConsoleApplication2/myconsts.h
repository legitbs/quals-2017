#pragma once

#define NOBLOCKSIZELEFT -5
#define INVALID_ALLOCATOR -6
#define BAD_COOKIE -7
#define NO_MORE_BUCKETS -8
#define VALIDATE_HEAP_FAILED -9
#define INVALID_CD_CHOOSEN -10
#define TOO_MANY_COMMENTS -11
#define INVALID_TRACK_CHOOSEN -12
#define ATTEMPTED_EXPLOIT -13


#define DATA_FREE 0
#define DATA_GUARD 1
#define DATA_ALLOCATED 2

#define POLICY_FIRST_FIT 0
#define POLICY_BEST_FIT 1
#define POLICY_WORST_FIT 2
#define HIGHEST_POLICY 3
#define STR_TERM 255
#define SUCCESS 0
#define FALSE 0
#define TRUE 1

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define HEAP_SIZE  4096*1024
#define MAX_BUCKETS  10


unsigned int COOKIE = 0x41424344;
unsigned int BLINDER = 0xdeadbe3f;
unsigned int  my_key[4] = { 0 } ;
int NUMAIRPORTS = 15;
#ifndef _MSC_VER
typedef char bool;
#endif
unsigned int current_alloc_policy = POLICY_FIRST_FIT;

#define NUM_PASSENGERS 512
#define NUM_PLANES 512

//#define always_inline ,

