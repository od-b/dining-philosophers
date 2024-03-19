#ifndef PHILOSOPHERS_H
#define PHILOSOPHERS_H

/* philosopher delays */
#define THINK_DELAY_MIN 10
#define THINK_DELAY_MAX 60
#define EAT_DELAY_MIN 10
#define EAT_DELAY_MAX 60

/* frequency of prints done by the status thread */
#define PRINT_FREQ_MS 50

#ifndef PRINT_STATUS_TABLE
/* set to 0 to print a status summary instead of table */
#define PRINT_STATUS_TABLE 1
#endif /* PRINT_STATUS_TABLE */

/* disable the status printing thread entirely.
 * this will override PRINT_FREQ_MS and PRINT_STATUS_TABLE.
 * a summary will still be printed at the end of the simulation */
#define DISABLE_STAT_PRINTS 0

#endif /* PHILOSOPHERS_H */
