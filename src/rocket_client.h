#ifndef ROCKET_CLIENT_H
#define ROCKET_CLIENT_H

#include "com.h"

extern int client_num;

int rocket_client_init(int addr_size, int number_of_clients, const char* SERVER_IP_ADDR);

int rocket_client_exit();

void rocket_write_addr(void* addr, void* data, int num_bytes);

void rocket_read_addr(void* addr, void* buf, int num_bytes);

#endif  // ROCKET_CLIENT_H
