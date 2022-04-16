#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../deps/edutils.h"

#define PORT 9999
#define BUFSIZE 1024

int main(int argc, char const *argv[]) {
        struct sockaddr_in saddr;
        char obuf[BUFSIZE];
        char ibuf[BUFSIZE];

        const int cs = socket(AF_INET, SOCK_STREAM, 0);
        const int l = sizeof(saddr);
        const char *ip = "127.0.0.1";

        bzero(&saddr, l);
        saddr.sin_family = AF_INET;
        saddr.sin_addr.s_addr = inet_addr(ip);
        saddr.sin_port = htons(PORT);

        connect(cs, (const struct sockaddr *)&saddr, l);

        while (1) {
                printf("------------------------------------------\n");
                bzero(ibuf, BUFSIZE);
                if (fgets(ibuf, BUFSIZE, stdin) == NULL)
                        break;

                if (write(cs, ibuf, BUFSIZE) <= 0)
                        break;

                bzero(obuf, BUFSIZE);
                if (read(cs, obuf, BUFSIZE) <= 0)
                        break;

                printf("%s", obuf);
        }

        close(cs);

        return 0;
}