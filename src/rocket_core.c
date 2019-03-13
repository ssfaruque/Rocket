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


PageOwnership* create_pageownerships(int number_of_pages, int number_of_clients)
{
    PageOwnership* pageOwnership = (PageOwnership*) malloc(sizeof(PageOwnership) * number_of_pages);

    int i;

    for(i = 0; i < number_of_pages; i++)
    {
        pageOwnership[i].clientReaders.num_readers = 0;
        pageOwnership[i].clientExclusiveWriter.client_socket = -1;
        pageOwnership[i].clientExclusiveWriter.sig_socket = -1;
    }

    return pageOwnership;
}
