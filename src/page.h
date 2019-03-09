#ifndef PAGE_H
#define PAGE_H

#include "com.h"

#define PAGE_SIZE 4096
#define MAX_READER_SIZE 5

typedef struct
{
    socket_t      client_socket;
    socket_t      sig_socket;
} ClientInfo;


typedef struct
{
    int         num_readers;
    ClientInfo readers[MAX_READER_SIZE];
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
    ClientReaders         clientReaders;
    ClientExclusiveWriter clientExclusiveWriter;
} PageOwnership;


typedef struct
{
    unsigned char bytes[PAGE_SIZE];
} Page;


#endif  // PAGE_H
