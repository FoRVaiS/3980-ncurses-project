#ifndef UTILS_H
#define UTILS_H

#include <arpa/inet.h>

#define MS_PER_SEC 1000

long      get_time_ms(void);
in_port_t convert_port(const char *str, int *err);

#endif
