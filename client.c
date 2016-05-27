#include<stdio.h> //printf
#include <stdlib.h>
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr

void *connection_handler(void *socket_desc){
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size,i;
    char server_message[2000];

    //Receive a message from server
    while((read_size = recv(sock , server_message, 2000 , 0)) > 0){
        for(i=0;i<read_size;i++)
            printf("%c",server_message[i]);
    }

    if(read_size == 0){
        puts("You have been disconnected\n");
        puts("Exiting...\n");
        fflush(stdout);
        exit(0);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
    close(socket_desc);
    return 0;
}


int main(int argc , char *argv[]){

    int sock,i,login=0;
    struct sockaddr_in server;
    char message[1000];
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1){
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );

    //Connect to the server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");
    puts("Welcome to the chat Room!");

    pthread_t recv_thread;
    int *new_sock = malloc(1);
    *new_sock = sock;

    if(pthread_create( &recv_thread , NULL ,  connection_handler , (void*) new_sock)<0){
		printf("Could not create receiving thread\n");
		return 1;
	}

    while(1){
        if(login==0){
            printf("Enter the username for the Chat room: ");
            login=1;
        }
    	fgets(message, sizeof(message),stdin);
        //Send some data
        if( send(sock , message , strlen(message) , 0) < 0){
            puts("Send failed");
            return 1;
        }
    }
    close(sock);
    return 0;
}
