#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include "com.h"
#include "com.c"
//Page size will be default 4KB
#define page_size 4096

//For now setting it to be the start of user space (0x40000000). This is to be used as input for mmap among other things
void* get_base_address()
{ 
  return (void*)(1<<30);
}

// Programmed this like on the sigaction manpage: http://man7.org/linux/man-pages/man2/sigaction.2.html
//TODO: locking, buffering, socket communication, copying incoming page, setting protection for page using mprotect, etc.
void sigfault_handler(int sig, siginfo_t *info, void *ucontext)
{
  char* curr_addr = info->si_addr;
  char* base_addr;
  void* temp = get_base_address();
  base_addr = (char*)temp;
  int page_number = ((int)(curr_addr - base_addr))/page_size;
}

void launch_rocket(char* master_ip, int master_port, char* slave_ip, int slave_port, int num_pages)
{
   
  //Using mmap for mapping the addresses-- private copy-on-write mapping
  char* addr = mmap(get_base_address(), num_pages * page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1,0);

  //Setting up the signal catching code
  struct sigaction sa;
  sa.sa_sigaction = sigfault_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_SIGINFO;

  //TODO: Assigning default pages to master and slave nodes to start with (need to demarcate them somehow), socket code goes here for all communication, thread running function that responds to page requests on both master and slave nodes runs here too.
  
  return;

}

void land_rocket(socket_t sock)
{
  //TODO: terminate sockets, perform clean up, etc.
  return;
}

int main()
{
  void* addr = NULL;
  addr = get_base_address();
  printf("Starting Address: %p\n", addr);
  return 0;
}
