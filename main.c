#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
// #include <errno.h>

int main(int argc, char *argv[]) {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(8080);
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  connect(socket_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
  char *sendBuffer = (char *)malloc(sizeof(char) * 1024);
  char *receiveBuffer = (char *)malloc(sizeof(char) * 1024 * 1024);
  sendBuffer =
      "GET /echo/hello_world HTTP/1.1\r\nHost: localhost\r\nUser-Agent: "
      "c/1.0.0\r\nAccept: */*\r\n\r\n";
  send(socket_fd, sendBuffer, strlen(sendBuffer), 0);
  recv(socket_fd, receiveBuffer, 1024 * 1024, 0);
  printf("%s", receiveBuffer);
  close(socket_fd);
}
