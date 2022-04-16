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
#define INPUT_SIZE 1024
#define CALLBACK_SIZE OUTPUT_SIZE - sizeof(int32_t) - 1
#define QUEUE_SIZE 0
#define PORT 9999

static char CALLBACK_BUFFER[CALLBACK_SIZE];

static void message_callback(const char const *message);
static char perform_connection(int32_t listen_socket, int32_t *);

int32_t main() {
        int32_t ls = -1;
        int32_t cs = -1;
        struct sockaddr_in saddr;
        int32_t optval = 1;

        saddr = get_address(PORT);
        CHECK(ls = socket(AF_INET, SOCK_STREAM, 0));
        CHECK(setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int)));
        CHECK(bind(ls, (struct sockaddr *)&saddr, sizeof(saddr)));
        CHECK(listen(ls, QUEUE_SIZE));

        for (char q = 0; !q; q = perform_connection(ls, &cs) == 0)
                ;

        close(ls);
        close(cs);
        return 0;
}

static void message_callback(const char const *message) {
        sprintf(CALLBACK_BUFFER, "%s%s", CALLBACK_BUFFER, message);
}

static char perform_connection(const int32_t listen_socket, int32_t *connection_socket_p) {
        struct sockaddr_in caddr;
        int32_t addrlen = -1;
        int32_t cs = -1;

        addrlen = sizeof(caddr);
        CHECK(*connection_socket_p = accept(listen_socket, (struct sockaddr *)&caddr, &addrlen));
        cs = *connection_socket_p;

        print_client_address(caddr);

        while (check_connection_status(cs)) {
                char ibuf[INPUT_SIZE];
                char obuf[OUTPUT_SIZE];
                int32_t res = 0;

                memset(ibuf, 0, INPUT_SIZE);
                memset(obuf, 0, OUTPUT_SIZE);
                memset(CALLBACK_BUFFER, 0, CALLBACK_SIZE);

                CHECK_RETURN(read(cs, ibuf, INPUT_SIZE));

                if (calculate(ibuf, &res, &message_callback))
                        sprintf(obuf, "%s%d\n", CALLBACK_BUFFER, res);
                else
                        sprintf(obuf, "FAIL\n");

                CHECK_RETURN(write(cs, obuf, strlen(obuf)));
        }

        return 0;
}