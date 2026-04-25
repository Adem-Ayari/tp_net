#ifndef UTILITIES_H
#define UTILITIES_H

#define PORT 8083
#define BACKLOG 10
#define BUF_SIZE (1024 * 1024)

void gracefulExit(int socket_fd, char *message, char *sendBuffer,
                  char *receiveBuffer);
int establishConnection(int port);
int bindPort(int port);

#endif
