#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include "file_operations.h"
#include "requests.h"


void parse_request(int client_fd) {
    
    char buffer[1024];
    char *token;
    int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    buffer[bytes_received] = '\0'; // Null-terminate the string

    if (strstr(buffer, "GET") && strstr(buffer, "HTTP/1.1")) {
        token = strtok(buffer, " "); // "GET"
        token = strtok(NULL, " ");   // path
        char filepath[256];
        if (token != NULL) {
            if (strcmp(token, "/") == 0) {
                snprintf(filepath, sizeof(filepath), "www/index.html");
                read_file(filepath, client_fd);
            } else if (token[0] == '/') {
                snprintf(filepath, sizeof(filepath), "www/%s", token + 1); // skip leading '/'
                read_file(filepath, client_fd);
            } else {
                send(client_fd, "HTTP/1.1 400 Bad Request\r\n\r\n", 30, 0);
            }
            printf("Handled GET request successfully.\n");
        } else {
            send(client_fd, "HTTP/1.1 400 Bad Request\r\n\r\n", 30, 0);
        }
    } else if(strstr(buffer, "POST") && strstr(buffer, "HTTP/1.1")) {
        // Handle POST request (not implemented in this example)

        char *body_start = strstr(buffer, "\r\n\r\n");
        if (body_start != NULL) {
            int body_length = bytes_received - (body_start - buffer) - 4; // 4 for "\r\n\r\n"
            write_file("www/post_data.txt", body_start + 4, client_fd);
            send(client_fd, "HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\n\r\n", 47, 0);
        } else {
            send(client_fd, "HTTP/1.1 400 Bad Request\r\n\r\n", 30, 0);
        }
        
    } else {
        send(client_fd, "HTTP/1.1 400 Bad Request\r\n\r\n", 30, 0);
    }
    
}


