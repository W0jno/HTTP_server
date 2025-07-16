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
#include "requests.h"
#include "file_operations.h"

#define BACKLOG_SIZE 10


int main(int argc, char *argv[]) {
    //check if www directory exists 
    
    DIR *dir = opendir("www");
    if (dir) {

        closedir(dir);
    } else if (ENOENT == errno) {
        fprintf(stderr, "Directory 'www' does not exist. Please create it.\n");
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

    if (getaddrinfo(NULL, argv[1], &hints, &res) != 0) {
        perror("getaddrinfo");
        return 1;
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    
    if (sockfd == -1) {
        perror("socket \n");
        freeaddrinfo(res);
        return 1;
    } 

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind \n");
        close(sockfd);
        freeaddrinfo(res);
        return 1;
    }

   //listen(sockfd, BACKLOG_SIZE);

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
        
        parse_get_request(new_fd);

        close(new_fd);  
    }
    freeaddrinfo(res);
    return 0;
}


