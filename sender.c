#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/stat.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <unistd.h>
#define BYTESIZE 1024
#define PORT 8181

int main() {
  int packet_counter = 0;
  struct stat buffer;
  int sending_iterations = 5;
  char data[BYTESIZE] = {0};
  char *ip = "127.0.0.1";
  int port = 8181;
  uint32_t total_filesize;
  struct sockaddr_in server_addr;
  FILE *fp;
  char *filename = "/mnt/c/Users/shmue/Documents/Git/Communication_Ex3/input/1mb.txt";
  stat(filename, &buffer);
  total_filesize = buffer.st_size;
  printf("The chosen filesize is: %d\n", total_filesize);
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);
  int e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(e == -1) {
    perror("[-]Error in socket");
    exit(1);
  }
 printf("[+]Connected to Server.\n");

  fp = fopen(filename, "r");
  if (fp == NULL) {
    perror("[-]Error in reading file.");
    exit(1);
  }
  uint32_t hosttonetwork = htonl(total_filesize);
  printf("The converted htonl filesize is: %d\n", hosttonetwork);
  send(sockfd, &hosttonetwork, sizeof(uint32_t), 0);
  for (int i=0; i<= sending_iterations; i++) {
    while(fgets(data, BYTESIZE, fp) != NULL) {
      if (send(sockfd, data, sizeof(data), 0) == -1) {
        perror("[-]Error in sending file.");
        exit(1);
      }
    packet_counter++;
    bzero(data, BYTESIZE);
    }
    rewind(fp);
    if (fgets(data, BYTESIZE, fp) == NULL) {

    }
  }
  printf("[+]File data sent successfully.\n");
  printf("[+]Closing the connection.\n");
  printf("Packets Sent %d\n",packet_counter);
  close(sockfd);
  return 0;
}