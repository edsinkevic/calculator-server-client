#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
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

#define CHECK(X) ({int32_t __val=(X); (__val ==-1 ? \
({fprintf(stderr,"ERROR (" __FILE__":%d) -- %s\n",__LINE__,strerror(errno));\
exit(-1);-1;}) : __val); })

#define CHECK_RETURN(X) ({int32_t __val=(X); (__val <= 0 ? \
({if(__val == -1) fprintf(stderr,"ERROR (" __FILE__":%d) -- %s\n",__LINE__,strerror(errno));\
return -1;}) : __val); })

static char CALLBACK_BUFFER[CALLBACK_SIZE];

static char perform_connection(int32_t ls);
static void read_set_clients(int32_t ls, int32_t *fds, fd_set *fdset);
static int32_t find_free_slot(int32_t *fds);
static int32_t handle_read(int32_t fd);
static int32_t handle_connect(int32_t fd);
static int32_t find_maxfd(int32_t *fds, fd_set *fdset);
static void handle_new_connections(int32_t ls, int32_t *fds, fd_set *fdset);
static void read_set_clients(int32_t ls, int32_t *fds, fd_set *fdset);
static void init_empty_clients(int *fds);

int32_t main(int32_t argc, char **argv) {
        int32_t ls = -1;
        struct sockaddr_in saddr;
        int32_t optval = 1;

        saddr = get_address(PORT);
        CHECK(ls = socket(AF_INET, SOCK_STREAM, 0));
        CHECK(setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int32_t)));
        CHECK(bind(ls, (struct sockaddr *)&saddr, sizeof(saddr)));
        CHECK(listen(ls, 0));

        perform_connection(ls);

        close(ls);
        return 0;
}

static int32_t find_free_slot(int32_t fds[]) {
        for (int32_t i = 0; i < MAXCLIENTS; i++)
                if (fds[i] == -1)
                        return i;
        return -1;
}

static void message_callback(const char const *message) {
        sprintf(CALLBACK_BUFFER, "%s%s", CALLBACK_BUFFER, message);
}

static int32_t handle_read(int32_t fd) {
        int32_t res = 0;
        char ibuf[INPUT_SIZE];
        char obuf[OUTPUT_SIZE];

        memset(&ibuf, 0, INPUT_SIZE);
        memset(&obuf, 0, OUTPUT_SIZE);
        memset(CALLBACK_BUFFER, 0, CALLBACK_SIZE);

        CHECK_RETURN(read(fd, &ibuf, INPUT_SIZE));

        if (calculate(ibuf, &res, &message_callback))
                sprintf(obuf, "%s%d\n", CALLBACK_BUFFER, res);
        else
                sprintf(obuf, "FAIL\n");

        CHECK_RETURN(write(fd, obuf, strlen(obuf)));

        return 1;
}

static int32_t handle_connect(int32_t fd) {
        int32_t cs;
        struct sockaddr_in caddr;
        int32_t caddrlen = sizeof(caddr);
        memset(&caddr, 0, caddrlen);
        CHECK_RETURN(cs = accept(fd, (struct sockaddr *)&caddr, &caddrlen));
        print_client_address(caddr);

        return cs;
}

static int32_t find_maxfd(int32_t *fds, fd_set *fdset) {
        int32_t maxfd = -1;
        for (int32_t i = 0; i < MAXCLIENTS; i++)
                if (fds[i] != -1) {
                        FD_SET(fds[i], fdset);
                        if (fds[i] > maxfd)
                                maxfd = fds[i];
                }
        return maxfd;
}

static void handle_new_connections(int32_t ls, int32_t *fds, fd_set *fdset) {
        if (FD_ISSET(ls, fdset)) {
                int32_t id = find_free_slot(fds);
                if (id != -1)
                        fds[id] = handle_connect(ls);
        }
}

static void read_set_clients(int32_t ls, int32_t *fds, fd_set *fdset) {
        for (int32_t i = 0; i < MAXCLIENTS; i++)
                if (FD_ISSET(fds[i], fdset) && handle_read(fds[i]) <= 0) {
                        close(fds[i]);
                        fds[i] = -1;
                }
}

static void init_empty_clients(int *fds) {
        for (int32_t i = 0; i < MAXCLIENTS; i++)
                fds[i] = -1;
}

static char perform_connection(const int32_t ls) {
        int32_t cs[MAXCLIENTS];
        char ibuf[INPUT_SIZE];
        char obuf[OUTPUT_SIZE];
        int32_t addrlen = -1;
        fd_set readfds;
        int32_t maxfd = -1;

        init_empty_clients(cs);

        for (;;) {
                FD_ZERO(&readfds);
                maxfd = find_maxfd(cs, &readfds);
                FD_SET(ls, &readfds);
                if (ls > maxfd)
                        maxfd = ls;
                select(maxfd + 1, &readfds, NULL, NULL, NULL);
                handle_new_connections(ls, cs, &readfds);
                read_set_clients(ls, cs, &readfds);
        }
        return 0;
}