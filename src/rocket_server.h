#ifndef ROCKET_SERVER_H
#define ROCKET_SERVER_H

int rocket_server_init(int addr_size);

int rocket_server_exit();

// parameters might be something like page num, client ID, etc...
int rocket_disable_page_ownsership();

#endif  // ROCKET_SERVER_H