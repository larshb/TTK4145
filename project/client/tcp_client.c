#include "tcp_client.h"

#include <arpa/inet.h>      // inet_pton
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netdb.h>
#include <pthread.h>        // pthread
#include <stdlib.h>         // atoi, exit
#include <string.h>         // bzero
#include <unistd.h>         // write, close
#include <stdio.h>          // sprintf, perror

static int sockfd;          // Socket file descriptor
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
    printf("Client connecting to: \"%s\"\n", master_ip);

    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof servaddr);
 
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(DEFAULT_PORT);
    
    // Use master_ip string to set servaddr socket_in
    inet_pton(AF_INET, master_ip, &(servaddr.sin_addr));
    
    char debugstr[255];
    inet_ntop(AF_INET, &(servaddr.sin_addr), debugstr, 255);
    //printf("master_ip = %s\n", master_ip);
    //printf("Socket IP = %s\n", debugstr);
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

    //debug
    //if (strcmp(instruction, recvline) != 0)
    //    puts("non-echo");
    //puts("done");

    //debug
    if (!success) {
        new_master_flag = 1;
        //tcp_client_init(common_get_next_master_ip());
    }
    //char hei[255];
    //strcpy(hei, recvline);
    pthread_mutex_unlock(&lock);
    return recvline;
}

void tcp_client_kill() {
    close(sockfd);
}