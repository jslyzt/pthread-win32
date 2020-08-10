#if !defined(_UWIN)
/*#   include <process.h> */
#endif
#include "pthread.h"
#include "implement.h"


int ptw32_semwait(sem_t* sem)
/*
 * ------------------------------------------------------
 * DESCRIPTION
 *      This function waits on a POSIX semaphore. If the
 *      semaphore value is greater than zero, it decreases
 *      its value by one. If the semaphore value is zero, then
 *      the calling thread (or process) is blocked until it can
 *      successfully decrease the value.
 *
 *      Unlike sem_wait(), this routine is non-cancelable.
 *
 * RESULTS
 *              0               successfully decreased semaphore,
 *              -1              failed, error in errno.
 * ERRNO
 *              EINVAL          'sem' is not a valid semaphore,
 *              ENOSYS          semaphores are not supported,
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
    } else {
        if ((result = pthread_mutex_lock(&s->lock)) == 0) {
            int v;

            /* See sem_destroy.c
             */
            if (*sem == NULL) {
                (void) pthread_mutex_unlock(&s->lock);
                errno = EINVAL;
                return -1;
            }

            v = --s->value;
            (void) pthread_mutex_unlock(&s->lock);

            if (v < 0) {
                /* Must wait */
                if (WaitForSingleObject(s->sem, INFINITE) == WAIT_OBJECT_0) {
#if defined(NEED_SEM)
                    if (pthread_mutex_lock(&s->lock) == 0) {
                        if (*sem == NULL) {
                            (void) pthread_mutex_unlock(&s->lock);
                            errno = EINVAL;
                            return -1;
                        }

                        if (s->leftToUnblock > 0) {
                            --s->leftToUnblock;
                            SetEvent(s->sem);
                        }
                        (void) pthread_mutex_unlock(&s->lock);
                    }
#endif
                    return 0;
                }
            } else {
                return 0;
            }
        }
    }

    if (result != 0) {
        errno = result;
        return -1;
    }

    return 0;

} /* ptw32_semwait */
