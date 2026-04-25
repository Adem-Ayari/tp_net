#include "utilities.c"
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void) {
  int socket_fd = establishConnection(PORT);

  char *sendBuffer = malloc(BUF_SIZE);
  char *receiveBuffer = malloc(BUF_SIZE);

  if (!sendBuffer || !receiveBuffer) {
    gracefulExit(socket_fd, "allocation error", sendBuffer, receiveBuffer);
  }

  const char *hello = "Bonjour\n";
  if (send(socket_fd, hello, strlen(hello), 0) < 0) {
    gracefulExit(socket_fd, "send error", sendBuffer, receiveBuffer);
  }

  int n = 60;
  while (n--) {
    ssize_t received = recv(socket_fd, receiveBuffer, BUF_SIZE - 1, 0);
    if (received <= 0) {
      gracefulExit(socket_fd, "recv error", sendBuffer, receiveBuffer);
    }
    receiveBuffer[received] = '\0';
    printf("received: %s", receiveBuffer);
  }

  const char *bye = "Au revoir\n";
  if (send(socket_fd, bye, strlen(bye), 0) < 0) {
    gracefulExit(socket_fd, "send error", sendBuffer, receiveBuffer);
  }

  free(sendBuffer);
  free(receiveBuffer);
  close(socket_fd);
  return 0;
}
