#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>

#include "common.h"
#include "philosophers.h"

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
    int n_meals;
};

static int n_meals;
static int n_phils;
static pthread_mutex_t waiter = PTHREAD_MUTEX_INITIALIZER;
static volatile bool session_done = false;

static const char *phl_state_str[] = {
    [PHL_THINKING] = "thinking",
    [PHL_HUNGRY] = "hungry",
    [PHL_EATING] = "eating",
    [PHL_FULL] = "full"
};

/*
 * === philosopher thread && helpers ===
 */

static void think(philosopher_t *self) {
    self->state = PHL_THINKING;
    ms_delay(&self->rseed, THINK_DELAY_MIN, THINK_DELAY_MAX);
}

/* attempt to acquire forks, blocking unless both are ready */
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

/* release held forks, signal left and right that forks are now available */
static void release_forks(philosopher_t *self) {
    self->state = PHL_FULL;
    pthread_mutex_lock(&waiter);
    self->has_fork_l = false;
    self->has_fork_r = false;


    /* set break condition for main loop if target is reached */
    if (self->n_meals == n_meals) {
        session_done = true;
    }
    /*
     * signal adjacent philosophers, regardless whether they have requested a fork.
     * This simulates some degree of missing information
     */
    pthread_cond_signal(&self->left->fork_ready);
    pthread_cond_signal(&self->right->fork_ready);
    pthread_mutex_unlock(&waiter);
}

/* contains the main loop, which will run until the first
 * philosopher has eaten n_meals */
static void philosophize_thread(philosopher_t *self) {
    while (!session_done) {
        think(self);
        acquire_forks(self);
        eat(self);
        release_forks(self);
    }
}

/*
 * === table printing thread && helpers ===
 */

static void print_summary(philosopher_t *phil) {
    long total_meals = 0;
    philosopher_t *max = phil, *min = phil;

    for (int i = 0; i < n_phils; i++) {
        total_meals += phil->n_meals;

        if (phil->n_meals > max->n_meals) {
            max = phil;
        } else if (phil->n_meals < min->n_meals) {
            min = phil;
        }
        phil = phil->right;
    }

    double avg_meals = (double) total_meals / (double) n_phils;

    printf(" * Cumulative meals: %li\n", total_meals);
    printf(" * Average no. meals: %.1f\n", avg_meals);
    printf(" * Meal range (difference): %d\n", max->n_meals - min->n_meals);
}

static void print_table(philosopher_t *phl) {
    /* print a row with status on each philosopher */
    for (int i = 0; i < n_phils; i++) {
        fprintf(stdout, "%-3u  %-8s  %-6u\n", 
                phl->id, phl_state_str[phl->state], phl->n_meals);
        phl = phl->right;
    }

    /* probably not needed, but since we are using fprinft */
    fflush(stdout);
}

static bool all_phils_finished(philosopher_t *phil) {
    /* Check if philosophers are still active */
    if (session_done) {
        int phils_done = 0;

        /* check if all are finished. loop around the list using first. */
        for (int i = 0; i < n_phils; i++) {
            if (phil->state == PHL_FULL) {
                phils_done++;
                phil = phil->right;
            }
        }

        /* all philosophers finished their routines, exit main loop */
        if (phils_done == n_phils) {
            return true;
        }
    }
    return false;
}

/*
 * Thread to print a status table on philosophers
 * This thread acquires no locks, and simply reads from the list of philosophers.
 * returns when all philosophers have returned
 */
static void stat_print_thread(philosopher_t *first) {
    if (DISABLE_STAT_PRINTS) {
        return;
    }
    philosopher_t *curr = first;

    /* reset pointer, erase forward, cursor up */
    const char fmt_clear_line[] = "\r\33[2K\033[A";

    /* print summary instead if status table is set to not print */
    while (!PRINT_STATUS_TABLE) {
        print_summary(first);
        /* clear 3 lines, as printed by summary */
        printf("%s%s%s", fmt_clear_line, fmt_clear_line, fmt_clear_line);
        if (all_phils_finished(first)) {
            return;
        }
        ms_delay(NULL, PRINT_FREQ_MS, PRINT_FREQ_MS);
    }

    printf(" === Dinner Table ===\n");
    /* print header row */
    printf("%-3s  %-8s  %-6s\n", "phl", "state", "meals");

    /* print status table untill all phils have returned to main */
    while (1) {
        print_table(first);

        /* delay a bit. We won't be able to print exact information fast enough anyhow. */
        ms_delay(NULL, PRINT_FREQ_MS, PRINT_FREQ_MS);

        if (all_phils_finished(first)) {
            return;
        }

        /* clear table */
        for (int i = 0; i < n_phils; i++) {
            fprintf(stdout, fmt_clear_line);
        }
    }
}

/*
 * === main && misc. helpers ===
 */

/*
 * allocates and returns an array of philosophers
 */
static philosopher_t *philosophers_create() {
    philosopher_t *phils = calloc((size_t) n_phils, sizeof(philosopher_t));
    struct timeval curr_time;   // used for seeding

    /* create philosopher threads */
    for (int i = 0; i < n_phils; i++) {
        philosopher_t *phl = &phils[i];

        /* initialize philosphers here as the allocated arrays are in scope */
        phl->id = i;
        phl->left = &phils[(i + n_phils - 1) % n_phils];
        phl->right = &phils[(i + 1) % n_phils];
        phl->fork_ready = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
        phl->has_fork_l = false;
        phl->has_fork_r = false;
        phl->state = PHL_THINKING;
        phl->n_meals = 0;

        /* seed with nanoseconds */
        gettimeofday(&curr_time, NULL);
        phl->rseed = curr_time.tv_usec;

        /* delay 1ms to ensure different seeds  */
        ms_delay(NULL, 1, 1);
    }

    return phils;
}

/*
 * validate and/or assign args, print usage, etc.
 * returns 0 on error/info, otherwise 1
 */
static int parse_args(int argc, char **argv) {
    /* missing args */
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <num_philosophers> <n_meals>\n", get_basename(argv[0]));

        /* --h or --help prints further info */
        if (argc == 2 && (strcmp(argv[1], "--h") == 0 || strcmp(argv[1], "--help") == 0)) {
            fprintf(stderr, "Example:\n '%s 7 1000'\n -> will spawn 7 philosophers.",
                    get_basename(argv[0]));
            fprintf(stderr, "  They will continue until any one of them consumes 1000 meals\n");
            fprintf(stderr, "See src/philosophers.h to adjust delay parameters\n");
        }

        return 0;
    }

    n_phils = atoi(argv[1]);
    n_meals = atoi(argv[2]);

    if (n_meals <= 0 && n_phils <= 0) {
        /* args are <= 0 or NAN */
        fprintf(stderr, "%s: argument error\n", get_basename(argv[0]));
        if (n_phils <= 0) {
            fprintf(stderr, "  <n_phils> (arg 1) - must be a positive integer\n");
        }
        if (n_meals <= 0) {
            fprintf(stderr, "  <n_meals> (arg 2) - must be a positive integer\n");
        }

        return 0;
    }

    /* args ok */
    return 1;
}

int main(int argc, char **argv) {
    if (!parse_args(argc, argv)) {
        return 1;
    }

    const int n_threads = n_phils + 1;
    pthread_t *threads = calloc((size_t) n_threads, sizeof(pthread_t));
    philosopher_t *phils = philosophers_create();

    printf("%d philosophers are about to eat. They will continue until one of them consumes %d meals\n",
           n_phils, n_meals);

    /* spawn philosopher threads */
    for (int i = 0; i < n_phils; i++) {
        if (pthread_create(&threads[i], NULL, (void *) philosophize_thread, &phils[i])) {
            fprintf(stderr, "creation of thread #%u failed: %s\n", i, strerror(errno));
            return 1;
        }
    }

    /* spawn table printing thread */
    if (pthread_create(&threads[n_threads-1], NULL, (void *) stat_print_thread, &phils[0])) {
        fprintf(stderr, "creation of print thread failed: %s\n", strerror(errno));
        return 1;
    }

    /* Wait for threads to finish. */
    for (int i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
    }


    printf(" === All philosophers left the table ===\n");
    printf("Meal Summary:\n");

    /* print summary */
    print_summary(phils);

    return 0;
}
