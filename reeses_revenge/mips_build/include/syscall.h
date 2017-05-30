#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "errno.h"
#include "syscalls.h"

typedef unsigned int size_t;

_syscall0(unsigned int,time)
_syscall0(unsigned int,rand)
_syscall1(int,exit,int,status)
_syscall3(size_t,read,int,fd,void*,buf,size_t,count)
_syscall3(size_t,write,int,fd,void*,buf,size_t,count)
_syscall3(void*,mmap,void*,address,size_t,length,unsigned int,prot)
/*int exit(int a) 
{                                                                       
        long err;                                                       
        long sys_result;                                                
        {                                                               
        register unsigned long __v0 asm("$2");                          
        register unsigned long __a0 asm("$4") = (unsigned long) a;      
        register unsigned long __a3 asm("$7");                          
        __asm__ volatile (                                              
        ".set   noreorder\n\t"                                          
        "li     $2, %3\t\t\t# exit \n\t"                            
        "syscall\n\t"                                                   
        ".set reorder"                                                  
        : "=r" (__v0), "=r" (__a3)                                      
        : "r" (__a0), "i" (SYS_ify(exit))                               
        : "$1", "$3", "$8", "$9", "$10", "$11", "$12", "$13",           
                "$14", "$15", "$24", "$25", "memory");                  
        err = __a3;                                                     
        sys_result = __v0;                                              
        }                                                               
        if (err == 0)                                                   
                return (int) sys_result;                               
        __set_errno(sys_result);                                        
        return (int)-1;                                                
}
*/

#endif // _SYSCALL_H
