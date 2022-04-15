#ifndef CALC_H_INCLUDED
#define CALC_H_INCLUDED
#include "stack.h"
#include "edutils.h"
int32_t calculate(char *input, int32_t *result, void (*message_callback)(const char const *message));

#endif