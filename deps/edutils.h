#ifndef EDUTILS_H_INCLUDED
#define EDUTILS_H_INCLUDED

#include <errno.h>

#define CHECK(X) ({int32_t __val=(X); (__val ==-1 ? \
({fprintf(stderr,"ERROR (" __FILE__":%d) -- %s\n",__LINE__,strerror(errno));\
exit(-1);-1;}) : __val); })

char *clean_token(char *s, const int slen, char (*predicate)(const char));
char check_connection_status(int socket_fd);
struct sockaddr_in get_address(int port);
void error();
void print_client_address(struct sockaddr_in client_address);

#endif