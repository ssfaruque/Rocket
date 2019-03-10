#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "rocket_core.h"
#include "rocket_client.h"

#define _1KB 1024


//const int SHARED_MEM_SIZE = _1KB * 20
const int NUM_CLIENTS = 4;


void client_read()
{
    // assuming client 0 is the writer
    while(1)
    {
        sleep(3);
        int read_num = -1;
        rocket_read_addr(get_base_address(), &read_num, sizeof(int));

        printf("read num: %d\n", read_num);
    }
}


int main(int argc, char* argv[])
{ 
    if(rocket_client_init((1 << 30), NUM_CLIENTS) == -1)
    {
        printf("Failed to initialize rocket client!\n");
        return 1;
    }



    // int temp = -1;

    // if(client_num == 1)
    // {    temp = 66;
    //     rocket_write_addr(get_base_address(), &temp, sizeof(int));
    // }

    // if(client_num == 1)
    //     printf("Address %p: %d\n", get_base_address(), *((int*)(get_base_address())));


    printf("Inside client.c, client_num: %d\n", client_num);


    // DEMO WW2R
    /*
    if(client_num == 0)
    {
        int num = 123;
        printf("Client %d is writing\n", client_num);
        rocket_write_addr(get_base_address(), &num, sizeof(int));
        printf("Address %p: %d\n", get_base_address(), *((int*)(get_base_address())));
    }

    else if(client_num == 1)
    {
        sleep(5);
        int num = 321;
        printf("Client %d is writing\n", client_num);
        rocket_write_addr(get_base_address(), &num, sizeof(int));
        printf("Address %p: %d\n", get_base_address(), *((int*)(get_base_address())));
    }


    else if(client_num == 2)
    {
        sleep(10);
        printf("Client Num: %d\n", client_num);
        int read_num = -1;
        rocket_read_addr(get_base_address(), &read_num, sizeof(int));
        printf("read num: %d\n", read_num);
    }

    else if(client_num == 3)
    {
        sleep(10);
        printf("Client Num: %d\n", client_num);
        int read_num = -1;
        rocket_read_addr(get_base_address(), &read_num, sizeof(int));
        printf("read num: %d\n", read_num);
    }
    */

    // DEMO W2RW2R
    if(client_num == 0)
    {
        int num = 123;
        printf("Client %d is writing\n", client_num);
        rocket_write_addr(get_base_address(), &num, sizeof(int));
        printf("Address %p: %d\n", get_base_address(), *((int*)(get_base_address())));
    }

    else if(client_num == 1)
    {
        sleep(10);
        int num = 321;
        printf("Client %d is writing\n", client_num);
        rocket_write_addr(get_base_address(), &num, sizeof(int));
        printf("Address %p: %d\n", get_base_address(), *((int*)(get_base_address())));
    }


    else if(client_num == 2)
    {
        sleep(5);
        printf("Client Num: %d\n", client_num);
        int read_num = -1;
        rocket_read_addr(get_base_address(), &read_num, sizeof(int));
        printf("read num: %d\n", read_num);

        sleep(20);
        printf("Client Num: %d\n", client_num);
        read_num = -1;
        rocket_read_addr(get_base_address(), &read_num, sizeof(int));
        printf("read num: %d\n", read_num);
    }

    else if(client_num == 3)
    {
        sleep(5);
        printf("Client Num: %d\n", client_num);
        int read_num = -1;
        rocket_read_addr(get_base_address(), &read_num, sizeof(int));
        printf("read num: %d\n", read_num);

        sleep(20);
        printf("Client Num: %d\n", client_num);
        read_num = -1;
        rocket_read_addr(get_base_address(), &read_num, sizeof(int));
        printf("read num: %d\n", read_num);
    }
    



    
    while(1);


    rocket_client_exit();

    return 0;
}


