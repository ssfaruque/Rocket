#ifndef ROCKET_COM_H
#define ROCKET_COM_H

#include <netinet/in.h>

typedef int socket_t;
typedef struct sockaddr_in sockaddr_in_t;
typedef struct Page page;
/*
 * create_socket
 * 
 * Creates a new socket
 * 
 * Return: -1 if failed to create socket, otherwise successfull
 */
socket_t create_socket();


/*
 * create_socket_addr
 * @port: The port to configure the socket address with
 * @internet_addr: The IP address to configure the socket address with
 * 
 * Configures a struct based on the port and internet_addr parameters
 * 
 * Return: A sockaddr_in_t struct configured with the parameters provided 
 */ 
sockaddr_in_t create_socket_addr(int port,  const char* internet_addr);


/*
 * connect_socket
 * @sock: The socket to connect to
 * @sock_addr: The struct configured for the socket
 * 
 * Connects to the specified socket
 * 
 * Return: 0 for success or -1 for error
 */
int connect_socket(socket_t sock, sockaddr_in_t* sock_addr);


/*
 * bind_socket
 * @sock: The socket to bind to
 * @sock_addr: The struct configured for the socket
 * 
 * Binds to a socket (typically used in server applications)
 * 
 * Return: 0 for success or -1 for error
 */
int bind_socket(socket_t sock, sockaddr_in_t* sock_addr);


/*
 * listen_for_connections
 * @sock: The socket to listen for connections
 * @queue_size: Maximum number of connection requests that will be queued
 * 
 * Listens for any incoming connections (typically used in server applications)
 * 
 * Return: 0 for success or -1 for error
 */ 
int listen_for_connections(socket_t sock, int queue_size);


/*
 * accept_connection
 * @sock: The socket to accept a connection from
 * @addr: Output of the sockaddr_in_t associated with the client
 * @addr_length: Output of the address's actual length
 * 
 * Awaits a connection on the socket passed in
 * 
 * Return: New socket's descriptor or -1 for error
 */
int accept_connection(socket_t sock, sockaddr_in_t* addr, int* addr_length);


/*
 * recv_msg
 * @sock: The socket associated with the communication
 * @buffer: Buffer that will store the received message
 * @num_bytes: Specified number of bytes is read into the buffer parameter
 * 
 * Receives a message using a socket that is connected to
 * 
 * Return: Number of bytes read or -1 for error
 */
int recv_msg(socket_t sock, char* buffer, int num_bytes);


/*
 * send_msg
 * @sock: The socket associated with the message to send
 * @buffer: Buffer containing the message to send
 * @num_bytes: Number of bytes to send from the buffer
 * 
 * Sends a message using a socket that is connected to
 * 
 * Return: Number of bytes sent or -1 for error
 */
int send_msg(socket_t sock, char* buffer, int num_bytes);


/*
 * close_socket
 * @sock: The specified socket to close
 * 
 * Closes the specified socket and cleans up its associated resources
 * 
 * Return: 0 for success or -1 for error
 */
int close_socket(socket_t sock);

#endif  // ROCKET_COM_H
