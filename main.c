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
int calculate(char *input, long *result);
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

char check_connection_status(int socket_fd)
{
    int error = 0;
    socklen_t len = sizeof(error);
    int retval = getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, &len);

    return error;
}

void perform_connection(int listen_socket)
{
    int connection_socket = 0;
    struct sockaddr_in client_address;
    int address_length = sizeof(client_address);
    char input_buffer[BUFSIZE];
    char output_buffer[BUFSIZE];

    connection_socket = accept(listen_socket, (struct sockaddr *)&client_address, &address_length);
    if (connection_socket < 0)
        error();

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
        if (calculate(input_buffer, &calculation_result))
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

int calculate(char *input, long *result)
{
    char result_status;
    Stack stack = init();
    char *token = strtok(input, " ");

    while (token != NULL)
    {
        if (!handle_token(&stack, token))
            return 0;
        token = strtok(NULL, " ");
    }

    result_status = pop(&stack, result);

    free_stack(&stack);
    return result_status;
}

int isnumber(char *string)
{
    int i = 0;
    int length = strlen(string);
    if (length > 1 && string[0] == '-')
        ++i;

    for (i; i < length; ++i)
        if (!isdigit(string[i]))
            return 0;

    return 1;
}

void bin_op_stack(Stack *stack, long (*f)(long, long))
{
    if (ssize(stack->head) >= 2)
    {
        printf("fart");
        long popped_value1;
        pop(stack, &popped_value1);
        long popped_value2;
        pop(stack, &popped_value2);
        push(stack, (*f)(popped_value1, popped_value2));
    }
}

long fadd(long a, long b) { return a + b; }
long fminus(long a, long b) { return a - b; }
long ftimes(long a, long b) { return a * b; }

char *clean_token(char *s)
{
    int j, n = strlen(s);

    char *accumulator = (char *)calloc(n, sizeof(char));

    for (int i = j = 0; i < n; i++)
        if (isalnum(s[i]) || s[i] == '-' || s[i] == '+' || s[i] == '*')
            accumulator[j++] = s[i];

    accumulator[j] = '\0';

    return accumulator;
}

char handle_token(Stack *stack, char *dirty_token)
{
    char *token = clean_token(dirty_token);
    int token_length = strlen(token);

    printf("length: %d\n", token_length);

    if (isnumber(token) == 1)
        push(stack, atol(token));
    else if (strncmp(token, "+", 1) == 0 && token_length == 1)
        bin_op_stack(stack, &fadd);
    else if (strncmp(token, "-", 1) == 0 && token_length == 1)
        bin_op_stack(stack, &fminus);
    else if (strncmp(token, "*", 1) == 0 && token_length == 1)
        bin_op_stack(stack, &ftimes);
    else
    {
        free(token);
        return 0;
    }

    printf("[%s]\n", token);

    free(token);
    return 1;
}
