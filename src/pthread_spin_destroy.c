#include "pthread.h"
#include "implement.h"


int pthread_spin_destroy(pthread_spinlock_t* lock) {
    register pthread_spinlock_t s;
    int result = 0;

    if (lock == NULL || *lock == NULL) {
        return EINVAL;
    }

    if ((s = *lock) != PTHREAD_SPINLOCK_INITIALIZER) {
        if (s->interlock == PTW32_SPIN_USE_MUTEX) {
            result = pthread_mutex_destroy(&(s->u.mutex));
        } else if ((PTW32_INTERLOCKED_LONG) PTW32_SPIN_UNLOCKED !=
                   PTW32_INTERLOCKED_COMPARE_EXCHANGE_LONG((PTW32_INTERLOCKED_LONGPTR) &s->interlock,
                           (PTW32_INTERLOCKED_LONG) PTW32_SPIN_INVALID,
                           (PTW32_INTERLOCKED_LONG) PTW32_SPIN_UNLOCKED)) {
            result = EINVAL;
        }

        if (0 == result) {
            /*
             * We are relying on the application to ensure that all other threads
             * have finished with the spinlock before destroying it.
             */
            *lock = NULL;
            (void) free(s);
        }
    } else {
        /*
         * See notes in ptw32_spinlock_check_need_init() above also.
         */
        ptw32_mcs_local_node_t node;

        ptw32_mcs_lock_acquire(&ptw32_spinlock_test_init_lock, &node);

        /*
         * Check again.
         */
        if (*lock == PTHREAD_SPINLOCK_INITIALIZER) {
            /*
             * This is all we need to do to destroy a statically
             * initialised spinlock that has not yet been used (initialised).
             * If we get to here, another thread
             * waiting to initialise this mutex will get an EINVAL.
             */
            *lock = NULL;
        } else {
            /*
             * The spinlock has been initialised while we were waiting
             * so assume it's in use.
             */
            result = EBUSY;
        }

        ptw32_mcs_lock_release(&node);
    }

    return (result);
}
