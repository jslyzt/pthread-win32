#include "pthread.h"
#include "semaphore.h"
#include "implement.h"


int sem_post_multiple(sem_t* sem, int count)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function posts multiple wakeups to a semaphore.
 *
 * PARAMETERS
 *      sem
 *              pointer to an instance of sem_t
 *
 *      count
 *              counter, must be greater than zero.
 *
 * DESCRIPTION
 *      This function posts multiple wakeups to a semaphore. If there
 *      are waiting threads (or processes), n <= count are awakened;
 *      the semaphore value is incremented by count - n.
 *
 * RESULTS
 *              0               successfully posted semaphore,
 *              -1              failed, error in errno
 * ERRNO
 *              EINVAL          'sem' is not a valid semaphore
 *                              or count is less than or equal to zero.
 *              ERANGE          semaphore count is too big
 *
 * ------------------------------------------------------
 */
{
    int result = 0;
    long waiters;
    sem_t s = *sem;

    if (s == NULL || count <= 0) {
        result = EINVAL;
    } else if ((result = pthread_mutex_lock(&s->lock)) == 0) {
        /* See sem_destroy.c
         */
        if (*sem == NULL) {
            (void) pthread_mutex_unlock(&s->lock);
            result = EINVAL;
            return -1;
        }

        if (s->value <= (SEM_VALUE_MAX - count)) {
            waiters = -s->value;
            s->value += count;
            if (waiters > 0) {
#if defined(NEED_SEM)
                if (SetEvent(s->sem)) {
                    waiters--;
                    s->leftToUnblock += count - 1;
                    if (s->leftToUnblock > waiters) {
                        s->leftToUnblock = waiters;
                    }
                }
#else
                if (ReleaseSemaphore(s->sem, (waiters <= count) ? waiters : count, 0)) {
                    /* No action */
                }
#endif
                else {
                    s->value -= count;
                    result = EINVAL;
                }
            }
        } else {
            result = ERANGE;
        }
        (void) pthread_mutex_unlock(&s->lock);
    }

    if (result != 0) {
        errno = result;
        return -1;
    }

    return 0;

} /* sem_post_multiple */
