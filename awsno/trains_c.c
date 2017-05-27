#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int send_string( int fd, char *data )
{
    int count = 0;

    if ( data == NULL ) {
        return 0;
    }

    while ( data[count] ) {
        write(fd, data + count, 1);

        count += 1;
    }

    return count;
}

void menu( int fd )
{
    send_string( fd, "1) Add train\n");
    send_string( fd, "2) Print Trains\n");
    send_string( fd, "3) Remove Train\n");
    send_string( fd, "4) Update Train\n");
    send_string( fd, "5) Exit\n");
    send_string( fd, ": ");

    return;
}

void setblocking( int fd )
{
    int o;

    o = fcntl(fd, F_GETFL);

    o = o & ~O_NONBLOCK;

    fcntl(fd, F_SETFL, o);

    return;
}

void unsetblocking( int fd )
{
    int o;

    o = fcntl(fd, F_GETFL);

    o = o | O_NONBLOCK;

    fcntl(fd, F_SETFL, o);

    return;
}

int read_line( int fd, char *data, int ml )
{
    int count = 0;

    if ( data == NULL ) {
        return -1;
    }

    setblocking( fd );
    while ( count < ml ) {
        if ( read( fd, data + count, 1) <= 0 ) {
            count = -1;
            goto end;
        }

        if ( data[count] == '\n' ) {
            data[count] = '\x00';
            goto end;
        }
        count++;
    }

end:
    unsetblocking(fd);
    return count;
}

int readint( int fd )
{
    char buffer[16];
    int retval;

    memset( buffer, 0, 16);

    
    if ( read_line( fd, buffer, 15) <= 0 ) {
        return -1;
    }
    

    retval = atoi( buffer);

    return retval;
}