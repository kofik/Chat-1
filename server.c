#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
  //declarations
  struct sockaddr_storage client_addr;
  socklen_t addr_size;
  struct addrinfo hints, *res;
  int sock, client_sock;
  size_t mematch;
  
  mematch = (3 * sizeof(char));

  //check for an argument
  if (argc != 2) {
    fprintf(stderr, "Error: Wrong number of arguments. Usage: ./server $PORT");
    exit(1);
  }

  //set up buffers
  char *recv_buffer;
  recv_buffer = malloc(256 * sizeof(char));
  char *buffer;
  buffer = malloc(256 * sizeof(char));
  char *name;
  name = malloc(32 * sizeof(char));
  char *recv_name;
  recv_name = malloc(32 * sizeof(char));
  char *mebuffer;
  mebuffer = malloc(256 * sizeof(char));

  /*give an initial message to send.
    we could potentially turn this into an auth handshake so
    that people are talking to who they think they're talking to */
  printf("Send /quit in order to terminate the program but give a name first.\nName:");
  gets(name);
  printf("%s: ", name);
  fgets(buffer, 250, stdin);

  // blank out hints and fill it out with info about the machine
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  getaddrinfo(NULL, argv[1], &hints, &res);
  
  //create the socket, bind to it, listen on it, accept when it is connected to
  sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  bind(sock,res->ai_addr,res->ai_addrlen);
  listen(sock, 10);
  client_sock = accept(sock,(struct sockaddr *)&client_addr, &addr_size);

  send(client_sock, name, 32, 0);
  recv(client_sock, recv_name, 32, 0);
  printf("Now talking to %s\n", recv_name);

  while ((strcmp(buffer, "/quit\n")) != 0) {
    send(client_sock, buffer, 256, 0);
    recv(client_sock, recv_buffer, 256, 0);
    if ((strcmp(recv_buffer, "/quit\n")) == 0) 
      break;
    if ((strncmp(recv_buffer, "/me", mematch)) == 0) {
      mebuffer = strtok(recv_buffer, " ");
      mebuffer = strtok(NULL, "");
      printf("*%s %s%s: ", recv_name, mebuffer, name);
    } else {
      printf("%s: %s%s: ", recv_name, recv_buffer, name);
    }
    fgets(buffer, 250, stdin);
  }

  send(client_sock, buffer, 256, 0);

  //free some stuff up so it closes neatly...
  freeaddrinfo(res);
  close(sock); 
  free(recv_buffer);
  free(buffer);
  return 0;
}
