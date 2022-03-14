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
    int error = 0;
    socklen_t len = sizeof(error);
    int retval = getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, &len);

    return error;
}