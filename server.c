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
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>

#define BACKLOG_SIZE 10

void parse_request(int client_fd);
void read_file(const char *filename, int client_fd);

int main(int argc, char *argv[]) {
    //check if dummy directory exists 
    
    DIR *dir = opendir("dummy");
    if (dir) {

        closedir(dir);
    } else if (ENOENT == errno) {
        fprintf(stderr, "Directory 'dummy' does not exist. Please create it.\n");
        return 1;
    } 


    //Socket setup and request handling
    struct addrinfo hints, *res;
    struct sockaddr_in their_addr;
    socklen_t addr_size;
    int sockfd, new_fd;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port> <document_root>\n", argv[0]);
        return 1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    getaddrinfo(NULL, argv[1], &hints, &res);

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    
    if (sockfd == -1) {
        perror("socket \n");
        freeaddrinfo(res);
        return 1;
    } 

    bind(sockfd, res->ai_addr, res->ai_addrlen);

    listen(sockfd, BACKLOG_SIZE);

    if(listen(sockfd, BACKLOG_SIZE) == -1) {
        perror("listen \n");
        close(sockfd);
        freeaddrinfo(res);
        return 1;
    }

    //accept incoming connections
    while(1){

        addr_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);

        printf("Accepted connection from %s\n", inet_ntoa(their_addr.sin_addr));

        if(new_fd == -1) {
            perror("accept \n");
            continue; // Continue to accept next connection
        }

        inet_ntop(their_addr.sin_family, &their_addr.sin_addr, 
                  (char *)&their_addr.sin_port, sizeof(their_addr.sin_port));
        
        parse_request(new_fd);

        

        close(new_fd);  
    }
    freeaddrinfo(res);
    return 0;
}

void parse_request(int client_fd) {
    
    char buffer[1024];
    char *token;
    int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (strstr(buffer, "GET") && strstr(buffer, "HTTP/1.0")) {
        token = strtok(buffer, " "); // "GET"
        token = strtok(NULL, " ");   // path
        char filepath[256];
        if (token != NULL) {
            if (strcmp(token, "/") == 0) {
                snprintf(filepath, sizeof(filepath), "dummy/index.html");
                read_file(filepath, client_fd);
            } else if (token[0] == '/') {
                snprintf(filepath, sizeof(filepath), "dummy/%s", token + 1); // skip leading '/'
                read_file(filepath, client_fd);
            } else {
                send(client_fd, "HTTP/1.0 400 Bad Request\r\n\r\n", 30, 0);
            }
            printf("Handled GET request successfully.\n");
        } else {
            send(client_fd, "HTTP/1.0 400 Bad Request\r\n\r\n", 30, 0);
        }
    } else {
        send(client_fd, "HTTP/1.0 400 Bad Request\r\n\r\n", 30, 0);
    }
    
}

void read_file(const char *filename, int client_fd) {
    FILE *file = fopen(filename, "r");
    if (file) {
        send(client_fd, "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n", 45, 0);
        char buffer[1024];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            send(client_fd, buffer, bytes_read, 0);
        }
        fclose(file);
    } else {
        send(client_fd, "HTTP/1.0 404 Not Found\r\n\r\n", 26, 0);
    }
}