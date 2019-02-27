#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#include "page.h"
#include "rocket_server.h"

typedef struct
{
    int client_num;
    sockaddr_in_t client_addr;
} ClientInfo;


int num_connected_clients = 0;

socket_t server_socket;
ClientInfo* clientInfos = NULL;


int rocket_server_init(int addr_size, int num_clients)
{
    static int init = 0;

    if(!init)
    {
        init = 1;
        
        server_socket = create_socket();
        sockaddr_in_t addr = create_socket_addr(9002, INADDR_ANY);
        bind_socket(server_socket, &addr);
        listen_for_connections(server_socket, num_clients);

        clientInfos = (ClientInfo*) malloc(sizeof(ClientInfo) * num_clients);
        int client_num;

        for(client_num = 0; client_num < num_clients; client_num++)
        {
            /* connect to a client */
            sockaddr_in_t client_addr;
            int addr_length;
            socket_t client_socket = accept_connection(server_socket, &client_addr, &addr_length);

            if(client_socket == -1)
            {
                printf("Server failed to connect to client %d!\n", client_num);
                return -1;
            }

            printf("Connected to client %d!\n", client_num);

            num_connected_clients++;

            printf("REACHED HERE\n");

            /* send the client num to the connected client */
            int num_bytes_sent = send_msg(server_socket, (char*) &client_num, sizeof(client_num));

            printf("Server sent %d bytes to client %d\n", num_bytes_sent, client_num);

            clientInfos[client_num].client_num  = client_num;
            clientInfos[client_num].client_addr = client_addr;

            int received = 0;
            recv_msg(server_socket, (char*)&received, sizeof(received));

            if(received)
            {
                printf("Client %d has received its client number from the server\n", client_num);
            }
        }
    }
    
    // int server_socket = create_socket();
    // server_communicate(server_socket);
    
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
