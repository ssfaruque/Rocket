# Rocket: A Distributed Shared Memory (DSM) Framework

## Overview
- [__Code structure__](https://github.com/ecs251-w19-ucdavis/Rocket#code-structure)
- [__A basic example to explain the logic__](https://github.com/ecs251-w19-ucdavis/Rocket#a-basic-motivating-example-to-explain-the-code-logic)
- [__Understanding the role of signal sockets and client sockets__](https://github.com/ecs251-w19-ucdavis/Rocket#the-client-and-signal-sockets)
- [__Understanding the client code and architecture__](https://github.com/ecs251-w19-ucdavis/Rocket#the-client-code-and-architecture)
- [__Understanding the server code and architecture__](https://github.com/ecs251-w19-ucdavis/Rocket#the-server-code-and-architecture)
- [__The code on the client and the server for the in-class demo (W2RW2R)__](https://github.com/ecs251-w19-ucdavis/Rocket#understanding-the-code-for-the-testin-class-demo-w2rw2r)
- [__Steps to run the in-class demo (W2RW2R)__](https://github.com/ecs251-w19-ucdavis/Rocket#steps-to-run-test-cases-for-w2rw2r-example-demo)


### Code structure
![Alt text](/images/RocketUML.svg)

### A basic motivating example to explain the code logic
![alt_img](/images/expl2.png)

We will use the above example/use-case to explain how the code would work. While writing Rocket, we would take simple examples and think about how the code should be written to allow for that functionality. We would then generalize these DSM functionalities to allow the framework to scale.

__The example at a high-level__:
- The example basically shows two clients and the master/server. Client 2 owns a page that Client 1 wants to write to. Thus, a page fault is raised in Client 1 since it does not own the page.
- Since Client 1 has no permissions for that page (in code, denoted by the `PROT_NONE` flag passed through to `mprotect`), to be able to request that page, Client 1 sends a write request with the page number to Master.
- Master then requests the page from Client 2, invalidates Client 2's write permissions to the page, and then sends it back to Client 1. 
- Client 1 is now sent the page, and given write access to it. Client 1 will now also copy the page over in memory, and in case any other client needs to read/write to it, the same process would happen all over again.

##### Some things to keep in mind when there are readers:
- If only one client wants read access, the process for that is exactly similar to the above mentioned procedure. The client with write access is the one the server requests the pages from and then sends it to the client who wants to read, revoking permissions to write on the original client, while granting read permissions to the new client. For reading `mprotect` takes in the `PROT_READ` flag.
- We use the CREW protocol, therefore while there can be only one exclusive writer, there can be concurrent readers. Now if a client has read permissions and another client also wants to read, we follow a similar process to grant them permissions again. The new client who wants to read basically sends a read request for that page to master, and master finds the last client who was given read permissions, and then requests the page from them. However, it does not invalidate this reader, like in the writing case.

### The client and signal sockets
To isolate the logic for communicating we use two separate sockets for the communication between clients and master. These are the signal sockets (`sig sockets`) and client sockets (`client sockets`):
- While we will get into this later, every time a page fault occurs at the client, there is a signal handler that intiates the read/write request to the master server. We use `sig sockets` only for communication to and from this signal fault handler at the client. Every time a communication or connection is required to either send or receive messages (pages) involving this signal fault handler `sig sockets` are used.
- `client sockets` are used everywhere else. In particular, each client runs a process in a detached thread called `independent_listener_client` which is used to listen if the server ever requests any pages from it. Communication for pages done by this thread always uses `client sockets`. Moreover, the server also has independent threads for each client through a thread function `independent_listener_server` through which it keeps in touch with the clients and then communicates with them to respond to their signal faults as well as request them for pages. Since here we require communication with both the requesting client and the client to be requested, both the `sig sockets` and `client sockets` are used. This is because this thread function will be in touch with both the signal handler at the requesting client and the independent listener at the client to be requested. Hence, the `sig socket` and the `client socket` are both used.

### The client code and architecture
We will now discuss the client side code (at least the relevant parts!) in detail. Starting off, a quick look at `rocket_client.h`, the header file for the clients:
```c
#ifndef ROCKET_CLIENT_H
#define ROCKET_CLIENT_H

#include "com.h"

extern int client_num;

int rocket_client_init(int addr_size, int number_of_clients, const char* SERVER_IP_ADDR);

int rocket_client_exit();

void rocket_write_addr(void* addr, void* data, int num_bytes);

void rocket_read_addr(void* addr, void* buf, int num_bytes);

#endif  // ROCKET_CLIENT_H
```
Here, we expose the Rocket Client API functions: `rocket_write_addr()` and `rocket_read_addr()`. To utilize DSM, we provide these API abstractions in Rocket, clients cannot do this just by using raw pointers. Moving on to the `rocket_client.c` file-- starting off, each client is initialized with 1GB contiguous memory with write access, while for 4 clients, each client will only have write access for their 1GB out of 4GB of total shared memory. Thus, if a client wishes to write to a page it does not own, a signal fault is triggered. We can catch this signal, and then take appropriate actions to send over the page eventually, using a signal fault handler function. This signal fault handler function has the following definition:
```c
void sigfault_helper(int sig, siginfo_t *info, void *ucontext)
{
    printf("[INFO] STARTING SEGFAULT HANDLER!\n");

    char *curr_addr = info->si_addr;

    void *temp = get_base_address();
    char *base_addr = (char *)temp;

    int page_number = ((int)(curr_addr - base_addr)) / PAGE_SIZE;

    printf("[INFO] segfault handler, page number: %d\n", page_number);

    pthread_mutex_lock(&lock[page_number]);
    char buf[BASE_BUFFER_SIZE];
    snprintf(buf, BASE_BUFFER_SIZE, "%d,%d,%d", client_num, (int)currentOperation, page_number);

    if (send_msg(sig_socket, buf, strlen(buf)) <= 0)
    {
        printf("Could not send message for page request!\n");
        exit(1);
    }

    printf("[INFO] Successfully sent page request, client req\n");

    char data[PAGE_SIZE];

    for (int total = PAGE_SIZE, index = 0; total != 0;)
    {
        int val = recv_msg(sig_socket, &data[index], total);
        total = total - val;
        index = index + val;
    }

    mprotect(curr_addr, PAGE_SIZE, PROT_WRITE);
    memcpy(curr_addr, data, PAGE_SIZE);

    if (currentOperation == READING)
        mprotect(curr_addr, PAGE_SIZE, PROT_READ);

    pthread_mutex_unlock(&lock[page_number]);

    printf("[INFO] FINISHING SEGFAULT HANDLER!\n");
}
```

As can be seen above, this function essentially does the following:
- As soon as there is a page fault (signal fault), the function tries to find the address of the page which is causing the fault. This is stored in `curr_addr`. 
- Since we have a simplifying assumption in our system where we know the starting base addresses as well as the page sizes used (4KB is standard), we can easily find out the page number by doing basic arithmetic: `int page_number = ((int)(curr_addr - base_addr)) / PAGE_SIZE;` Thus, the client now knows the page number it wants access to, as well as the kind of access it wants (read or write). Since the programmer has to use our API for reading or writing, we know whether they want to read or write to a page in or out of memory, and we store this information in `currentOperation`. This type can hold values of `READING` or `WRITING` or `NONE`.
- Now, the client sends the request for this page by sending it's client number, the operation it wants to perform (access level), as well as the page number. This information is sent as a string which we will parse later at the server side.
- It then assumes that the server would have complied with the request and hence, found out the page and attempted to now send it back to it (this client). Thus, the signal handler concludes by receiving the page from the master server and then doing an `mprotect` to grant itself write permissions (`PROT_WRITE` or `PROT_READ` depending on `currentOperation`) to this page address and then a `memcpy` to copy the page in memory at that address.

Next, we discuss another key functionality of the Rocket client. Going back to the diagram above, since Client 1 wanted permissions for a page owned by Client 2: Client 1 would encounter a page fault and have to send a page request to the server. This is the part we have already discussed so far, where the signal fault handler comes into play. The other functionality needed in the client is the response back with a page when contacted by the server. That is, once Client 1 sends the server a page request, the server contacts Client 2 and asks it for the same page. The functionality present in the clients which allow them to respond to such requests as well as send back a page while revoking their own permissions is defined in the `independent_listener_client` function:
```c
void *independent_listener_client(void *param)
{
    char buf[BASE_BUFFER_SIZE];
    while (1)
    {
        int val = recv_msg(master_socket, buf, BASE_BUFFER_SIZE);

        if (val == 0)
            continue;

        buf[val] = '\0';
        char *temp_str;
        int page_number = (int)strtol(buf, &temp_str, 10);
        printf("[INFO] Page number: %d \n", page_number);

        pthread_mutex_lock(&lock[page_number]);
        void *page_addr = ((char *)get_base_address()) + (page_number * PAGE_SIZE);
        mprotect(page_addr, PAGE_SIZE, PROT_READ);

        if (send_msg(master_socket, page_addr, PAGE_SIZE) <= 0)
        {
            printf("Could not send page requested!\n");
            exit(1);
        }
        printf("[INFO] Client successfully sent page back to server\n");
        mprotect(page_addr, PAGE_SIZE, PROT_NONE);
        pthread_mutex_unlock(&lock[page_number]);
    }
    return NULL;
}
```
A number of things are happening here that are relevant:
- This function basically runs on a independent thread on each client listening for server requests. 
- Now, it first receives the page number from the server. 
- Then, it aims to send the page back to the server and if the operation is successful, it will revoke it's current permissions to `NONE` (or `PROT_NONE` in `mprotect`). 
- Unlike the signal fault handler function which was using the `sig sockets`, this function utilizes `client sockets` for communication.
- Moreover, we utilize locks to ensure that only one client enters the critical sections (of both signal fault handler and this function) for a particular page number at any given time to provide consistency guarantees and avoid race conditions.

We can also get a quick look at the Rocket API for reading and writing to memory. They are very simple, and are used so that we know the operation the programmer wants to perform and then wrap code that we know will trigger signal faults if the page is not owned by the client:
```c
void rocket_write_addr(void *addr, void *data, int num_bytes)
{
    currentOperation = WRITING;
    memcpy(addr, data, num_bytes);
    memcpy(addr, data, num_bytes);
}

void rocket_read_addr(void *addr, void *buf, int num_bytes)
{
    currentOperation = READING;
    memcpy(buf, addr, num_bytes);
    memcpy(buf, addr, num_bytes);
}
```

### The server code and architecture
The server architecture is now much easier to understand since we have already discussed it's responsibilities when we were detailing the client code base. In `rocket_server.h` we have the following functions:
```c
#ifndef ROCKET_SERVER_H
#define ROCKET_SERVER_H

#include "com.h"

int rocket_server_init(int addr_size, int num_clients);

int rocket_server_exit();

#endif  // ROCKET_SERVER_H
```

Since the API is only required at the client side, we do not have to expose any relevant functionalities on the server. Moreover, coming back to the original use-case in the diagram at the start, the server's role is manifold:
- Client 1's signal fault handler gets in touch with the server using `sig sockets` to request access to a page. Thus, the server is tasked with using this same communication medium (`sig sockets`) to respond to Client 1 with the page.
- The second task the server has to do, is to be able to get in touch with Client 2 and request it for a page, get it via the `client sockets`, and then send the page back to Client 1.  
- Both the aforementioned tasks are undertaken in one function in `rocket_server.c` called `independent_listener_server()`. This function runs as a thread for all client server connections so we have as many threads running on server as the number of clients. These are used for completing the tasks outlined above.
- Since the server sees a global view of the system, it also has to keep track of all page ownerships by all the clients. We do this using the `clientInfos` struct which maintains a list of all client readers or one exclusive writer for that page. This will become clearer with the defintion of the `independent_listener_server()` function below:

```c
void *independent_listener_server(void *param)
{
    int acc_sock = *((int *)param);
    printf("Listening to accepted socket: %d\n", acc_sock);
    char buf[BASE_BUFFER_SIZE];
    sleep(5);

    while (1)
    {

        //RECEIVING PAGE NUMBER FROM CLIENT 1
        int val = recv_msg(acc_sock, buf, BASE_BUFFER_SIZE);
        if (val == -1)
        {
            continue;
        }
        buf[val] = '\0';

        pthread_mutex_lock(&server_socket_lock);

        int client_number = -1, operation = -1, page_number = -1;
        char copy_buf[BASE_BUFFER_SIZE];
        strcpy(copy_buf, buf);
        parse_buf(copy_buf, strlen(copy_buf), &client_number, &operation, &page_number);

        if (operation == WRITING)
        {

            int target_client_sock = pageOwnerships[page_number].clientExclusiveWriter.client_socket;

            if (target_client_sock == -1)
            {
                int index = pageOwnerships[page_number].clientReaders.num_readers - 1;
                target_client_sock = pageOwnerships[page_number].clientReaders.readers[index].client_socket;
            }

            //SENDING PAGE REQUEST TO CLIENT 2
            if (send_msg(target_client_sock, &page_number, sizeof(int)) <= 0)
            {
                printf("Could not send message for page request!\n");
                exit(1);
            }
            printf("[INFO] Page request sent to client\n");
            char data[PAGE_SIZE];

            //RECEIVING PAGE FROM CLIENT 2
            for (int total = PAGE_SIZE, index = 0; total != 0;)
            {
                int val = recv_msg(target_client_sock, &data[index], total);
                total = total - val;
                index = index + val;
            }
            printf("[INFO] Page requested successfully from client\n");
            //UPDATE PAGE OWNERSHIPS!!! WE ARE ONLY CHANGING SOCKET. NOTHING ELSE.

            pageOwnerships[page_number].clientExclusiveWriter.client_socket = clientInfos[client_number].client_socket;
            pageOwnerships[page_number].clientReaders.num_readers = 0;

            printf("[INFO] Updated page ownership\n");
            //SENDING PAGE FROM MASTER TO CLIENT 1
            printf("[INFO] Attempting to send page from master to client...\n");

            send_msg(acc_sock, &data, PAGE_SIZE);

            pthread_mutex_unlock(&server_socket_lock);
        }

        else if (operation == READING)
        {
            int target_client_sock = pageOwnerships[page_number].clientExclusiveWriter.client_socket;

            // if there is no exclusive writer
            if (target_client_sock == -1)
            {
                int index = pageOwnerships[page_number].clientReaders.num_readers - 1;
                target_client_sock = pageOwnerships[page_number].clientReaders.readers[index].client_socket;
            }

            //SENDING PAGE REQUEST TO CLIENT 2
            if (send_msg(target_client_sock, &page_number, sizeof(int)) <= 0)
            {
                printf("Could not send message for page request!\n");
                exit(1);
            }
            printf("[INFO] Page request sent to client\n");
            char data[PAGE_SIZE];

            //RECEIVING PAGE FROM CLIENT 2
            for (int total = PAGE_SIZE, index = 0; total != 0;)
            {
                int val = recv_msg(target_client_sock, &data[index], total);
                total = total - val;
                index = index + val;
            }
            printf("[INFO] Page requested successfully from client\n");

            //UPDATE PAGE OWNERSHIPS!!!
            int index = pageOwnerships[page_number].clientReaders.num_readers;
            pageOwnerships[page_number].clientReaders.readers[index].client_socket = clientInfos[client_number].client_socket;
            pageOwnerships[page_number].clientReaders.num_readers++;

            pageOwnerships[page_number].clientExclusiveWriter.client_socket = -1;
            pageOwnerships[page_number].clientExclusiveWriter.sig_socket = -1;

            printf("[INFO] Updated page ownership\n");
            //SENDING PAGE FROM MASTER TO CLIENT 1
            printf("[INFO] Attempting to send page from master to client...\n");

            if (send_msg(acc_sock, &data, PAGE_SIZE) <= 0)
            {
                printf("Failed to send page data\n");
            }

            pthread_mutex_unlock(&server_socket_lock);
        }
    }
    return NULL;
}
```

This is a large and complicated function, but the working can be broken down as follows. First, the function starts to receive the page request from the client (in our example, Client 1) via the `sig socket` for Client 1. Since this request contains the client number, the operation (reading or writing), and the page number as a single string, we have defined a `parse_buf()` function that separates these and returns them into variables. Next, we take these values and see what is the operation at hand. Consider the exclusive writing case first. In case Client 1 wants to write to that page, we first need to determine who the original owner is. The server does this by checking the `pageOwnerships` struct and finding the current exclusive writer. If there is a current writer, the socket value will be an integer not equal to -1, and that value is set to the `target_client_sock` variable. In case this value is a -1, it means that at this point in time we are only looking at concurrent readers, and thus we find the last reader who was given access to the page to get the page from now. Then, we send the page request to the target client socket (in this case the `client socket` for Client 2) and attempt to receive the 4KB page via the client socket. Assuming this goes successfully, the server then has to update page ownerships. Since this is the writing case, we can only have one exclusive writer. We set that exclusive writer to the current client requesting the page (Client 1), and then set the readers to 0, since at the time of writing no readers can access this page. Finally, we send this page to Client 1 and wrap up the function.

In case instead of requesting write access to the page, Client 1 wanted read access, the code progresses slightly differently. Like before, we find the `target_client_sock` and see if that value is a -1 or not. It can only be -1 in case there are no active exclusive writers at the moment. If it is, we again set the target client to be the last reader who had access to the page. Everything else progresses as before, and the only key difference is in the updation of page ownerships. Since this was a read request, we add the current reader (Client 1, say) to the list of all existing concurrent readers for now. We increment the indexing variable to ensure that the next time a reader is added, they will be again assigned to the next possible position in the array. Finally, in case this wasn't already so, we reinitialize the writer sockets to be -1, so that there cannot be a writer at this point of time. 

### Understanding the code for the test/in-class demo (W2RW2R):
The code for the clients for the tests/demo will be discussed here. The server code is just starting the server using `rocket_server_init()` so that is easy to understand as is. The `main` function definition for `client.c` (in `tests/segfault_handling/`) is detailed below:
```c
int main(int argc, char* argv[])
{ 


    if(rocket_client_init(SHARED_MEM_SIZE, NUM_CLIENTS, "128.120.211.76") == -1)
    {
        printf("Failed to initialize rocket client!\n");
        return 1;
    }

    // Writes the value 123 to the base address
    if(client_num == 0)
    {
        int num = 123;
        printf("Client %d is writing\n", client_num);
        rocket_write_addr(get_base_address(), &num, sizeof(int));
        printf("[OUTPUT] ADDRESS WRITTEN TO %p WITH VALUE %d --- %s\n", get_base_address(), *((int*)(get_base_address())), get_local_time());
    }

    // writes the value 321 to the base address
    else if(client_num == 1)
    {
        sleep(8);
        int num = 321;
        printf("Client %d is writing\n", client_num);
        rocket_write_addr(get_base_address(), &num, sizeof(int));
        printf("[OUTPUT] ADDRESS WRITTEN TO %p WITH VALUE %d --- %s\n", get_base_address(), *((int*)(get_base_address())), get_local_time());
    }


    // reads from base address 2 times
    else if(client_num == 2)
    {
        sleep(5);
        printf("Client Num: %d\n", client_num);
        int read_num = -1;
        rocket_read_addr(get_base_address(), &read_num, sizeof(int));
        printf("[OUTPUT] READ VALUE: %d --- %s\n", read_num, get_local_time());

        sleep(11);
        printf("Client Num: %d\n", client_num);
        read_num = -1;
        rocket_read_addr(get_base_address(), &read_num, sizeof(int));
        printf("[OUTPUT] READ VALUE: %d --- %s\n", read_num, get_local_time());
    }

    // reads from base address 2 times
    else if(client_num == 3)
    {
        sleep(5);
        printf("Client Num: %d\n", client_num);
        int read_num = -1;
        rocket_read_addr(get_base_address(), &read_num, sizeof(int));
        printf("[OUTPUT] READ VALUE: %d --- %s\n", read_num, get_local_time());

        sleep(11);
        printf("Client Num: %d\n", client_num);
        read_num = -1;
        rocket_read_addr(get_base_address(), &read_num, sizeof(int));
        printf("[OUTPUT] READ VALUE: %d --- %s\n", read_num, get_local_time());
    }
    
    while(1);

    rocket_client_exit();

```

Overall this example as well as the above function is simple to understand. We are looking to do a single write (W) by a client who already owns the page followed by 2 readers reading from the page (2R), then another write by a new client (W), then the same two readers again requesting read access to the page and reading it. This is why our example is labeled W2RW2R.

The code progresses as follows: The first client (or with client num 0) decides to write 123 to the base address at 0x40000000. This client already owns this page in the memory, so it should not be a problem. Next, clients with client nums 2 and 3 who are the readers will choose to read this value. Thus, if our framework is correct, both of them should read 123. And this is what they are able to do. For client with client num 1, after a certain sleep period, it will decide to write to the same address 0x40000000. Since it doesn't own the page, it will request access to it and write the value 321. If all works well, the same two readers will now decide to read this value and we will see that they both read 321. The working of this demo test as well as the steps to run it are discussed in more detail in the next section.

### Steps to run test cases for W2RW2R example (demo):
- Here, to emulate the distributed systems, we will be using the CSIF machines available to CS students. We will show how to `ssh` into these to set up the server as well as the clients. We assume that your Kerberos username is represented as `{username}`. We are also assuming that you have downloaded/cloned and stored our repository in the root directory in the CSIF machines.

#### Master/Server set-up:
`$ ssh {username}@pc19.cs.ucdavis.edu`

`$ cd Rocket/tests/segfault_handling/`

Now, you should use `ifconfig` to check the IP address of the server machine. The IP address needs to be updated in `client.c` in the current directory in case it is different from the value we have in the file at the moment:
![img](/images/i2.png)

`$ ./build.sh`

![img](/images/i1.png)

`$ ./server`


#### Client 1 set-up (First writer):
`$ ssh {username}@pc21.cs.ucdavis.edu`

`$ cd Rocket/tests/segfault_handling/`

`$ ./client`


#### Client 2 set-up (Reader):
`$ ssh {username}@pc22.cs.ucdavis.edu`

`$ cd Rocket/tests/segfault_handling/`

`$ ./client`

#### Client 3 set-up (reader):

`$ ssh {username}@pc23.cs.ucdavis.edu`

`$ cd Rocket/tests/segfault_handling/`

`$ ./client`

#### Client 4 set-up (Second Writer):
`$ ssh {username}@pc24.cs.ucdavis.edu`

`$ cd Rocket/tests/segfault_handling/`

`$ ./client`


#### The W2RW2R test in action:
- As soon as all the clients are started, the server will aim to connect to them. This will be apparent from the messages you will notice on the terminals:
![img](/images/i3.png)

- Next, two things to note: The clients each get a starting address in memory: Client1 gets 0x40000000, Client2 gets 0x50000000, Client3 gets 0x60000000, and Client4 gets 0x70000000. Then Client1, which owns Page 0 at the base address of 0x40000000 attempts to write the integer value 123 to that address using `rocket_write_addr` API of our framework:
![img](/images/i4.png)

- Next, two operations take place again. Client3 and Client4 attempt to read the page (using `rocket_read_addr` API call of our framework) that Client1 had written to, at 0x40000000. Right after this is done we see that both clients read 123 which was written by Client1 in the previous write step (observe Client3 and Client4's terminals). Then after this Client2 executes a write request at the same address, writing 321:
![img](/images/i5.png)

- Finally, Client3 and Client4 issue concurrent read requests for the same page again, and it can be seen that they both read 321:
![img](/images/i7.png)

