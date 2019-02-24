#ifndef ROCKET_CLIENT_H
#define ROCKET_CLIENT_H

struct Page;

int rocket_client_init(int addr_size);

int rocket_client_exit();

struct Page* rocket_client_access_page(int page_num);




#endif  // ROCKET_CLIENT_H