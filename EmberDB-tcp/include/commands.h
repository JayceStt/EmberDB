#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdbool.h>

int process_command(int client_fd, char *buffer);

bool set_key(const char *key, const char *value);
char *get_value(const char *key);
bool delete_key(const char *key);
bool exists_key(const char *key);

#endif
