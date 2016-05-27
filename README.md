
##Distributed and Concurrent Programming Project
---
###++++++++++++++++++++++++++++++++++   </br>| &nbsp;&nbsp;&nbsp;&nbsp;Chat Room using Sockets in Linux &nbsp;&nbsp;&nbsp;&nbsp;|</br>   ++++++++++++++++++++++++++++++++++

The Project is about creating a chat room using sockets in localhost server in Linux by implementing POSIX Threads Programming in C language. It consists of 2 files called client.c and server.c  
The server.c file is run on the server, whereas the client.c file is run on the client. 

####The Client.c File:

`Int sock` – holds the value for the socket.  
`struct sockaddr_in server` – holds the value for the address of the Server.  
`server.sin_addr.s_addr = inet_addr("127.0.0.1")` – sets the address to the localhost.  
`server.sin_family = AF_INET` – used for internet sockets.  
`server.sin_port = htons( 8888 )` – the port of the connection to the server.  
`connect(sock , (struct sockaddr *)&server , sizeof(server)` -this is the connection to the server.  
`pthread_create( &recv_thread , NULL ,  connection_handler , (void*) new_sock)` – creation of the receiving thread used for receiving data from the server.  
`send(sock , message , strlen(message) , 0)` – used for sending data to the server using the socket sock. The message is created by `fgets(message, sizeof(message),stdin)` input method.  
`close(sock)` – used for closing the socket of the client just before the end of the program.   
`void *connection_handler(void *socket_desc)` – function used in creation of the receiving thread used for getting data from the server.  
`read_size = recv(sock , server_message, 2000 , 0)` – this line is called in a loop and until there is response from the server the server_message variable will hold the data from it. However, if the read size is equal or smaller than 0, the client program will terminate.  

####The Server.c File:

`struct sockaddr_in server , client` – the address of the server and the client.  
`socket_desc = socket(AF_INET , SOCK_STREAM , 0)` – creation of the server socket responsible for listening to new connections. For doing this, first it is bound to the port 8888 with `bind(socket_desc,(struct sockaddr *)&server , sizeof(server)`. Then it is set to listen to new incoming connections with `listen(socket_desc , 3)`. New connections are accepted by this `new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)` where `new_socket` holds the value for the new socket for that connection. Than this socket is used as a parameter in a connection thread. This thread is created for every new connection established.  
`void *connection_handler(void *socket_desc)` is the function called when creating the connection thread for each user after calling the `pthread_create( &conn_thread , NULL ,  connection_handler , (void*) new_sock)` method.
Upon thread creation, the program checks if the user is logged in. if not, it will log him in. The server permits at most of 100 users to be logged in (`struct connection connections[100]`). The information for each user includes his corresponding socket number and his username. Since this data is being accessed and modified by multiple threads, mutex is being used to protect this data: `pthread_mutex_lock (&mutexcon)` for locking and `pthread_mutex_unlock (&mutexcon)` for unlocking the data. If the user is already logged in the program checks for 3 commands:  
* `if(compare(client_message,"/exit\n",read_size)==0)` – if the command is /exit, the server will close the connection to the user.
* `if(compare(client_message,"/online\n",read_size)==0)` – if the command is /online, the server will list all online users.
* `if(compare(client_message,"/whisper ",9)==0)` – if the command is /whisper, the server will first check for validity of the syntax of this command which follows “/whisper” + “ “ + “username” + “ “ + “message. If it finds no errors, the server will write to the corresponding user in the command a whisper.  

`int msg(char* x, char* y, int z, int t)` – formats the client message in “user: “+message format.  
`int assign_conn(int x, char* y)` – it assigns connection to the user in the users struct array.   
`int online(char *x)` – it formats the client_message for sending back to the client by listing all online users in the server. 
`void init_conn()` – used for initiation if the connections array for the users. 
`int compare(char* x, char* y, int z)` -used for comparing 2 strings to a certain length.   
`int srch(char* x, int y)` – used for searching of valid users from the whisper command.  
`int ms(char* x, int y, int t, int z)` – used for formatting the client message in the whisper command in the following order: “whisper <” + “sender” +”>” +”message”.

---

Mentor:    __Professor Marian Neagul__  
Students:  __Kliment Trajcheski, Denis Bajrami__

---
