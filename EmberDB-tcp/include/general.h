#ifndef GENERAL_H
#define GENERAL_H

#include <stdbool.h> 

#define NAME_FILE "resources/Name.txt" 

bool read_print(char filename[]); 
bool read_print_client(char filename[], int client_fd); 

#endif 