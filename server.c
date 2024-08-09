#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void error(char * msg) {
  perror(msg);
  exit(1);
}

void read_and_print(int sockfd) {
  char buffer[256];
  int n = read(sockfd, buffer, 255);
  if(n < 0) {
    error("ERROR reading from the socket");
  }
  printf("%s\n", buffer);
}

int main(int argc, char ** argv) {
  int sockfd, newsockfd, portno, clilen, n;
  char buffer[256] = {0};
  struct sockaddr_in serv_addr = {'\0'}, cli_addr = {'\0'};

  if(argc < 2) {
    fprintf(stderr, "Too few arguments; No port number provided.\n");
    exit(1);
  }
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    error("ERROR opening socket!");
  }
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(portno);
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  if(bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
    error("ERROR on binding");
  }
  listen(sockfd, 5);
  clilen = sizeof(cli_addr);
  int pid;
  while(1) {
    newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, (socklen_t *)&clilen );
    if(newsockfd < 0) {
      error("ERROR on accept");
    }
    pid = fork();
    if(pid < 0) {
      error("ERROR on fork");
    }
    if(pid == 0) {
      close(sockfd);
      read_and_print(newsockfd);
      exit(0);
    } else {
      close(newsockfd);
    }
  }
  return 0;
}
