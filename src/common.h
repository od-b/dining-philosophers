#ifndef COMMON_H
#define COMMON_H

/*
 * sleep between ms_min and ms_max.
 * interrupts are let through before going back to sleep.
 * seed may be set to NULL if ms_min==ms_max
 *
 * returns the total time slept in ms, or -1 on error
 */
long ms_delay(unsigned int *seed, long ms_min, long ms_max);

/*
 * Given a path, returns a pointer to it's basename
 */
char *get_basename(char *path);

#endif /* COMMON_H */
