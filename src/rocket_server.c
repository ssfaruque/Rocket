#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#include "rocket_core.h"
#include "page.h"
#include "com.h"


int num_connected_clients = 0;
socket_t server_socket    = -1;

ClientInfo* clientInfos    = NULL;
SharedMemory* sharedMemory = NULL;


void add_reader_to_page(SharedMemory* mem, ClientInfo* clientInfo, int page_num)
{
}


void give_exclusive_write_access(SharedMemory* mem, int client_num)
{

}


void invalidate_readers_on_page(SharedMemory* mem, int page_num)
{   
     
}


void init_server_socket(int num_clients, int port, const char* IPV4_ADDR)
{
    server_socket = create_socket();

    if(server_socket == -1)
    {
        printf("Failed to create server socket!\n");
        exit(1);
    }

    sockaddr_in_t addr = create_socket_addr(port, IPV4_ADDR);

    if(bind_socket(server_socket, &addr) == -1)
    {
        printf("Failed to bind server socket!\n");
        exit(1);
    }

    if(listen_for_connections(server_socket, num_clients) == -1)
    {
        printf("Failed to setup listening for connections!\n");
        exit(1);
    }
}


void setup_client_connections(int num_clients)
{
    clientInfos  = (ClientInfo*) malloc(sizeof(ClientInfo) * num_clients);

    init_server_socket(num_clients, 9002, INADDR_ANY);

    int client_num;

    for (client_num = 0; client_num < num_clients; client_num++)
    {
        /* connect to a client */
        sockaddr_in_t client_addr;
        int addr_length;
        socket_t client_socket = accept_connection(server_socket, &client_addr, &addr_length);

        if (client_socket == -1)
        {
            printf("Server failed to connect to client %d!\n", client_num);
            exit(1);
        }

        clientInfos[client_num].client_num    = client_num;
        clientInfos[client_num].client_socket = client_socket;
        clientInfos[client_num].client_addr   = client_addr;

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

        printf("<client %d> - acknowledged: %d\n", client_num, received);

        num_connected_clients++;
    }
}


int rocket_server_init(int addr_size, int num_clients)
{
    static int init = 0;

    if(!init)
    {
        init = 1;

        sharedMemory = create_shared_memory(addr_size / PAGE_SIZE, num_clients);
        setup_client_connections(num_clients);
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
