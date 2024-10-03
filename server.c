#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

void error(char * msg) {
  perror(msg);
  exit(1);
}
void not_found(int sockfd) {
  char response[256]=  "HTTP/1.0 404 Not Found";
  int n = write(sockfd,response,sizeof(response));
  if(n < 0) {
    error("ERROR writing to the socket");
  }
}
void send_response(int sockfd, char* location, char* content_type) {
    FILE * f  = fopen(location,"r");
    if(f == NULL) {
      fprintf(stderr, "Failed to open %s\n",location);
      not_found(sockfd);
      return;
    }
    
    fseek (f, 0, SEEK_END);
    size_t length = ftell (f);
    fseek (f, 0, SEEK_SET);
    char *buffer = malloc (length);
    fread (buffer, 1, length, f);
    fclose (f);
    char response[256];
    sprintf(response, "HTTP/1.0 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n", content_type, length);
    int n = write(sockfd,response,strlen(response));
    if(n < 0) {
      error("ERROR writing to the socket");
    }
    n = write(sockfd,buffer,length);
    if(n < 0) {
      error("ERROR writing to the socket");
    }
    
}

void read_and_print(int sockfd) {
  int n = 0;
  const size_t buffer_len = 512;
  char *buffer = NULL;
  buffer = malloc(buffer_len);
  n = read(sockfd, buffer, buffer_len-1);
  if(n < 0) {
    error("ERROR reading from the socket");
  }

  printf("%s\n",buffer);
  char * token;
  token = strtok(buffer, " \n");
  if(strcmp(token, "GET") !=0 ) {
    char response[256]=  "HTTP/1.0 501 Not Implemented";
    n = write(sockfd,response,sizeof(response));
    free(buffer);
    if(n < 0) {
      error("ERROR writing to the socket");
    }
    return;
  }
  token = strtok(NULL, " \n");
  if(strcmp(token, "/") == 0) {
    free(buffer);
    send_response(sockfd,"res/index.html", "text/html");
    return;

  } else if (token != NULL){

    char location[256];
    sprintf(location,"res%s",token);
    if(strstr(token, ".html")) {
        send_response(sockfd, location, "text/html");
    } else if (strstr(token, ".jpg")){
        send_response(sockfd, location, "image/jpg");
    } else if (strstr(token, ".css")){
        send_response(sockfd, location, "text/css");
    }
    free(buffer);
    return;

  }

  free(buffer);

}

int main(int argc, char ** argv) {
  int sockfd, newsockfd, portno, clilen, n;
  char buffer[256] = {0};
  struct sockaddr_in serv_addr = {'\0'}, cli_addr = {'\0'};

  if(argc != 2) {
    fprintf(stderr, "Usage: %s <portno>\n", argv[0]);
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
