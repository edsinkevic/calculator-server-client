#include <stdio.h>     // perror, printf
#include <stdlib.h>    // exit, atoi
#include <unistd.h>    // write, read, close
#include <arpa/inet.h> // sockaddr_in, AF_INET, SOCK_STREAM, INADDR_ANY, socket etc...
#include <string.h>    // strlen, memset

#define PORT 9999
#define BUFSIZE 1024

int main(int argc, char const *argv[])
{
    struct sockaddr_in server_address;
    char output_buffer[BUFSIZE];
    char input_buffer[BUFSIZE];

    const int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    const int length = sizeof(server_address);

    const char *server_ip = "127.0.0.1";

    bzero(&server_address, length);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_ip);
    server_address.sin_port = htons(PORT);

    connect(server_socket, (const struct sockaddr *)&server_address, length);

    for (;;)
    {
        scanf("%s", output_buffer);
        write(server_socket, output_buffer, BUFSIZE);
        read(server_socket, input_buffer, BUFSIZE);
        printf("%s", input_buffer);
    }
}