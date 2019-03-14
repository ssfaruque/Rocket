# Rocket: A Distributed Shared Memory (DSM) Framework


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

This should look something like this:

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


#### The W2RW2R in action:
- As soon as all the clients are started, the server will aim to connect to them. This will be apparent from the messages you will notice on the terminals:
![img](/images/i3.png)

- Next, two things to note: The clients each get a starting address in memory: Client1 gets 0x40000000, Client2 gets 0x50000000, Client3 gets 0x60000000, and Client4 gets 0x70000000. Then Client1, which owns Page 0 at the base address of 0x40000000 attempts to write the integer value 123 to that address using `rocket_write_addr` API of our framework:
![img](/images/i4.png)

- Next, two operations take place again. Client3 and Client4 attempt to read the page (using `rocket_read_addr` API call of our framework) that Client1 had written to, at 0x40000000. Right after this is done we see that both clients read 123 which was written by Client1 in the previous write step (observe Client3 and Client4's terminals). Then after this Client2 executes a write request at the same address, writing 321:
![img](/images/i5.png)

- Finally, Client3 and Client4 issue concurrent read requests for the same page again, and it can be seen that they both read 321:
![img](/images/i7.png)

