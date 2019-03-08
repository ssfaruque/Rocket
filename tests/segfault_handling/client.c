#include <stdio.h>

#include "rocket_core.h"
#include "rocket_client.h"
#include <string.h>

#define _1KB 1024


//const int SHARED_MEM_SIZE = _1KB * 20
const int NUM_CLIENTS = 2;

int main(int argc, char* argv[])
{ 
    if(rocket_client_init((1 << 30), NUM_CLIENTS) == -1)
    {
        printf("Failed to initialize rocket client!\n");
        return 1;
    }


   // FOR TESTING
    char* ptr = (char*)get_base_address();
    
    if(client_num == 0)
        strcpy(ptr, "Client 0 was here!");
    printf("Address %p: %s\n", ptr, ptr);

    while(1);


    rocket_client_exit();

    return 0;
}


