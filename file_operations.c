#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "file_operations.h"
#include <string.h>
#include <sys/socket.h>

void read_file(const char *filename, int client_fd) {
    FILE *file = fopen(filename, "r");
    if (file) {
        // Determine content type based on file extension
        const char *ext = strrchr(filename, '.');
        const char *content_type = "text/plain";
        if (ext) {
            if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) {
                content_type = "text/html";
            } else if (strcmp(ext, ".css") == 0) {
                content_type = "text/css";
            } else if (strcmp(ext, ".js") == 0) {
                content_type = "application/javascript";
            } else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) {
                content_type = "image/jpeg";
            } else if (strcmp(ext, ".png") == 0) {
                content_type = "image/png";
            } else if (strcmp(ext, ".gif") == 0) {
                content_type = "image/gif";
            }
        }

        char header[256];
        int header_len = snprintf(header, sizeof(header),
            "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nConnection: close\r\n\r\n", content_type);
        if (send(client_fd, header, header_len, 0) == -1) {
            perror("send header");
            fclose(file);
            return;
        }

        char buffer[1024];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            if (send(client_fd, buffer, bytes_read, 0) == -1) {
                perror("send file");
                break;
            }
        }
        fclose(file);
    } else {
        if (send(client_fd, "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n", 45, 0) == -1) {
            perror("send 404");
        }
    }
}

void write_file(const char *filename, const char *content, int client_fd) {
    FILE *file = fopen(filename, "a");
    if(file){
        fprintf(file, "%s\n", content);
        fclose(file);
        
    }
}