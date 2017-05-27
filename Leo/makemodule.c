#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

// stupid simple buffer overflow, but ASLR and NX are on, so ROP is necessary
void module(unsigned char *data, int count) {

int x = count/2 +1;
char buffer[10];
int i = 17;


  for (i=0; i < count; ++i) {

    // stupid check to see if the stack frame has been corrupted
    if (x != count/2+1) {
  	 return;
    }
  	
    buffer[i] = data[i];

  }

return;

}


void END_SHELLCODE(void) {}

int main(int argc, char **argv) {
unsigned char *data;
int size;
int i;

    FILE *output_file = fopen("module.bin", "w");

    size = (long long int)END_SHELLCODE - (long long int)module;

    data = malloc(size);

    if (data == NULL) {

      printf("error with malloc()\n");
      exit(-1);
    }

    for (i=0; i<size; ++i ) {

      *(data+i) = *((unsigned char *)module+i) ^ 0xaa;
    }

    fwrite(data, size, 1, output_file);
    fclose(output_file);

    return 0;

}
