#include "utilities.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int socket_fd = establishConnection(80);

  char *sendBuffer = (char *)malloc(sizeof(char) * 1024);
  char *receiveBuffer = (char *)malloc(sizeof(char) * 1024 * 1024);

  if (!sendBuffer || !receiveBuffer) {
    gracefulExit(socket_fd, "allocation error", sendBuffer, receiveBuffer);
  }

  printf("Enter a new request\n");

  ssize_t readBytes;
  if ((readBytes = read(0, sendBuffer, 1023)) < 0) {
    gracefulExit(socket_fd, "error reading from stdin", sendBuffer,
                 receiveBuffer);
  }
  sendBuffer[readBytes] = '\0';
  printf("%s", sendBuffer);

  ssize_t sendBytes = send(socket_fd, sendBuffer, readBytes, 0);

  if (sendBytes < 0) {
    gracefulExit(socket_fd, "send error", sendBuffer, receiveBuffer);
  }

  ssize_t received = recv(socket_fd, receiveBuffer, 1024 * 1024 - 1, 0);
  if (received > 0) {
    receiveBuffer[received] = '\0';
    printf("received: %s", receiveBuffer);
  } else {
    gracefulExit(socket_fd, "recv error", sendBuffer, receiveBuffer);
  }

  free(sendBuffer);
  free(receiveBuffer);
  close(socket_fd);
  return 0;
}
