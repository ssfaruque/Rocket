#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#include "rocket_core.h"
#include "page.h"
#include "com.h"

#define BASE_BUFFER_SIZE 1024

int num_connected_clients = 0;
socket_t master_socket    = -1;

int address_size = -1;

ClientInfo* clientInfos    = NULL;
SharedMemory* sharedMemory = NULL;
PageOwnership* pageOwnerships = NULL;

void add_reader_to_page(SharedMemory* mem, ClientInfo* clientInfo, int page_num)
{
}


void give_exclusive_write_access(SharedMemory* mem, int client_num)
{

}


void invalidate_readers_on_page(SharedMemory* mem, int page_num)
{   
     
}

void* independent_listener_server(void* param)
{
  int acc_sock = *((int *)param);                                                                                                                                                                           
  printf("Listening to accepted socket: %d\n", acc_sock);                                                                                                                                                   
  char buf[BASE_BUFFER_SIZE];

  while (1) {
    //RECEIVING PAGE FROM CLIENT 1
      int val = recv_msg(master_socket, buf, BASE_BUFFER_SIZE);
      printf("Server receiving msg of size %d\n", val);

      buf[val] = '\0';
      char* temp_str;
      int page_number = (int)strtol(buf, &temp_str, 10);
      printf("Page number: %d \n", page_number);

      int target_client_sock = pageOwnerships[page_number].clientExclusiveWriter->client_socket;

      char buf[BASE_BUFFER_SIZE];
      snprintf(buf, BASE_BUFFER_SIZE, "%d", page_number);

      //SENDING PAGE REQUEST TO CLIENT 2
      if(send_msg(target_client_sock, buf, strlen(buf)) != 0)
	{
	  printf("Could not send message for page request!\n");
	  exit(1);
	}
      
      char data[PAGE_SIZE];

      //RECEIVING PAGE FROM CLIENT 2
      for(int total = PAGE_SIZE, index = 0; total != 0; )
	{
	  int val = recv_msg(target_client_sock, &data[index], total);
	  total = total - val;
	  index = index + val;
	}

      //UPDATE PAGE OWNERSHIPS!!! WE ARE ONLY CHANGING SOCKET. NOTHING ELSE.

      pageOwnerships[page_number].clientExclusiveWriter->client_socket = acc_sock;

      //SENDING PAGE FROM MASTER TO CLIENT 1

      void* page_addr = get_base_address() + (page_number*PAGE_SIZE);
      if(send_msg(acc_sock, page_addr, PAGE_SIZE) != 0)
        {
          printf("Could not send page requested!\n");
          exit(1);
        }
      
  }
  return NULL;
}

void init_server_socket(int num_clients, int port, const char* IPV4_ADDR)
{
    master_socket = create_socket();

    if(master_socket == -1)
    {
        printf("Failed to create server socket!\n");
        exit(1);
    }

    sockaddr_in_t addr = create_socket_addr(port, IPV4_ADDR);

    if(bind_socket(master_socket, &addr) == -1)
    {
        printf("Failed to bind server socket!\n");
        exit(1);
    }

    if(listen_for_connections(master_socket, num_clients) == -1)
    {
        printf("Failed to setup listening for connections!\n");
        exit(1);
    }
}


void print_client_info(ClientInfo* clientInfo)
{
    sockaddr_in_t client_addr = clientInfo->client_addr;
    unsigned short int sin_family = client_addr.sin_family;
    uint16_t port = client_addr.sin_port;
    uint32_t s_addr = client_addr.sin_addr.s_addr;

    printf("client ip addr: %s\n", clientInfo->client_ip_addr);
    printf("client num: %d\n", clientInfo->client_num);
    printf("socket: %d\n", clientInfo->client_socket);
    
    printf("sin_family: %d\n", sin_family);
    printf("port: %d\n", port);
    printf("s_addr: %d\n\n", s_addr);
}


void setup_client_connections(int num_clients)
{
    clientInfos  = (ClientInfo*) malloc(sizeof(ClientInfo) * num_clients);
 
    const char* SERVER_IP = INADDR_ANY; // TODO: change to actula server ip
    init_server_socket(num_clients, 9002, SERVER_IP);

    int client_num;

    for (client_num = 0; client_num < num_clients; client_num++)
    {
        /* connect to a client */
        sockaddr_in_t client_addr;
        int addr_length;
        socket_t client_socket = accept_connection(master_socket, &client_addr, &addr_length);

        if (client_socket == -1)
        {
            printf("Server failed to connect to client %d!\n", client_num);
            exit(1);
        }

        clientInfos[client_num].client_num    = client_num;
        clientInfos[client_num].client_socket = client_socket;
        clientInfos[client_num].client_addr   = client_addr;

        print_client_info(&clientInfos[client_num]);

        printf("Connected to client %d!\n", client_num);

        /* send the client num to the connected client */
        int num_bytes_sent = send_msg(client_socket, (void*)&client_num, sizeof(client_num));

        if(num_bytes_sent <= 0 )
        {
            printf("Server failed to send client number %d to the respective client\n", client_num);
            exit(1);
        }

        /* receive an acknowledgement from the client */
        int received = 0;
        recv_msg(client_socket, (void*)&received, sizeof(int));

        if(!received)
        {
            printf("Server failed to receive an acknowledgement from client %d\n", client_num);
            exit(1);
        }

        printf("<client %d> - acknowledged: %d\n\n", client_num, received);

        num_connected_clients++;

    }


    int page_num;
    int num_pages_each_clients = (address_size / PAGE_SIZE) / num_clients;

    for(page_num = 0; page_num < address_size / PAGE_SIZE; page_num++)
    {
        int client_index = page_num / num_pages_each_clients;
        pageOwnerships[page_num].clientExclusiveWriter = &clientInfos[client_index];
    }




    int connect_to_all_clients_success = (num_connected_clients == num_clients) ? 1 : 0;

    if(connect_to_all_clients_success)
    {
        for (client_num = 0; client_num < num_clients; client_num++)
        {
            int num_bytes_sent = send_msg(clientInfos[client_num].client_socket, (void*)&connect_to_all_clients_success, sizeof(connect_to_all_clients_success));

            if(num_bytes_sent <= 0 )
            {
                printf("Server failed to send client number %d to the respective client\n", client_num);
                exit(1);
            }
        }

        printf("Server successfully connected to all %d clients!\n", num_clients);
        sleep(5);
    }

    else
    {
        printf("Failed to connect to all %d clients!\n", num_clients);
        exit(1);
    }
}


void setup_connecting_clients()
{
    // Currently connecting to only one client.
    // TODO: Connect to all clients (suppoed to get all the client ips from clientInfos)
    const char* CLIENT_IP = INADDR_ANY; // TODO: change to actual slave IP
   
    int client_socket = create_socket();

    if(client_socket == -1)
    {
        printf("Failed to create server socket!\n");
        exit(1);
    }

    sockaddr_in_t addr = create_socket_addr(9002, CLIENT_IP);

    /* Attempting to establish a connection on the socket */
    if(connect_socket(client_socket, &addr) == -1)
    {
        printf("Server could not connect to client with IP: %s!\n", CLIENT_IP);
        exit(1);
    }
    printf("Connected to client with IP: %s.\n", CLIENT_IP);
}


int rocket_server_init(int addr_size, int num_clients)
{
    static int init = 0;

    address_size = addr_size;
    if(!init)
    {
        init = 1;

        sharedMemory = create_shared_memory(addr_size / PAGE_SIZE, num_clients);
        pageOwnerships = create_pageownerships(addr_size / PAGE_SIZE, num_clients);

        setup_client_connections(num_clients);

        //setup_connecting_clients();
    }
        
    return 0;
}


int rocket_server_exit()
{
    return 0;
}


void server_communicate(socket_t server_socket)
{
    struct sockaddr_in addr = create_socket_addr(9002, INADDR_ANY);

    /* bind the socket to the struct containing info
       about the port and IP address */
    bind_socket(server_socket, &addr);

    /* listen for a maximum of 3 clients on the specified socket,
       (clients are backlogged to a queue of size 3) */
    listen_for_connections(server_socket, 3);

    int client_socket;
    char* val = NULL;

    /* 'accept_connection' function will write to these if specified */
    sockaddr_in_t client_addr;
    int addr_length;

    /* server accepts client that is in front of the internal queue */
    while((client_socket = accept_connection(server_socket, &client_addr, &addr_length)) != -1)
    {
        unsigned short int sin_family = client_addr.sin_family;
        uint16_t port = client_addr.sin_port;
        uint32_t s_addr = client_addr.sin_addr.s_addr;

        /* Printing out information about the client */
        printf("sin_family: %d\n", sin_family);
        printf("port: %d\n", port);
        printf("s_addr: %d\n", s_addr);
        printf("addr_length: %d\n\n", addr_length);

        send_msg(client_socket, (char*) &val, sizeof(val));
        //val++;
    }
    close_socket(server_socket);
}
