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
#include <ctype.h>
#include "calc.h"
#include "../edutils.h"

#define OUTPUT_SIZE 1024
#define INPUT_SIZE 1024
#define CALLBACK_SIZE OUTPUT_SIZE - sizeof(long) - 1
#define QUEUE_SIZE 0
#define PORT 9999

char CALLBACK_BUFFER[CALLBACK_SIZE];

int create_socket();
void error();
struct sockaddr_in get_address(int port);
void bind_to_port(int listen_socket, struct sockaddr_in serveraddr);
char perform_connection(int listen_socket, int *);

int main(int argc, char **argv)
{
    const int ls = socket(AF_INET, SOCK_STREAM, 0);
    if (ls < 0)
        error();

    const struct sockaddr_in saddr = get_address(PORT);

    const int optval = 1;
    setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    if (bind(ls, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
        error();
    if (listen(ls, QUEUE_SIZE) < 0)
        error();

    int cs = -1;
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
    int addrlen = sizeof(caddr);

    *connection_socket_p = accept(listen_socket, (struct sockaddr *)&caddr, &addrlen);
    const int cs = *connection_socket_p;
    if (cs < 0)
        error();

    while (check_connection_status(cs))
    {
        bzero(ibuf, INPUT_SIZE);
        bzero(obuf, OUTPUT_SIZE);
        bzero(CALLBACK_BUFFER, CALLBACK_SIZE);

        const int n = read(cs, ibuf, INPUT_SIZE);
        if (n == 0)
            return 0;
        if (n < 0)
            error();

        if (strncmp(ibuf, "shutdown", 8) == 0)
            return 1;

        print_client_address(caddr);

        long res = 0;
        if (calculate(ibuf, &res, &message_callback))
        {
            printf("%s", ibuf);
            printf("%ld\n", res);
            sprintf(obuf, "%s%ld\n", CALLBACK_BUFFER, res);

            const int n = write(cs, obuf, strlen(obuf));
            if (n < 0)
                error();
        }
        else
        {
            sprintf(obuf, "FAIL\n");
            if (write(cs, obuf, strlen(obuf)) < 0)
                error();
        }
    }

    sprintf(obuf, "QUITTING\n");
    if (write(cs, obuf, strlen(obuf)) < 0)
        error();

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
    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons((unsigned short)PORT);
    return server_address;
}