#include "timeutils.h"

int timeval_now(struct timeval *time)
{
    return gettimeofday(time, NULL);
}

float timeval_to_float_sec(struct timeval *time)
{

    return (time->tv_sec + (time->tv_usec/1000000.0f));
}

int timeval_to_mili(struct timeval *time)
{
    return (int) ((time->tv_sec * 1000) + (time->tv_usec / 1000));
}

int timeval_to_micro(struct timeval *time)
{
    return (int) ((time->tv_sec * 1000000) + time->tv_usec);
}

struct timeval micro_to_timeval(int micro)
{
    struct timeval t;
    t.tv_sec = micro / 1000000;
    t.tv_usec = (micro % 1000000);

    return t;
}

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    return x->tv_sec < y->tv_sec;
}
