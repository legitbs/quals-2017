#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

#if DEBUG
#define MAX_IDLE_SECS 60*60
#else
#define MAX_IDLE_SECS 30
#endif

#include "EMPMA.h"
#include "proto.h"

using namespace std;

EMPMA g_empMem;

void sig_alarm_handler( int signum )
{
        exit( 1 );
}

int main(void) {

    char buf[sizeof(pMessage)];
    uint8_t type = 0;
    int byte = 0;
    int size = 0;
    int idx = 0;

    signal( SIGALRM, sig_alarm_handler );
    alarm( MAX_IDLE_SECS );

    signal( SIGABRT, sig_alarm_handler );

    setvbuf( stdout, NULL, _IONBF, 0 );
    
    for(;;) {
        memset(buf, 0, sizeof(buf));
        byte = fgetc(stdin);

        type = empGetMtype(byte);

        if (type) {
            if (DEBUG) 
                cout << "++ ok" << "\n";
            
            empDcdPmsg(NULL, byte, 0);
            empDcdPmsgCmd();
        }
        else if (byte != EOF || byte != '\n') {
            if (DEBUG)
                cout << "-- invalid\n";
        }

        if (g_msgCnt > MESSAGE_LIMIT) {
            if (DEBUG) 
                cout << "message limit hit\n";
            break;
        }
    }

    return 0;
}
