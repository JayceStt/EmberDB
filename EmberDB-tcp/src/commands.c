// command logic for parsing user input

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

#include "hashmap.h"
#include "commands.h"
#include "persistance.h"
#include "server.h"

#define BUFFER_SIZE 4096
 
// Helper function prototypes
bool set_key(const char *key, const char *value);
char *get_value(const char *key);
bool delete_key(const char *key);
bool exists_key(const char *key);

int process_command(int client_fd, char *buffer) {
    char *command = strtok(buffer, " \r\n");

    if (command == NULL) {
        write(client_fd, "ERR empty command\n", strlen("ERR empty command\n"));
        return 1;
    }

    if (strcasecmp(command, "PING") == 0) {
        write(client_fd, "PONG\n", strlen("PONG\n"));
        return 1;
    }

    else if (strcasecmp(command, "SET") == 0) {
        char *key = strtok(NULL, " \r\n");
        char *value = strtok(NULL, " \r\n");

        if (key == NULL || value == NULL) {
            write(client_fd, "ERR usage: SET key value\n", strlen("ERR usage: SET key value\n"));
            return 1;
        }

        if (set_key(key, value)) {
            printf("SET command - %s:%s\n", key, value);
            write(client_fd, "OK\n", strlen("OK\n"));
        } else {
            write(client_fd, "ERR set failed\n", strlen("ERR set failed\n"));
        }

        return 1;
    }

    else if (strcasecmp(command, "GET") == 0) {
        char *key = strtok(NULL, " \r\n");

        if (key == NULL) {
            write(client_fd, "ERR usage: GET key\n", strlen("ERR usage: GET key\n"));
            return 1;
        }

        char *value = get_value(key);

        if (value == NULL) {
            write(client_fd, "(nil)\n", strlen("(nil)\n"));
        } else {
            printf("GET command - %s:%s\n", key, value);
            write(client_fd, value, strlen(value));
            write(client_fd, "\n", 1);
        }

        return 1;
    }

    else if (strcasecmp(command, "EXISTS") == 0) {
        char *key = strtok(NULL, " \r\n");


        if (key == NULL) {
            write(client_fd, "ERR usage: EXISTS key\n", strlen("ERR usage: EXISTS key\n"));
            return 1;
        }

        printf("EXISTS command - %s\n", key);

        if (exists_key(key)) {
            write(client_fd, "Exists\n", strlen("Exists\n"));
        } else {
            write(client_fd, "Does not Exist\n", strlen("Does not Exist\n"));
        }

        return 1;
    }

    else if (strcasecmp(command, "END") == 0) {
        write(
            client_fd,
            "ENDING SERVER.\nDATA NOT SAVED WILL BE LOST\n",
            strlen("ENDING SERVER.\nDATA NOT SAVED WILL BE LOST\n")
        );

        printf("END command - Terminating Database\n");
        Hash_Clear();
        write(client_fd, "SERVER CLOSING GOODBYE\n", strlen("SERVER CLOSING GOODBYE\n"));
        end_server();
        return 0;

    }

    else if (strcasecmp(command, "DELETE") == 0 || strcasecmp(command, "DEL") == 0) {
        char *key = strtok(NULL, " \r\n");

        if (key == NULL) {
            write(client_fd, "ERR usage: DELETE key\n", strlen("ERR usage: DELETE key\n"));
            return 1;
        }
        printf("DELETE/DEL command - %s\n", key);

        if (delete_key(key)) {
            write(client_fd, "Key Deleted\n", strlen("Key Deleted\n"));
        } else {
            write(client_fd, "Key Not Deleted\n", strlen("Key Not Deleted\n"));
        }

        return 1;
    }

    else if (strcasecmp(command, "QUIT") == 0) {
        write(client_fd, "BYE\n", strlen("BYE\n"));
        return 0;
    }

    else if (strcasecmp(command, "MAXWIPE") == 0) {
        write(
            client_fd,
            "DELETING CURRENT SERVER DATA\n",
            strlen("DELETING CURRENT SERVER DATA\n")
        );

        printf("MAXWIPE command - Deleting Database\n");
        Hash_Clear();
        write(client_fd, "DATA DELETED\n", strlen("DATA DELETED\n"));
        return 1;
    }

    else if (strcasecmp(command, "SAVE") == 0) {
        write(
            client_fd,
            "CREATING BACKUP DATA WILL DELETE ANY CURRENTLY SAVED BACKUP DATA\n",
            strlen("CREATING BACKUP DATA WILL DELETE ANY CURRENTLY SAVED BACKUP DATA\n")
        );

        if (create_backup()) {
            printf("SAVE command - Backup Created\n");
            write(client_fd, "BACKUP CREATED\n", strlen("BACKUP CREATED\n"));
        }
        else write(client_fd, "BACKUP NOT CREATED(ERROR)\n", strlen("BACKUP NOT CREATED(ERROR)\n"));
        return 1;
    }

    else if (strcasecmp(command, "BACKUPWIPE") == 0) {
        write(
            client_fd,
            "DELETING BACKUP DATA\n",
            strlen("DELETING BACKUP DATA\n")
        );
       
        if(backup_wipe()){
            printf("BACKUPWIPE command - Deleting backup data\n");
            write(client_fd, "BACKUP DATA DELETED\n", strlen("BACKUP DATA DELETED\n"));
        }
        else {
            write(client_fd, "BACKUP DELETE FAILED\n", strlen("BACKUP DELETE FAILED\n"));
        }
        return 1;
    }

    else {
        write(client_fd, "ERR unknown command\n", strlen("ERR unknown command\n"));
        return 1;
    }
}

bool set_key(const char *key, const char *value) {
    return Hash_Set(key, value);
}

char *get_value(const char *key) {
    Node *trophy = Hash_Find(key);

    if (trophy == NULL) {
        return NULL;
    }

    return trophy->value;
}

bool delete_key(const char *key) {
    return Hash_Delete(key);
}

bool exists_key(const char *key) {
    return Hash_Find(key) != NULL;
}