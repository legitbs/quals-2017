#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sphinxbase/err.h>
#include <pocketsphinx.h>
#include <sphinxbase/cmd_ln.h>
#include <zlib.h>
#include <signal.h>

#define MODELDIR "./model"
//#define MODELDIR "cmusphinx-en-us-8khz-5.2"

typedef void (*sighandler_t)(int);
#define AlarmTime 30

static void alarmHandle(int sig)
{
	write(1, "Alarmed\n", 8);
	_exit(0);
}

int main(int argc, char *argv[])
{
	ps_decoder_t *ps;
	cmd_ln_t *config;
	char *hyp;
	int16 buf[0x10000];
	int8 buf8[0x10000];
	int8 buf_comp[0x10000];
	int32 score;
	int FileSize, CurSize;
	int CurPos;
	short XorVal[8] = {0x8000, 0x8000, 0x8000, 0x8000,0x8000, 0x8000, 0x8000, 0x8000};
	int Opcode;
	unsigned long Stack[1000];
	int StackPos;
	int IP;
	const char Insanity[] = "insanity";
	unsigned long Entry1, Entry2;
	char *NewString;
	unsigned long *LoadEntry;
	int ret;
	z_stream strm;

	//make sure we stop if they try to just hang the connection open
        signal(SIGALRM, (sighandler_t)alarmHandle);
        alarm(AlarmTime);

	setbuf(stdout, NULL);
/*
	//make sure we are in our folder of the binary incase our working path didn't get setup properly
	readlink("/proc/self/exe", (char *)buf8, sizeof(buf8));
	NewString = strrchr((char *)buf8, '/');
	*NewString = 0;
	chdir((char *)buf8);
*/

	memset(Stack, 0, sizeof(Stack));

	err_set_logfp(NULL);
	config = cmd_ln_init(NULL, ps_args(), TRUE,
		 "-hmm", MODELDIR "/en-us",
		 "-lm", MODELDIR "/en-us.lm.bin",
		 "-dict", MODELDIR "/cmudict-en-us.dict",
		 NULL);
	if (config == NULL) {
		fprintf(stderr, "Failed to create config object, see log for details\n");
		return -1;
	}

	ps = ps_init(config);
	if (ps == NULL) {
		fprintf(stderr, "Failed to create recognizer, see log for details\n");
		return -1;
	}

	Stack[0] = (unsigned long)Stack | 0x8000000000000000;
	StackPos = 2;
	while(1)
	{
	        alarm(AlarmTime);

		CurPos = read(0, &FileSize, sizeof(FileSize));
		if(CurPos <= 0)
			return 0;

		if(FileSize == 0)
			break;

		ps_start_utt(ps);

		write(1, ".", 1);

		//prep inflate
		memset(&strm, 0, sizeof(strm));
		ret = inflateInit(&strm);
		if (ret != Z_OK)
			return 0;

		CurPos = 0;
		while (FileSize && (ret != Z_STREAM_END)) {
			size_t nsamp;

			if(FileSize > sizeof(buf_comp))
				CurSize = sizeof(buf_comp);
			else
				CurSize = FileSize;

			//read a buffer full of data
			CurPos = 0;
			while(CurPos < CurSize)
			{
				nsamp = read(0, &buf_comp[CurPos], CurSize - CurPos);
				if(nsamp <= 0)
					return 0;

				CurPos += nsamp;
			}

			//adjust how much we read
			FileSize -= CurSize;

			//decompress it into buf8
			strm.avail_in = CurSize;
			strm.next_in = buf_comp;

			/* run inflate() on input until output buffer not full */
			do {
				strm.avail_out = sizeof(buf8);
				strm.next_out = buf8;
				ret = inflate(&strm, Z_NO_FLUSH);
				switch (ret) {
					case Z_STREAM_ERROR:
					case Z_NEED_DICT:
					case Z_DATA_ERROR:
					case Z_MEM_ERROR:
						return 0;
				}
				nsamp = sizeof(buf8) - strm.avail_out;

				//for(i = 0; i <nsamp; i++)
				//	buf[i] = (((short)buf8[i]) << 8) ^ 0x8000;

				//assembly for the commented loop above
				__asm__("lea %2, %%rsi\n"
					"lea %3, %%rdi\n"
					"mov %0, %%rcx\n"
					"shr $5, %%rcx\n"
					"add $1, %%rcx\n"
					"movntdqa %1, %%xmm1\n"
					"convert_loop:\n"
					"movntdqa (%%rsi), %%xmm2\n"	//xmm2 is our data
					"movntdqa 16(%%rsi), %%xmm3\n"	//xmm3 is our data
					"pxor %%xmm0, %%xmm0\n"
					"punpcklbw %%xmm2, %%xmm0\n"	//xmm0 - low 4 of xmm2 intermixed with 0's
					"pxor %%xmm1, %%xmm0\n"
					"movntdq %%xmm0, (%%rdi)\n"
					"pxor %%xmm0, %%xmm0\n"
					"punpckhbw %%xmm2, %%xmm0\n"	//xmm0 - high 4 of xmm2 intermixed with 0's
					"pxor %%xmm1, %%xmm0\n"
					"movntdq %%xmm0, 16(%%rdi)\n"
					"pxor %%xmm0, %%xmm0\n"
					"punpcklbw %%xmm3, %%xmm0\n"	//xmm0 - low 4 of xmm2 intermixed with 0's
					"pxor %%xmm1, %%xmm0\n"
					"movntdq %%xmm0, 32(%%rdi)\n"
					"pxor %%xmm0, %%xmm0\n"
					"punpckhbw %%xmm3, %%xmm0\n"	//xmm0 - high 4 of xmm2 intermixed with 0's
					"pxor %%xmm1, %%xmm0\n"
					"movntdq %%xmm0, 48(%%rdi)\n"
					"add $64, %%rdi\n"
					"add $32, %%rsi\n"
					"loop convert_loop\n"
					:
					: "g" (nsamp), "m" (XorVal), "m" (buf8), "m" (buf)
					: "rcx", "rsi", "rdi"
					);

			        alarm(AlarmTime);
				ps_process_raw(ps, buf, nsamp, FALSE, FALSE);
			} while (strm.avail_out == 0);
		}

		//if we still have data then fail it
		if(FileSize || (ret != Z_STREAM_END))
			return 0;

		//go decode the line
		ps_end_utt(ps);
		hyp = ps_get_hyp(ps, &score);
		//printf("score: %d\n", ps_get_prob(ps));
		//puts(hyp);

		//start processing on the input
		CurPos = 0;
		Opcode = 0;
		while(hyp[CurPos] && ((StackPos-2) <= (sizeof(Stack) / sizeof(Stack[0]))))
		{
			//see what we are parsing
			if(memcmp(&hyp[CurPos], "insanity ", 9) == 0)
			{
				Opcode++;
				CurPos += 9; //insanity + space
			}
			else if(memcmp(&hyp[CurPos], "insane\0", 7) == 0)
			{
				//store the opcode
				CurPos += 7;	//insane + space
				Stack[StackPos++] = Opcode;

				//new opcode
				Opcode = 0;
			}
			else
				break;
		};
	};

	write(1, "\n", 1);

	Stack[StackPos] = 0;

	//point to the rest of the data
	Stack[1] = (unsigned long)&hyp[CurPos] | 0x8000000000000000;

	//stack is filled, start processing on it
	IP = 2;
	while(Stack[IP])
	{
		//printf("cmd: %ld\n", Stack[IP]);
		switch(Stack[IP])
		{
			case 1:
				if((StackPos+1) == (sizeof(Stack) / sizeof(Stack[0])))
				{
					puts("Internal limitation\n");
					_exit(0);
				}

				//point to the string and set a flag indicating a string
				Stack[++StackPos] = (unsigned long)Insanity | 0x8000000000000000;
				break;

			case 2:
				//add two stack entries together
				Entry1 = Stack[StackPos--];
				Entry2 = Stack[StackPos--];

				//see if we are combining strings
				if(Entry1 & Entry2 & 0x8000000000000000)
				{
					Entry1 &= 0x7fffffffffffffff;
					Entry2 &= 0x7fffffffffffffff;

					CurPos = strlen((char *)Entry1);
					CurPos += strlen((char *)Entry2);
					CurPos++;

					//allocate the new string
					NewString = (char *)malloc(CurPos);
					strcpy(NewString, (char *)Entry1);
					strcat(NewString, (char *)Entry2);
					Stack[++StackPos] = (unsigned long)NewString;

					//free the originals if need be
					if(Entry1 != (unsigned long)Insanity)
						free((void *)Entry1);
					if(Entry2 != (unsigned long)Insanity)
						free((void *)Entry2);
				}
				else if((Entry1 & 0x8000000000000000) || (Entry2 & 0x8000000000000000))
				{
					//can't do string and number
					puts("Invalid add\n");
					_exit(0);
				}
				else
					Stack[++StackPos] = Entry1 + Entry2;
				break;

			case 3:
				Entry1 = Stack[StackPos--];
				Entry2 = Stack[StackPos--];

				//see if we are combining strings
				if((Entry1 & 0x8000000000000000) || (Entry2 & 0x8000000000000000))
				{
					//can't do string and number
					puts("Invalid subtract\n");
					_exit(0);
				}
				else
					Stack[++StackPos] = Entry2 - Entry1;
				break;

			case 4:
				Entry1 = Stack[StackPos--];
				Entry2 = Stack[StackPos--];

				//see if we are combining strings
				if((Entry1 & 0x8000000000000000) || (Entry2 & 0x8000000000000000))
				{
					//can't do string and number
					puts("Invalid multiply\n");
					_exit(0);
				}
				else
					Stack[++StackPos] = Entry1 * Entry2;
				break;

			case 5:
				Entry1 = Stack[StackPos--];
				Entry2 = Stack[StackPos--];

				//see if we are combining strings
				if(Entry1 & Entry2 & 0x8000000000000000)
				{
					Entry1 &= 0x7fffffffffffffff;
					Entry2 &= 0x7fffffffffffffff;

					Stack[StackPos++] = (strcmp((char *)Entry1, (char *)Entry2) == 0);
				}
				else if((Entry1 & 0x8000000000000000) || (Entry2 & 0x8000000000000000))
				{
					//can't do string and number
					puts("Invalid compare\n");
					_exit(0);
				}
				else
					Stack[++StackPos] = (Entry1 == Entry2);
				break;

			case 6:
				Entry1 = Stack[++IP];
				if((Entry1 != 0) && (Entry1 != 1))
				{
					puts("Invalid load\n");
					_exit(0);
				}

				//make sure we have a pointer
				LoadEntry = (unsigned long *)Stack[Entry1];
				if(!((unsigned long)LoadEntry & 0x8000000000000000))
				{
					puts("Invalid load\n");
					_exit(0);
				}

				LoadEntry = (unsigned long *)((unsigned long)LoadEntry & 0x7fffffffffffffff);
				Entry2 = Stack[StackPos--];
				//printf("load - [%ld] = %lx\n", Entry2, LoadEntry[Entry2]);
				Stack[++StackPos] = LoadEntry[Entry2];
				break;

			case 7:
				Entry1 = Stack[StackPos--];
				Entry2 = Stack[StackPos--];

				//see if they are trying to store using a string which is invalid
				if(Entry1 & 0x8000000000000000)
				{
					//can't do string and number
					puts("Invalid store\n");
					_exit(0);
				}

				//printf("store - [%ld] = %lx\n", Entry1, Entry2);
				Stack[Entry1] = Entry2;
				break;

			case 8:
				Entry1 = Stack[StackPos--];
				Entry2 = Stack[StackPos--];

				//see if we are combining strings
				if((Entry1 & 0x8000000000000000) || (Entry2 & 0x8000000000000000))
				{
					//can't do string and number
					puts("Invalid jump\n");
					_exit(0);
				}

				//if entry then jump
				if(Entry2)
					IP += Entry1 - 1;

				break;

			case 9:
				if((StackPos+1) == (sizeof(Stack) / sizeof(Stack[0])))
				{
					puts("Internal limitation\n");
					_exit(0);
				}

				//add char
				NewString = malloc(2);
				NewString[0] = Stack[--StackPos] & 0xff;
				NewString[1] = 0;
				Stack[++StackPos] = (unsigned long)NewString | 0x8000000000000000;
				break;

			default:
				if((StackPos+1) == (sizeof(Stack) / sizeof(Stack[0])))
				{
					puts("Internal limitation\n");
					_exit(0);
				}

				Stack[++StackPos] = Stack[IP] - 10;
		};

		IP++;
	};

	//print the result
	if(Stack[StackPos] & 0x8000000000000000)
		printf("result: %s\n", (char *)(Stack[StackPos] & 0x7fffffffffffffff));
	else
		printf("result: %lx\n", Stack[StackPos]);

	return 0;
}
