#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>


#define N_PHL 5
#define MS_DELAY_MIN 600
#define MS_DELAY_MAX 900

typedef struct philosopher philosopher_t;
struct philosopher {
    philosopher_t *left;
    philosopher_t *right;
    uint8_t idx;
};

static philosopher_t philosophers[N_PHL];

/* 
 * put thread to sleep between MS_DELAY_MIN and MS_DELAY_MAX milliseconds 
 */
static void delay() {
    long tv_msec = MS_DELAY_MIN + ((MS_DELAY_MAX - MS_DELAY_MIN) % rand());
    struct timespec rem, req = { 0, (tv_msec * 1000000) };

    /* sleep until request is complete, going back to sleep if interrupted */
    int status;
    do {
        status = nanosleep(&req, &rem);
    } while (status && (errno == EINTR));
}

static void philosophize(int idx) {
    philosopher_t *self = &philosophers[idx];
    self->idx = idx;
    self->left = &philosophers[(self->idx + N_PHL - 1) % N_PHL];
    self->right = &philosophers[(self->idx + 1) % N_PHL];
    printf("initialized philosopher #%u\n", self->idx);
}

int main() {
    srand(time(NULL));
    pthread_t threads[N_PHL];

    for (uint64_t i = 0; i < N_PHL; i++) {
        if (pthread_create(&threads[i], NULL, (void *)philosophize, (void *)i)) {
            fprintf(stderr, "creation of philosopher #%llu failed\n", i);
            return 1;
        }
    }

    for (int i = 0; i < N_PHL; i++) {
        pthread_join(threads[i], NULL);
    }

    fprintf(stderr, "main: all philosophers returned\n");
    return 0;
}
