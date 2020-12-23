#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h> 
#define BYTESIZE 1024
#define PORT 8181

int main() {
    int receive_count = 0;
    int package_counter = 0;
    uint32_t received_file_size;
    int receive_status;
    char cc_type[256];
    FILE *fp;
    char *filename = "/mnt/c/Users/shmue/Documents/Git/Communication_Ex3/output/1mb.txt";
    fp = fopen(filename, "a");
    char buffer[BYTESIZE];
    char *ip = "127.0.0.1";
    int port = 8181;
    int e;
    socklen_t len;

    int sockfd, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    //char buffer[BYTESIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    len = sizeof(cc_type); 
    getsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, cc_type, &len);
    if(sockfd < 0) {
        perror("[-]Error in socket");
        exit(1);
    }
    printf("Current CC: %s\n", cc_type); 
    printf("[+]Server socket created successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(e < 0) {
        perror("[-]Error in bind");
        exit(1);
    }
    printf("[+]Binding successfull.\n");
    if (listen(sockfd, 10) == 0) {
        printf("[+]Listening....\n");
    } else {
        perror("[-]Error in listening");
        exit(1);
    }
    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
    recv(sockfd, (void *)&received_file_size, sizeof(uint32_t), 0);
    printf("Received Filesize = %d\n", received_file_size);
    uint32_t temp = ntohl(received_file_size);
    printf("Received Filesize = %d\n", temp);
    while (1)
    {
        receive_status = recv(new_sock, buffer, BYTESIZE, 0);
        fprintf(fp, "%s", buffer);
        bzero(buffer, BYTESIZE);
        if (receive_status == 0) {
            receive_count++;
        }
        else {
            package_counter++;
        }
        if (receive_status < 0) {
            perror("[-]Error in writing file");
            exit(1);
            break;
            return 0;
        }
        if (receive_count == 5) {
            break;
        }
    }
    printf("File receive time count = %d\n", receive_count);
    printf("Packets recivied = %d\n", package_counter);
    printf("[+]Data written in the file successfully.\n");
    strcpy(buf, "reno"); 
    len = strlen(buf);
    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0) {
        perror("Set CC to reno Problem"); 
        return -1;
    }
    return 0;
}
 
        
