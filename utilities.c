#include "utilities.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

void gracefulExit(int socket_fd, char *message, char *sendBuffer,
                  char *receiveBuffer) {
  perror(message);
  free(sendBuffer);
  free(receiveBuffer);
  close(socket_fd);
  exit(EXIT_FAILURE);
}

int establishConnection(int port) {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_fd < 0) {
    perror("socket error");
    close(socket_fd);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  if (inet_aton("127.0.0.1", &serverAddr.sin_addr) == 0) {
    perror("error binding address");
    close(socket_fd);
    exit(EXIT_FAILURE);
  }

  if (connect(socket_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) <
      0) {
    perror("connection error");
    close(socket_fd);
    exit(EXIT_FAILURE);
  }
  return socket_fd;
}
int bindPort(int port) {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_fd < 0) {
    perror("socket error");
    close(socket_fd);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  if (inet_aton("127.0.0.1", &serverAddr.sin_addr) == 0) {
    perror("error binding address");
    close(socket_fd);
    exit(EXIT_FAILURE);
  }

  if (bind(socket_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    perror("connection error");
    close(socket_fd);
    exit(EXIT_FAILURE);
  }
  return socket_fd;
}
