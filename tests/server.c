#include <stdio.h>

#include "rocket_server.h"

#define _1KB 1024

const int SHARED_MEM_SIZE = _1KB * 20;
const int NUM_CLIENTS = 2;

int main(int argc, char* argv[])
{
    if(rocket_server_init((1 << 30), NUM_CLIENTS) == -1)
    {
        printf("Failed to initialize rocket server!\n");
        return 1;
    }

    while (1);

    // rocket_server_exit();
    
    return 0;
}