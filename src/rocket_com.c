
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "rocket_com.h"
#include "page.h"

socket_t create_socket()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}


sockaddr_in_t create_socket_addr(int port, const char* internet_addr)
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if(!internet_addr)
        address.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        address.sin_addr.s_addr = inet_addr(internet_addr);

    return address;
}


int connect_socket(socket_t sock, sockaddr_in_t* sock_addr)
{
    return connect(sock, (struct sockaddr*) sock_addr, sizeof(*sock_addr));
}


int bind_socket(socket_t sock, sockaddr_in_t* sock_addr)
{
    return bind(sock, (struct sockaddr*) sock_addr, sizeof(*sock_addr));
}


int listen_for_connections(socket_t sock, int queue_size)
{
    return listen(sock, queue_size);
}


int accept_connection(socket_t sock, sockaddr_in_t* addr, int* addr_length)
{
    return accept(sock, (struct sockaddr*) addr, (socklen_t*) addr_length);
}


int recv_msg(socket_t sock, Page* buffer, int num_bytes)
{
    return recv(sock, buffer, num_bytes, 0);
}


int send_msg(socket_t sock, char* buffer, int num_bytes)
{
    return send(sock, buffer, num_bytes, 0);
}


int close_socket(socket_t sock)
{
    return close(sock);
}
