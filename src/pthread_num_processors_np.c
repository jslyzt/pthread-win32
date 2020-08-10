#include "pthread.h"
#include "implement.h"

/*
 * pthread_num_processors_np()
 *
 * Get the number of CPUs available to the process.
 */
int pthread_num_processors_np(void) {
    int count;

    if (ptw32_getprocessors(&count) != 0) {
        count = 1;
    }

    return (count);
}
