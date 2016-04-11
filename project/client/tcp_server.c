#include "tcp_server.h"
#include "common.h"
#include "elevator.h"

#include <stdio.h>
#include <string.h>     // strlen
#include <stdlib.h>     // strlen
#include <sys/socket.h>
#include <arpa/inet.h>  // inet_addr
#include <unistd.h>     // write
#include <pthread.h>    // for threading

static int socket_desc , client_sock , c , *new_sock;
static struct sockaddr_in server , client;

void tcp_server_init()
{
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 20022 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("bind failed. Error");
        return;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
}

void *tcp_server_test() {
	
	while((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))) {
        puts("Connection accepted");
         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
         
        if( pthread_create( &sniffer_thread , NULL ,  elevator_connection_handler , (void*) new_sock) < 0) {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }
     
    if (client_sock < 0) {
        perror("accept failed");
        return 1;
    }
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *elevator_connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[255];
    
    //message = "Now type something and i shall repeat what you type \n";
    //write(sock , message , strlen(message));
     
    //Receive a message from client
    int direction;
    int floor;
    int ownership;
    while( (read_size = recv(sock , client_message , 255 , 0)) > 0 )
    {
        //Send the message back to client
        //write(sock , client_message , strlen(client_message));
        /*switch (client_message[0]) {
            case 'b':
            if (client_message[1] == 'r') {
                printf("Elevator called ");
            }
            else if (client_message[1] == 'c') {
                printf("Elevator completed ");
            }
            switch(client_message[2]) {
                case 'u':
                printf("up");
                break;
                case 'd':
                printf("down");
                break;
            }
            printf(" on floor %c\n", client_message[3]);
            break;
        }*/
        switch (client_message[0]) {
        	case 'b':
        	direction = client_message[2] == 'd';
        	floor = client_message[3] - '0';
        	ownership = client_message[1] == 'c' ? 0 : 2;
        	common_set_request(floor, direction, ownership);
        	break;
        }
        printf("%s\n", client_message);
        write(sock , client_message , strlen(client_message));
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
         
    //Free the socket pointer
    free(socket_desc);
     
    return 0;
}