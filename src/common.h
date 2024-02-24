#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>
#include <sys/time.h>

long ms_delay(unsigned int *seed, long ms_min, long ms_max);

#endif /* COMMON_H */
