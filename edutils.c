#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "edutils.h"

char *clean_token(char *s, const int slen, char (*predicate)(const char))
{
    char *accumulator = (char *)calloc(slen, sizeof(char));

    int j = 0;
    for (int i = j = 0; i < slen; i++)
        if ((*predicate)(s[i]))
            accumulator[j++] = s[i];

    accumulator[j] = '\0';

    return accumulator;
}

char check_connection_status(int socket_fd)
{
    const int bufsize = 1024;
    char *test_buffer[bufsize];
    return recv(socket_fd, test_buffer, bufsize, MSG_PEEK) != 0;
}