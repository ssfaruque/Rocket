#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>

#include "page.h"
#include "rocket_client.h"


/* The shared memory that each client will have */
unsigned char* shared_mem;


// For now setting it to be the start of user space (0x40000000). This is to be used as input for mmap among other things
void* get_base_address()
{ 
    return (void*)(1<<30);
}


// Programmed this like on the sigaction manpage: http://man7.org/linux/man-pages/man2/sigaction.2.html
// TODO: locking, buffering, socket communication, copying incoming page, setting protection for page using mprotect, etc.
void sigfault_handler(int sig, siginfo_t *info, void *ucontext)
{
    //char *curr_addr = info->si_addr;
    //char *base_addr;
    //void *temp = get_base_address();
    //base_addr = (char *)temp;
    //int page_number = ((int)(curr_addr - base_addr)) / PAGE_SIZE;
}


int rocket_client_init(int addr_size)
{
    // Using mmap for mapping the addresses-- private copy-on-write mapping
    shared_mem = mmap(get_base_address(), addr_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // Setting up the signal catching code
    struct sigaction sa;
    sa.sa_sigaction = sigfault_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;

    // TODO: Assigning default pages to master and slave nodes to start with (need to demarcate them somehow), socket code goes here for all communication, thread running function that responds to page requests on both master and slave nodes runs here too.

    return 0;
}


int rocket_client_exit()
{
    return 0;
}


/*  Client gets ownership of the page
    Return: 0 for success, -1 for failure */
int own_page()
{
    return 0;
}


/*  Client gives up ownership of the page
    Return: 0 for success, -1 for failure */
int relinquish_page()
{
    return 0;
}


struct Page* rocket_client_access_page(int page_num)
{
    return NULL;
}