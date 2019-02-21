#include <stdio.h>
#include <stdlib.h>

#include "../../src/com.h"

int main(int argc, char* argv[])
{
    /* first create the socket */
    socket_t network_socket = create_socket();

    /* struct containing details about port and IP address */
    sockaddr_in_t addr = create_socket_addr(9002, INADDR_ANY);

    /* Note: With 'INADDR_ANY', server-client programs will only
     * work on a single machine. In order for server-client programs
     * to work across multiple computers, change 'INADDR_ANY' into
     * the IPv4 address of the server machine. This can be found
     * using the 'ifconfig' command in Linux. 
     * Example of an IP address to be passed in as a string: "10.1.2.3" 
     * /

    /* attempting to establish a connection on the socket */
    int connection_status = connect_socket(network_socket, &addr);

    if(connection_status == -1)
    {
        printf("Could not connect!\n");
        exit(1);
    }

    int val = -1;

    /* read sizeof(val) number of bytes and put bytes into the variable 'val' */
    recv_msg(network_socket, (char*) &val, sizeof(val));

    printf("CLIENT, val: %d\n", val);

    /* free all resources associated with the socket */
    close_socket(network_socket);

    return 0;
}