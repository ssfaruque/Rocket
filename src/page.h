#ifndef PAGE_H
#define PAGE_H

#include "com.h"

#define PAGE_SIZE 4096


typedef struct
{
    char          client_ip_addr[32];
    int           client_num;
    socket_t      client_socket;
    sockaddr_in_t client_addr;

} ClientInfo;


typedef struct
{
    int         num_readers;
    ClientInfo* readers;
} ClientReaders;

/*
typedef struct
{
    ClientInfo* exclusiveWriter;
} ClientExclusiveWriter;
*/

typedef ClientInfo ClientExclusiveWriter;


typedef struct 
{
    ClientReaders*         clientReaders;
    ClientExclusiveWriter* clientExclusiveWriter;
} PageOwnership;


typedef struct
{
    unsigned char bytes[PAGE_SIZE];
} Page;


#endif  // PAGE_H
