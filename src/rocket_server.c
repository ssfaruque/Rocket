#include <stdio.h>
#include <stdlib.h>

#include "page.h"
#include "rocket_server.h"

int rocket_server_init(int addr_size)
{
    static int init = 0;

    if(!init)
    {
        init = 1;

    }

    return 0;
}


int rocket_server_exit()
{

    return 0;
}
