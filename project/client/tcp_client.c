#include "tcp_client.h"
#include "elev.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

#define MASTER_IP "129.241.187.141"

static int sockfd; //, n;
static char sendline[255];
static char recvline[255];
static struct sockaddr_in servaddr;

void tcp_client_init() {
    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof servaddr);
 
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(20022);
 
    inet_pton(AF_INET, MASTER_IP, &(servaddr.sin_addr));
 
    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
}

void tcp_common_call(char button, char action, int floor) {
    bzero(sendline, 255);
    bzero(recvline, 255);
    sendline[0] = 'b';
    sendline[1] = action;
    sendline[2] = button;
    sendline[3] = floor + '0';
    write(sockfd, sendline, strlen(sendline)+1);
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