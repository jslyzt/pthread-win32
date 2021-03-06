#include "pthread.h"
#include "implement.h"


int pthread_mutex_destroy(pthread_mutex_t* mutex) {
    int result = 0;
    pthread_mutex_t mx;

    /*
     * Let the system deal with invalid pointers.
     */

    /*
     * Check to see if we have something to delete.
     */
    if (*mutex < PTHREAD_ERRORCHECK_MUTEX_INITIALIZER) {
        mx = *mutex;

        result = pthread_mutex_trylock(&mx);

        /*
         * If trylock succeeded and the mutex is not recursively locked it
         * can be destroyed.
         */
        if (0 == result || ENOTRECOVERABLE == result) {
            if (mx->kind != PTHREAD_MUTEX_RECURSIVE || 1 == mx->recursive_count) {
                /*
                 * FIXME!!!
                 * The mutex isn't held by another thread but we could still
                 * be too late invalidating the mutex below since another thread
                 * may already have entered mutex_lock and the check for a valid
                 * *mutex != NULL.
                 */
                *mutex = NULL;

                result = (0 == result) ? pthread_mutex_unlock(&mx) : 0;

                if (0 == result) {
                    if (mx->robustNode != NULL) {
                        free(mx->robustNode);
                    }
                    if (!CloseHandle(mx->event)) {
                        *mutex = mx;
                        result = EINVAL;
                    } else {
                        free(mx);
                    }
                } else {
                    /*
                     * Restore the mutex before we return the error.
                     */
                    *mutex = mx;
                }
            } else {		/* mx->recursive_count > 1 */
                /*
                 * The mutex must be recursive and already locked by us (this thread).
                 */
                mx->recursive_count--;	/* Undo effect of pthread_mutex_trylock() above */
                result = EBUSY;
            }
        }
    } else {
        ptw32_mcs_local_node_t node;

        /*
         * See notes in ptw32_mutex_check_need_init() above also.
         */

        ptw32_mcs_lock_acquire(&ptw32_mutex_test_init_lock, &node);

        /*
         * Check again.
         */
        if (*mutex >= PTHREAD_ERRORCHECK_MUTEX_INITIALIZER) {
            /*
             * This is all we need to do to destroy a statically
             * initialised mutex that has not yet been used (initialised).
             * If we get to here, another thread
             * waiting to initialise this mutex will get an EINVAL.
             */
            *mutex = NULL;
        } else {
            /*
             * The mutex has been initialised while we were waiting
             * so assume it's in use.
             */
            result = EBUSY;
        }
        ptw32_mcs_lock_release(&node);
    }

    return (result);
}
