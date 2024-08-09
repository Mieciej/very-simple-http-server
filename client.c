#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

void error(char *msg) {
  perror(msg);
  exit(1);
}
int main(int argc, char ** argv) {
  int sockfd, portno, n;
  struct sockaddr_in serv_addr = {0};
  struct hostent *server;
  if (argc < 3) {
    fprintf(stderr, "usage %s hostname portname\n", argv[0]);
    exit(0);
  }
  char buffer[256];
  portno = atoi(argv[2]);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    error("ERROR opening socket");
  }
  server = gethostbyname(argv[1]);
  if(server == NULL) {
    fprintf(stderr, "could not find host %s\n", argv[1]);
    exit(0);
  }
  serv_addr.sin_family = AF_INET;
  memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
  serv_addr.sin_port = htons(portno);
  if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    error("ERROR connecting");
  }
  printf("Enter message: ");
  fgets(buffer, 255, stdin);
  n = write(sockfd,buffer, strlen(buffer));
  if(n < 0) {
    error("ERROR writing to socket");
  }
  memset(buffer, '\0', 256);
  n = read(sockfd, buffer, sizeof(buffer));
  if(n < 0) {
    error("ERROR reading socket");
  }
  printf("%s\n", buffer);
  return 0;
}
