#ifndef __TRAINS_H__
#define __TRAINS_H__

int send_string( int fd, char *data );

void menu( int fd );

int readint( int fd );

int read_line( int fd, char *data, int ml );

#endif