#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "check.h"

#define CODE_LEN 80

int main() {
  char* code = calloc(sizeof(char), CODE_LEN + 1);

  puts("enter code:");
  fflush(stdout);

  fgets(code, CODE_LEN, stdin);

  long sum = check(code);

  printf("sum is %ld\n", sum);

  return 0;
}
