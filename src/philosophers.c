#include "philosophers.h"

static const char *phl_state_str[] = {
    [PHL_THINKING] = "thinking",
    [PHL_HUNGRY] = "hungry",
    [PHL_EATING] = "eating",
    [PHL_FULL] = "full"
};

static pthread_mutex_t waiter = PTHREAD_MUTEX_INITIALIZER;

static void think(philosopher_t *self) {
    self->state = PHL_THINKING;
    ms_delay(&self->rseed, THINK_DELAY_MIN, THINK_DELAY_MAX);
}

static void acquire_forks(philosopher_t *self) {
    self->state = PHL_HUNGRY;
    pthread_mutex_lock(&waiter);
    while (self->right->has_fork_l || self->left->has_fork_r) {
        pthread_cond_wait(&self->fork_ready, &waiter);
    }
    self->has_fork_l = true;
    self->has_fork_r = true;
    pthread_mutex_unlock(&waiter);
}

static void eat(philosopher_t *self) {
    self->state = PHL_EATING;
    self->n_meals++;
    ms_delay(&self->rseed, EAT_DELAY_MIN, EAT_DELAY_MAX);
}

static void release_forks(philosopher_t *self) {
    self->state = PHL_FULL;
    pthread_mutex_lock(&waiter);
    self->has_fork_l = false;
    self->has_fork_r = false;
    pthread_cond_signal(&self->left->fork_ready);
    pthread_cond_signal(&self->right->fork_ready);
    pthread_mutex_unlock(&waiter);
}

static void philosophize(philosopher_t *self) {
    while (1) {
        think(self);
        acquire_forks(self);
        eat(self);
        release_forks(self);
    }
}

static void print_table_state(philosopher_t *first) {
    philosopher_t *curr = first;
    /* print header row */
    printf("phl  %-8s  no. meals\n", "state");

    while (curr != NULL) {
        /* print a row with status on each philosopher */
        for (int i = 0; i < N_PHLS; i++) {
            fprintf(stdout, "%-3lu  %-8s  %u\n",
                    curr->id, phl_state_str[curr->state], curr->n_meals);
            curr = curr->right;
        }

        /* probably not needed, but since we are using fprinft */
        fflush(stdout);

        /* delay a bit. We won't be able to print exact information fast enough anyhow. */
        ms_delay(NULL, PRINT_FREQ_MS, PRINT_FREQ_MS);

        /* clear prints from stdout. 
         * This will not be applied until the next newline or flush. */
        for (int i = 0; i < N_PHLS; i++) {
            fprintf(stdout, "\r" ERASE_LINE CURSOR_UP);
        }
    }
}

int main() {
    pthread_t threads[N_THREADS];
    philosopher_t philosophers[N_PHLS];
    philosopher_t *phl_ptrs[N_PHLS];

    struct timeval curr_time;
    int rc;

    /* create philosopher threads */
    for (uintptr_t i = 0; i < N_PHLS; i++) {
        phl_ptrs[i] = &philosophers[i];
        philosopher_t *phl = phl_ptrs[i];

        /* initialize philosphers here as the allocated arrays are in scope */
        phl->id = i;
        phl->left = &philosophers[(i + N_PHLS - 1) % N_PHLS];
        phl->right = &philosophers[(i + 1) % N_PHLS];
        phl->fork_ready = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
        phl->has_fork_l = false;
        phl->has_fork_r = false;
        phl->state = PHL_THINKING;
        phl->n_meals = 0;

        /* seed with nanoseconds, delaying 1ms to ensure variation */
        gettimeofday(&curr_time, NULL);
        phl->rseed = curr_time.tv_usec;
        ms_delay(NULL, 1, 1);

        /* create and start thread */
        rc = pthread_create(&threads[i], NULL, (void *) philosophize, phl);
        if (rc) {
            fprintf(stderr, "creation of thread #%lu failed: %s\n", i, strerror(errno));
            return 1;
        }
    }

    /* create table printing thread */
    rc = pthread_create(&threads[N_THREADS-2], NULL, (void *) print_table_state, phl_ptrs[0]);
    if (rc) {
        fprintf(stderr, "creation of print thread failed: %s\n", strerror(errno));
        return 1;
    }

    /* Wait for threads. This will never be reached unless the implementation is altered. */
    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
