#include "utilities.c"
#include "utilities.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define PORT 8083
#define BACKLOG 10
#define BUF_SIZE (1024 * 1024)

void handleClient(int client_fd) {
  char *sendBuffer = malloc(BUF_SIZE);
  char *receiveBuffer = malloc(BUF_SIZE);

  if (!sendBuffer || !receiveBuffer) {
    gracefulExit(client_fd, "allocation error", sendBuffer, receiveBuffer);
  }

  ssize_t received = recv(client_fd, receiveBuffer, BUF_SIZE - 1, 0);
  if (received <= 0) {
    gracefulExit(client_fd, "recv error", sendBuffer, receiveBuffer);
  }
  receiveBuffer[received] = '\0';
  printf("[PID %d] received: %s\n", getpid(), receiveBuffer);

  int n = 60;
  while (n--) {
    time_t t;
    time(&t);
    char *timeStr = ctime(&t);
    strcpy(sendBuffer, timeStr);
    ssize_t len = strlen(sendBuffer);

    if (send(client_fd, sendBuffer, len, 0) < 0) {
      gracefulExit(client_fd, "send error", sendBuffer, receiveBuffer);
    }
    sleep(1);
  }

  received = recv(client_fd, receiveBuffer, BUF_SIZE - 1, 0);
  if (received <= 0) {
    gracefulExit(client_fd, "recv error", sendBuffer, receiveBuffer);
  }
  receiveBuffer[received] = '\0';
  printf("[PID %d] received: %s\n", getpid(), receiveBuffer);

  free(sendBuffer);
  free(receiveBuffer);
  close(client_fd);
}

int main(void) {
  int socket_fd = bindPort(PORT);

  if (listen(socket_fd, BACKLOG) < 0) {
    perror("listen error");
    close(socket_fd);
    exit(EXIT_FAILURE);
  }

  printf("Concurrent TCP server listening on port %d...\n", PORT);

  struct sigaction sa;
  sa.sa_handler = SIG_DFL;
  sa.sa_flags = SA_NOCLDWAIT;
  sigaction(SIGCHLD, &sa, NULL);

  while (1) {
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    int client_fd =
        accept(socket_fd, (struct sockaddr *)&clientAddr, &clientLen);
    if (client_fd < 0) {
      perror("accept error");
      continue;
    }

    printf("New client connected. Forking...\n");

    pid_t pid = fork();
    if (pid < 0) {
      perror("fork error");
      close(client_fd);
    } else if (pid == 0) {
      close(socket_fd);
      handleClient(client_fd);
      exit(EXIT_SUCCESS);
    } else {
      close(client_fd);
    }
  }

  close(socket_fd);
  return 0;
}
