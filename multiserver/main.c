#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../deps/calc.h"
#include "../deps/edutils.h"

#define OUTPUT_SIZE 1024
#define INPUT_SIZE OUTPUT_SIZE
#define CALLBACK_SIZE OUTPUT_SIZE - sizeof(int32_t) - 1
#define PORT 9999
#define MAXCLIENTS 10

typedef int in32_t;

static char perform_connection(int ls);
static void handle_messages(int ls, int *fds, fd_set *fdset);
static int find_free_slot(int *fds);
static int handle_read(int fd);
static int handle_connect(int fd);
static int set_existing_clients(int *fds, fd_set *fdset);
static void handle_new_connections(int ls, int *fds, fd_set *fdset);
static void handle_messages(int ls, int *fds, fd_set *fdset);
static void init_empty_clients(int *fds);

static int CLIENT_SOCKETS[MAXCLIENTS];
static char CALLBACK_BUFFER[CALLBACK_SIZE];

static void sigint_handler(int sig) {
        for (int i = 0; i < MAXCLIENTS; ++i)
                close(CLIENT_SOCKETS[i]);
        exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
        signal(SIGINT, sigint_handler);

        int ls;
        struct sockaddr_in saddr;
        int optval;

        ls = -1;
        optval = 1;
        saddr = get_address(PORT);
        CHECK(ls = socket(AF_INET, SOCK_STREAM, 0));
        CHECK(setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)));
        CHECK(bind(ls, (struct sockaddr *)&saddr, sizeof(saddr)));
        CHECK(listen(ls, 0));
        perform_connection(ls);

        CHECK_PRINT(close(ls));
        return 0;
}

static char perform_connection(const int ls) {
        fd_set readfds;
        int maxfd;

        init_empty_clients(CLIENT_SOCKETS);
        for (;;) {
                FD_ZERO(&readfds);
                maxfd = set_existing_clients(CLIENT_SOCKETS, &readfds);
                FD_SET(ls, &readfds);
                if (ls > maxfd)
                        maxfd = ls;
                CHECK_PRINT(select(maxfd + 1, &readfds, NULL, NULL, NULL));
                handle_new_connections(ls, CLIENT_SOCKETS, &readfds);
                handle_messages(ls, CLIENT_SOCKETS, &readfds);
        }

        return 0;
}

static int find_free_slot(int fds[]) {
        for (int i = 0; i < MAXCLIENTS; i++)
                if (fds[i] == -1)
                        return i;
        return -1;
}

static void message_callback(const char const *message) {
        sprintf(CALLBACK_BUFFER, "%s%s", CALLBACK_BUFFER, message);
}

static int handle_read(int fd) {
        int res = 0;
        char ibuf[INPUT_SIZE];
        char obuf[OUTPUT_SIZE];

        memset(&ibuf, 0, INPUT_SIZE);
        memset(&obuf, 0, OUTPUT_SIZE);
        memset(CALLBACK_BUFFER, 0, CALLBACK_SIZE);

        CHECK_RETURN(read(fd, &ibuf, INPUT_SIZE));

        if (!calculate(ibuf, &res, &message_callback))
                sprintf(obuf, "%s%d\n", CALLBACK_BUFFER, res);
        else
                sprintf(obuf, "FAIL\n");

        CHECK_RETURN(write(fd, obuf, strlen(obuf)));

        return 1;
}

static int handle_connect(int fd) {
        int cs;
        struct sockaddr_in caddr;
        int caddrlen;

        caddrlen = sizeof(caddr);
        memset(&caddr, 0, caddrlen);
        CHECK_RETURN(cs = accept(fd, (struct sockaddr *)&caddr, &caddrlen));
        CHECK_PRINT(print_client_address(caddr));

        return cs;
}

static int set_existing_clients(int *fds, fd_set *fdset) {
        int maxfd = -1;
        for (int i = 0; i < MAXCLIENTS; i++)
                if (fds[i] != -1) {
                        FD_SET(fds[i], fdset);
                        if (fds[i] > maxfd)
                                maxfd = fds[i];
                }
        return maxfd;
}

static void handle_new_connections(int ls, int *fds, fd_set *fdset) {
        if (FD_ISSET(ls, fdset)) {
                int id;

                id = find_free_slot(fds);
                if (id != -1)
                        fds[id] = handle_connect(ls);
        }
}

static void handle_messages(int ls, int *fds, fd_set *fdset) {
        for (int i = 0; i < MAXCLIENTS; i++)
                if (FD_ISSET(fds[i], fdset) && handle_read(fds[i]) <= 0) {
                        CHECK_PRINT(close(fds[i]));
                        fds[i] = -1;
                }
}

static void init_empty_clients(int *fds) {
        for (int i = 0; i < MAXCLIENTS; i++)
                fds[i] = -1;
}
