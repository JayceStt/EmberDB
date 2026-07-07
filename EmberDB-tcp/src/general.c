//General functions for simplifying use cases 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>


bool read_print(char filename[]){
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        return false;
    }
    char line[1024];

    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }

    fclose(file);
    return true; 
}

bool read_print_client(const char filename[], int client_fd) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        return false;
    }

    char line[1024];

    while (fgets(line, sizeof(line), file) != NULL) {
        write(client_fd, line, strlen(line));
    }

    fclose(file);
    return true; 
}

