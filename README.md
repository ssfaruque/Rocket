# Rocket: A Distributed Shared Memory (DSM) Framework

## Overview
- [__A basic example to explain the logic__](https://github.com/ecs251-w19-ucdavis/Rocket#a-basic-motivating-example-to-explain-the-code-logic)
- [__Understanding the role of signal sockets and client sockets__]()
- [__Understanding the client code and architecture__]()
- [__Understanding the server code and architecture__]()
- [__The code on the client and the server for the demo (W2RW2R)__]()
- [__Steps to run the in-class demo (W2RW2R)__]()


### A basic motivating example to explain the code logic
![alt_img](/images/expl.png)

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
To isolate the logic for communicating we use two separate sockets for the communication between clients and master. These are the signal sockets (`sig sockets`) or client sockets (`client sockets`):
- While we will get into this later, every time a page fault occurs at the client, there is a signal handler that intiates the read/write request to the master server. We use `sig sockets` only for communication to and from this signal fault handler at the client. Every time a communication or connection is required to either send or receive messages (pages) involving this signal fault handler `sig sockets` are used.
- `client sockets` are used everywhere else. In particular, each client runs a process in a detached thread called `independent_listener_client` which is used to listen if the server ever requests any pages from it. Communication for pages done by this thread always uses `client sockets`. Moreover, the server also has independent threads for each client through a thread function `independent_listener_server` through which it keeps in touch with the clients and then communicates with them to respond to their signal faults as well as request them for pages. Since here we require communicate with both the requesting client and the client to be requested, both the `sig sockets` and `client sockets` are used. This is because this thread function will be in touch with both the signal handler at the requesting client and the independent listener at the client to be requested. Hence, the `sig socket` and the `client socket` are both used.

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

