#ifndef PHILOSOPHERS_H
#define PHILOSOPHERS_H

#include <pthread.h>
#include <stdbool.h>

/* additional threads (philosophers) to spawn */
#define N_PHLS 7
#define N_THREADS ( N_PHLS + 1 )

#define THINK_DELAY_MIN 10
#define THINK_DELAY_MAX 60
#define EAT_DELAY_MIN 10
#define EAT_DELAY_MAX 60
#define PRINT_FREQ_MS 50

#define TERM_ERASE_LINE "\33[2K"
#define TERM_CURSOR_UP "\033[A"

enum phl_state {
    PHL_THINKING = 0,
    PHL_HUNGRY,
    PHL_EATING,
    PHL_FULL
};

typedef struct philosopher philosopher_t;
struct philosopher {
    unsigned int id;
    philosopher_t *left;
    philosopher_t *right;
    volatile bool has_fork_l;
    volatile bool has_fork_r;
    pthread_cond_t fork_ready;
    enum phl_state state;
    unsigned int rseed;
    unsigned int n_meals;
};

#endif /* PHILOSOPHERS_H */
