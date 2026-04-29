#include "utilities.c"
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <TIME|CMD>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int socket_fd = establishConnection(PORT);

  char *sendBuffer = malloc(BUF_SIZE);
  char *receiveBuffer = malloc(BUF_SIZE);

  if (!sendBuffer || !receiveBuffer) {
    gracefulExit(socket_fd, "allocation error", sendBuffer, receiveBuffer);
  }

  snprintf(sendBuffer, BUF_SIZE, "Bonjour %s\n", argv[1]);
  if (send(socket_fd, sendBuffer, strlen(sendBuffer), 0) < 0) {
    gracefulExit(socket_fd, "send error", sendBuffer, receiveBuffer);
  }

  if (strcmp(argv[1], "TIME") == 0) {
    int n = 60;
    while (n--) {
      ssize_t received = recv(socket_fd, receiveBuffer, BUF_SIZE - 1, 0);
      if (received <= 0) {
        gracefulExit(socket_fd, "recv error", sendBuffer, receiveBuffer);
      }
      receiveBuffer[received] = '\0';
      printf("received: %s", receiveBuffer);
    }
  } else if (strcmp(argv[1], "CMD") == 0) {
    ssize_t received = recv(socket_fd, receiveBuffer, BUF_SIZE - 1, 0);
    if (received <= 0) {
      gracefulExit(socket_fd, "recv error", sendBuffer, receiveBuffer);
    }
    receiveBuffer[received] = '\0';
    printf("%s\n", receiveBuffer);
  } else {
    fprintf(stderr, "Unknown service: %s\n", argv[1]);
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
