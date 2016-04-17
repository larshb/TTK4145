#include "tcp_client.h" 
#include "common.h"

#include <arpa/inet.h>      // inet_pton
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>        // pthread
#include <stdio.h>
#include <stdlib.h>         // atoi
#include <string.h>
#include <unistd.h>         // write

#include <assert.h>

static int sockfd; //, n;
//static char sendline[255];
static char recvline[255];
static struct sockaddr_in servaddr;

//dårlig løsning?
static int new_master_flag = 0;

int new_master() {
    if (new_master_flag) {
        new_master_flag = 0;
        return 1;
    }
    return 0;
}

pthread_mutex_t lock;

void tcp_client_init(const char* master_ip) {
    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof servaddr);
 
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(20022);
 
    inet_pton(AF_INET, master_ip, &(servaddr.sin_addr));
 
    int success = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (success < 0) {
        perror("client connect");
        exit(1);
    }
}

int tcp_common_call(char direction, char action, int floor) {
    char instruction[255];
    bzero(instruction, 255);
    instruction[0] = 'b';
    instruction[1] = action;
    instruction[2] = direction;
    instruction[3] = floor + '0';
    const char* rank_str = tcp_client_send(instruction);
    return atoi(rank_str);
}

int tcp_get_station_rank() { // role (master/slave), rank
    char instruction[255];
    bzero(instruction, 255);
    instruction[0] = 'p';
    instruction[1] = 'r';
    const char* recieved = tcp_client_send(instruction);
    char role_char[4];
    strncpy(role_char, recieved, 3);
    return atoi(role_char);
}

const char* tcp_get_next_master_ip() {
    char instruction[255]; 
    bzero(instruction, 255);
    instruction[0] = 'p';
    instruction[1] = 'n';
    return tcp_client_send(instruction);
}

void tcp_update_status(int state, int direction, int floor) {
    char instruction[255];
    bzero(instruction, 255);
    char floor_str[3];
    sprintf(floor_str, "%03d", floor);
    instruction[0] = 'u';
    instruction[1] = 's';
    switch (state) {
        case 0:
        instruction[2] = 'i';
        break;
        case 1:
        instruction[2] = 'm';
        break;
        case 2:
        instruction[2] = 'd';
        break;
        case 3:
        instruction[2] = 's';
        break;
    }
    instruction[3] = direction ? 'd' : 'u';
    for (int i = 0; i < 3; i++) {
        instruction[i + 4] = floor_str[i];
    }
    tcp_client_send(instruction);
}

void tcp_get_common_requests(int new_common_request[N_FLOORS][2]) {
    char instruction[255];
    bzero(instruction, 255);
    sprintf(instruction, "pa");
    const char* new_common_request_str = tcp_client_send(instruction);
    if (new_common_request_str[0] > 0) {//not blank response
        for (int flr = 0; flr < N_FLOORS; flr++) {
            new_common_request[flr][0] = new_common_request_str[flr * 2] - '0';
            new_common_request[flr][1] = new_common_request_str[flr*2 + 1] - '0';
        }
    }
    
}

const char* tcp_client_send(char instruction[255]) {
    pthread_mutex_lock(&lock);
    bzero(recvline, 255);
    write(sockfd, instruction, 255);
    int success = recv(sockfd, recvline, 255 , 0);
    if (strcmp(instruction, recvline) != 0)
        puts("non-echo");
    puts("done");

    //debug
    if (!success) {
        new_master_flag = 1;
        tcp_client_init(common_get_next_master_ip());
    }

    pthread_mutex_unlock(&lock);
    return recvline;
}

void tcp_client_kill() {
    close(sockfd);
}
/*
void tcp_test() {
    tcp_client_init();
    while(!elev_get_stop_signal())
    {
        bzero(sendline, 255);
        bzero(recvline, 255);
        //fgets(sendline, 255, stdin); //stdin = 0 , for standard input 

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
*/