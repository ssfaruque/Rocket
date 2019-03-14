# Rocket: A Distributed Shared Memory (DSM) Framework


### Steps to run test cases for W2RW2R example (demo):
- Here, to emulate the distributed systems, we will be using the CSIF machines available to CS students. We will show how to `ssh` into these to set up the server as well as the clients. We assume that your Kerberos username is represented as `{username}`. We are also assuming that you have downloaded/cloned and stored our repository in the root directory in the CSIF machines.

#### Master/Server set-up:
`$ ssh {username}@pc19.cs.ucdavis.edu`

`$ cd Rocket/tests/segfault_handling/`

`$ ./build.sh`

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

