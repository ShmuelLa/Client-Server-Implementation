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
#define PORT 9008

int main() {
    int packet_counter = 0;
    int sending_count = 0;
    int send_status = 0;
    int total_bytes_sent = 0;
    char cc_type[256];
    char data[BYTESIZE] = {0};
    char *ip = "127.0.0.1";
    int filesize;
    socklen_t len;
    struct sockaddr_in server_addr;
    FILE *file;
    char *filename = "/mnt/c/Users/shmue/Documents/Git/Communication_Ex3/input/1mb.txt";
    struct stat buffer;
    stat(filename, &buffer);
    filesize = buffer.st_size;
    printf("____________________Sender (Client)____________________\n");
    printf("==| Sending received file size: %d Bytes\n", filesize);
    int sender_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(sender_socket < 0) {
        perror("!!| Error in socket");
        exit(1);
    }
    len = sizeof(cc_type); 
    getsockopt(sender_socket, IPPROTO_TCP, TCP_CONGESTION, cc_type, &len);
    printf("==| Current congestion control algorithm: %s\n", cc_type); 
    printf("==| Server socket created successfully.\n");
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    int e = connect(sender_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(e == -1) {
        perror("!!| Error in socket");
        exit(1);
    }
    printf("==| Connected to Server.\n");
    send(sender_socket, &filesize, sizeof(filesize), 0);
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("!!| Error in reading file.");
        exit(1);
    }
    for(int i=0; i < 5; i++) {
        while(fgets(data, BYTESIZE, file) != NULL) {
            send_status = send(sender_socket, data, sizeof(data), 0);
            total_bytes_sent += BYTESIZE;
            if (send_status == -1) {
                perror("!!| Error in sending file.");
                exit(1);
            }
            packet_counter++;
            bzero(data, BYTESIZE);
            if (fgets(data, BYTESIZE, file) == NULL) {
                sending_count++;
            }
        }
        rewind(file);
    }
    printf("==| File data sent successfully.\n");
    printf("==| Closing the connection.\n");
    printf("==| Packets Sent in cubic %d\n",packet_counter);
    printf("==| Bytes Sent in cubic %d\n",total_bytes_sent);
    printf("==| File was sent %d times successfully\n",sending_count);
    strcpy(cc_type, "reno"); 
    len = strlen(cc_type);
    if (setsockopt(sender_socket, IPPROTO_TCP, TCP_CONGESTION, cc_type, len) != 0) {
        perror("!!| Set CC to reno Problem"); 
        return -1;
    }
    printf("==| Current CC: %s\n", cc_type); 
    packet_counter = 0;
    total_bytes_sent =0;
    for(int i=0; i < 5; i++) {
        while(fgets(data, BYTESIZE, file) != NULL) {
            send_status = send(sender_socket, data, sizeof(data), 0);
            total_bytes_sent += BYTESIZE;
            if (send_status == -1) {
                perror("!!| Error in sending file.");
                exit(1);
            }
            packet_counter++;
            bzero(data, BYTESIZE);
            if (fgets(data, BYTESIZE, file) == NULL) {
                sending_count++;
            }
        }
        rewind(file);
    }
    printf("==| Packets in reno Sent %d\n",packet_counter);
    printf("==| Bytes Sent in reno %d\n",total_bytes_sent);
    printf("==| File was sent %d times successfully\n",sending_count);
    close(sender_socket);
    return 0;
}