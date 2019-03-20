#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "rocket_core.h"
#include "rocket_client.h"

const int SHARED_MEM_SIZE = (1 << 30);
const int NUM_CLIENTS = 4;


// https://stackoverflow.com/questions/5141960/get-the-current-time-in-c
const char* get_local_time()
{
    time_t rawtime;
    struct tm* timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    return asctime(timeinfo);
}


int main(int argc, char* argv[])
{ 


    if(rocket_client_init(SHARED_MEM_SIZE, NUM_CLIENTS, "128.120.211.76") == -1)
    {
        printf("Failed to initialize rocket client!\n");
        return 1;
    }






	if(client_num == 0)
	{
		int num = 100;
        printf("Client %d is writing\n", client_num);
        rocket_write_addr(get_base_address(), &num, sizeof(int));
        printf("[OUTPUT] ADDRESS WRITTEN TO %p WITH VALUE %d --- %s\n", get_base_address(), *((int*)(get_base_address())), get_local_time());		
	}


	else if(client_num == 1)
	{
		sleep(4);
        printf("Client Num: %d\n", client_num);
        int read_num = -1;
        rocket_read_addr(get_base_address(), &read_num, sizeof(int));
        printf("[OUTPUT] READ VALUE: %d --- %s\n", read_num, get_local_time());
	}


	else if(client_num == 2)
	{
		sleep(8);
		int num = -111;
        printf("Client %d is writing\n", client_num);
        rocket_write_addr(get_base_address(), &num, sizeof(int));
        printf("[OUTPUT] ADDRESS WRITTEN TO %p WITH VALUE %d --- %s\n", get_base_address(), *((int*)(get_base_address())), get_local_time());	
	}



	else if(client_num == 3)
	{
		sleep(12);
        printf("Client Num: %d\n", client_num);
        int read_num = -1;
        rocket_read_addr(get_base_address(), &read_num, sizeof(int));
        printf("[OUTPUT] READ VALUE: %d --- %s\n", read_num, get_local_time());
	}


    while(1);

    rocket_client_exit();

    return 0;
}


