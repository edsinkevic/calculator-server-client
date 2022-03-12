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

#define BUFSIZE 1024
#define QUEUE_SIZE 100
#define PORT 9999

int callback_socket;
char callback_buffer[BUFSIZE];

int create_socket();
void error();
struct sockaddr_in set_address(struct sockaddr_in server_address);
void bind_to_port(int listen_socket, struct sockaddr_in serveraddr);
void perform_connection(int listen_socket);

int main(int argc, char **argv)
{
    int listen_socket, optval = 1;
    struct sockaddr_in server_address;

    listen_socket = create_socket();
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    server_address = set_address(server_address);
    bind_to_port(listen_socket, server_address);

    while (1)
        perform_connection(listen_socket);
}

void print_client_address(struct sockaddr_in client_address)
{
    struct hostent *hostp;
    char *hostaddrp;

    hostp = gethostbyaddr((const char *)&client_address.sin_addr.s_addr, sizeof(client_address.sin_addr.s_addr), AF_INET);
    if (hostp == NULL)
        error();

    hostaddrp = inet_ntoa(client_address.sin_addr);
    if (hostaddrp == NULL)
        error();

    printf("Client: %s (%s)\n", hostaddrp, hostp->h_name);
}

void message_callback(char *message)
{
    sprintf(callback_buffer, "%s", message);
    write(callback_socket, callback_buffer, strlen(callback_buffer));
}

char check_connection_status(int socket_fd)
{
    int error = 0;
    socklen_t len = sizeof(error);
    int retval = getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, &len);

    return error;
}

void perform_connection(int listen_socket)
{
    char input_buffer[BUFSIZE];
    char output_buffer[BUFSIZE];
    int connection_socket = 0;
    struct sockaddr_in client_address;
    int address_length = sizeof(client_address);

    connection_socket = accept(listen_socket, (struct sockaddr *)&client_address, &address_length);
    if (connection_socket < 0)
        error();

    callback_socket = connection_socket;

    char connection_status = 1;
    while (check_connection_status(connection_socket) < 1)
    {
        bzero(input_buffer, BUFSIZE);
        bzero(output_buffer, BUFSIZE);

        int bytes_read_amount = read(connection_socket, input_buffer, BUFSIZE);
        if (bytes_read_amount < 0)
            error();

        print_client_address(client_address);
        printf("Received %d bytes:\n%s", bytes_read_amount, input_buffer);

        long calculation_result;
        if (calculate(input_buffer, &calculation_result, &message_callback))
        {
            printf("-------- %ld --------\n", calculation_result);
            sprintf(output_buffer, "%ld\n", calculation_result);
            int bytes_written_amount = write(connection_socket, output_buffer, strlen(output_buffer));
            if (bytes_written_amount < 0)
                error();
        }
        else
        {
            sprintf(output_buffer, "FAIL\n");
            if (write(connection_socket, output_buffer, strlen(output_buffer)) < 0)
                error();
            printf("-------- CALCULATION FAILED/QUIT --------\n");
        }
    }

    close(connection_socket);
}

int create_socket()
{
    int lsocket = socket(AF_INET, SOCK_STREAM, 0);

    if (lsocket < 0)
        error();

    return lsocket;
}

void error()
{
    perror("ERROR");
    exit(1);
}

struct sockaddr_in set_address(struct sockaddr_in server_address)
{
    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons((unsigned short)PORT);
    return server_address;
}

void bind_to_port(int listen_socket, struct sockaddr_in server_address)
{
    if (bind(listen_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
        error();
    if (listen(listen_socket, QUEUE_SIZE) < 0)
        error();
}