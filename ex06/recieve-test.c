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

int main(void) {
	char message[1024];
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
	tv.tv_sec = 3;
	tv.tv_usec = 0;
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
		perror("Error");
	}

	bzero((char *) &name, sizeof(name));
	name.sin_family = AF_INET;
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	name.sin_port = htons(PORT);

	if (bind(sock, (struct sockaddr *) &name, sizeof(name))) {
		perror("bind");
		exit(1);
	}

	printf("Listening on port number #%d...\n", ntohs(name.sin_port));

	if ((bytes = read(sock, message, 1024)) > 0) {
		message[bytes] = '\0';
		printf("Recieved: %s\n", message);
	}
	else {
		perror("read");
		exit(1);
	}

	close(sock);
	getchar();
	return 0;
}