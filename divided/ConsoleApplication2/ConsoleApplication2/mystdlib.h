#pragma once
#include "myconsts.h"
#ifndef _MSC_VER
#include <libcgc.h>
#else
#include "CGCSys.h"
#include <stdio.h>
#endif



typedef struct malloc_meta {
	unsigned int cookie;
	void (*gethash)(void*);
	struct malloc_meta* back_pointer;
	struct malloc_meta* front_pointer;
	int allocated;
	int size;
} malloc_meta;



unsigned int total_size = HEAP_SIZE*MAX_BUCKETS;
malloc_meta * HEAP_BUCKETS[MAX_BUCKETS];

static void * my_malloc(int size);
static void my_free(void * data);
static void init_heap();
static void validate_heap();
static void transmit_newline();
static void transmit_int(unsigned int );
static size_t read_until( char *dst, char delim, size_t max );
static void transmit_str(char * str);
unsigned int assigned_buckets=0;
unsigned int useless_var = 9;
unsigned int amount_allocated=0;
void (*cleanup_func)() = NULL;
unsigned int comments_missed;

void quit(int);


typedef struct {
	int amount_read ;
	int amount_sent ;
	char buffered_data[1024];
} buffer_object;
buffer_object bobj;

#ifdef _MSC_VER
void print_node(malloc_meta * mm){
  printf("Node = %08x\n\tSize = %d\n\tBlockSize = %d\n\tCookie = %08x\n\tAlloc = %d\n\tNext= %08x\n\tPrev = %08x\n", 
		mm,
		mm->size,
		mm->size - sizeof(malloc_meta),
		mm->cookie,
		mm->allocated,
		mm->front_pointer,
		mm->back_pointer
		);

return;
}
#endif

INLINER
static char buffered_read()
{
	size_t  rx;
	if(bobj.amount_read == bobj.amount_sent){
		bobj.amount_sent=0;
		bobj.amount_read=0;
		if(receive( STDIN, bobj.buffered_data, 1024, &rx )!=0)
		{
			quit(0);
		}
		if(rx==0){
			quit(-1);
		}
		else{
			bobj.amount_read=rx;
		}

	}
	return bobj.buffered_data[bobj.amount_sent++];
}

INLINER
static void  allocate_heap_block(malloc_meta* mmeta,unsigned int size){
	malloc_meta* next_mmeta;
	unsigned char* val = (unsigned char *)mmeta;
	val+= size;
	val+= sizeof(malloc_meta);

	next_mmeta = (malloc_meta *) val;
	next_mmeta->cookie = COOKIE;
	next_mmeta->size = mmeta->size-size-sizeof(malloc_meta);
	next_mmeta->allocated = DATA_FREE;
	next_mmeta->back_pointer = mmeta;
	next_mmeta->front_pointer = mmeta->front_pointer;
	next_mmeta->gethash = NULL;
	mmeta->allocated = DATA_ALLOCATED;
	if(mmeta->front_pointer){
		mmeta->front_pointer->back_pointer=next_mmeta;
	}
	mmeta->front_pointer= next_mmeta;
	mmeta->size=size+sizeof(malloc_meta);
	char * heap_mem = (char *)(mmeta+1);

	for(int i =0; i < mmeta->size-sizeof(malloc_meta);i++){
		heap_mem[i]=0xcd;
	}
}

INLINER
static void is_worst_fit(malloc_meta * mmeta, malloc_meta ** best_fit, unsigned int size){
	if(mmeta->size >= size +sizeof(malloc_meta) && 
		mmeta->allocated==DATA_FREE){
		if(*best_fit != NULL &&
			(*best_fit)->size < mmeta->size){
			*best_fit = mmeta;
		}
		else if(*best_fit == NULL && mmeta->size > size){
			*best_fit = mmeta;
		}
	}

}
INLINER
static void is_best_fit(malloc_meta * mmeta, malloc_meta ** best_fit, unsigned int size){
	if(mmeta->size >= size +sizeof(malloc_meta) &&
		mmeta->allocated==DATA_FREE){
		if(*best_fit != NULL &&
			(*best_fit)->size > mmeta->size){
			*best_fit = mmeta;
		}
		else if(*best_fit == NULL && mmeta->size > size){
			*best_fit = mmeta;
		}
	}
}

INLINER
static void *  worst_fit_alloc(int size){
	while (TRUE){
		for(int i=0; i < assigned_buckets; i++){
			malloc_meta* mmeta= HEAP_BUCKETS[i];
			malloc_meta* best_fit=NULL;
			while(mmeta != NULL){
				is_worst_fit(mmeta,&best_fit,size);
				mmeta = mmeta->front_pointer;
			}
			if(best_fit == NULL ){
				continue;
			}
			if(best_fit->size < size+sizeof(malloc_meta)*2 ){
				best_fit->allocated = DATA_ALLOCATED;
				return best_fit+1; // hackey but works so it ends up pushing +1
			}
			allocate_heap_block(best_fit,size);
			return best_fit+1;
		}
		if(assigned_buckets < MAX_BUCKETS){
			init_heap();
		}else{
			break;
		}
	}
	quit(NOBLOCKSIZELEFT);
	return NULL;
}

INLINER
static void * best_fit_alloc(int size){
	while (TRUE){
		for(int i=0; i < assigned_buckets; i++){
			malloc_meta* mmeta= HEAP_BUCKETS[i];
			malloc_meta* best_fit=NULL;
			while(mmeta != NULL){
				is_best_fit(mmeta, &best_fit, size);
				mmeta = mmeta->front_pointer;
			}
			if(best_fit == NULL ){
				continue;
			}
			if(best_fit->size < size+sizeof(malloc_meta)*2 ){
				best_fit->allocated = DATA_ALLOCATED;
				return best_fit+1; // hackey but works so it ends up pushing +1
			}
			allocate_heap_block(best_fit,size);
			return best_fit+1;
		}
		if(assigned_buckets < MAX_BUCKETS){
			init_heap();
		}else{
			break;
		}
	}
	quit(NOBLOCKSIZELEFT);
	return NULL;
}
INLINER
static void * first_fit_alloc(int size){
	while (TRUE){
		malloc_meta * ret;
		for(int i=0; i < assigned_buckets; i++){
			malloc_meta* mmeta= HEAP_BUCKETS[i];
			while(mmeta != NULL  && 
				(mmeta->allocated != DATA_FREE || mmeta->size < size+sizeof(malloc_meta))
				 ){
					mmeta = mmeta->front_pointer;
			}
			if(mmeta == NULL ){
				continue;
			}
			if(mmeta->size < size+sizeof(malloc_meta)*2 ){
				mmeta->allocated = DATA_ALLOCATED;
				return mmeta+1; // hackey but works so it ends up pushing +1
			}
			allocate_heap_block(mmeta,size);
			ret=mmeta+1;
			return ret;//conversion of size gives +1 
		}
		if(assigned_buckets < MAX_BUCKETS){
			init_heap();
		}
		else{
			break;
		}
	}
	quit(NOBLOCKSIZELEFT);
	return NULL;

}
INLINER
void * my_malloc(int size){
	if (assigned_buckets == 0){
		init_heap();
	}
	//size = size+(size%8)+16;
	current_alloc_policy = (current_alloc_policy +1) % (HIGHEST_POLICY);
	if (current_alloc_policy==POLICY_FIRST_FIT){
		void * a= first_fit_alloc(size);
		return a;
	}
	else if (current_alloc_policy==POLICY_BEST_FIT){
		void * a = best_fit_alloc(size);
		return a;
	}
	else if (current_alloc_policy==POLICY_WORST_FIT){
		void * a= worst_fit_alloc(size);
		return a;
	}
	else{
		quit(INVALID_ALLOCATOR);
	}



	return NULL;
}

INLINER
static void my_free(void * data){
	malloc_meta * to_free =(malloc_meta*)data;
	to_free=to_free-1;
	malloc_meta * coalesce_block = NULL;

	if(to_free->cookie != COOKIE){
		//printf("BAD BLOCK %08x\n",to_free);
		quit(-7);
	}
	if(to_free->gethash != NULL){
		to_free->gethash(data);
	}
	to_free->gethash=NULL;
	to_free->allocated = DATA_FREE;
	while(to_free->back_pointer !=NULL
		&& to_free->back_pointer->allocated==DATA_FREE){
			if(to_free->cookie != COOKIE){
				quit(BAD_COOKIE);
			}
			if(to_free->back_pointer->cookie != COOKIE){
				quit(BAD_COOKIE);
			}
			to_free= to_free->back_pointer;
			if(to_free->front_pointer!=NULL){
				to_free->front_pointer->front_pointer->back_pointer=to_free;
			}
			to_free->size+=to_free->front_pointer->size;
			to_free->front_pointer=to_free->front_pointer->front_pointer;
			to_free->gethash= NULL;
	}
	while(to_free->front_pointer !=NULL
		&& to_free->front_pointer->allocated==DATA_FREE){
			if(to_free->cookie != COOKIE){
				quit(BAD_COOKIE);
			}
			to_free->size+=to_free->front_pointer->size;
			to_free->front_pointer=to_free->front_pointer->front_pointer;
			to_free->gethash= NULL;
			if(to_free->front_pointer!=NULL){
				to_free->front_pointer->back_pointer=to_free;
			}
	}
	return;
}



static void * getBasePages(int i){
	void * addr;
	allocate(i,0,&addr);
	return addr;
}

INLINER
static void init_heap(){
	if (assigned_buckets < MAX_BUCKETS){
		HEAP_BUCKETS[assigned_buckets] = (malloc_meta *)getBasePages(HEAP_SIZE);
		malloc_meta * mmeta =HEAP_BUCKETS[assigned_buckets]; 
		mmeta->back_pointer=(malloc_meta *)NULL;
		mmeta->front_pointer=(malloc_meta *)NULL;
		mmeta->cookie = COOKIE;
		mmeta->size= HEAP_SIZE;
		assigned_buckets++;
	}
	else{
		quit(NO_MORE_BUCKETS);
	}
}

INLINER
char * int_to_array(unsigned long long a){
	int num_chars =0;
	unsigned long long res=a;
	bool is_negative = FALSE;
	int iter=1;
	char *ret;

	do{
		num_chars+=1;
		res=res/10;
	}while(res!=0);
	ret=(char *)my_malloc(num_chars+1); 
	res=a;
	do{
		ret[(num_chars-iter)]=res%10+48;
		iter++;
		res=res/10;
	}while(res!=0);
	ret[iter-1]=STR_TERM;

	return ret;
}
int my_strlen(char * arr){
	int i =0;
	while((char)arr[i]!= (char)STR_TERM){
		i++;
		int k = arr[i];
	}
	return i;
}

INLINER
int array_to_int(char * arr){
	long long i =0;
	long long ret=0;
	int place =0;
	while(48<=arr[i]&& arr[i] <=57){
		ret+= arr[i++]-48;
		ret*=10;
	}
	ret=ret/10;
	return ret;
}


INLINER
static int receive_until(int fd, void* buf, size_t count, size_t *rx_bytes,char delim) {
	*rx_bytes=read_until((char *)buf,delim,count);
	return 0;
}




INLINER
int transmit_all(int fd, void *buf, size_t count, size_t *tx_bytes) {

  int ret = SUCCESS;
  size_t bytes_left = count;
  size_t tx_bytes_local = 0;

  while (bytes_left) {
	tx_bytes_local = 0;
	if (SUCCESS != (ret = transmit(fd, buf, bytes_left, &tx_bytes_local))) {
		return ret;
	}

	  bytes_left -= tx_bytes_local;
  }

  // If we got here, then we got all the bytes.
  if (NULL != tx_bytes) { *tx_bytes = count; }
  return ret;
}


INLINER
int read_int() {
	char tbuf[13];
	size_t amount_read;
	for (int i = 0; i < 13; i++) {
		tbuf[i] = 0;
	}
	receive_until(STDIN, (void*)tbuf, 12, &amount_read, 0xa);
	tbuf[12] = STR_TERM;
	return array_to_int(tbuf);
}

INLINER
unsigned long long array_to_ull(char * arr) {
	unsigned long long i = 0;
	unsigned long long ret = 0;
	int place = 0;
	while (48 <= arr[i] && arr[i] <= 57) {
		ret += arr[i++] - 48;
		ret *= 10;
	}
	i = ret / 10;
	return i;
}

INLINER
unsigned long long read_ull() {
	char tbuf[22];
	size_t amount_read;
	for (int i = 0; i < 22; i++) {
		tbuf[i] = 0;
	}
	receive_until(STDIN, (void*)tbuf, 21, &amount_read, 0xa);
	tbuf[21] = STR_TERM;
	return array_to_ull(tbuf);
}

INLINER
void validate_heap(){
	for(int i=0; i < assigned_buckets; i++){
		malloc_meta* mmeta= HEAP_BUCKETS[i];
		malloc_meta* pp=NULL;
		unsigned int totSize = 0;
		while(mmeta != NULL ){
			totSize += mmeta->size;
			if(mmeta->cookie!= COOKIE ||
				(mmeta->back_pointer != NULL &&mmeta->back_pointer->front_pointer != mmeta) ||
				(mmeta->front_pointer !=NULL && mmeta->front_pointer->back_pointer !=mmeta)){
				quit(VALIDATE_HEAP_FAILED);
			}
			pp=mmeta;
			mmeta = mmeta->front_pointer;
		}
		if(totSize!=HEAP_SIZE){
			quit(VALIDATE_HEAP_FAILED);
		}
		while(mmeta != NULL ){
			if(mmeta->gethash !=NULL){
				mmeta->gethash(mmeta);
			}
			mmeta = mmeta->front_pointer;
		}
	}
}


INLINER
void transmit_str(char * str){
	size_t amountread;
	transmit_all(STDOUT,str,my_strlen(str),&amountread);
}
#ifndef _MSC_VER
__attribute__((always_inline))
#else
__forceinline
#endif
void transmit_int(unsigned int a){
	char * to_str = int_to_array(a);
	transmit_str(to_str);
	my_free(to_str);
}

INLINER
void transmit_ull(unsigned long long a) {
	char * to_str = int_to_array(a);
	transmit_str(to_str);
	my_free(to_str);
}


INLINER
void transmit_char(char a){
	size_t amount_read;
	transmit_all(STDOUT,(void *)&a,1,&amount_read);
}
#ifndef _MSC_VER
__attribute__((always_inline))
#else
__forceinline
#endif
void transmit_newline(){
	size_t amount_read;
	transmit_char(0xa);
}

#ifndef _MSC_VER
__attribute__((always_inline))
#else
__forceinline
#endif
char * read_line(unsigned int buf_len){
	size_t amount_read;
	char * tbuf = (char *)my_malloc(buf_len+3);
	receive_until(STDIN,(void*) tbuf,buf_len+1,&amount_read, 0xa);// two is for new line on windows 
	tbuf[amount_read]=0xff;
	return tbuf;
}
#ifndef _MSC_VER
__attribute__((always_inline))
#else
__forceinline
#endif
void add_cleanup(void* data,void (*gethashs)(void*)){
	malloc_meta * to_free =(malloc_meta*)(((unsigned char*) data)-sizeof(malloc_meta));
	to_free->gethash=gethashs;
}

#ifndef _MSC_VER
__attribute__((always_inline))
#else
__forceinline
#endif
void quit(int code){
	if(code ==0){
		if(cleanup_func!=NULL){
			cleanup_func();
		}
	}
	ExitProcess(code);
}

/*
size_t read_until( char *dst, char delim, size_t max )
{
    size_t len = 0;
    char c = 0;

    while( len < max ) {
        dst[len] = 0x00;
		c=buffered_read();

        if ( c == delim ) {
    		return len;
	    }

        dst[len] = c;
        len++;
    }
end:
    return len;
}*/
#ifndef _MSC_VER
__attribute__((always_inline))
#else
__forceinline
#endif
static size_t read_until( char *dst, char delim, size_t max )
{
    size_t len = 0;
    size_t rx = 0;
    char c = 0;

    while( len < max ) {
        dst[len] = 0x00;
        c= buffered_read();
        if ( c == delim ) {
    		return len;
	    }

        dst[len] = c;
        len++;
    }
end:
    return len;
}


#ifndef _MSC_VER
#pragma GCC push_options
#pragma GCC optimize ("O0")
__attribute__((always_inline))
#else
__forceinline
#endif
static void swiggle(){
	unsigned int to_swig = comments_missed;
	for(int i =0; i < 16; i++){
		to_swig *= 17;
		to_swig ^=0xf3f3efde;
		to_swig &=0xfffeffff;
		to_swig |=0x000000a0;
		to_swig -=0x00fe03a0;
		to_swig +=(i*0xff);
	}
	comments_missed= to_swig;
}

#ifndef _MSC_VER
__attribute__((always_inline))
#else
__forceinline
#endif
static
void fucking_ida(unsigned long var,unsigned  long var2) {
	var *=useless_var;
	var += var2;
	/*if(var % 3 == 0){
		asm volatile ( "add $800,%esp");
	}
	useless_var++;
	if(((var % 9) ^ 0xff2) !=0xff2){
		asm volatile ( "add %eax,%esp");
	}
	if((var^0xfefefefe)*8==var*MAX_BUCKETS){
		asm volatile ( "xor %ebx,%esp");var++;
	}
	if((var+17)*0xff2==var*MAX_BUCKETS){
		asm volatile ( "add $600,%esp");var++;
	}
	if(sttp->padding[(var%25)+6]!=NULL){
		asm volatile ( "add $600,%esp");var++;
	}
	//if((((var+7)%7)^0xf)!=0xf){asm volatile ( "add %edx,%esp");}
	//if(((((var*39)%3))^0xabd)!=0xabd){asm volatile ( "xor %ecx,%esp");}
	if(useless_var < 108){
		asm volatile ( "sub $800,%esp");
	}
	useless_var--;*/
}

#ifndef _MSC_VER
__attribute__((always_inline))
#else
__forceinline
#endif
static void encrypt (unsigned int * v, unsigned int * k) {
    unsigned int v0=v[0], v1=v[1], sum=0, i;           /* set up */
    unsigned int delta=0x9e3779b9;                     /* a key schedule constant */
    unsigned int k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    for (i=0; i < 32; i++) {                       /* basic cycle start */
        sum += delta;
        v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);  
    }                                              /* end cycle */
    v[0]=v0; v[1]=v1;
}


#ifndef _MSC_VER
__attribute__((always_inline))
#else
__forceinline
#endif
static void decrypt (unsigned int* v, unsigned int* k) {
    unsigned int v0=v[0], v1=v[1], sum=0xC6EF3720, i;  /* set up */
    unsigned int delta=0x9e3779b9;                     /* a key schedule constant */
    unsigned int k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    for (i=0; i<32; i++) {                         /* basic cycle start */
        v1 -= ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
        v0 -= ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        sum -= delta;
    }                                              /* end cycle */
    v[0]=v0; v[1]=v1;
}

