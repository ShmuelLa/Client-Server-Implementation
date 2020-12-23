#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h> 
#include <unistd.h>
#define BYTESIZE 1024
#define PORT 9008

int main() {
    int receive_count = 0;
    int file_size;
    int package_counter = 0;
    int bytes_received_count =0;
    int receive_status;
    char cc_type[256];
    FILE *fp;
    char *filename = "/mnt/c/Users/shmue/Documents/Git/Communication_Ex3/output/1mb.txt";
    fp = fopen(filename, "a");
    char buffer[BYTESIZE];
    char *ip = "127.0.0.1";
    int bind_check;
    socklen_t len;
    int sockfd, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    len = sizeof(cc_type); 
    getsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, cc_type, &len);
    if(sockfd < 0) {
        perror("!!| Error in socket");
        exit(1);
    }
    printf("____________________Measure (Server)____________________\n");
    printf("==| Current Server Measure CC: %s\n", cc_type); 
    printf("==| Server socket created successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    bind_check = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(bind_check < 0) {
        perror("!!| Error in bind");
        exit(1);
    }
    printf("==| Binding successfull.\n");
    if (listen(sockfd, 10) == 0) {
        printf("==| Started listening\n");
    } else {
        perror("!!| Error in listening");
        exit(1);
    }
    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
    getsockopt(new_sock, IPPROTO_TCP, TCP_CONGESTION, cc_type, &len);
    if(sockfd < 0) {
        perror("!!| Error in socket");
        exit(1);
    }
    printf("==| Current client accepting socket CC: %s\n", cc_type); 
    recv(new_sock, &file_size, sizeof(file_size), 0);
    printf("==| Received filesize: %d\n",file_size);
    int for_loop_index = file_size / BYTESIZE;
    int file_remainder = file_size % BYTESIZE;
    // 5000
    // 1024
    // 5000 / 1024 = 4  - four time recv (1024)
    // 5000 % 1024 = x - one time recv (x)
    for (int j=0; j < 5; j++) {
        for (int i=0; i < for_loop_index; i++) {
            receive_status = recv(new_sock, buffer, BYTESIZE, 0);
            bytes_received_count += receive_status;
            package_counter++;
            fprintf(fp, "%s", buffer);
            bzero(buffer, BYTESIZE);
            if (receive_status < 0) {
                perror("!!| Error in writing file");
                exit(1);
                break;
                return 0;
            }
        }
        if (file_remainder > 0) {
            receive_status = recv(new_sock, buffer, file_remainder, 0);
            bytes_received_count += file_remainder;
            package_counter++;
            fprintf(fp, "%s", buffer);
            bzero(buffer, file_remainder);
            if (receive_status < 0) {
                perror("!!| Error in writing file");
                exit(1);
                return 0;
            }
        }
    }

    bytes_received_count = 0;
    printf("==| File was received %d times\n", receive_count);
    printf("==| Packets recivied in cubic %d\n", package_counter);
    printf("==| Data written in the file successfully.\n");
    fp = fopen(filename, "a");






    package_counter = 0;
    strcpy(cc_type, "reno"); 
    len = strlen(cc_type);
    if (setsockopt(new_sock, IPPROTO_TCP, TCP_CONGESTION, cc_type, len) != 0) {
        perror("==| Set CC to reno Problem"); 
        return -1;
    }
    printf("==| Current changed CC: %s\n", cc_type);
    while (receive_count != 10)
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
            perror("!!| Error in writing file");
            exit(1);
            break;
            return 0;
        }
    }
    printf("==| File was received %d times\n", receive_count);
    printf("==| Packets recivied in reno %d\n", package_counter);
    fclose(fp);
    close(sockfd);
    return 0;
}
 
        
