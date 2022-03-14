#ifndef CALC_H_INCLUDED
#define CALC_H_INCLUDED
#include "stack.h"
#include "../edutils.h"
int calculate(char *input, long *result, void (*message_callback)(const char const *message));

#endif