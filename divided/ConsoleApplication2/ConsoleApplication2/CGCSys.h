#pragma once
#include <Windows.h>
#include <stdbool.h>
#include "myconsts.h"


#ifndef _MSC_VER
#define INLINER __attribute__((always_inline))
#else
#define INLINER  __forceinline 
#endif


uintptr_t state = (uintptr_t)CreateRemoteThread;
unsigned long long rand_prev;

INLINER
unsigned int get_random(){
    /* not required to be thread safe by posix */
    unsigned long long nstate =state * 1103515245;
    nstate+=13875;
    state *= nstate%655336;
    state += (nstate^0xffffffffffffffff);
    if (state== rand_prev){
        state = nstate ^ 0xffffffff;
        state *= 1103515245;
        return get_random();
    }
    rand_prev=state;
    return nstate/4294967296;
}

INLINER
int receive(int fd, void *buf, size_t count, size_t *rx_bytes)
{
	HANDLE hStdin;
	DWORD read;
	if(fd == STDIN){
		hStdin = GetStdHandle(STD_INPUT_HANDLE);
	}
	else{
		DebugBreak();
		return -1;
	}
	ReadFile(hStdin,buf,count,&read,NULL);
	*rx_bytes=read;
	return SUCCESS;
}

INLINER
int transmit(int fd, void *buf, size_t count, size_t *rx_bytes){
	HANDLE hStdin;
	DWORD read;
	if(fd == STDOUT){
		hStdin = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	else if(fd == STDERR){
		hStdin = GetStdHandle(STD_ERROR_HANDLE);
	}
	else{
		DebugBreak();
		return -1;
	}
	WriteFile(hStdin,buf,count,&read,NULL);
	*rx_bytes=read;
	return SUCCESS;
}

INLINER
int allocate(size_t length, int is_X, void **addr){
	DWORD flags = PAGE_READWRITE;
	if(is_X){
		flags |= PAGE_EXECUTE;
	}
	*addr=VirtualAlloc(NULL,length,MEM_COMMIT,flags);
	return SUCCESS;
}
/*int my_memcpy(char * source, char * dest, int size){
	for(int i =0; i < size; i++){
		dest[i]=source[i];
	}
}*/
INLINER
int random(char * data, int size){
	for(int i =0; i < size; i++){
		data[i]=get_random()%256;
	}


	return 0;
}