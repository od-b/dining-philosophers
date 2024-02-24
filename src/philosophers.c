#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>

#include "common.h"

/* additional threads (philosophers) to spawn */
#define N_THREADS 5
#define MS_DELAY_MIN 100
#define MS_DELAY_MAX 600

typedef struct philosopher philosopher_t;
struct philosopher {
    philosopher_t *left;
    philosopher_t *right;
    uintptr_t id;
    unsigned int rseed;
};

pthread_mutex_t init_guard = PTHREAD_MUTEX_INITIALIZER;

static void philosophize(philosopher_t *self) {
    printf("[%lu] initialized, seed=%u\n", self->id, self->rseed);

    for (int i = 0; i < 3; i++) {
        long dur = ms_delay(&self->rseed, MS_DELAY_MIN, MS_DELAY_MAX);
        printf("[%lu] slept for %ldms\n", self->id, dur);
    }
}

int main() {
    pthread_t threads[N_THREADS];
    philosopher_t philosophers[N_THREADS];
    philosopher_t *phl_ptrs[N_THREADS];

    struct timeval curr_time;

    for (uintptr_t i = 0; i < N_THREADS; i++) {
        phl_ptrs[i] = &philosophers[i];

        /* initialize philosphers here as the allocated arrays are in scope */
        philosophers[i].id = i;
        philosophers[i].left = &philosophers[(i + N_THREADS - 1) % N_THREADS];
        philosophers[i].right = &philosophers[(i + 1) % N_THREADS];

        /* seed with nanoseconds, delaying 1ms to ensure variation */
        gettimeofday(&curr_time, NULL);
        philosophers[i].rseed = curr_time.tv_usec;
        ms_delay(NULL, 1, 1);

        /* create and start thread */
        if (pthread_create(&threads[i], NULL, (void *) philosophize, phl_ptrs[i])) {
            fprintf(stderr, "creation of thread #%lu failed: %s\n", i, strerror(errno));
            return 1;
        }
    }

    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    fprintf(stderr, "all threads returned\n");
    return 0;
}
