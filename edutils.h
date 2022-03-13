#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

char *clean_token(char *s, const int slen, char (*predicate)(const char));

#endif // STACK_H_INCLUDED