// server data and logic 

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

#include "server.h"
#include "commands.h"
#include "general.h"

#define PORT 3200
#define BUFFER_SIZE 4096
#define MAX_CLIENTS 8
#define MAX_FDS (MAX_CLIENTS + 1)
#define COMMAND_INST "resources/Commands.txt"

static int server_fd = -1;
static int server_run = 1;

static struct pollfd pfds[MAX_FDS];
static char client_names[MAX_FDS][64];

void Init_Server(void) {
    int opt = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    init_poll_array();

    pfds[0].fd = server_fd;
    pfds[0].events = POLLIN; //Check to see if client is listening 

    printf("Server is listening on port %d\n", PORT);
}

void run_server(void) {
    char buffer[BUFFER_SIZE];

    while (server_run) {
        int ready_count = poll(pfds, MAX_FDS, -1);

        if (ready_count < 0) {
            if (errno == EINTR) {
                continue;
            }

            perror("poll failed");
            break;
        }

        if (pfds[0].revents & POLLIN) {
            accept_new_client();
            ready_count--;
        }

        for (int i = 1; i < MAX_FDS && ready_count > 0; i++) {
            if (pfds[i].fd == -1) {
                continue;
            }

            if (pfds[i].revents == 0) {
                continue;
            }

            if (pfds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                remove_client(i);
                ready_count--;
                continue;
            }

            if (pfds[i].revents & POLLIN) {
                memset(buffer, 0, BUFFER_SIZE);

                ssize_t bytes_read = read(pfds[i].fd, buffer, BUFFER_SIZE - 1);

                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';

                    int keep_client = process_command(pfds[i].fd, buffer);

                    if (!keep_client) {
                        remove_client(i);
                    }

                    if (!server_run) {
                        break;
                    }
                }
                else if (bytes_read == 0) {
                    printf("Client disconnected: %s\n", client_names[i]);
                    remove_client(i);
                }
                else {
                    perror("read failed");
                    remove_client(i);
                }

                ready_count--;
            }
        }
    }

    close_all_clients();

    if (server_fd != -1) {
        close(server_fd);
        server_fd = -1;
    }

    printf("Server closed.\n");
}

void end_server(void) {
    server_run = 0;
}

static void init_poll_array(void) {
    for (int i = 0; i < MAX_FDS; i++) {
        pfds[i].fd = -1;
        pfds[i].events = POLLIN;
        pfds[i].revents = 0;
        client_names[i][0] = '\0';
    }
}

static void remove_client(int index) {
    if (pfds[index].fd != -1) {
        printf("Closing connection with client: %s\n", client_names[index]);

        close(pfds[index].fd);

        pfds[index].fd = -1;
        pfds[index].events = POLLIN;
        pfds[index].revents = 0;
        client_names[index][0] = '\0';
    }
}

static void close_all_clients(void) {
    for (int i = 1; i < MAX_FDS; i++) {
        if (pfds[i].fd != -1) {
            remove_client(i);
        }
    }
}

static bool add_client(int new_client_fd, struct sockaddr_in *client_addr) {
    for (int i = 1; i < MAX_FDS; i++) {
        if (pfds[i].fd == -1) {
            char client_ip[INET_ADDRSTRLEN];

            if (inet_ntop(AF_INET, &(client_addr->sin_addr), client_ip, sizeof(client_ip)) == NULL) {
                strcpy(client_ip, "unknown");
            }

            snprintf(
                client_names[i],
                sizeof(client_names[i]),
                "%s:%d",
                client_ip,
                ntohs(client_addr->sin_port)
            );

            pfds[i].fd = new_client_fd;
            pfds[i].events = POLLIN;
            pfds[i].revents = 0;

            printf("Connection established with client: %s\n", client_names[i]);

            read_print_client(COMMAND_INST, new_client_fd);
            write(new_client_fd, "\n", 1);

            return true;
        }
    }

    write(new_client_fd, "ERR server full\n", strlen("ERR server full\n"));
    close(new_client_fd);

    return false;
}

static void accept_new_client(void) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int new_client_fd = accept(
        server_fd,
        (struct sockaddr *)&client_addr,
        &addr_len
    );

    if (new_client_fd < 0) {
        perror("Accept failed");
        return;
    }

    add_client(new_client_fd, &client_addr);
}