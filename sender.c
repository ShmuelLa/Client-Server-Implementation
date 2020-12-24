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

int send_file(int sender_socket, FILE *file, int total_bytes_sent) {
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
        while(fgets(data, BYTESIZE, file) != NULL) {
            send_status = send(sender_socket, data, sizeof(data), 0);
            total_bytes_sent += send_status;
            if (send_status == -1) {
                perror("!!| Error in sending file.");
                exit(1);
                return -1;
            }
            iteration_counter++;
            bzero(data, BYTESIZE);
            if (fgets(data, BYTESIZE, file) == NULL) {
                sending_count++;
            }
        }
        rewind(file);
    }
    clock_t end = clock();
    operation_time = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("==| %s sending time - %f Seconds\n",cc_type,operation_time);
    printf("==| Number of iterations in %s algorithm = %d\n",cc_type, iteration_counter);
    printf("==| Bytes Sent in %s %d\n",cc_type, total_bytes_sent);
    return sending_count;
}

int main() {
    char cc_type[256];
    socklen_t len = sizeof(cc_type);
    int sending_count = 0;
    int total_bytes_sent = 0;
    char *ip = "127.0.0.1";
    int filesize;
    struct sockaddr_in server_addr;
    FILE *file;
    char *filename = "input/1mb.txt";
    struct stat buffer;
    stat(filename, &buffer);
    filesize = buffer.st_size;
    printf("____________________Sender (Client)____________________\n");
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
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("!!| Error in file reading");
        exit(1);
    }
    sending_count += send_file(sender_socket, file, total_bytes_sent);
    printf("==| File was sent %d times successfully\n",sending_count);
    strcpy(cc_type, "reno"); 
    len = strlen(cc_type);
    if (setsockopt(sender_socket, IPPROTO_TCP, TCP_CONGESTION, cc_type, len) != 0) {
        perror("!!| Set CC to reno Problem"); 
        return -1;
    }
    printf("==| Congestion Control Algorithm changed to %s, Resending files\n", cc_type); 
    total_bytes_sent =0;
    sending_count += send_file(sender_socket, file, total_bytes_sent);
    printf("==| File was sent %d times successfully\n",sending_count);
    close(sender_socket);
    return 0;
}