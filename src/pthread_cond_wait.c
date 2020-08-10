#include "pthread.h"
#include "implement.h"

/*
 * Arguments for cond_wait_cleanup, since we can only pass a
 * single void * to it.
 */
typedef struct {
    pthread_mutex_t* mutexPtr;
    pthread_cond_t cv;
    int* resultPtr;
} ptw32_cond_wait_cleanup_args_t;

static void PTW32_CDECL
ptw32_cond_wait_cleanup(void* args) {
    ptw32_cond_wait_cleanup_args_t* cleanup_args =
        (ptw32_cond_wait_cleanup_args_t*) args;
    pthread_cond_t cv = cleanup_args->cv;
    int* resultPtr = cleanup_args->resultPtr;
    int nSignalsWasLeft;
    int result;

    /*
     * Whether we got here as a result of signal/broadcast or because of
     * timeout on wait or thread cancellation we indicate that we are no
     * longer waiting. The waiter is responsible for adjusting waiters
     * (to)unblock(ed) counts (protected by unblock lock).
     */
    if ((result = pthread_mutex_lock(&(cv->mtxUnblockLock))) != 0) {
        *resultPtr = result;
        return;
    }

    if (0 != (nSignalsWasLeft = cv->nWaitersToUnblock)) {
        --(cv->nWaitersToUnblock);
    } else if (INT_MAX / 2 == ++(cv->nWaitersGone)) {
        /* Use the non-cancellable version of sem_wait() */
        if (ptw32_semwait(&(cv->semBlockLock)) != 0) {
            *resultPtr = errno;
            /*
             * This is a fatal error for this CV,
             * so we deliberately don't unlock
             * cv->mtxUnblockLock before returning.
             */
            return;
        }
        cv->nWaitersBlocked -= cv->nWaitersGone;
        if (sem_post(&(cv->semBlockLock)) != 0) {
            *resultPtr = errno;
            /*
             * This is a fatal error for this CV,
             * so we deliberately don't unlock
             * cv->mtxUnblockLock before returning.
             */
            return;
        }
        cv->nWaitersGone = 0;
    }

    if ((result = pthread_mutex_unlock(&(cv->mtxUnblockLock))) != 0) {
        *resultPtr = result;
        return;
    }

    if (1 == nSignalsWasLeft) {
        if (sem_post(&(cv->semBlockLock)) != 0) {
            *resultPtr = errno;
            return;
        }
    }

    /*
     * XSH: Upon successful return, the mutex has been locked and is owned
     * by the calling thread.
     */
    if ((result = pthread_mutex_lock(cleanup_args->mutexPtr)) != 0) {
        *resultPtr = result;
    }
}				/* ptw32_cond_wait_cleanup */

static INLINE int
ptw32_cond_timedwait(pthread_cond_t* cond,
                     pthread_mutex_t* mutex, const struct timespec* abstime) {
    int result = 0;
    pthread_cond_t cv;
    ptw32_cond_wait_cleanup_args_t cleanup_args;

    if (cond == NULL || *cond == NULL) {
        return EINVAL;
    }

    /*
     * We do a quick check to see if we need to do more work
     * to initialise a static condition variable. We check
     * again inside the guarded section of ptw32_cond_check_need_init()
     * to avoid race conditions.
     */
    if (*cond == PTHREAD_COND_INITIALIZER) {
        result = ptw32_cond_check_need_init(cond);
    }

    if (result != 0 && result != EBUSY) {
        return result;
    }

    cv = *cond;

    /* Thread can be cancelled in sem_wait() but this is OK */
    if (sem_wait(&(cv->semBlockLock)) != 0) {
        return errno;
    }

    ++(cv->nWaitersBlocked);

    if (sem_post(&(cv->semBlockLock)) != 0) {
        return errno;
    }

    /*
     * Setup this waiter cleanup handler
     */
    cleanup_args.mutexPtr = mutex;
    cleanup_args.cv = cv;
    cleanup_args.resultPtr = &result;

#if defined(_MSC_VER) && _MSC_VER < 1400
#pragma inline_depth(0)
#endif
    pthread_cleanup_push(ptw32_cond_wait_cleanup, (void*) &cleanup_args);

    /*
     * Now we can release 'mutex' and...
     */
    if ((result = pthread_mutex_unlock(mutex)) == 0) {

        /*
         * ...wait to be awakened by
         *              pthread_cond_signal, or
         *              pthread_cond_broadcast, or
         *              timeout, or
         *              thread cancellation
         *
         * Note:
         *
         *      sem_timedwait is a cancellation point,
         *      hence providing the mechanism for making
         *      pthread_cond_wait a cancellation point.
         *      We use the cleanup mechanism to ensure we
         *      re-lock the mutex and adjust (to)unblock(ed) waiters
         *      counts if we are cancelled, timed out or signalled.
         */
        if (sem_timedwait(&(cv->semBlockQueue), abstime) != 0) {
            result = errno;
        }
    }

    /*
     * Always cleanup
     */
    pthread_cleanup_pop(1);
#if defined(_MSC_VER) && _MSC_VER < 1400
#pragma inline_depth()
#endif

    /*
     * "result" can be modified by the cleanup handler.
     */
    return result;

} /* ptw32_cond_timedwait */


int pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function waits on a condition variable until
 *      awakened by a signal or broadcast.
 *
 *      Caller MUST be holding the mutex lock; the
 *      lock is released and the caller is blocked waiting
 *      on 'cond'. When 'cond' is signaled, the mutex
 *      is re-acquired before returning to the caller.
 *
 * PARAMETERS
 *      cond
 *              pointer to an instance of pthread_cond_t
 *
 *      mutex
 *              pointer to an instance of pthread_mutex_t
 *
 *
 * DESCRIPTION
 *      This function waits on a condition variable until
 *      awakened by a signal or broadcast.
 *
 *      NOTES:
 *
 *      1)      The function must be called with 'mutex' LOCKED
 *              by the calling thread, or undefined behaviour
 *              will result.
 *
 *      2)      This routine atomically releases 'mutex' and causes
 *              the calling thread to block on the condition variable.
 *              The blocked thread may be awakened by
 *                      pthread_cond_signal or
 *                      pthread_cond_broadcast.
 *
 * Upon successful completion, the 'mutex' has been locked and
 * is owned by the calling thread.
 *
 *
 * RESULTS
 *              0               caught condition; mutex released,
 *              EINVAL          'cond' or 'mutex' is invalid,
 *              EINVAL          different mutexes for concurrent waits,
 *              EINVAL          mutex is not held by the calling thread,
 *
 * ------------------------------------------------------
 */
{
    /*
     * The NULL abstime arg means INFINITE waiting.
     */
    return (ptw32_cond_timedwait(cond, mutex, NULL));

} /* pthread_cond_wait */


int pthread_cond_timedwait(pthread_cond_t* cond,
                       pthread_mutex_t* mutex,
                       const struct timespec* abstime)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function waits on a condition variable either until
 *      awakened by a signal or broadcast; or until the time
 *      specified by abstime passes.
 *
 * PARAMETERS
 *      cond
 *              pointer to an instance of pthread_cond_t
 *
 *      mutex
 *              pointer to an instance of pthread_mutex_t
 *
 *      abstime
 *              pointer to an instance of (const struct timespec)
 *
 *
 * DESCRIPTION
 *      This function waits on a condition variable either until
 *      awakened by a signal or broadcast; or until the time
 *      specified by abstime passes.
 *
 *      NOTES:
 *      1)      The function must be called with 'mutex' LOCKED
 *              by the calling thread, or undefined behaviour
 *              will result.
 *
 *      2)      This routine atomically releases 'mutex' and causes
 *              the calling thread to block on the condition variable.
 *              The blocked thread may be awakened by
 *                      pthread_cond_signal or
 *                      pthread_cond_broadcast.
 *
 *
 * RESULTS
 *              0               caught condition; mutex released,
 *              EINVAL          'cond', 'mutex', or abstime is invalid,
 *              EINVAL          different mutexes for concurrent waits,
 *              EINVAL          mutex is not held by the calling thread,
 *              ETIMEDOUT       abstime ellapsed before cond was signaled.
 *
 * ------------------------------------------------------
 */
{
    if (abstime == NULL) {
        return EINVAL;
    }

    return (ptw32_cond_timedwait(cond, mutex, abstime));

} /* pthread_cond_timedwait */
