#include "backup.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT 20022
#define GROUP "127.0.0.1"
#define TIMEOUT 3 //seconds
#define MESSAGELENGTH 20

void printHelp(void) {
	printf("  -m\t\trun as master\n");
	printf("  -b\t\trun as backup\n");
}

void sendMessage(const char *message) {
    struct sockaddr_in addr;
    int fd;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(GROUP);
    addr.sin_port=htons(PORT);

    if (sendto(fd, message, strlen(message), 0,(struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("sendto");
        exit(1);
    }
}

int recieveMessage(char* message) {
	int sock;
	struct sockaddr_in name;
	struct hostent *hp, *gethostbyname();
	int bytes;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)   {
		perror("socket");
		exit(1);
	}

	struct timeval tv;
	tv.tv_sec = TIMEOUT;
	tv.tv_usec = 0;
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
		perror("setsockopt");
	}

	bzero((char *) &name, sizeof(name));
	name.sin_family = AF_INET;
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	name.sin_port = htons(PORT);

	if (bind(sock, (struct sockaddr *) &name, sizeof(name))) {
		perror("bind");
		exit(1);
	}

	//printf("Listening on port number #%d...\n", ntohs(name.sin_port));

	if ((bytes = read(sock, message, 1024)) > 0) {
		message[bytes] = '\0';
		//printf("Recieved: %s\n", message);
	}
	/*else {
		printf("Recieved nothing...");
	}*/

	close(sock);

	return bytes;
}

void _runMaster(int i) {
	printf("~ MASTER ~\n");
	system("gnome-terminal -e \"./main -b\"");
	char message[1024];
	while(1) {
		printf("%i\n", i);
		sprintf(message, "Still alive! Count: %i", i++);
		sendMessage(message);
		sleep(1);
	}
}

void runBackup(void) {
	printf("~ BACKUP ~\n");
	char message[1024];
	int bytes;
	int digits;
	int count;
	while (1) {
		//sleep(1); // debug
		bytes = recieveMessage(message);
		if (bytes > 0) {
			//printf("Message recieved: %s\n", message);
			printf(".");
		}
		else {
			printf("Master died!\n");
			return;
		}
	}
}
/*
int _main(int argc, char *argv[]) {
	if (argc != 2) {
		printHelp();
	}
	else {
		if (argv[1][0] == '-') {
			switch (argv[1][1]) {
				case 'm':
				runMaster(1);
				break;

				case 'b':
				runBackup();
				break;

				default:
				printHelp();
				break;
			}
		}
		else
			printHelp();
	}
	return 0;
}
*/