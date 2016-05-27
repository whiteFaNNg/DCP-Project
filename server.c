#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>	//socket types
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<stdbool.h>	//for bool values
#include<pthread.h> //for threading 

void *connection_handler(void *);
int msg(char*, char*, int, int);
int assign_conn(int, char*);
int online(char*);
void test();
void init_conn();
void* test_th();
int compare(char*, char*, int);
int srch(char*, int);
int ms(char* , int , int, int);

struct connection {
    bool busy;
	int desc;
	char name[20];
};
struct connection connections[100];

pthread_mutex_t mutexcon;

int main(int argc , char *argv[])
{

    int socket_desc , new_socket , c , *new_sock;
    struct sockaddr_in server , client;
    char *message;
    init_conn();
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    //Binding the socket
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("bind failed");
        return 1;
    }
    puts("bind done");
    pthread_mutex_init(&mutexcon, NULL);
    //Listen
    listen(socket_desc , 3);
    pthread_t test_thread;
    pthread_create( &test_thread , NULL ,  test_th , NULL);
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
        pthread_t conn_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;

        if( pthread_create( &conn_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        puts("Handler assigned");
    }

    if (new_socket<0)
    {
        perror("accept failed");
        return 1;
    }
    return 0;
}

//This will handle connection for each client
 
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size,len,id,login=0;
    char message[230] , client_message[2000];
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        int i;
        if(login==0){
            id=assign_conn(sock,client_message);
            if(id<0){
                write(sock, "Room is full" , 12);
                close(sock);
                free(socket_desc);
                pthread_exit(NULL);
            }
            login=1;
        }else{
			//execute the /exit command
            if(compare(client_message,"/exit\n",read_size)==0){
                pthread_mutex_lock (&mutexcon);
                connections[id].busy=false;
                pthread_mutex_unlock (&mutexcon);
                close(sock);
                free(socket_desc);
                pthread_exit(NULL);
            }
			//execute the /online command
            if(compare(client_message,"/online\n",read_size)==0){
                len = online(client_message);
                write(sock, client_message , len);
                continue;
            }
			//execute the /whisper command
            if(compare(client_message,"/whisper ",9)==0){
                int usr = srch(client_message,read_size);
                if(usr < 0){
                    write(sock,"Error in whisper\n",17);
                    continue;
                }else{
                    int s=ms(client_message, id, usr, read_size);
                    write(connections[usr].desc, client_message,s);
                    continue;
                }
            }
			//send message to all online users
            for(i=0;i<100;i++){
                if(connections[i].busy==true && sock!=connections[i].desc){
                    len=msg(message,client_message,read_size,id);
                    write(connections[i].desc, message , len);
                }
            }
        }
    }

    if(read_size == 0)
    {

        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
    pthread_mutex_lock (&mutexcon);
    connections[id].busy=false;
    pthread_mutex_unlock (&mutexcon);
    free(socket_desc);

    return 0;
}

int msg(char* x, char* y, int z, int t){
    int i;
    int j=strlen(connections[t].name)-1;
    for(i=0;i<j;i++){
        x[i]= connections[t].name[i];
    }
    x[j]=':';
    x[j+1]=' ';
    for(i=0;i<z;i++){
        x[i+j+2]=y[i];
    }
    return j+z+2;
}

int assign_conn(int x, char* y){
    int i, id=-1;
    pthread_mutex_lock (&mutexcon);
    for(int i =0;i<100;i++){
        if(connections[i].busy==false){
            connections[i].busy = true;
            connections[i].desc = x;
            printf("Added socket to the table: %i\n",connections[i].desc);
            strcpy(connections[i].name, y);
            id=i;
            break;

        }
    }
    pthread_mutex_unlock (&mutexcon);
    return id;
}

int online(char *x){
    int i,j,len=0;

    for(i=0;i<100;i++){
        if(connections[i].busy==true){
            for(j=0;j<strlen(connections[i].name);j++){
                x[len]=connections[i].name[j];
                len++;
            }
        }
    }
    return len;
}

void test(){
    int i;
    int found=0;
    for(i=0;i<100;i++){
        if(connections[i].busy==true){
            printf(" busy-%i ",i);
            found=1;
        }
    }
    if(found==0){
        printf("No connections!");
    }
    printf("\n");
}

void init_conn(){
    int i;
    for(i=0;i<100;i++){
        connections[i].busy=false;
    }
}

void* test_th(){
    while(1){
        sleep(5);
        test();
    }
}

int compare(char* x, char* y, int z){
    int i,clean=0,j=strlen(y);
    for(i=0;i<j;i++){
        if(x[i]!=y[i])
            clean=1;
    }
    if(z!=j){
        clean=1;
    }
    return clean;
}

int srch(char* x, int y){
    int i,j,k,usr=-1,found=-1;
    for(i=9;i<y;i++){
        if(x[i]==' '){
            found=i;
            break;
        }
    }
    if(found!=-1){
        for(i=0;i<100;i++){
            if(connections[i].busy){
                int flag=1;
                k=strlen(connections[i].name);
                for(j=0;j<k,(j+9)<found;j++){
                    if(x[9+j]!=connections[i].name[j]){
                        flag=-1;
                        break;
                    }
                }
                if(flag==1){
                    return i;
                }
            }
        }
    }
    return found;
}

int ms(char* x, int y, int t, int z){
    int i,offset=9+strlen(connections[t].name),len=z-offset;
    char m[2000];
    for(i=0;i<len;i++){
        m[i]=x[i+offset];
    }
    strcpy(x,"whisper <");
    for(i=0;i<(strlen(connections[y].name)-1);i++)
        x[i+9]=connections[y].name[i];
    x[8+strlen(connections[y].name)]='>';
    x[9+strlen(connections[y].name)]=' ';
    for(i=0;i<len;i++){
        x[10+strlen(connections[y].name)+i]=m[i];
    }
    return 10+strlen(connections[y].name)+len;
}
