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
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../deps/calc.h"
#include "../deps/edutils.h"

typedef int int32_t;

#define OUTPUT_SIZE 1024
#define INPUT_SIZE OUTPUT_SIZE
#define CALLBACK_SIZE OUTPUT_SIZE - sizeof(int) - 1
#define PORT 9999
#define MAXCLIENTS 10

static int perform_connection(int ls, int *cs);
static void handle_messages(int ls, int *fds, fd_set *fdset);
static int find_free_slot(int *fds);
static int handle_read(int fd);
static int handle_connect(int fd);
static int set_existing_clients(int *fds, fd_set *fdset);
static void handle_new_connections(int ls, int *fds, fd_set *fdset);
static void handle_messages(int ls, int *fds, fd_set *fdset);
static void init_empty_clients(int *fds);
static void close_clients(int fds[]);

static char CALLBACK_BUFFER[CALLBACK_SIZE];

int main(int argc, char **argv) {
        int ls;
        int cs[MAXCLIENTS];
        struct sockaddr_in saddr;
        int optval;

        ls = -1;
        optval = 1;
        saddr = get_address(PORT);
        CHECK(ls = socket(AF_INET, SOCK_STREAM, 0));
        CHECK(setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)));
        CHECK(bind(ls, (struct sockaddr *)&saddr, sizeof(saddr)));
        CHECK(listen(ls, 0));
        CHECK_PRINT(perform_connection(ls, cs));

        CHECK(close(ls));
        close_clients(cs);
        return EXIT_SUCCESS;
}

static int perform_connection(const int ls, int *cs) {
        fd_set readfds;
        int maxfd;
        struct timeval time;

        time.tv_usec = 0;
        time.tv_sec = 0;

        init_empty_clients(cs);
        for (;;) {
                FD_ZERO(&readfds);
                maxfd = set_existing_clients(cs, &readfds);
                FD_SET(ls, &readfds);
                if (ls > maxfd)
                        maxfd = ls;
                CHECK_RETURN(select(maxfd + 1, &readfds, NULL, NULL, NULL));
                handle_new_connections(ls, cs, &readfds);
                handle_messages(ls, cs, &readfds);
        }

        return EXIT_SUCCESS;
}

static void close_clients(int fds[]) {
        for (int i = 0; i < MAXCLIENTS; i++)
                if (fds[i] != -1)
                        CHECK_PRINT(close(fds[i]));
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
