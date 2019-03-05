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

/* The shared memory that each client and server will have */
SharedMemory* sharedMemory = NULL;
int total_page_numbers = -1;

int master_socket = -1; // used to read from server
int slave_socket  = -1; // used to send to server
int num_clients   = -1; // total number of clients in the application
int client_num    = -1; // client id corresponding to this client, starts from 0
int address_size  = -1; // total size of the shared memory in bytes  

char server_addr[32];  // IPV4 address of the server

pthread_mutex_t *lock; // used for independent listender
struct sigaction sa;   // used for signal handling


/* retrieves the starting address of the memory region corresponding 
   to the particular client
*/
void* get_respective_client_base_address()
{
    int address_offset = (client_num * address_size);
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


//Defined this way because it is going to be running as a thread independently where void* param will be the accepted socket file descriptor
void* independent_listener (void* param) 
{
  int acc_sock = *((int *)param);
  printf("Listening to accepted socket: %d\n", acc_sock);
  char buf[BASE_BUFFER_SIZE];

  for(; ;)
    {
      int val = recv_msg(acc_sock, buf, BASE_BUFFER_SIZE);
      printf("Client receiving msg of size %d\n", val);

      buf[val] = '\0';
      char* temp_str;
      int page_number = (int)strtol(buf, &temp_str, 10);
      printf("Page number: %d \n", page_number);
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
    void *temp      = get_base_address();
    char *base_addr = (char*) temp;

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


// Setting up the signal catching code
void setup_signal_handler()
{
    sa.sa_sigaction = sigfault_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
}



void init_client_socket(int num_clients, int port, const char* IPV4_ADDR)
{
    master_socket = create_socket();

    if(master_socket == -1)
    {
        printf("Failed to create server socket!\n");
        exit(1);
    }

    sockaddr_in_t addr = create_socket_addr(port, IPV4_ADDR);

    /* Attempting to establish a connection on the socket */
    if(connect_socket(master_socket, &addr) == -1)
    {
        printf("Client could not connect to server with IP: %s!\n", IPV4_ADDR);
        exit(1);
    }

}


void get_client_number_from_server()
{
    int num_bytes_received = recv_msg(master_socket, &client_num, sizeof(client_num));

    if(num_bytes_received <= 0)
    {
        printf("Client failed to receive its client number from the server!\n");
        exit(1);
    }
}


void send_acknowledgement_to_server()
{
    int ack = 1;
    int num_bytes_sent = send_msg(master_socket, (char*) &ack, sizeof(ack));

    if(num_bytes_sent == -1)
    {
        printf("Client %d failed to send its acknowledgement to the client!\n", client_num);
        exit(1);
    }
}


void get_finished_status_from_server()
{
    int server_finished_connecting = 0;
    int num_bytes_received = recv_msg(master_socket, &server_finished_connecting, sizeof(server_finished_connecting));

    if(num_bytes_received <= 0)
    {
        printf("Client failed to receive its client number from the server!\n");
        exit(1);
    }

    if(!server_finished_connecting)
    {
        printf("Server did not finish connecting to all of the clients\n");
        exit(1);
    }

    printf("The server has finished connecting to all %d clients\n", num_clients);
}


void setup_listener_locks(){
    pthread_mutex_t mutexes[total_page_numbers];
    for (int i = 0; i < total_page_numbers; i++) {
        pthread_mutex_init(&mutexes[i], NULL);
    }

    lock = mutexes;
}

void setup_accepting_server_connection()
{
    slave_socket = create_socket();
    const char* CLIENT_IP = INADDR_ANY; // TODO: change slave IP address
    sockaddr_in_t slave_addr = create_socket_addr(9002, CLIENT_IP);
    bind_socket(slave_socket, &slave_addr);
    listen_for_connections(slave_socket, 1);
    
    sockaddr_in_t addr;
    int addr_length;
    socket_t network_socket = accept_connection(slave_socket, &addr, &addr_length);

    if(network_socket == -1)
    {
        printf("Client failed to accept connection.");
        exit(1);
    }

    printf("Server connection accepted. \n");

    setup_listener_locks();
    
    pthread_t thread;
    int indp_lisn = pthread_create(&thread, NULL, independent_listener, (void*) &network_socket);
    if (indp_lisn != 0) {
        printf("Client failed to create independent thread.");
    } 
    printf("Listener thread created.\n");

}

int rocket_client_init(int addr_size, int number_of_clients)
{
    address_size = addr_size;
    num_clients = number_of_clients;

    total_page_numbers = addr_size / PAGE_SIZE;
    sharedMemory = create_shared_memory(total_page_numbers, number_of_clients);

    setup_signal_handler();

    // TODO: Assigning default pages to master and slave nodes to start with (need to demarcate them somehow), socket code goes here for all communication, thread running function that responds to page requests on both master and slave nodes runs here too.
    const char* SERVER_IP = INADDR_ANY; // TODO: change to actual client ip
    init_client_socket(num_clients, 9002, SERVER_IP);

    get_client_number_from_server();

    printf("Client received client number: %d\n", client_num);

    send_acknowledgement_to_server();

    printf("Client %d sent its acknowledgement to the server!\n", client_num);

    get_finished_status_from_server();

    // listening for server request
    setup_accepting_server_connection();
 
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


char* client_communicate(socket_t network_socket)
{ 
    return 0;  
}
