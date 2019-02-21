#include <stdio.h>

#include "com.h"

int val1 = 1, val2 = 2;

int main(int argc, char* argv[])
{
    char server_message[256] = "You have reached the server!";

    // create the server socket
    int server_socket = create_socket();

    // define the server address
    struct sockaddr_in addr = create_sock_addr(9002, INADDR_ANY);

    // bind the socket to our specified IP and port
    bind_sock(server_socket, &addr);

    listen_for_connections(server_socket, 3);

    int client_socket;
    int val = 0;

    while((client_socket = accept_connection(server_socket, NULL, NULL)) != -1)
    {
        send_msg(client_socket, (char*) &val, sizeof(val));
        val++;
    }

    

    

    close_sock(server_socket);

    return 0;
}