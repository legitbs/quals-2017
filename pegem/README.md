# pegem

## Author Information

Selir

## Description:
This service implements a subleq (subtract and branch if less than or equal to zero) OISC emulator.  It emulates this instruction:

subleq A, B, C

A, B, and C are two-byte short integers.  The instruction is equivalent to:

Mem[B] = Mem[B] - Mem[A]
if (Mem[B] <= 0) goto C

There are special values of A and B which allow input from stdin and output to stdout.  If A == -1, one byte is read from stdin and stored in Mem[B].  If B == -1, the byte at Mem[A] is written to stdout.  Note that both of these special instructions operate on single-byte values, not the 2-byte integers of the underlying memory.

The challenge binary contains a small subleq program that implements the 'peg solitaire' game commonly found at Cracker Barrel restaurants.  A detailed description of the game can be found at https://en.wikipedia.org/wiki/Peg_solitaire.  The goal of the game is to win all 15 possible starting 'holes'.  

## Vulnerability
If the player manages to win all 15 games, they are prompted to provide their name.  An overflow exists in this function which allows them to overwrite a portion of the peg program giving them control of the subleq execution.  But, they don't have control over the emulator itself.  They can only execute subleq instructions.

The flag is stored in a string within the subleq program.  By properly crafting the overflow, they can execute subleq instructions to read the bytes of this flag string and print them to stdout.  The copy of the binary the teams are given will have a string "THE FLAG WILL BE HERE IN THE VULNERABLE SERVICE" so that teams won't need to guess where to look for the flag.

## Challenges
Analyzing the binary will allow the teams to determine that it is a subleq emulator.  

Then, they'll need to RE the embedded subleq program to find the overflow vulnerability.  Some basic fuzzing should be sufficient for them to determine that an overflow bug of some kind exists in the "Name" prompt code.  The program will exit with an error if an invalid address is provided to a given instruction.  So, overflowing the Name variable with invalid address bytes will eventually exit the program with an error.  And, they'll be able to determine that the overflowable subleq function which is implementing the prompt is near the top of the program (with a little gdb work monitoring the emulator's program counter variable).  So, there will be no need to reverse the subleq code beyond that function.  It's pretty clear that the function reads input and their analysis of the emulator binary will clearly show that read() calls have '-1' for the "A" value.  So, locating that chunk of code in the subleq binary will be easier.

Next, they'll need to determine which portions of the peg game their overflow will overwrite.  The "Name" prompt code they'll be analyzing is actually the first function after the "Name" global variable (which is where the overflow is written).  So, that means, the bytes they'll be writing past the end of that buffer will be directly re-writing the prompt reading code.  Eventually after they overwrite enough of that function, it'll overwrite the instruction they're currently executing and allow them to take control of "C" which gives them control over the program counter and they can jump back into the Name buffer for execution.

One other challenge to them putting their own shellcode into the Name buffer is that the read() code only accepts values from 0-255 (and remember the subleq instructions are made up of three 2-byte short integers.  So, they can't write the necessary "-1" value to make an instruction that will output text to stdout.  But, they can write subleq shellcode that will self-modify and write the "-1" value to itself wherever it's needed.  This will allow them to construct a basic for loop that prints each character of the FLAG variable...FTW.


## Note
To compile this service, you'll need the excellent subleq compiler by http://mazonka.com/subleq/hsq.html.
