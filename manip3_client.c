#include "utilities.c"
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void sendMSG(int socket_fd, char *sendBuffer, char *receiveBuffer) {
  ssize_t readBytes;
  if ((readBytes = read(0, sendBuffer, BUF_SIZE - 1)) < 0) {
    gracefulExit(socket_fd, "error reading from stdin", sendBuffer,
                 receiveBuffer);
  }
  sendBuffer[readBytes] = '\0';

  ssize_t sendBytes = send(socket_fd, sendBuffer, readBytes, 0);

  if (sendBytes < 0) {
    gracefulExit(socket_fd, "send error", sendBuffer, receiveBuffer);
  }
}
void receiveMSG(int socket_fd, char *sendBuffer, char *receiveBuffer) {
  ssize_t received = recv(socket_fd, receiveBuffer, BUF_SIZE - 1, 0);
  if (received > 0) {
    receiveBuffer[received] = '\0';
    printf("received: %s", receiveBuffer);
  } else {
    gracefulExit(socket_fd, "recv error", sendBuffer, receiveBuffer);
  }
}

int main(int argc, char *argv[]) {
  int socket_fd = establishConnection(PORT);

  char *sendBuffer = (char *)malloc(sizeof(char) * BUF_SIZE);
  char *receiveBuffer = (char *)malloc(sizeof(char) * BUF_SIZE);

  if (!sendBuffer || !receiveBuffer) {
    gracefulExit(socket_fd, "allocation error", sendBuffer, receiveBuffer);
  }

  write(1, ">>", 2);

  sendMSG(socket_fd, sendBuffer, receiveBuffer);

  int n = 60;
  while (n--) {
    receiveMSG(socket_fd, sendBuffer, receiveBuffer);
  }

  write(1, ">> ", 3);
  sendMSG(socket_fd, sendBuffer, receiveBuffer);

  free(sendBuffer);
  free(receiveBuffer);
  close(socket_fd);
  return 0;
}
