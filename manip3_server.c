#include "utilities.c"
#include "utilities.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

void sendMSG(int socket_fd, char *sendBuffer, char *receiveBuffer) {
  time_t t;
  time(&t);
  char *temp = ctime(&t);
  strcpy(sendBuffer, temp);
  ssize_t sendBytes = send(socket_fd, sendBuffer, strlen(sendBuffer), 0);

  if (sendBytes < 0) {
    gracefulExit(socket_fd, "send error", sendBuffer, receiveBuffer);
  }
}
void receiveMSG(int socket_fd, char *sendBuffer, char *receiveBuffer) {
  ssize_t received = recv(socket_fd, receiveBuffer, 1024 * 1024 - 1, 0);
  if (received > 0) {
    receiveBuffer[received] = '\0';
    printf("received: %s", receiveBuffer);
  } else {
    gracefulExit(socket_fd, "recv error", sendBuffer, receiveBuffer);
  }
}

int main(int argc, char *argv[]) {
  int server_fd = bindPort(8081);

  if (listen(server_fd, 5) < 0) {
    perror("listen error");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in clientAddr;
  socklen_t clientLen = sizeof(clientAddr);
  int client_fd = accept(server_fd, (struct sockaddr *)&clientAddr, &clientLen);
  if (client_fd < 0) {
    perror("accept error");
    close(server_fd);
    close(client_fd);
    exit(EXIT_FAILURE);
  }

  printf("Client connected.\n");

  char *sendBuffer = (char *)malloc(sizeof(char) * 1024 * 1024);
  char *receiveBuffer = (char *)malloc(sizeof(char) * 1024 * 1024);

  if (!sendBuffer || !receiveBuffer) {
    gracefulExit(server_fd, "allocation error", sendBuffer, receiveBuffer);
  }

  printf("write a message\n");

  receiveMSG(server_fd, sendBuffer, receiveBuffer);

  int n = 60;
  while (n--) {
    sendMSG(server_fd, sendBuffer, receiveBuffer);
    sleep(1);
  }
  receiveMSG(server_fd, sendBuffer, receiveBuffer);

  free(sendBuffer);
  free(receiveBuffer);
  close(server_fd);
  return 0;
}
