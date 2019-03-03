#include <stdio.h>

#include "rocket_client.h"

#define _1KB 1024

const int SHARED_MEM_SIZE = _1KB * 20;
const int NUM_CLIENTS = 2;

int main(int argc, char* argv[])
{ 
    if(rocket_client_init(SHARED_MEM_SIZE, NUM_CLIENTS) == -1)
    {
        printf("Failed to initialize rocket client!\n");
        return 1;
    }


    rocket_client_exit();

    return 0;
}