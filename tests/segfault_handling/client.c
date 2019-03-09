#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "rocket_core.h"
#include "rocket_client.h"

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



    int temp = -1;

    if(client_num == 1)
    {    temp = 66;
        rocket_write_addr(get_base_address(), &temp, sizeof(int));
    }

    if(client_num == 1)
        printf("Address %p: %d\n", get_base_address(), *((int*)(get_base_address())));






    // if(client_num == 0)
    // {
    //     int num = 123;
    //     rocket_write_addr(get_base_address(), &num, sizeof(int));
    // }

    // else if(client_num == 1)
    // {
    //     sleep(5);
    //     int read_num = -1;
    //     rocket_read_addr(get_base_address(), &read_num, sizeof(int));

    //     printf("read num: %d\n", read_num);
    // }


    
    while(1);


    rocket_client_exit();

    return 0;
}


