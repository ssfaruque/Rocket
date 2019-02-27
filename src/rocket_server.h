#ifndef ROCKET_SERVER_H
#define ROCKET_SERVER_H

#include "com.h"

int rocket_server_init(int addr_size, int num_clients);

int rocket_server_exit();

// parameters might be something like page num, client ID, etc...
int rocket_disable_page_ownsership();

void server_communicate(socket_t server_socket);

#endif  // ROCKET_SERVER_H
