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
#include "calc.h"
#include "../edutils.h"

#define OUTPUT_SIZE 1024
#define INPUT_SIZE 1024
#define CALLBACK_SIZE OUTPUT_SIZE - sizeof(long) - 1
#define QUEUE_SIZE 0
#define PORT 9999

#define CHECK(X) ({int __val=(X); (__val ==-1 ? \
({fprintf(stderr,"ERROR (" __FILE__":%d) -- %s\n",__LINE__,strerror(errno));\
exit(-1);-1;}) : __val); })

char CALLBACK_BUFFER[CALLBACK_SIZE];

int create_socket();
void error();
struct sockaddr_in get_address(int port);
void bind_to_port(int listen_socket, struct sockaddr_in serveraddr);
char perform_connection(int listen_socket, int *);

int main(int argc, char **argv)
{
    int ls = -1;
    int cs = -1;
    struct sockaddr_in saddr;
    int optval = 1;

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

void print_client_address(struct sockaddr_in client_address)
{
    const struct hostent const *hostp =
        gethostbyaddr((const char *)&client_address.sin_addr.s_addr, sizeof(client_address.sin_addr.s_addr), AF_INET);
    if (hostp == NULL)
        error();

    const char const *hostaddrp = inet_ntoa(client_address.sin_addr);
    if (hostaddrp == NULL)
        error();

    printf("Client: %s (%s)\n", hostaddrp, hostp->h_name);
}

void message_callback(const char const *message)
{
    sprintf(CALLBACK_BUFFER, "%s%s", CALLBACK_BUFFER, message);
}

char perform_connection(const int listen_socket, int *connection_socket_p)
{
    char ibuf[INPUT_SIZE];
    char obuf[OUTPUT_SIZE];
    struct sockaddr_in caddr;
    int addrlen = -1;
    int cs = -1;

    addrlen = sizeof(caddr);
    CHECK(*connection_socket_p = accept(listen_socket, (struct sockaddr *)&caddr, &addrlen));
    cs = *connection_socket_p;

    while (check_connection_status(cs))
    {
        int n = 0;
        long res = 0;

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
            sprintf(obuf, "%s%ld\n", CALLBACK_BUFFER, res);
        else
            sprintf(obuf, "FAIL\n");

        CHECK(write(cs, obuf, strlen(obuf)));
    }

    sprintf(obuf, "QUITTING\n");
    CHECK(write(cs, obuf, strlen(obuf)));

    return 0;
}

void error()
{
    perror("ERROR");
    exit(1);
}

struct sockaddr_in get_address(int port)
{
    struct sockaddr_in server_address;
    memset((char *)&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons((unsigned short)PORT);
    return server_address;
}