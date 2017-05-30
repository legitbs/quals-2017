#include <stdio.h>
#include <stdlib.h>

char name[128];

void playGame(char *nameptr) {
    int dicerolls[16];
    int numdice;
    int i;
    char again;
    printf("Welcome to peROPdo, %s\n", nameptr);
    do {
        puts("How many dice would you like to roll?");
        fflush(stdout);
        scanf("%d", &numdice);
        for(i=0;i<numdice;i++) {
            dicerolls[i] = rand();
        }
        printf("You rolled: ");
        for(i=0;i<numdice;i++){
            printf("%d ", (dicerolls[i] % 6)+1);
        }
        printf("\nWould you like to play again? ");
        fflush(stdout);
        scanf("%1s", &again);
    } while (again == 'y');
}

int main(int argc, char ** argv)
{
    puts("What is your name?");
    fflush(stdout);
    scanf("%s", name);
    srand(*(int *)name);
    playGame(name);
}
