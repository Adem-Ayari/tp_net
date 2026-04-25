#include "utilities.c"
#include "utilities.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

void sendMSG(int socket_fd, char *sendBuffer, char *receiveBuffer,
             struct sockaddr_in *clientAddr, socklen_t clientLen) {
  time_t t;
  time(&t);
  char *timeStr = ctime(&t);
  strcpy(sendBuffer, timeStr);
  ssize_t len = strlen(sendBuffer);

  ssize_t sendBytes = sendto(socket_fd, sendBuffer, len, 0,
                             (struct sockaddr *)clientAddr, clientLen);
  if (sendBytes < 0) {
    gracefulExit(socket_fd, "sendto error", sendBuffer, receiveBuffer);
  }
}
void receiveMSG(int socket_fd, char *sendBuffer, char *receiveBuffer,
                struct sockaddr_in *clientAddr, socklen_t *clientLen) {
  ssize_t received = recvfrom(socket_fd, receiveBuffer, BUF_SIZE - 1, 0,
                              (struct sockaddr *)clientAddr, clientLen);
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
  serverAddr.sin_port = htons(PORT);
  if (inet_aton("127.0.0.1", &serverAddr.sin_addr) == 0) {
    perror("error binding address");
    close(socket_fd);
    exit(EXIT_FAILURE);
  }

  if (bind(socket_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    perror("bind error");
    close(socket_fd);
    exit(EXIT_FAILURE);
  }

  printf("UDP server listening on port 8082...\n");

  struct sockaddr_in clientAddr;
  socklen_t clientLen = sizeof(clientAddr);

  char *sendBuffer = (char *)malloc(sizeof(char) * BUF_SIZE);
  char *receiveBuffer = (char *)malloc(sizeof(char) * BUF_SIZE);

  if (!sendBuffer || !receiveBuffer) {
    gracefulExit(socket_fd, "allocation error", sendBuffer, receiveBuffer);
  }

  receiveMSG(socket_fd, sendBuffer, receiveBuffer, &clientAddr, &clientLen);

  int n = 60;
  while (n--) {
    sendMSG(socket_fd, sendBuffer, receiveBuffer, &clientAddr, clientLen);
    sleep(1);
  }

  receiveMSG(socket_fd, sendBuffer, receiveBuffer, &clientAddr, &clientLen);

  free(sendBuffer);
  free(receiveBuffer);
  close(socket_fd);
  return 0;
}
