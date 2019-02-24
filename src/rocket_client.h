#ifndef ROCKET_CLIENT_H
#define ROCKET_CLIENT_H

struct Page;

int rocket_client_init(int addr_size);

int rocket_client_exit();

void* rocket_alloc(int num_bytes);

int rocket_dealloc(void* address);

#endif  // ROCKET_CLIENT_H