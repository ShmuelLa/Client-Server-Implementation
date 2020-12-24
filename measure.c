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
#include <time.h>
#define BYTESIZE 1024
#define PORT 9009

int receive_file(int socket, FILE *file, int file_size, char cc_type[256]) {
    double operation_time;
    int receive_count = 0;
    int buffer_validation = 0;
    int iteration_counter = 0;
    char buffer[BYTESIZE];
    int receive_status;
    int for_loop_index = file_size / BYTESIZE;
    int file_remainder = file_size % BYTESIZE;
    clock_t begin = clock();
    for (int j=0; j < 5; j++) {
        for (int i=0; i < for_loop_index; i++) {
            receive_status = recv(socket, buffer, BYTESIZE, 0);
            buffer_validation += receive_status;
            iteration_counter++;
            fprintf(file, "%s", buffer);
            bzero(buffer, BYTESIZE);
            if (receive_status < 0) {
                perror("!!| Error in writing file");
                exit(1);
                break;
                return 0;
            }
        }
        if (file_remainder > 0) {
            receive_status = recv(socket, buffer, file_remainder, 0);
            buffer_validation += file_remainder;
            iteration_counter++;
            fprintf(file, "%s", buffer);
            bzero(buffer, file_remainder);
            if (receive_status < 0) {
                perror("!!| Error in writing file");
                exit(1);
                return 0;
            }
        }
        receive_count++;
    }
    clock_t end = clock();
    operation_time = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("==| %s receiving time - %f Seconds\n",cc_type ,operation_time);
    printf("==| Number of iterations in %s algorithm - %d\n",cc_type , iteration_counter);
    printf("==| Bytes received in %s algorithm - %d\n",cc_type , buffer_validation);
    return receive_count;
}

int main() {
    int receive_count = 0;
    int file_size;
    int iteration_counter = 0;
    int receive_status;
    char cc_type[256];
    FILE *file;
    char *filename = "output/1mb.txt";
    file = fopen(filename, "w");
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
    //Getting the servers listening port number for later sniffing
    struct sockaddr_in  local_address;
    socklen_t socket_addr_size = sizeof(local_address);
    getsockname(sockfd, (struct sockaddr *)&local_address, &socket_addr_size);
    printf("==| Current Server Measure Listening Port: %d\n", ntohs(local_address.sin_port));


    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
    getsockopt(new_sock, IPPROTO_TCP, TCP_CONGESTION, cc_type, &len);
    if(sockfd < 0) {
        perror("!!| Error in socket");
        exit(1);
    }
    printf("==| Current client accepting CC: %s\n", cc_type); 
    recv(new_sock, &file_size, sizeof(file_size), 0);
    printf("==| Received filesize: %d\n",file_size);


    receive_count += receive_file(new_sock, file, file_size, cc_type);



    printf("==| File was received %d times\n", receive_count);
    printf("==| Data written in the file successfully.\n");
    //file = fopen(filename, "a");
    iteration_counter = 0;
    strcpy(cc_type, "reno"); 
    len = strlen(cc_type);
    if (setsockopt(new_sock, IPPROTO_TCP, TCP_CONGESTION, cc_type, len) != 0) {
        perror("==| Problem changing the CC to reno"); 
        return -1;
    }
    printf("==| Current changed CC: %s\n", cc_type);

    //receive_count += receive_file(new_sock, file, file_size, cc_type);
    
    int testbytes = 0;


    while (receive_count != 10)
    {
        receive_status = recv(new_sock, buffer, BYTESIZE, 0);
        testbytes += receive_status;
        fprintf(file, "%s", buffer);
        bzero(buffer, BYTESIZE);
        if (receive_status == 0) {
            receive_count++;
        }
        else {
            iteration_counter++;
        }
        if (receive_status < 0) {
            perror("!!| Error in writing file");
            exit(1);
            break;
            return 0;
        }
    }


    printf("==| File was received %d times\n", receive_count);
    printf("==| Packets recivied in reno %d\n", iteration_counter);
    printf("==| Bytes recivied in reno %d\n", testbytes);
    fclose(file);
    close(sockfd);
    return 0;
}
 