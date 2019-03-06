#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

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


SharedMemory* create_shared_memory(int number_of_pages, int number_of_clients) 
{
    SharedMemory* mem   = (SharedMemory*)  malloc(sizeof(SharedMemory));
    mem->pageOwnerships = (PageOwnership*) malloc(sizeof(PageOwnership) * number_of_pages);
    mem->num_pages      = number_of_pages;
    mem->num_clients    = number_of_clients;

    int i;
    for(i = 0; i < number_of_pages; ++i)
    {
        mem->pageOwnerships[i].clientExclusiveWriter = NULL;
        mem->pageOwnerships[i].clientReaders         = NULL;
    }

    mem->pages = mmap(get_base_address(), number_of_pages * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if(mem->pages == MAP_FAILED)
    {
        printf("Failed to map shared memory of size %d bytes\n", number_of_pages * PAGE_SIZE);
        exit(1);
    }

    init_shared_memory(mem);

    return mem;
}



PageOwnership* create_pageownerships(int number_of_pages, int number_of_clients)
{
    PageOwnership* pageOwnership = (PageOwnership*) malloc(sizeof(PageOwnership));

    int i;

    for(i = 0; i < number_of_pages; i++)
    {
        pageOwnership->clientReaders = NULL;
        pageOwnership->clientExclusiveWriter = NULL;
    }

    return pageOwnership;
}



Page* retrieve_page(SharedMemory* mem, void* addr)
{
    int num_bytes_from_base = (int)(((char*) addr) - ((char*) get_base_address()));
    int page_num = num_bytes_from_base / PAGE_SIZE;
    return &mem->pages[page_num];
}
