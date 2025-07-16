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
#include "requests.h"

void parse_get_request(int client_fd) {
    
    char buffer[1024];
    char *token;
    int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

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
    } else {
        send(client_fd, "HTTP/1.1 400 Bad Request\r\n\r\n", 30, 0);
    }
    
}