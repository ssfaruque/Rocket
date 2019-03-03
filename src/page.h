#ifndef PAGE_H
#define PAGE_H

#include "com.h"

#define PAGE_SIZE 4096

enum PagePermission
{
    PAGE_CONCURRENT_READ,
    PAGE_EXCLUSIVE_WRITE,
    PAGE_PERMISSION_NONE
};


typedef struct
{
    int client_num;
    sockaddr_in_t client_addr;
} ClientInfo;


typedef struct
{
    int num_readers;
    ClientInfo* readers;
} ClientReaders;


typedef struct
{
    ClientInfo* exclusiveWriter;
} ClientExclusiveWriter;


typedef struct Page 
{
    ClientReaders* clientReaders;
    ClientExclusiveWriter* clientExclusiveWriter;
    unsigned char bytes[PAGE_SIZE];
} Page;








#endif  // PAGE_H