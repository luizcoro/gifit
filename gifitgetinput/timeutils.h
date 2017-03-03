#ifndef TIMEUTILS
#define TIMEUTILS

#include <sys/time.h>
#include <stdlib.h>

int timeval_now(struct timeval *time);

float timeval_to_float_sec(struct timeval *time);

int timeval_to_mili(struct timeval *time);

int timeval_to_micro(struct timeval *time);

struct timeval micro_to_timeval(int micro);

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y);

#endif
