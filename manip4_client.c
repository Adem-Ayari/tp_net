#include "utilities.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void sendMSG(int socket_fd, char *sendBuffer, char *receiveBuffer,
             struct sockaddr_in *serverAddr, socklen_t serverLen) {
  ssize_t readBytes = read(0, sendBuffer, 1024 * 1024 - 1);
  if (readBytes < 0) {
    gracefulExit(socket_fd, "error reading from stdin", sendBuffer,
                 receiveBuffer);
  }
  sendBuffer[readBytes] = '\0';

  ssize_t sendBytes = send(socket_fd, sendBuffer, readBytes, 0);
  if (sendBytes < 0) {
    gracefulExit(socket_fd, "sendto error", sendBuffer, receiveBuffer);
  }
}

void receiveMSG(int socket_fd, char *sendBuffer, char *receiveBuffer) {
  ssize_t received =
      recvfrom(socket_fd, receiveBuffer, 1024 * 1024 - 1, 0, NULL, NULL);
  if (received > 0) {
    receiveBuffer[received] = '\0';
    printf("received: %s\n", receiveBuffer);
  } else {
    gracefulExit(socket_fd, "recvfrom error", sendBuffer, receiveBuffer);
  }
}

int main(int argc, char *argv[]) {
  int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (socket_fd < 0) {
    perror("socket error");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(8082);
  if (inet_aton("127.0.0.1", &serverAddr.sin_addr) == 0) {
    perror("invalid address");
    close(socket_fd);
    exit(EXIT_FAILURE);
  }
  socklen_t serverLen = sizeof(serverAddr);

  char *sendBuffer = (char *)malloc(sizeof(char) * 1024 * 1024);
  char *receiveBuffer = (char *)malloc(sizeof(char) * 1024 * 1024);

  if (!sendBuffer || !receiveBuffer) {
    gracefulExit(socket_fd, "allocation error", sendBuffer, receiveBuffer);
  }

  printf("write a message\n");

  write(1, ">> ", 3);
  sendMSG(socket_fd, sendBuffer, receiveBuffer, &serverAddr, serverLen);

  int n = 60;
  while (n--) {
    receiveMSG(socket_fd, sendBuffer, receiveBuffer);
  }

  write(1, ">> ", 3);
  sendMSG(socket_fd, sendBuffer, receiveBuffer, &serverAddr, serverLen);

  free(sendBuffer);
  free(receiveBuffer);
  close(socket_fd);
  return 0;
}
