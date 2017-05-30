#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char ** argv)
{
    char smashme[64];
    puts("Welcome to the Dr. Phil Show. Wanna smash?");
    fflush(stdout);
    gets(smashme);
    if(strstr(smashme, "Smash me outside, how bout dAAAAAAAAAAA")) {
        return 0;
    }
    exit(0);
}