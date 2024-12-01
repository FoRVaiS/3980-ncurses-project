#include "utils.h"
#include <sys/time.h>
#include <time.h>

long get_time_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * MS_PER_SEC) + (tv.tv_usec / MS_PER_SEC);
}
