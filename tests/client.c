#include <stdio.h>

#include "rocket_client.h"


#define _1KB 1024

const int ADDRESS_SIZE = _1KB * 10;

int main(int argc, char* argv[])
{ 
    if(rocket_client_init(ADDRESS_SIZE) == -1)
    {
        printf("Failed to initialize rocket client!\n");
        return 1;
    }

    


    rocket_client_exit();

    return 0;
}