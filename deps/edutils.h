#ifndef EDUTILS_H_INCLUDED
#define EDUTILS_H_INCLUDED

char *clean_token(char *s, const int slen, char (*predicate)(const char));
char check_connection_status(int socket_fd);
struct sockaddr_in get_address(int port);
void error();
void print_client_address(struct sockaddr_in client_address);

#endif