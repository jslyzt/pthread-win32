#include "pthread.h"
#include "semaphore.h"
#include "implement.h"


int sem_trywait(sem_t* sem)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function tries to wait on a semaphore.
 *
 * PARAMETERS
 *      sem
 *              pointer to an instance of sem_t
 *
 * DESCRIPTION
 *      This function tries to wait on a semaphore. If the
 *      semaphore value is greater than zero, it decreases
 *      its value by one. If the semaphore value is zero, then
 *      this function returns immediately with the error EAGAIN
 *
 * RESULTS
 *              0               successfully decreased semaphore,
 *              -1              failed, error in errno
 * ERRNO
 *              EAGAIN          the semaphore was already locked,
 *              EINVAL          'sem' is not a valid semaphore,
 *              ENOTSUP         sem_trywait is not supported,
 *              EINTR           the function was interrupted by a signal,
 *              EDEADLK         a deadlock condition was detected.
 *
 * ------------------------------------------------------
 */
{
    int result = 0;
    sem_t s = *sem;

    if (s == NULL) {
        result = EINVAL;
    } else if ((result = pthread_mutex_lock(&s->lock)) == 0) {
        /* See sem_destroy.c
         */
        if (*sem == NULL) {
            (void) pthread_mutex_unlock(&s->lock);
            errno = EINVAL;
            return -1;
        }

        if (s->value > 0) {
            s->value--;
        } else {
            result = EAGAIN;
        }

        (void) pthread_mutex_unlock(&s->lock);
    }

    if (result != 0) {
        errno = result;
        return -1;
    }

    return 0;

} /* sem_trywait */
