#ifndef UTILITIES_H
#define UTILITIES_H

void gracefulExit(int socket_fd, char *message, char *sendBuffer,
                  char *receiveBuffer);
int establishConnection(int port);
int bindPort(int port);

#endif
