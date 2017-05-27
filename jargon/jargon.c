#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

typedef void (*sighandler_t)(int);
#define AlarmTime 30

char *NumNames[10] = {"lightning","fuzyll","hoju","gynophage","hj","selir","sirgoon","vito","bja","jymbolia"};

typedef void (*mathfunc)();
typedef unsigned char (*disasfunc)(unsigned char Pos);

typedef struct FuncStruct
{
	char		*Command;
	mathfunc	func;
	disasfunc	disas;
} FuncStruct;

char Stack[256];
unsigned char Code[128];
unsigned int StackPos;
unsigned int CodePos;

static void alarmHandle(int sig)
{
	write(1, "Alarmed\n", 8);
	_exit(0);
}

void IntToText(int Value, int DoReturn)
{
	int CurValue;
	int DigitCount;
	int i;

	//if negative, indicate it
	if(Value < 0)
	{
		printf("rogue ");
		Value = -Value;
	}

	//figure out how many digits we need to print
	CurValue = Value;
	DigitCount = 1;
	while(CurValue > 9)
	{
		DigitCount++;
		CurValue /= 10;
	};

	//we now have a number of digits to print, cycle on them
	for(;DigitCount > 0; DigitCount--)
	{
		//get the high digit
		CurValue = Value;
		for(i = 1; i < DigitCount; i++)
			CurValue /= 10;

		//print the digit
		printf("%s ", NumNames[CurValue]);

		//get the value to subtract from the starting value;
		for(i = 1; i < DigitCount; i++)
			CurValue *= 10;

		//subtract the value so we have 1 less digit to handle
		Value = Value - CurValue;
	};

	if(DoReturn)
		printf("\n");
	fflush(stdout);
}

void die(char *error)
{
	int i;

	printf("\n%s\n\nip: ", error);

	//print the stack and code location
	IntToText(CodePos, 1);

	printf("Stack\n-----\n");
	for(i = 0; i < StackPos; i++)
		IntToText(Stack[i], 1);

	fflush(stdout);
	exit(0);
}

char pop()
{
	//if the stack is empty then die
	if(StackPos == 0)
		die("Stack empty");

	//adjust our position
	StackPos--;
	return Stack[StackPos];
}

void push(char Value)
{
	//if the stack is full then die
	if(StackPos == 256)
		die("Stack full");

	//add it and adjust
	Stack[StackPos] = Value;
	StackPos++;
}

void math_add()
{
	//pop 2 values off the stack, add them and push the result
	push(pop() + pop());
}

void math_sub()
{
	//pop 2 values off the stack, subtract them and push the result
	push(pop() - pop());
}

void math_mul()
{
	//pop 2 values off the stack, multiply them and push the result
	push(pop() * pop());
}

void math_mod()
{
	//pop 2 values off the stack, modulus them and push the result
	push(pop() % pop());
}

void math_div()
{
	//pop 2 values off the stack, divide them and push the result
	unsigned char Value1;
	unsigned char Value2;

	Value1 = pop();
	Value2 = pop();

	if(Value2 == 0)
		die("Divide by 0");

	push(Value1 / Value2);
}

void math_and()
{
	//pop 2 values off the stack, and them and push the result
	push(pop() & pop());
}

void math_or()
{
	//pop 2 values off the stack, or them and push the result
	push(pop() | pop());
}

void math_xor()
{
	//pop 2 values off the stack, xor them and push the result
	push(pop() ^ pop());
}

void math_neg()
{
	//pop 1 value off the stack, negate it then push the result
	push(-pop());
}

void math_not()
{
	//pop 1 value off the stack, not it then push the result
	push(!pop());
}

void halt()
{
	die("halted");
}

void my_syscall()
{
	int SysCallNum;
	char Buffer[256];
	int fd, i;
	int KeySize;

	//pop the value off the stack, see what sys call to run then run it
	SysCallNum = pop();

	if(SysCallNum == 42)
	{
		//grab the key file and start pushing it to the stack
		fd = open("/home/jargon/flag", O_RDONLY);
		KeySize = read(fd, Buffer, sizeof(Buffer));
		close(fd);

		//dump the key onto the stack
		for(i = 0; i < KeySize; i++)
			push(Buffer[i]);
	}
	else
	{
		printf("Syscall ");
		IntToText(SysCallNum, 0);
		printf("is not implemented!\n");
		die("");
	}
}

void pop_stack()
{
	//pop entry off stack
	pop();
}

void push_stack()
{
	unsigned char Bits;

	//grab 4 bits from the code and push it onto the stack
	Bits = Code[CodePos / 2];

	//see if high or low part of the byte
	if((CodePos & 1) == 0)
		Bits >>= 4;
	Bits &= 0x0f;

	//flip the bits around
	Bits = ((Bits & 0x08) >> 3) | ((Bits & 0x04) >> 1) | ((Bits & 0x02) << 1) | ((Bits & 0x01) << 3);

	//push the value on
	push(Bits);

	//advance our position as we did a read
	CodePos = (CodePos + 1) % (sizeof(Code) * 2);
}

void dupe_stack()
{
	char Value;

	//duplicate top entry
	Value = pop();
	push(Value);
	push(Value);
}

void swap_stack()
{
	char Value1, Value2;

	//swap 2 top entries
	Value1 = pop();
	Value2 = pop();
	push(Value1);
	push(Value2);
}

void compare_and_jump()
{
	//compare if the value is 0, if so then jump to the location specified in the next 8 bits
	int Compare;
	int JumpPos;
	unsigned char Bits1;
	unsigned char Bits2;

	Compare = pop();

	//grab 8 bits from the code and advance code position
	Bits1 = Code[CodePos / 2];
	CodePos = (CodePos + 1) % (sizeof(Code) * 2);

	//if we are reading the low 4 bits then capture the top 4 of the next byte
	//because we advanced a spot we check against 0
	if((CodePos & 1) == 0)
	{
		Bits2 = Code[CodePos / 2];

		Bits2 >>= 4;
		Bits2 &= 0x0f;
		Bits1 = ((Bits1 & 0x0f) << 4) | Bits2;
	}

	//flip the bits around
	Bits1 = ((Bits1 & 0x80) >> 7) | ((Bits1 & 0x40) >> 5) | ((Bits1 & 0x20) >> 3) | ((Bits1 & 0x10) >> 1) |
		((Bits1 & 0x08) << 1) | ((Bits1 & 0x04) << 3) | ((Bits1 & 0x02) << 5) | ((Bits1 & 0x01) << 7);

	//advance our position as we did a read of the 2nd 4 bits
	CodePos = (CodePos + 1) % (sizeof(Code) * 2);
	JumpPos = Bits1;

	//if 0 then change code location
	if(!Compare)
		CodePos = JumpPos;
}

extern FuncStruct Functions[];
unsigned char push_disas(unsigned char Pos)
{
	unsigned char Opcode;
	unsigned char Bits;

	//get our opcode
	Opcode = Code[Pos / 2];
	if((Pos & 1) == 0)
		Opcode >>= 4;
	Opcode &= 0x0f;
	Pos++;

	//get 4 bits to report
	Bits = Code[Pos / 2];
	if((Pos & 1) == 0)
		Bits >>= 4;
	Bits &= 0x0f;
	Pos++;

	//flip the bits around
	Bits = ((Bits & 0x08) >> 3) | ((Bits & 0x04) >> 1) | ((Bits & 0x02) << 1) | ((Bits & 0x01) << 3);

	//print our entry
	printf("%s ", Functions[Opcode].Command);
	IntToText(Bits, 1);

	//return new position
	return Pos;
}

unsigned char caj_disas(unsigned char Pos)
{
	unsigned char Opcode;
	unsigned char Bits1, Bits2;

	//get our opcode
	Opcode = Code[Pos / 2];
	if((Pos & 1) == 0)
		Opcode >>= 4;
	Opcode &= 0x0f;
	Pos++;

	//grab 8 bits to report
	Bits1 = Code[Pos / 2];
	Pos++;

	//if we are reading the low 4 bits then capture the top 4 of the next byte
	//because we advanced a spot we check against 0
	if((Pos & 1) == 0)
	{
		Bits2 = Code[Pos / 2];

		Bits2 >>= 4;
		Bits2 &= 0x0f;
		Bits1 = ((Bits1 & 0x0f) << 4) | Bits2;
	}
	Pos++;

	//flip the bits around
	Bits1 = ((Bits1 & 0x80) >> 7) | ((Bits1 & 0x40) >> 5) | ((Bits1 & 0x20) >> 3) | ((Bits1 & 0x10) >> 1) |
		((Bits1 & 0x08) << 1) | ((Bits1 & 0x04) << 3) | ((Bits1 & 0x02) << 5) | ((Bits1 & 0x01) << 7);

	//print our entry
	printf("%s ", Functions[Opcode].Command);
	IntToText(Bits1, 1);

	//return new position
	return Pos;
}

FuncStruct Functions[] =
{
	{"rot13", halt, 0},
	{"xyzzy", math_add, 0},
	{"kluge", math_sub, 0},
	{"hello world", math_mul, 0},
	{"pr0n", math_div, 0},
	{"stomp on", math_and, 0},
	{"vax", math_or, 0},
	{"war dialer", math_xor, 0},
	{"warez", math_neg, 0},
	{"ykybhtlw", math_mod, 0},
	{"this can't happen", my_syscall, 0},
	{"dead beef", push_stack, push_disas},
	{"reality check", pop_stack, 0},
	{"shareware", dupe_stack, 0},
	{"phreaker", swap_stack, 0},
	{"murphy's law", compare_and_jump, caj_disas}
};

void Disassemble(unsigned int Len)
{
	int i;
	unsigned char Opcode;

	//disassemble all opcodes
	printf("\nDisassembly\n-----------\n");
	for(i = 0; i < Len;)
	{
		//get 4 bits
		Opcode = Code[i / 2];
		if((i & 1) == 0)
			Opcode >>= 4;
		Opcode &= 0x0f;

		//if we have a function call it otherwise just print the string
		if(Functions[Opcode].disas)
			i = Functions[Opcode].disas(i);
		else
		{
			printf("%s\n", Functions[Opcode].Command);
			i++;
		}
	}
	printf("\n");
	fflush(stdout);
}

int IsNumeric(char *Buffer, int Len)
{
	int i;

	//check for being numeric, change newline to null
	for(i = 0; i < Len; i++)
	{
		//if newline and not on the first character then we are fine
		if(Buffer[i] == 0xa)
		{
			//set to null, if the first character then fail 
			Buffer[i] = 0;
			if(i == 0)
				return 0;
			return 1;
		}
		else if((Buffer[i] < 0x30) || (Buffer[i] > 0x39))
			return 0;	//not 0 to 9
	}

	//all good
	return 1;
}

int ReadLine(int fd, char *Buffer, int Len)
{
    int Count = 0;
    int TotalCount = 0;
    while(TotalCount < Len)
    {
        Count = read(fd, Buffer, 1);
        if(Count > 0)
        {
            TotalCount += Count;
            if(*Buffer == '\n')
                return TotalCount;
            Buffer += Count;
        }
	else
		exit(0);
    };

    return TotalCount;
}

int main(int argc, char **argv)
{
	int LoopCount;
	unsigned char OpEntry;
	int DataIn;
	int DataLen;
	char Buffer[5];

	//make sure we stop if they try to just hang the connection open
        signal(SIGALRM, (sighandler_t)alarmHandle);
        alarm(AlarmTime);

	memset(Stack, 0, sizeof(Stack));
	memset(Code, 0, sizeof(Code));
	memset(Buffer,0, sizeof(Buffer));

	StackPos = 0;
	CodePos = 0;
	LoopCount = 0;

	//sanity check that we can read the flag file
	DataIn = open("/home/jargon/flag", O_RDONLY);
	if(DataIn < 0)
	{
		printf("Error opening the flag file\n");
		fflush(stdout);
		return 0;
	}
	close(DataIn);

	//find out how many bytes of code to read
	printf("How many bytes of code (max %ld)?\n", sizeof(Code));
	fflush(stdout);
	DataIn = ReadLine(0, Buffer, sizeof(Buffer) - 1);

	//make sure it is all numeric
	if(!IsNumeric(Buffer, sizeof(Buffer) - 1))
	{
		printf("Invalid number\n");
		fflush(stdout);
		return 0;
	}

	//check the length
	DataLen = atoi(Buffer);
	if((DataLen <= 0) || (DataLen > sizeof(Code)))
	{
		printf("Invalid code length\n");
		fflush(stdout);
		return 0;
	}

	//get our code buffer
	printf("Reading %d bytes of code\n", DataLen);
	fflush(stdout);
	while(LoopCount < DataLen)
	{
		//read the buffer, if 0 bytes then abort
		DataIn = read(0, &Code[LoopCount], DataLen - LoopCount);
		if(DataIn == 0)
			return 0;

		LoopCount += DataIn;
	};

	//disassemble the code, 4 bits per opcode so double length
	Disassemble(DataLen * 2);

	alarm(AlarmTime);

	//die will cause us to exit if we loop too much
	for(LoopCount = 0; LoopCount < 1000; LoopCount++)
	{
		//get the opcode
		OpEntry = Code[CodePos / 2];

		//if the upper half then shift
		if((CodePos & 1) == 0)
			OpEntry = (OpEntry >> 4);

		//next 4 bits of code
		CodePos = (CodePos + 1) % (sizeof(Code) * 2);

		//mask to 4 bits
		OpEntry &= 0x0f;
		Functions[OpEntry].func();
	};

	die("1000 opcodes executed");
	return 0;
}
