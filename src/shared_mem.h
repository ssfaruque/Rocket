#ifndef SHARED_MEM_H
#define SHARED_MEM_H

#include "types.h"

struct shared_mem;


struct shared_mem* create_shared_mem(int num_bytes, int page_size);

void clear_shared_mem(byte val);

void destroy_shared_mem(struct shared_mem* mem);



#endif  // SHARED_MEM_H