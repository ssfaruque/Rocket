#ifndef ROCKET_CLIENT_H
#define ROCKET_CLIENT_H

#include "com.h"

struct Page;

int rocket_client_init(int addr_size, int number_of_clients);

int rocket_client_exit();

void* rocket_alloc(int num_bytes);

int rocket_dealloc(void* address);

char* client_communicate(socket_t network_socket);


#endif  // ROCKET_CLIENT_H
