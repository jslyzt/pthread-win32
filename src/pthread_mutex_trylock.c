#include "pthread.h"
#include "implement.h"


int pthread_mutex_trylock(pthread_mutex_t* mutex) {
    pthread_mutex_t mx;
    int kind;
    int result = 0;

    /*
     * Let the system deal with invalid pointers.
     */

    /*
     * We do a quick check to see if we need to do more work
     * to initialise a static mutex. We check
     * again inside the guarded section of ptw32_mutex_check_need_init()
     * to avoid race conditions.
     */
    if (*mutex >= PTHREAD_ERRORCHECK_MUTEX_INITIALIZER) {
        if ((result = ptw32_mutex_check_need_init(mutex)) != 0) {
            return (result);
        }
    }

    mx = *mutex;
    kind = mx->kind;

    if (kind >= 0) {
        /* Non-robust */
        if (0 == (PTW32_INTERLOCKED_LONG) PTW32_INTERLOCKED_COMPARE_EXCHANGE_LONG(
                    (PTW32_INTERLOCKED_LONGPTR) &mx->lock_idx,
                    (PTW32_INTERLOCKED_LONG) 1,
                    (PTW32_INTERLOCKED_LONG) 0)) {
            if (kind != PTHREAD_MUTEX_NORMAL) {
                mx->recursive_count = 1;
                mx->ownerThread = pthread_self();
            }
        } else {
            if (kind == PTHREAD_MUTEX_RECURSIVE &&
                    pthread_equal(mx->ownerThread, pthread_self())) {
                mx->recursive_count++;
            } else {
                result = EBUSY;
            }
        }
    } else {
        /*
         * Robust types
         * All types record the current owner thread.
         * The mutex is added to a per thread list when ownership is acquired.
         */
        pthread_t self;
        ptw32_robust_state_t* statePtr = &mx->robustNode->stateInconsistent;

        if ((PTW32_INTERLOCKED_LONG)PTW32_ROBUST_NOTRECOVERABLE ==
                PTW32_INTERLOCKED_EXCHANGE_ADD_LONG(
                    (PTW32_INTERLOCKED_LONGPTR)statePtr,
                    (PTW32_INTERLOCKED_LONG)0)) {
            return ENOTRECOVERABLE;
        }

        self = pthread_self();
        kind = -kind - 1; /* Convert to non-robust range */

        if (0 == (PTW32_INTERLOCKED_LONG) PTW32_INTERLOCKED_COMPARE_EXCHANGE_LONG(
                    (PTW32_INTERLOCKED_LONGPTR) &mx->lock_idx,
                    (PTW32_INTERLOCKED_LONG) 1,
                    (PTW32_INTERLOCKED_LONG) 0)) {
            if (kind != PTHREAD_MUTEX_NORMAL) {
                mx->recursive_count = 1;
            }
            ptw32_robust_mutex_add(mutex, self);
        } else {
            if (PTHREAD_MUTEX_RECURSIVE == kind &&
                    pthread_equal(mx->ownerThread, pthread_self())) {
                mx->recursive_count++;
            } else {
                if (EOWNERDEAD == (result = ptw32_robust_mutex_inherit(mutex))) {
                    mx->recursive_count = 1;
                    ptw32_robust_mutex_add(mutex, self);
                } else {
                    if (0 == result) {
                        result = EBUSY;
                    }
                }
            }
        }
    }

    return (result);
}
