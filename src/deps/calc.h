#ifndef CALC_H_INCLUDED
#define CALC_H_INCLUDED
#include <stdint.h>

int calculate(char *input, int *result, void (*message_callback)(const char const *message));

#endif