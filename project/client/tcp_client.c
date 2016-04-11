#include "tcp_client.h"
#include "common.h"
#include "elev.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

#include <assert.h>

#define MASTER_IP "129.241.187.141"

static int sockfd; //, n;
static char sendline[255];
static char recvline[255];
static struct sockaddr_in servaddr;

pthread_mutex_t lock;

void tcp_client_init() {
    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof servaddr);
 
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(20022);
 
    inet_pton(AF_INET, MASTER_IP, &(servaddr.sin_addr));
 
    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
}

void tcp_common_call(char button, char action, int floor) {
    char instruction[255];
    bzero(recvline, 255);
    instruction[0] = 'b';
    instruction[1] = action;
    instruction[2] = button;
    instruction[3] = floor + '0';
    tcp_client_send(instruction);
}

void tcp_client_send(char instruction[255]) {
    pthread_mutex_lock(&lock);
    bzero(recvline, 255);
    write(sockfd, instruction, 255);
    recv(sockfd, recvline, 255 , 0);
    if (strcmp(instruction, recvline) != 0)
        puts("fail");
    puts("done");
    pthread_mutex_unlock(&lock);
}

void tcp_elevator_recieve_assignment() {
    //recv()
}

void tcp_client_kill() {
    close(sockfd);
}
 
void tcp_test() {
    tcp_client_init();
    while(!elev_get_stop_signal())
    {
        bzero(sendline, 255);
        bzero(recvline, 255);
        //fgets(sendline, 255, stdin); /*stdin = 0 , for standard input */

        if (elev_get_button_signal(BUTTON_COMMAND, 2)) 
        {
            strcpy(sendline, "bc2");
            write(sockfd, sendline, strlen(sendline)+1);
            read(sockfd, recvline, 255);
            printf("%s\n", recvline);
        }
    }
    close(sockfd);
}