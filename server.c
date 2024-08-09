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
  int n = 0;
  do {
  char buffer[256] = {0};
  n = read(sockfd, buffer, 255);
  if(n < 0) {
    error("ERROR reading from the socket");
  }
  printf("%s", buffer);
  } while(n == 255);
  char hello[256]=  "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nContent-Lenght: 20\r\n\r\n<html><body><h1>Kocham Natalke</h1></body></html>";
  n = write(sockfd,hello,sizeof(hello));
  if(n < 0) {
    error("ERROR writing to the socket");
  }
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
