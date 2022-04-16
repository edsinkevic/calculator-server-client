#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../deps/edutils.h"

#define PORT 9999
#define BUFSIZE 1024

int main(int argc, char *argv[]) {
        struct sockaddr_in saddr;
        char obuf[BUFSIZE];
        char ibuf[BUFSIZE];
        int cs;
        int l;
        char *ip;

        CHECK(cs = socket(AF_INET, SOCK_STREAM, 0));
        l = sizeof(saddr);
        ip = "127.0.0.1";

        memset(&saddr, 0, l);
        saddr.sin_family = AF_INET;
        saddr.sin_addr.s_addr = inet_addr(ip);
        saddr.sin_port = htons(PORT);

        CHECK(connect(cs, (struct sockaddr *)&saddr, l));

        for (;;) {
                memset(ibuf, 0, BUFSIZE);
                memset(obuf, 0, BUFSIZE);

                printf("------------------------------------------\n");

                if (fgets(ibuf, BUFSIZE, stdin) == NULL)
                        break;

                if (write(cs, ibuf, BUFSIZE) <= 0)
                        break;

                if (read(cs, obuf, BUFSIZE) <= 0)
                        break;

                printf("%s", obuf);
        }

        close(cs);

        return 0;
}