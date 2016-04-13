#include "tcp_server.h"
#include "common.h"
#include "elevator.h"
#include "manager.h"

#include <stdio.h>
#include <string.h>     // strlen
#include <stdlib.h>     // strlen
#include <sys/socket.h>
#include <arpa/inet.h>  // inet_addr
#include <unistd.h>     // write
#include <pthread.h>    // for threading

static int socket_desc , client_sock , c , *new_sock;
static struct sockaddr_in server , client;

pthread_mutex_t elevator_lock;

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
    for (int i = 0; i < MAX_ELEVATORS; i++)
        remote_elevator[i].active = 0;
	while((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))) {
        puts("Connection accepted");
         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
         
        if( pthread_create( &sniffer_thread , NULL ,  elevator_connection_handler , (void*) new_sock) < 0) {
            perror("could not create thread");
            return (int*)1;
        }
        else { 
            puts("Created thread");
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        
        pthread_detach(sniffer_thread);
        printf("Handler assigned: %d\n\n", client_sock);

    }
     
    if (client_sock < 0) {
        perror("accept failed");
        return (int*)1;
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
    //char *message
    char client_message[255];
    
    // add elevator
    int elevator_id = add_remote_elevator(sock);
    remote_elevator[elevator_id].rank = sock;

    //DEBUG PRINT
    for (int i = 0; i < MAX_ELEVATORS; i++) {
        pthread_mutex_lock(&elevator_lock);
        printf("remote_elevator[%2d] = %3d\n", i,
                                                remote_elevator[i].active
                                                ? remote_elevator[i].rank
                                                : -1);
        pthread_mutex_unlock(&elevator_lock);
    }

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

        printf("Recieved C/S: %s\n", client_message);
        switch (client_message[0]) {
        	case 'b': // button assignment
        	direction = client_message[2] == 'd';
        	floor = client_message[3] - '0';
            if (client_message[1] == 'c')
        	    common_set_request(floor, direction, 0);
            else {
                ownership = manager_assign(direction, floor);
                //char ownership_str[3];
                sprintf(client_message, "%03d", ownership);
                //bzero(client_message);
                //for (int i = 0; i < 3; i++)

            }
        	break;

            case 'p': // polling
            switch (client_message[1]) {
                case 'r': // rank
                sprintf(client_message, "%03d", sock);
                break;
            }
            break;

            case 'u': // update
            switch (client_message[1]) {
                case 's': // status
                switch (client_message[2]) {
                    case 'i':
                    remote_elevator[elevator_id].state = 0;
                    break;
                    case 'm':
                    remote_elevator[elevator_id].state = 1;
                    break;
                    case 'd':
                    remote_elevator[elevator_id].state = 2;
                    break;
                    case 's':
                    remote_elevator[elevator_id].state = 3;
                    break;
                }
                remote_elevator[elevator_id].direction = client_message[3] == 'd';
                char floor_str[3];
                strncpy(floor_str, client_message + 4, 3);
                remote_elevator[elevator_id].floor = atoi(floor_str);
                break;
            }
            break;
        }
        printf("Sending  S/C: %s\n", client_message);
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
    delete_remote_elevator(elevator_id);
    free(socket_desc);
    
    printf("Socket freed: %d\n", sock);
    
    return 0;
}


//DEBUG/FØRSTEUTKAST////////////////////////
int add_remote_elevator(int socket){
    int elevator_id = -1;
    pthread_mutex_lock(&elevator_lock);
    for (int i = 0; i < MAX_ELEVATORS; i++) {
        if (!remote_elevator[i].active) {
            remote_elevator[i].active = 1;
            remote_elevator[i].rank = socket;
            elevator_id = i;
            break;
        }
    }
    pthread_mutex_unlock(&elevator_lock);
    return elevator_id;
}

void delete_remote_elevator(int elevator_id){
    pthread_mutex_lock(&elevator_lock);
    remote_elevator[elevator_id].active = 0;
    int i = elevator_id + 1;
    if (remote_elevator[i].active)
        while(remote_elevator[i + 1].active && i < MAX_ELEVATORS)
            i++;
    remote_elevator[elevator_id] = remote_elevator[i];
    remote_elevator[i].active = 0;
    pthread_mutex_unlock(&elevator_lock);
}
/////////////////////////////////////////////