#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

void read_file(const char *filename, int client_fd);
void write_file(const char *filename, const char *content, int client_fd);
#endif // FILE_OPERATIONS_H