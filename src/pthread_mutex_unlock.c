#include "pthread.h"
#include "implement.h"


int pthread_mutex_unlock(pthread_mutex_t* mutex) {
    int result = 0;
    int kind;
    pthread_mutex_t mx;

    /*
     * Let the system deal with invalid pointers.
     */

    mx = *mutex;

    /*
     * If the thread calling us holds the mutex then there is no
     * race condition. If another thread holds the
     * lock then we shouldn't be in here.
     */
    if (mx < PTHREAD_ERRORCHECK_MUTEX_INITIALIZER) {
        kind = mx->kind;

        if (kind >= 0) {
            if (kind == PTHREAD_MUTEX_NORMAL) {
                LONG idx;

                idx = (LONG) PTW32_INTERLOCKED_EXCHANGE_LONG((PTW32_INTERLOCKED_LONGPTR)&mx->lock_idx,
                        (PTW32_INTERLOCKED_LONG)0);
                if (idx != 0) {
                    if (idx < 0) {
                        /*
                         * Someone may be waiting on that mutex.
                         */
                        if (SetEvent(mx->event) == 0) {
                            result = EINVAL;
                        }
                    }
                }
            } else {
                if (pthread_equal(mx->ownerThread, pthread_self())) {
                    if (kind != PTHREAD_MUTEX_RECURSIVE
                            || 0 == --mx->recursive_count) {
                        mx->ownerThread.p = NULL;

                        if ((LONG) PTW32_INTERLOCKED_EXCHANGE_LONG((PTW32_INTERLOCKED_LONGPTR)&mx->lock_idx,
                                (PTW32_INTERLOCKED_LONG)0) < 0L) {
                            /* Someone may be waiting on that mutex */
                            if (SetEvent(mx->event) == 0) {
                                result = EINVAL;
                            }
                        }
                    }
                } else {
                    result = EPERM;
                }
            }
        } else {
            /* Robust types */
            pthread_t self = pthread_self();
            kind = -kind - 1; /* Convert to non-robust range */

            /*
             * The thread must own the lock regardless of type if the mutex
             * is robust.
             */
            if (pthread_equal(mx->ownerThread, self)) {
                PTW32_INTERLOCKED_COMPARE_EXCHANGE_LONG((PTW32_INTERLOCKED_LONGPTR) &mx->robustNode->stateInconsistent,
                                                        (PTW32_INTERLOCKED_LONG)PTW32_ROBUST_NOTRECOVERABLE,
                                                        (PTW32_INTERLOCKED_LONG)PTW32_ROBUST_INCONSISTENT);
                if (PTHREAD_MUTEX_NORMAL == kind) {
                    ptw32_robust_mutex_remove(mutex, NULL);

                    if ((LONG) PTW32_INTERLOCKED_EXCHANGE_LONG((PTW32_INTERLOCKED_LONGPTR) &mx->lock_idx,
                            (PTW32_INTERLOCKED_LONG) 0) < 0) {
                        /*
                         * Someone may be waiting on that mutex.
                         */
                        if (SetEvent(mx->event) == 0) {
                            result = EINVAL;
                        }
                    }
                } else {
                    if (kind != PTHREAD_MUTEX_RECURSIVE
                            || 0 == --mx->recursive_count) {
                        ptw32_robust_mutex_remove(mutex, NULL);

                        if ((LONG) PTW32_INTERLOCKED_EXCHANGE_LONG((PTW32_INTERLOCKED_LONGPTR) &mx->lock_idx,
                                (PTW32_INTERLOCKED_LONG) 0) < 0) {
                            /*
                             * Someone may be waiting on that mutex.
                             */
                            if (SetEvent(mx->event) == 0) {
                                result = EINVAL;
                            }
                        }
                    }
                }
            } else {
                result = EPERM;
            }
        }
    } else if (mx != PTHREAD_MUTEX_INITIALIZER) {
        result = EINVAL;
    }

    return (result);
}
