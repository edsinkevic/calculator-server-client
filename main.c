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
#include "stack.h"

#define BUFSIZE 1024
#define QUEUE_SIZE 100
#define PORT 9999

int create_socket();
void error();
struct sockaddr_in set_address(struct sockaddr_in server_address);
void bind_to_port(int listen_socket, struct sockaddr_in serveraddr);
int calculate(char *input, int *result);
char handle_token(Stack *stack, char *token);
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

void get_client_address(struct sockaddr_in client_address)
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

void perform_connection(int listen_socket)
{
    int connection_socket = 0;
    struct sockaddr_in client_address;
    int address_length = sizeof(client_address);

    char buffer[BUFSIZE];
    int n;

    connection_socket = accept(listen_socket, (struct sockaddr *)&client_address, &address_length);
    if (connection_socket < 0)
        error();

    bzero(buffer, BUFSIZE);

    n = read(connection_socket, buffer, BUFSIZE);

    if (n < 0)
        error();

    get_client_address(client_address);

    printf("Received %d bytes:\n%s", n, buffer);

    int calculation_result;
    calculate(buffer, &calculation_result);
    printf("-------- %d --------\n", calculation_result);

    n = write(connection_socket, buffer, strlen(buffer));
    if (n < 0)
        error();

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

struct sockaddr_in
set_address(struct sockaddr_in server_address)
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

int calculate(char *input, int *result)
{
    Stack stack = init();
    char *token = strtok(input, " ");

    while (token != NULL)
    {
        handle_token(&stack, token);
        printf("[%s]\n", token);
        token = strtok(NULL, " ");
    }

    char result_status = pop(&stack, result);

    free_stack(&stack);
    return result_status;
}

int isnumber(char *string)
{
    for (int i = 0; i < strlen(string); ++i)
        if (isdigit(string[i]) == 0)
            return 0;

    return 1;
}

void bin_op_stack(Stack *stack, int (*f)(int, int))
{
    int popped_value1;
    pop(stack, &popped_value1);
    int popped_value2;
    pop(stack, &popped_value2);

    push(stack, (*f)(popped_value1, popped_value2));
}

int fadd(int a, int b) { return a + b; }
int fminus(int a, int b) { return a - b; }
int ftimes(int a, int b) { return a * b; }

char handle_token(Stack *stack, char *token)
{
    if (isnumber(token) == 1)
        push(stack, atoi(token));
    else if (strncmp(token, "+", 1) == 0)
        bin_op_stack(stack, &fadd);
    else if (strncmp(token, "-", 1) == 0)
        bin_op_stack(stack, &fminus);
    else if (strncmp(token, "*", 1) == 0)
        bin_op_stack(stack, &ftimes);
    else
        return 1;

    return 0;
}
