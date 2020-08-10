#include "pthread.h"
#include "semaphore.h"
#include "implement.h"


int sem_destroy(sem_t* sem)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function destroys an unnamed semaphore.
 *
 * PARAMETERS
 *      sem
 *              pointer to an instance of sem_t
 *
 * DESCRIPTION
 *      This function destroys an unnamed semaphore.
 *
 * RESULTS
 *              0               successfully destroyed semaphore,
 *              -1              failed, error in errno
 * ERRNO
 *              EINVAL          'sem' is not a valid semaphore,
 *              ENOSYS          semaphores are not supported,
 *              EBUSY           threads (or processes) are currently
 *                                      blocked on 'sem'
 *
 * ------------------------------------------------------
 */
{
    int result = 0;
    sem_t s = NULL;

    if (sem == NULL || *sem == NULL) {
        result = EINVAL;
    } else {
        s = *sem;

        if ((result = pthread_mutex_lock(&s->lock)) == 0) {
            if (s->value < 0) {
                (void) pthread_mutex_unlock(&s->lock);
                result = EBUSY;
            } else {
                /* There are no threads currently blocked on this semaphore. */

                if (!CloseHandle(s->sem)) {
                    (void) pthread_mutex_unlock(&s->lock);
                    result = EINVAL;
                } else {
                    /*
                     * Invalidate the semaphore handle when we have the lock.
                     * Other sema operations should test this after acquiring the lock
                     * to check that the sema is still valid, i.e. before performing any
                     * operations. This may only be necessary before the sema op routine
                     * returns so that the routine can return EINVAL - e.g. if setting
                     * s->value to SEM_VALUE_MAX below does force a fall-through.
                     */
                    *sem = NULL;

                    /* Prevent anyone else actually waiting on or posting this sema.
                     */
                    s->value = SEM_VALUE_MAX;

                    (void) pthread_mutex_unlock(&s->lock);

                    do {
                        /* Give other threads a chance to run and exit any sema op
                         * routines. Due to the SEM_VALUE_MAX value, if sem_post or
                         * sem_wait were blocked by us they should fall through.
                         */
                        Sleep(0);
                    } while (pthread_mutex_destroy(&s->lock) == EBUSY);
                }
            }
        }
    }

    if (result != 0) {
        errno = result;
        return -1;
    }

    free(s);

    return 0;

} /* sem_destroy */
