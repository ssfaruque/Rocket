#include "shared_mem.h"

struct shared_mem
{
    byte* mem;
    uin64 num_bytes;
    int num_clients;
};