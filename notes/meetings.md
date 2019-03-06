# Trello board
https://trello.com/b/DGMnzneN/rocket-project

# meeting4
# What we have done.
1. TCP connection between server and clients.
2. DSM logic for writing to pages.
3. DSM logic for page fault handling.

# What we're going to do this week 
1. DSM logic for reading from pages.
2. Add test cases
   * data consistency across clients
   * page fault handling
   * DSM logic for reading/writing.

# meeting3
# What we have done.
 1. Communication b/w server and client.
 2. Initialization of DSM system
 3. Sig fault handling
 4. Listening for the page request.

# What we're going to do this week 
 1. Integrating Sig fault handling with server client communication.
 2. Write tests that test the core DSM functionalities.
 
# Any issues we're having:
 1. Two way communication b/w server and client
     a) server to client: 1:m communication.
     b) client to server: 1:1 communication.
 2. Making sure its possible for clients with different ip address can communicate with server and vice versa.
 

# meeting2
# What we have done
1. implementing communication layer using socket
2. DSM layer planning   
   shared memory mechanism
# What we're going to do this week
   boilerplate code for DSM layer 
   * transfering ownership of pages(mprotect) 
   * handle sigfaults 
   * signal handling function
   * listening port for page requests
   * memory allocation (mmap)
# Any issues we're having:
   * Figuring out DSM mechanism and break tasks down
   
   

   
