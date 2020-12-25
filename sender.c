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
#include <time.h>
#define BYTESIZE 1024
#define PORT 9009

int send_file(int sender_socket, char *filename) {
    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("!!| Error in file reading");
        exit(1);
        return -1;
    }    
    struct stat filesize_buffer;
    stat(filename, &filesize_buffer);
    int filesize = filesize_buffer.st_size;
    int for_loop_index = filesize / BYTESIZE;
    int file_remainder = filesize % BYTESIZE;
    int read_validation = 0;
    int total_bytes_sent = 0;
    double operation_time;
    int sending_count = 0;
    char cc_type[256];
    int send_status = 0;
    char data[BYTESIZE] = {0};
    int iteration_counter = 0;
    socklen_t len = sizeof(cc_type); 
    getsockopt(sender_socket, IPPROTO_TCP, TCP_CONGESTION, cc_type, &len);
    printf("==| Current congestion control algorithm: %s\n", cc_type); 
    clock_t begin = clock();
    for(int i=0; i < 5; i++) {
        for (int j=0; j < for_loop_index; j++) {
            read_validation = fread(data, BYTESIZE, 1, file);
            if (read_validation < 0) {
                perror("!!| Error in reading file.");
                exit(1);
                return -1;                
            }
            send_status = send(sender_socket, data, BYTESIZE, 0);
            total_bytes_sent += send_status;
            if (send_status == -1) {
                perror("!!| Error in sending file.");
                exit(1);
                return -1;
            }
            iteration_counter++;         
        }
        if (file_remainder > 0) {
            read_validation = fread(data, file_remainder, 1, file);
            if (read_validation < 0) {
                perror("!!| Error in reading file.");
                exit(1);
                return -1;                
            }            
            send_status = send(sender_socket, data, file_remainder, 0);
            total_bytes_sent += send_status;
            if (send_status == -1) {
                perror("!!| Error in sending file.");
                exit(1);
                return -1;
            }
            iteration_counter++;          
        }
        sending_count++;
        rewind(file);
    }
    clock_t end = clock();
    operation_time = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("==| %s sending time - %f Seconds\n",cc_type,operation_time);
    printf("==| Number of iterations in %s algorithm = %d\n",cc_type, iteration_counter);
    printf("==| Bytes Sent in %s %d\n",cc_type, total_bytes_sent);
    fclose(file);
    return sending_count;
}

int main() {
    clock_t begin = clock();
    double total_sending_time;
    char cc_type[256];
    socklen_t len = sizeof(cc_type);
    int sending_count = 0;
    char *ip = "127.0.0.1";
    struct sockaddr_in server_addr;
    char *filename = "input/1mb.txt";
    struct stat filesize_buffer;
    stat(filename, &filesize_buffer);
    int filesize = filesize_buffer.st_size;
    printf("_____________________________Sender (Client)_____________________________\n");
    //Settings the client socket
    int sender_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(sender_socket < 0) {
        perror("!!| Error in socket");
        exit(1);
    }
    printf("==| Server socket created successfully.\n");
    //Checking CC algorithm
    //Setting client address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    int socket_validation = connect(sender_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(socket_validation == -1) {
        perror("!!| Error in socket");
        exit(1);
    }
    printf("==| Connected to Server.\n");
    printf("==| Sending received file size: %d Bytes\n", filesize);
    send(sender_socket, &filesize, sizeof(filesize), 0);
    sending_count += send_file(sender_socket, filename);
    printf("==| File was sent a total of %d times successfully\n",sending_count);
    strcpy(cc_type, "reno"); 
    len = strlen(cc_type);
    if (setsockopt(sender_socket, IPPROTO_TCP, TCP_CONGESTION, cc_type, len) != 0) {
        perror("!!| Set CC to reno Problem"); 
        return -1;
    }
    printf("==| Congestion Control Algorithm changed to %s, Resending files\n", cc_type); 
    sending_count += send_file(sender_socket, filename);
    printf("==| File was sent a total of %d times successfully\n",sending_count);
    close(sender_socket);
    clock_t end = clock();
    total_sending_time = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("==|____Total sending process time = %f____|\n", total_sending_time);
    return 0;
}