#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "page.h"
#include "rocket_client.h"

#define BASE_BUFFER_SIZE 1024

/* The shared memory that each client will have */
unsigned char* shared_mem;
int master_socket; //Initialize this in init Lesley/Shivang
int slave_socket; //Initialize this in init Lesley/Shivang

pthread_mutex_t *lock;

// For now setting it to be the start of user space (0x40000000). This is to be used as input for mmap among other things
void* get_base_address()
{ 
    return (void*)(1<<30);
}

//Defined this way because it is going to be running as a thread independently where void* param will be the accepted socket file descriptor
void* independent_listener (void* param) 
{
  int acc_sock = *((int *)param);
  char buf[BASE_BUFFER_SIZE];

  for(; ;)
    {
      int val = recv_msg(acc_sock, buf, BASE_BUFFER_SIZE);
      buf[val] = '\0';
      char* temp_str;
      int page_number = (int)strtol(buf, &temp_str, 10);
      pthread_mutex_lock(&lock[page_number]);
      void* page_addr = get_base_address() + (page_number*PAGE_SIZE);
      mprotect(page_addr, PAGE_SIZE, PROT_READ);
      if(send_msg(acc_sock, page_addr, PAGE_SIZE) != 0)
	{
	  printf("Could not send page requested!\n");
	  exit(1);
	}
      mprotect(page_addr, PAGE_SIZE, PROT_NONE);
      pthread_mutex_unlock(&lock[page_number]);

    }
  
}

// Programmed this like on the sigaction manpage: http://man7.org/linux/man-pages/man2/sigaction.2.html
// TODO: locking, buffering, socket communication, copying incoming page, setting protection for page using mprotect, etc.
void sigfault_handler(int sig, siginfo_t *info, void *ucontext)
{
    char *curr_addr = info->si_addr;
    char *base_addr;
    void *temp = get_base_address();
    base_addr = (char *)temp;
    int page_number = ((int)(curr_addr - base_addr)) / PAGE_SIZE;
    
    pthread_mutex_lock(&lock[page_number]);
    char buf[BASE_BUFFER_SIZE];
    snprintf(buf, BASE_BUFFER_SIZE, "%d", page_number);
    
    if(send_msg(master_socket, buf, strlen(buf)) != 0)
      {
	printf("Could not send message for page request!\n");
	exit(1);
      }

    char data[PAGE_SIZE];

    for(int total = PAGE_SIZE, index = 0; total != 0; )
      {
	int val = recv_msg(master_socket, &data[index], total);
	total = total - val;
	index = index + val;
      }

    //void* page_addr = get_base_address() + (page_number*PAGE_SIZE);

    mprotect(curr_addr, PAGE_SIZE, PROT_WRITE);
    memcpy(curr_addr, data, PAGE_SIZE);
    pthread_mutex_unlock(&lock[page_number]);
    
    
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


    // Code: for establishing the communication with the client.
    socket_t network_socket = create_socket();
    //struct Page* page_requested = client_communicate(network_socket);
    char* page_requested = client_communicate(network_socket);
    close_socket(network_socket);
    printf("%p",page_requested);


    //To call indpendent listener function at the end, it will go something like this:
    //pthread_t thread
    //indp_lisn = pthread_create(&thread, NULL, independent_listener, (void*) fd returned from accept syscall);

    
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


struct Page* rocket_access_page(int page_num)
{
    return NULL;
}


void* rocket_alloc(int num_bytes)
{
    return 0;
}

int rocket_dealloc(void* address)
{
    return 0;
}


char* client_communicate(socket_t network_socket)
{ 
    
    sockaddr_in_t addr = create_socket_addr(9002, INADDR_ANY);

    /* attempting to establish a connection on the socket */
    int connection_status = connect_socket(network_socket, &addr);

    if(connection_status == -1)
    {
        printf("Could not connect!\n");
        exit(1);
    }

    //struct Page* page_request = NULL;
    char* page_request = NULL;

    /* read sizeof(val) number of bytes and put bytes into the variable 'val' */
    //recv_msg(network_socket, (Page*) page_request, sizeof(page_request));
    recv_msg(network_socket, page_request, sizeof(page_request));
    
    return page_request;  
}
