#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <ctype.h>
#include "../deps/calc.h"
#include "../deps/edutils.h"

#define OUTPUT_SIZE 1024
#define INPUT_SIZE 1024
#define CALLBACK_SIZE OUTPUT_SIZE - sizeof(int32_t) - 1
#define QUEUE_SIZE 0
#define PORT 9999

#define CHECK(X) ({int32_t __val=(X); (__val ==-1 ? \
({fprintf(stderr,"ERROR (" __FILE__":%d) -- %s\n",__LINE__,strerror(errno));\
exit(-1);-1;}) : __val); })

char CALLBACK_BUFFER[CALLBACK_SIZE];

char perform_connection(int32_t listen_socket, int32_t *);

int32_t main()
{
    int32_t ls = -1;
    int32_t cs = -1;
    struct sockaddr_in saddr;
    int32_t optval = 1;

    saddr = get_address(PORT);
    CHECK(ls = socket(AF_INET, SOCK_STREAM, 0));
    CHECK(setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int)));
    CHECK(bind(ls, (struct sockaddr *)&saddr, sizeof(saddr)));
    CHECK(listen(ls, QUEUE_SIZE));

    for (char q = 0; !q; q = perform_connection(ls, &cs))
        ;

    close(ls);
    close(cs);
    return 0;
}

void message_callback(const char const *message)
{
    sprintf(CALLBACK_BUFFER, "%s%s", CALLBACK_BUFFER, message);
}

char perform_connection(const int32_t listen_socket, int32_t *connection_socket_p)
{
    char ibuf[INPUT_SIZE];
    char obuf[OUTPUT_SIZE];
    struct sockaddr_in caddr;
    int32_t addrlen = -1;
    int32_t cs = -1;

    addrlen = sizeof(caddr);
    CHECK(*connection_socket_p = accept(listen_socket, (struct sockaddr *)&caddr, &addrlen));
    cs = *connection_socket_p;

    while (check_connection_status(cs))
    {
        int32_t n = 0;
        int32_t res = 0;

        memset(ibuf, 0, INPUT_SIZE);
        memset(obuf, 0, OUTPUT_SIZE);
        memset(CALLBACK_BUFFER, 0, CALLBACK_SIZE);

        CHECK(n = read(cs, ibuf, INPUT_SIZE));
        if (n == 0)
            return 0;

        if (strncmp(ibuf, "shutdown", 8) == 0)
            return 1;

        print_client_address(caddr);

        if (calculate(ibuf, &res, &message_callback))
            sprintf(obuf, "%s%d\n", CALLBACK_BUFFER, res);
        else
            sprintf(obuf, "FAIL\n");

        CHECK(write(cs, obuf, strlen(obuf)));
    }

    sprintf(obuf, "QUITTING\n");
    CHECK(write(cs, obuf, strlen(obuf)));

    return 0;
}