#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>
#include <sys/time.h>

#include "common.h"

/*
 * sleep between ms_min and ms_max
 * seed is nullable if ms_min==ms_max
 * returns the total time slept in ms, or -1 on error
 */
long ms_delay(unsigned int *seed, long ms_min, long ms_max) {
    /* delay time in milliseconds */
    long tv_msec = ms_min;
    if (ms_max > ms_min) {
        tv_msec += rand_r(seed) % (ms_max - ms_min);
    }

    /* remaining time, request time */
    struct timespec rem, req;

    /* initialize request, converting millisecs to seconds and nanoseconds */
    req.tv_sec = tv_msec / 1000;
    req.tv_nsec = (tv_msec - (req.tv_sec * 1000)) * 1000000;

    /* sleep until request is complete, going back to sleep if interrupted */
    int rc;
    do {
        rc = nanosleep(&req, &rem);
    } while (rc && (errno == EINTR));

    if (rc) {
        fprintf(stderr, "nanosleep failed: %s\n", strerror(errno));
        return -1;
    }

    return tv_msec;
}
