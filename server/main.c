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

char callback_buffer[CALLBACK_SIZE];

int create_socket();
void error();
struct sockaddr_in set_address(int port);
void bind_to_port(int listen_socket, struct sockaddr_in serveraddr);
char perform_connection(int listen_socket, int *);

int main(int argc, char **argv)
{
    const struct sockaddr_in server_address = set_address(PORT);
    const int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket < 0)
        error();
    const int optval = 1;
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    if (bind(listen_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
        error();
    if (listen(listen_socket, QUEUE_SIZE) < 0)
        error();

    int connection_socket = -1;
    for (char quit = 0; !quit; quit = perform_connection(listen_socket, &connection_socket))
        ;

    close(listen_socket);
    close(connection_socket);
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
    sprintf(callback_buffer, "%s%s", callback_buffer, message);
}

char perform_connection(const int listen_socket, int *connection_socket_p)
{
    char input_buffer[INPUT_SIZE];
    char output_buffer[OUTPUT_SIZE];
    struct sockaddr_in client_address;
    int address_length = sizeof(client_address);

    *connection_socket_p = accept(listen_socket, (struct sockaddr *)&client_address, &address_length);
    const int connection_socket = *connection_socket_p;
    if (connection_socket < 0)
        error();

    while (check_connection_status(connection_socket))
    {
        bzero(input_buffer, INPUT_SIZE);
        bzero(output_buffer, OUTPUT_SIZE);
        bzero(callback_buffer, CALLBACK_SIZE);

        const int bytes_read_amount = read(connection_socket, input_buffer, INPUT_SIZE);
        if (bytes_read_amount == 0)
            return 0;
        if (bytes_read_amount < 0)
            error();

        if (strncmp(input_buffer, "shutdown", 8) == 0)
            return 1;

        print_client_address(client_address);

        long calculation_result = 0;
        if (calculate(input_buffer, &calculation_result, &message_callback))
        {

            printf("%s", input_buffer);
            printf("%ld\n", calculation_result);
            sprintf(output_buffer, "%s%ld\n", callback_buffer, calculation_result);

            const int bytes_written_amount = write(connection_socket, output_buffer, strlen(output_buffer));
            if (bytes_written_amount < 0)
                error();
        }
        else
        {
            if (write(connection_socket, output_buffer, strlen(output_buffer)) < 0)
                error();
        }
    }

    return 0;
}

void error()
{
    perror("ERROR");
    exit(1);
}

struct sockaddr_in set_address(int port)
{
    struct sockaddr_in server_address;
    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons((unsigned short)PORT);
    return server_address;
}