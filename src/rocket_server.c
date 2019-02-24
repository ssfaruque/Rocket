#include <stdio.h>
#include <stdlib.h>

#include "page.h"
#include "rocket_server.h"


typedef struct PageTable
{
    Page* pages;
    int num_pages;
} PageTable;

PageTable pageTable = {0, 0};


int rocket_server_init(int addr_size)
{
    static int init = 0;

    if(!init)
    {
        init = 1;
        pageTable.num_pages = (addr_size / PAGE_SIZE);
        pageTable.pages = (Page*) malloc(addr_size);
    }

    return 0;
}


int rocket_server_exit()
{
    if(pageTable.pages)
    {
        free(pageTable.pages);
        pageTable.pages = NULL;
        pageTable.num_pages = 0;
    }

    return 0;
}
