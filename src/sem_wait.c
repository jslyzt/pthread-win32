#include "pthread.h"
#include "semaphore.h"
#include "implement.h"


static void PTW32_CDECL ptw32_sem_wait_cleanup(void* sem) {
    sem_t s = (sem_t) sem;

    if (pthread_mutex_lock(&s->lock) == 0) {
        /*
         * If sema is destroyed do nothing, otherwise:-
         * If the sema is posted between us being cancelled and us locking
         * the sema again above then we need to consume that post but cancel
         * anyway. If we don't get the semaphore we indicate that we're no
         * longer waiting.
         */
        if (*((sem_t*)sem) != NULL && !(WaitForSingleObject(s->sem, 0) == WAIT_OBJECT_0)) {
            ++s->value;
#if defined(NEED_SEM)
            if (s->value > 0) {
                s->leftToUnblock = 0;
            }
#else
            /*
             * Don't release the W32 sema, it doesn't need adjustment
             * because it doesn't record the number of waiters.
             */
#endif /* NEED_SEM */
        }
        (void) pthread_mutex_unlock(&s->lock);
    }
}

int sem_wait(sem_t* sem)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function  waits on a semaphore.
 *
 * PARAMETERS
 *      sem
 *              pointer to an instance of sem_t
 *
 * DESCRIPTION
 *      This function waits on a semaphore. If the
 *      semaphore value is greater than zero, it decreases
 *      its value by one. If the semaphore value is zero, then
 *      the calling thread (or process) is blocked until it can
 *      successfully decrease the value or until interrupted by
 *      a signal.
 *
 * RESULTS
 *              0               successfully decreased semaphore,
 *              -1              failed, error in errno
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

    pthread_testcancel();

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
#if defined(_MSC_VER) && _MSC_VER < 1400
#pragma inline_depth(0)
#endif
                /* Must wait */
                pthread_cleanup_push(ptw32_sem_wait_cleanup, (void*) s);
                result = pthreadCancelableWait(s->sem);
                /* Cleanup if we're canceled or on any other error */
                pthread_cleanup_pop(result);
#if defined(_MSC_VER) && _MSC_VER < 1400
#pragma inline_depth()
#endif
            }
#if defined(NEED_SEM)

            if (!result && pthread_mutex_lock(&s->lock) == 0) {
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

#endif /* NEED_SEM */

        }

    }

    if (result != 0) {
        errno = result;
        return -1;
    }

    return 0;

} /* sem_wait */
