#include <stdlib.h>
#include <string.h>

#include "rocket_core.h"


// For now setting it to be the start of user space (0x40000000). This is to be used as input for mmap among other things
void* get_base_address()
{ 
    return (void*)(1<<30);
}


void init_shared_memory(SharedMemory* mem)
{
    memset(mem->pages, 0, sizeof(Page) * mem->num_pages);
}


SharedMemory* create_shared_memory(int number_of_pages) 
{
    SharedMemory* mem = (SharedMemory*) malloc(sizeof(SharedMemory));
    mem->num_pages = number_of_pages;
    mem->pages = (Page*) malloc(sizeof(Page) * number_of_pages);
    mem->pageOwnerships = (PageOwnership*) malloc(sizeof(PageOwnership) * number_of_pages);

    int i;
    for(i = 0; i < number_of_pages; ++i)
    {
        mem->pageOwnerships->clientExclusiveWriter = NULL;
        mem->pageOwnerships->clientReaders         = NULL;
    }

    init_shared_memory(mem);

    return mem;
}


Page* retrieve_page(SharedMemory* mem, void* addr)
{
    int num_bytes_from_base = (int)(((char*) addr) - ((char*) get_base_address()));
    int page_num = num_bytes_from_base / PAGE_SIZE;
    return &mem->pages[page_num];
}