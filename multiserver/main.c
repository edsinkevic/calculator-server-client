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
#define CALLBACK_SIZE OUTPUT_SIZE - sizeof(long) - 1
#define QUEUE_SIZE 0
#define PORT 9999
#define MAXCLIENTS 10

#define CHECK(X) ({int __val=(X); (__val ==-1 ? \
({fprintf(stderr,"ERROR (" __FILE__":%d) -- %s\n",__LINE__,strerror(errno));\
exit(-1);-1;}) : __val); })

#define CHECK_RETURN(X) ({int __val=(X); (__val <= 0 ? \
({if(__val == -1) fprintf(stderr,"ERROR (" __FILE__":%d) -- %s\n",__LINE__,strerror(errno));\
return -1;}) : __val); })

char CALLBACK_BUFFER[CALLBACK_SIZE];

char perform_connection(int ls);

int main(int argc, char **argv)
{
    int ls = -1;
    struct sockaddr_in saddr;
    int optval = 1;

    saddr = get_address(PORT);
    CHECK(ls = socket(AF_INET, SOCK_STREAM, 0));
    CHECK(setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int)));
    CHECK(bind(ls, (struct sockaddr *)&saddr, sizeof(saddr)));
    CHECK(listen(ls, QUEUE_SIZE));

    perform_connection(ls);

    close(ls);
    return 0;
}

int find_free_slot(int fds[], int maxclients)
{
    int i;
    for (i = 0; i < maxclients; i++)
    {
        if (fds[i] == -1)
        {
            return i;
        }
    }
    return -1;
}

void message_callback(const char const *message)
{
    sprintf(CALLBACK_BUFFER, "%s%s", CALLBACK_BUFFER, message);
}

int handle_read(int fd)
{
    long res = 0;
    char ibuf[INPUT_SIZE];
    char obuf[OUTPUT_SIZE];

    memset(&ibuf, 0, INPUT_SIZE);
    memset(&obuf, 0, OUTPUT_SIZE);
    memset(CALLBACK_BUFFER, 0, CALLBACK_SIZE);

    CHECK_RETURN(read(fd, &ibuf, INPUT_SIZE));

    if (calculate(ibuf, &res, &message_callback))
        sprintf(obuf, "%s%ld\n", CALLBACK_BUFFER, res);
    else
        sprintf(obuf, "FAIL\n");

    CHECK_RETURN(write(fd, obuf, strlen(obuf)));

    return 1;
}

int handle_connect(int fd)
{
    int cs;
    struct sockaddr_in caddr;
    int caddrlen = sizeof(caddr);
    memset(&caddr, 0, caddrlen);
    CHECK_RETURN(cs = accept(fd, (struct sockaddr *)&caddr, &caddrlen));
    print_client_address(caddr);

    return cs;
}

char perform_connection(const int ls)
{
    int cs[MAXCLIENTS];
    char ibuf[INPUT_SIZE];
    char obuf[OUTPUT_SIZE];
    int addrlen = -1;
    fd_set read_set;
    int maxfd = -1;

    for (int i = 0; i < MAXCLIENTS; i++)
        cs[i] = -1;

    for (;;)
    {
        FD_ZERO(&read_set);
        for (int i = 0; i < MAXCLIENTS; i++)
        {
            if (cs[i] != -1)
            {
                FD_SET(cs[i], &read_set);
                if (cs[i] > maxfd)
                {
                    maxfd = cs[i];
                }
            }
        }

        FD_SET(ls, &read_set);
        if (ls > maxfd)
        {
            maxfd = ls;
        }

        select(maxfd + 1, &read_set, NULL, NULL, NULL);

        if (FD_ISSET(ls, &read_set))
        {
            int client_id = find_free_slot(cs, MAXCLIENTS);
            if (client_id != -1)
            {
                cs[client_id] = handle_connect(ls);
            }
        }

        for (int i = 0; i < MAXCLIENTS; i++)
        {
            if (cs[i] != -1)
            {
                if (FD_ISSET(cs[i], &read_set))
                    if (handle_read(cs[i]) <= 0)
                    {
                        close(cs[i]);
                        cs[i] = -1;
                    }
            }
        }
    }
    return 0;
}