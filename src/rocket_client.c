#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#include "rocket_core.h"
#include "page.h"
#include "com.h"


#define BASE_BUFFER_SIZE 1024

int total_num_pages = -1;

/* Shared memory is represented as an array of pages */
Page* pages;

int master_socket = -1; // used to read from server
int sig_socket    = -1;
int num_clients   = -1; // total number of clients in the application
int client_num    = -1; // client id corresponding to this client, starts from 0
int address_size  = -1; // total size of the shared memory in bytes  


int sig_dummy_val = -1;


char server_addr[32];  // IPV4 address of the server

pthread_mutex_t *lock = NULL; // used for independent listender




enum Operation currentOperation;


/* retrieves the starting address of the memory region corresponding 
   to the particular client
*/
void* get_respective_client_base_address()
{
    int address_offset = (client_num * (address_size / num_clients));
    return ((char*)get_base_address()) + address_offset;
}


/* Returns 1 if address is out of bounds for the respective client
   and 0 if it is within bounds */
int is_out_of_bounds(char* address)
{
    if(address == NULL)
    {
        printf("Address is NULL\n");
        return 1;
    }

    if(address < ((char*)get_respective_client_base_address()) || address > ((char*)get_respective_client_base_address()) + address_size)
    {
        printf("Address %p is out of bounds for client %d\n", address, client_num);
        return 1;
    }

    printf("Address %p within the bounds for client %d\n", address, client_num);
    return 0;
}


/*
ASSUMPTIONS:
1. Assuming master has now sent a request for the page (using page number)
2. This is "Client 2", and has the responsibility of sending back the page
3. Also, write permissions will be revoked now
 */
void* independent_listener_client(void* param)
{
  
  char buf[BASE_BUFFER_SIZE];
  while (1) {
      //printf("Inside independent_listener_client\n");

      int val = recv_msg(master_socket, buf, BASE_BUFFER_SIZE);

     if(val == 0)
        continue;

      //printf("Client receiving msg of size %d\n", val);

      buf[val] = '\0';
      //printf("buf: %s\n", buf);
      char* temp_str;
      int page_number = (int)strtol(buf, &temp_str, 10);
      printf("[INFO] Page number: %d \n", page_number);
      
      pthread_mutex_lock(&lock[page_number]);
      void* page_addr = ((char*)get_base_address()) + (page_number*PAGE_SIZE);
      mprotect(page_addr, PAGE_SIZE, PROT_READ);
      
      if(send_msg(master_socket, page_addr, PAGE_SIZE) <= 0)
      	{    printf("Could not send page requested!\n");
	        exit(1);
        }
      printf("[INFO] Client successfully sent page back to server\n");
      mprotect(page_addr, PAGE_SIZE, PROT_NONE);
      pthread_mutex_unlock(&lock[page_number]);

      
    }
  return NULL;
}


// Programmed this like on the sigaction manpage: http://man7.org/linux/man-pages/man2/sigaction.2.html
// TODO: locking, buffering, socket communication, copying incoming page, setting protection for page using mprotect, etc.

/*
1. This is Client 1 requesting page from master
2. This gets the page back from the server/master and sets write permission for itself.
*/

void sigfault_helper(int sig, siginfo_t *info, void *ucontext)
{
    printf("[INFO] STARTING SEGFAULT HANDLER!\n");

    char *curr_addr = info->si_addr;

    void *temp      = get_base_address();
    char *base_addr = (char*) temp;


    int page_number = ((int)(curr_addr - base_addr)) / PAGE_SIZE;

    printf("[INFO] segfault handler, page number: %d\n", page_number);
    
    pthread_mutex_lock(&lock[page_number]);
    char buf[BASE_BUFFER_SIZE];
    snprintf(buf, BASE_BUFFER_SIZE, "%d,%d,%d", client_num, (int)currentOperation, page_number);

    //printf("buf: %s\n", buf);
    

    if(send_msg(sig_socket, buf, strlen(buf)) <= 0)
    {
        printf("Could not send message for page request!\n");
        exit(1);
    }

    printf("[INFO] Successfully sent page request, client req\n");

    char data[PAGE_SIZE];

    for(int total = PAGE_SIZE, index = 0; total != 0; )
    {
        int val = recv_msg(sig_socket, &data[index], total);
        total = total - val;
        index = index + val;
    }

    //printf("data: %d\n", data[0]);

    //int protection = PROT_NONE;

    //printf("protection: %d %d %d\n", PROT_READ, PROT_WRITE, PROT_NONE);

    // switch(currentOperation)
    // {
    //     case WRITING:
    //     protection = PROT_WRITE;
    //     break;

    //     case READING:
    //     protection = PROT_READ;
    //     break;

    //     case NONE:
    //     protection = PROT_NONE;
    //     break;

    // }

    //printf("protection: %d\n", protection);

    // mprotect(curr_addr, PAGE_SIZE, protection);
    // memcpy(curr_addr, data, PAGE_SIZE);
    // printf("REACHED HEREQ@#@#@#\n");

    mprotect(curr_addr, PAGE_SIZE, PROT_WRITE);
    memcpy(curr_addr, data, PAGE_SIZE);

    if(currentOperation == READING)
    {
        mprotect(curr_addr, PAGE_SIZE, PROT_READ);
    }

    //printf("REACHED HEREQ@#@#@#\n");


    pthread_mutex_unlock(&lock[page_number]);

    printf("[INFO] FINISHING SEGFAULT HANDLER!\n");
}


void sigfault_handler(int sig, siginfo_t *info, void *ucontext)
{
    sigfault_helper(sig, info, ucontext);
}


// Setting up the signal catching code
void setup_signal_handler()
{
    struct sigaction sa;   // used for signal handling
    sa.sa_sigaction = sigfault_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    if(sigaction(SIGSEGV, &sa, NULL) < 0)
        printf("Failed to setup sigfault handler\n");
}


void init_socket(socket_t* sock, int num_clients, int port, const char* IPV4_ADDR)
{
    *sock = create_socket();

    if(*sock == -1)
    {
        printf("Failed to create server socket!\n");
        exit(1);
    }

    sockaddr_in_t addr = create_socket_addr(port, IPV4_ADDR);

    /* Attempting to establish a connection on the socket */
    if(connect_socket(*sock, &addr) == -1)
    {
        printf("Client could not connect to server with IP: %s!\n", IPV4_ADDR);
        exit(1);
    }
}


void get_val_from_server(socket_t* sock, int* val)
{
    int num_bytes_received = recv_msg(*sock, val, sizeof(int));

    if(num_bytes_received <= 0)
    {
        printf("Client failed to receive a val from the server!\n");
        exit(1);
    }
}



void send_acknowledgement_to_server(socket_t* sock)
{
    int ack = 1;
    int num_bytes_sent = send_msg(*sock, (char*) &ack, sizeof(ack));

    if(num_bytes_sent <= 0)
    {
        printf("Client %d failed to send its acknowledgement to the client!\n", client_num);
        exit(1);
    }
}


void get_finished_status_from_server(socket_t* sock)
{
    int server_finished_connecting = 0;
    int num_bytes_received = recv_msg(*sock, &server_finished_connecting, sizeof(server_finished_connecting));

    if(num_bytes_received <= 0)
    {
        printf("Client did not receive message from server!\n");
        exit(1);
    }

    if(!server_finished_connecting)
    {
        printf("Server did not finish connecting to all of the clients\n");
        exit(1);
    }
}


void setup_listener_locks()
{
    lock = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t) * total_num_pages);
    for (int i = 0; i < total_num_pages; i++) 
        pthread_mutex_init(&lock[i], NULL);
}


void setup_independent_listener()
{
    
    pthread_t thread;
    int indp_lisn = pthread_create(&thread, NULL, independent_listener_client, (void*) &master_socket);
    if (indp_lisn != 0) {
        printf("Client failed to create independent thread.");
    } 
    printf("Listener thread created.\n");
}


void init_pages(int addr_size)
{
    pages = mmap(get_base_address(), addr_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    //printf("1)\n");
    mprotect(get_base_address(), addr_size, PROT_NONE);
    //printf("2)\n");
    mprotect(get_respective_client_base_address(),  addr_size / num_clients, PROT_WRITE);
    //printf("3)\n");
   // memset(get_respective_client_base_address(), 0, addr_size / num_clients);
    //printf("4)\n");

    // // FOR TESTING
    // char* ptr = (char*)get_base_address();

    // if(client_num == 0)
    //     strcpy(ptr, "Client 0 was here!");
    // printf("Address %p: %s\n", ptr, ptr);
}


int rocket_client_init(int addr_size, int number_of_clients)
{
    address_size       = addr_size;
    num_clients        = number_of_clients;
    total_num_pages    = addr_size / PAGE_SIZE;



    setup_listener_locks();

    setup_signal_handler();

    const char* SERVER_IP = "128.120.211.170"; 







    init_socket(&master_socket, num_clients, 9002, SERVER_IP);

    get_val_from_server(&master_socket, &client_num);
    printf("[INFO] Client received client number: %d\n", client_num);

    send_acknowledgement_to_server(&master_socket);
    printf("Client %d sent its acknowledgement to the server!\n", client_num);

    get_finished_status_from_server(&master_socket);
    printf("The server has finished connecting to all %d clients for client sockets\n", num_clients);



    //printf("1. Client num: %d\n", client_num);

    sleep(5);








    init_socket(&sig_socket, num_clients, 9002 - 5353, SERVER_IP);

    get_val_from_server(&sig_socket, &sig_dummy_val);
    //printf("sig_socket received client number: %d\n", client_num);

    send_acknowledgement_to_server(&sig_socket);
    printf("sig_socket %d sent its acknowledgement to the server!\n", client_num);

    get_finished_status_from_server(&sig_socket);
    printf("The server has finished connecting to all %d clients for signal sockets\n", num_clients);

    //printf("2. Client num: %d\n", client_num);

    sleep(5);







    printf("[INFO] Client starting address: %p\n", (char*)get_respective_client_base_address());

    setup_independent_listener();

    init_pages(addr_size);
 
    return 0;
}


int rocket_client_exit()
{
    close_socket(master_socket);
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


void rocket_write_addr(void* addr, void* data, int num_bytes)
{
    currentOperation = WRITING;
    memcpy(addr, data, num_bytes);
    memcpy(addr, data, num_bytes);
}



void rocket_read_addr(void* addr, void* buf, int num_bytes)
{
    currentOperation = READING;
    memcpy(buf, addr, num_bytes);
    memcpy(buf, addr, num_bytes);
}
