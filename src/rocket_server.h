#ifndef ROCKET_SERVER_H
#define ROCKET_SERVER_H

#include "com.h"

int rocket_server_init(int addr_size, int num_clients);

int rocket_server_exit();

#endif  // ROCKET_SERVER_H
