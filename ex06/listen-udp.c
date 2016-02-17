#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#define SERVER_PORT 20022

main(int argc, char *argv[]) {
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
  
  bzero((char *) &name, sizeof(name));
  name.sin_family = AF_INET;
  name.sin_addr.s_addr = htonl(INADDR_ANY);
  name.sin_port = htons(SERVER_PORT);
  
  if (bind(sock, (struct sockaddr *) &name, sizeof(name))) {
    perror("bind");
    exit(1);
  }
  
  printf("Listening on port number #%d\n", ntohs(name.sin_port));
  
  if ((bytes = read(sock, message, 1024)) > 0) {
    message[bytes] = '\0';
    printf("Recieved: %s\n", message);
  }

  close(sock);
}

