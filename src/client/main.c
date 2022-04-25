#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../deps/edutils.h"

typedef int int32_t;
#define PORT 9999
#define BUFSIZE 1024

static int perform_connection(int cs);

int main(int argc, char *argv[]) {
        struct sockaddr_in saddr;
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

        if (perform_connection(cs) == -1)
                getchar();

        CHECK_PRINT(close(cs));

        return 0;
}

static int perform_connection(int cs) {
        char obuf[BUFSIZE];
        char ibuf[BUFSIZE];
        for (;;) {
                memset(ibuf, 0, BUFSIZE);
                memset(obuf, 0, BUFSIZE);

                printf("------------------------------------------\n");

                if (!fgets(ibuf, BUFSIZE, stdin))
                        return 0;

                CHECK_RETURN(write(cs, ibuf, BUFSIZE));
                CHECK_RETURN(read(cs, obuf, BUFSIZE));
                printf("%s", obuf);
        }

        return 0;
}