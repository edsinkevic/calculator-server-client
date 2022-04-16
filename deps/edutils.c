#include "edutils.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

char *clean_token(char *s, const int slen, char (*predicate)(const char)) {
        char *accumulator = (char *)calloc(slen, sizeof(char));

        int j = 0;
        for (int i = j = 0; i < slen; i++)
                if ((*predicate)(s[i]))
                        accumulator[j++] = s[i];

        accumulator[j] = '\0';

        return accumulator;
}

char check_connection_status(int socket_fd) {
        const int bufsize = 1024;
        char *test_buffer[bufsize];
        return recv(socket_fd, test_buffer, bufsize, MSG_PEEK) != 0;
}

struct sockaddr_in get_address(int port) {
        struct sockaddr_in server_address;
        memset((char *)&server_address, 0, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = htonl(INADDR_ANY);
        server_address.sin_port = htons((unsigned short)port);
        return server_address;
}

void error() {
        perror("ERROR");
        exit(1);
}

void print_client_address(struct sockaddr_in client_address) {
        const struct hostent const *hostp =
            gethostbyaddr((const char *)&client_address.sin_addr.s_addr, sizeof(client_address.sin_addr.s_addr), AF_INET);
        if (hostp == NULL)
                error();

        const char const *hostaddrp = inet_ntoa(client_address.sin_addr);
        if (hostaddrp == NULL)
                error();

        printf("Client: %s (%s)\n", hostaddrp, hostp->h_name);
}