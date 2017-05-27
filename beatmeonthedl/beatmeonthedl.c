#include <stdio.h>
#include <string.h>
#include "malloc.h"
#include <time.h>
#include <stdlib.h>

#define REQCNT  0x20

char *reqlist[REQCNT];

char *user = "mcfly";
char *pass = "awesnap";

char *welcometo = "\t\t __      __       .__                                  __           \n"
"\t\t/  \\    /  \\ ____ |  |   ____  ____   _____   ____   _/  |_  ____   \n"
"\t\t\\   \\/\\/   // __ \\|  | _/ ___\\/  _ \\ /     \\_/ __ \\  \\   __\\/  _ \\  \n"
"\t\t \\        /\\  ___/|  |_\\  \\__(  <_> )  Y Y  \\  ___/   |  | (  <_> ) \n"
"\t\t  \\__/\\  /  \\___  >____/\\___  >____/|__|_|  /\\___  >  |__|  \\____/  \n"
"\t\t       \\/       \\/          \\/            \\/     \\/                 \n";

char *thelairofthe = "\t  __  .__             .__         .__                 _____    __  .__            \n"
"\t_/  |_|  |__   ____   |  | _____  |__|______    _____/ ____\\ _/  |_|  |__   ____  \n"
"\t\\   __\\  |  \\_/ __ \\  |  | \\__  \\ |  \\_  __ \\  /  _ \\   __\\  \\   __\\  |  \\_/ __ \\ \n"
"\t |  | |   Y  \\  ___/  |  |__/ __ \\|  ||  | \\/ (  <_> )  |     |  | |   Y  \\  ___/ \n"
"\t |__| |___|  /\\___  > |____(____  /__||__|     \\____/|__|     |__| |___|  /\\___  >\n"
"\t           \\/     \\/            \\/                                      \\/     \\/ \n";

char *shadowbrokers = "  _________.__                .___              __________                __\n"                        
" /   _____/|  |__ _____     __| _/______  _  __ \\______   \\_______  ____ |  | __ ___________  ______\n"
" \\_____  \\ |  |  \\\\__  \\   / __ |/  _ \\ \\/ \\/ /  |    |  _/\\_  __ \\/  _ \\|  |/ \\// __ \\_  __ \\/  ___/\n"
" /        \\|   Y  \\/ __ \\_/ /_/ (  <_> )     /   |    |   \\ |  | \\(  <_> )    <\\  ___/|  |  \\/\\___ \\ \n"
"/_______  /|___|  (____  /\\____ |\\____/ \\/\\_/    |______  / |__|   \\____/|__|_ \\\\___  >__|    /____ >\n"
"        \\/      \\/     \\/      \\/                       \\/                    \\/    \\/             \\/ \n";

void header()
{
    printf("%s", welcometo);
    printf("%s", thelairofthe);
    printf("%s", shadowbrokers);
    return;
}

int checkuser( )
{
    char data[16];

    memset(data, 0, 16);

    printf("Enter username: ");
    if ( read( 0, data, 16) <= 0 ) {
        exit(1);
    }

    if ( memcmp( data, user, strlen(user) ) ) {
        printf("Invalid user: %s\n", data);
        return 0;
    }

    return 1;
}

int checkpass()
{
    char data[0x18];
    char *ptr = NULL;

    memset( data, 0, 0x18);
    printf("Enter Pass: ");

    if ( read(0, data, 0x18) <= 0 ) {
        exit(1);
    }

    ptr = malloc( 0x18 );
    memcpy( ptr, data, 0x18);

    if ( memcmp( ptr, pass, strlen(pass) ) ) {
        printf("Invalid pass: %s\n", data);
        return 0;
    }

    return 1;
}

int login( )
{
    if ( checkuser() != 1 ) {
        return 0;
    }

    if ( checkpass() != 1 ) {
        return 0;
    }

    return 1;
}

void init()
{
    if ( setvbuf( stdout, NULL, _IONBF, 0 ) ) {
        printf("[-] setvbuf failed\n");
        exit(1);
    }

    memset( reqlist, 0, sizeof(char*)*REQCNT);

    return;
}

void printmenu( )
{
    printf("I) Request Exploit.\n");
    printf("II) Print Requests.\n");
    printf("III) Delete Request.\n");
    printf("IV) Change Request.\n");
    printf("V) Go Away.\n");
    printf("| ");

    return;
}

int getsel( )
{
    char data[0x10];

    memset( data, 0, 0xf);

    if ( read(0, data, 0xf) <= 0 ) {
        exit(1);
    }

    return atoi( data );
}

void add_request()
{
    int i = 0;

    while ( i < REQCNT ) {
        if (reqlist[i] == NULL ) {
            break;
        }

        i += 1;
    }

    if (i == REQCNT ) {
        printf("[-] Request list full\n");
        return;
    }

    reqlist[i] = malloc( 56 );

    if ( reqlist[i] == NULL ) {
        printf("[-] Failed to allocate block\n");
        exit(1);
    }

    printf("Request text > ", i);

    if ( read(0, reqlist[i], 0x80) <= 0 ) {
        exit(1);
    }

    return;
}

void print_list()
{
    int i = 0;

    while ( i < REQCNT) {
        if ( reqlist[i] != NULL ) {
            printf("%d) %s\n", i, reqlist[i]);
        }

        i += 1;
    }

    return;
}

void delete_request()
{
    int i = 0;

    print_list();

    printf("choice: ");

    i = getsel( );

    if ( i >= REQCNT || i < 0 ) {
        printf("[-] Out of range\n");
        return;
    } 

    if (reqlist[i]) {
        free(reqlist[i]);
        reqlist[i] = NULL;
    }

    return;
}

void update_request()
{
    int i = 0;

    print_list();

    printf("choice: ");

    i = getsel( );

    if ( i >= REQCNT || i < 0 ) {
        printf("[-] Out of range\n");
        return;
    } 

    if (reqlist[i]) {
        printf("data: ");

        if ( read( 0, reqlist[i], 0x80) <= 0) {
            exit(1);
        }
    }

    return;
}

int main()
{
    int i = 0;

    init();

	header();

    while ( !login() ) {}

    while (1) {
        printmenu();

        i = getsel();

        switch(i) {
            case 1:
                add_request();
                break;
            case 2:
                print_list();
                break;
            case 3:
                delete_request();
                break;
            case 4:
                update_request();
                break;
            case 5:
                printf("--So long--\n");
                return 0;
            default:
                printf("--Nope--\n");
                return 0;
        };
    }
	return 0;
}
