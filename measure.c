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
    double total_operation_time;
    double singular_operation_time;
    int receive_count = 0;
    int buffer_validation = 0;
    int iteration_counter = 0;
    char buffer[BYTESIZE];
    int receive_status;
    int for_loop_index = file_size / BYTESIZE;
    int file_remainder = file_size % BYTESIZE;
    for (int j=0; j < 5; j++) {
        clock_t begin = clock();
        for (int i=0; i < for_loop_index; i++) {
            receive_status = recv(socket, buffer, BYTESIZE, 0);
            buffer_validation += receive_status;
            iteration_counter++;
            fwrite(buffer, 1, BYTESIZE, file);
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
        clock_t end = clock();
        singular_operation_time = (double)(end - begin) / CLOCKS_PER_SEC;
        total_operation_time += singular_operation_time;
        printf("==| File No' %d receiving time int %s = %f Seconds\n",j+1,cc_type ,singular_operation_time);
    }
    total_operation_time = total_operation_time/5;
    printf("==| %s average receiving time - %f Seconds\n",cc_type ,total_operation_time);
    printf("==| Number of iterations in %s algorithm - %d\n",cc_type , iteration_counter);
    printf("==| Bytes received in %s algorithm - %d\n",cc_type , buffer_validation);
    return receive_count;
}

int main() {
    int receive_count = 0;;
    int file_size;
    char cc_type[256];
    FILE *file;
    char *filename = "output/1mb.txt";
    file = fopen(filename, "w");
    char *ip = "127.0.0.1";
    int bind_check;
    socklen_t len;
    int measure_socket, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    measure_socket = socket(AF_INET, SOCK_STREAM, 0);
    len = sizeof(cc_type); 
    getsockopt(measure_socket, IPPROTO_TCP, TCP_CONGESTION, cc_type, &len);
    if(measure_socket < 0) {
        perror("!!| Error in socket");
        exit(1);
    }
    printf("_____________________________Measure (Server)_____________________________\n");
    printf("==| Current Server Measure Congestion Control algorithm: %s\n", cc_type); 
    printf("==| Server socket created successfully.\n");
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    bind_check = bind(measure_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(bind_check < 0) {
        perror("!!| Error in bind");
        exit(1);
    }
    printf("==| Binding successfull.\n");
    if (listen(measure_socket, 10) == 0) {
        printf("==| Started listening\n");
    } else {
        perror("!!| Error in listening");
        exit(1);
    }
    //Getting the servers listening port number for later sniffing
    struct sockaddr_in  local_address;
    socklen_t socket_addr_size = sizeof(local_address);
    getsockname(measure_socket, (struct sockaddr *)&local_address, &socket_addr_size);
    printf("==| Current Server Measure Listening Port: %d\n", ntohs(local_address.sin_port));


    addr_size = sizeof(new_addr);
    new_sock = accept(measure_socket, (struct sockaddr*)&new_addr, &addr_size);
    getsockopt(new_sock, IPPROTO_TCP, TCP_CONGESTION, cc_type, &len);
    if(measure_socket < 0) {
        perror("!!| Error in socket");
        exit(1);
    }
    printf("==| Current client accepting socket Congestion Control algorithm: %s\n", cc_type); 
    recv(new_sock, &file_size, sizeof(file_size), 0);
    printf("==| Received filesize: %d\n",file_size);

    //Receive in cubic
    receive_count += receive_file(new_sock, file, file_size, cc_type);


    printf("==| File was received %d times\n", receive_count);
    printf("==| Data written in the file successfully.\n");
    //file = fopen(filename, "a");
    strcpy(cc_type, "reno"); 
    len = strlen(cc_type);
    if (setsockopt(new_sock, IPPROTO_TCP, TCP_CONGESTION, cc_type, len) != 0) {
        perror("==| Problem changing the Congestion Control to reno"); 
        return -1;
    }
    printf("==| Congestion Control algorithm changed to: %s\n", cc_type);
    //Receive again in reno
    receive_count += receive_file(new_sock, file, file_size, cc_type);

    printf("==| File was received %d times\n", receive_count);
    fclose(file);
    close(measure_socket);
    return 0;
}
 