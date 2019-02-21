#include <stdio.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "com.h"

int main(int argc, char* argv[])
{
    socket_t network_socket = create_socket();


    struct hostent* p = gethostbyname("10.0.0.89");

    sockaddr_in_t addr = create_sock_addr(9002, INADDR_ANY);

    memcpy(&addr.sin_addr, p->h_addr, p->h_length);


    int connection_status = connect_sock(network_socket, &addr);


    if(connection_status == -1)
    {
        printf("Could not connect!\n");
    }

    char server_response[256];

    int val = -1;

    recv_msg(network_socket, (char*) &val, sizeof(val));

    printf("CLIENT, val: %d\n", val);

    close_sock(network_socket);



    return 0;
}