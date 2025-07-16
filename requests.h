#ifndef REQUESTS_H
#define REQUESTS_H
void parse_request(int client_fd);
void read_file(const char *filename, int client_fd);

#endif   