#ifndef ROCKET_CORE_H
#define ROCKET_CORE_H

#include <assert.h>

#include "page.h"

#define ASSERT(cond, str) {assert((cond) && (str));}


typedef struct
{
    int            num_pages;
    int            num_clients;
    Page*          pages;
    PageOwnership* pageOwnerships;
} SharedMemory;


void* get_base_address();

SharedMemory* create_shared_memory(int number_of_pages, int number_of_clients); 

void init_shared_memory(SharedMemory* mem);

Page* retrieve_page(SharedMemory* mem, void* addr);





#endif  // ROCKET_CORE_H