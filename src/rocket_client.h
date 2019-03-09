#ifndef ROCKET_CLIENT_H
#define ROCKET_CLIENT_H
#include "com.h"

extern int client_num;

struct Page;

int rocket_client_init(int addr_size, int number_of_clients);

int rocket_client_exit();

void* rocket_alloc(int num_bytes);

int rocket_dealloc(void* address);

void rocket_write_addr(void* addr, void* data, int num_bytes);


void rocket_read_addr(void* addr, void* buf, int num_bytes);



#endif  // ROCKET_CLIENT_H
