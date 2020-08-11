#include "pthread.h"
#include "implement.h"

static INLINE int ptw32_cond_unblock(pthread_cond_t* cond, int unblockAll)
/*
 * Notes.
 *
 * Does not use the external mutex for synchronisation,
 * therefore semBlockLock is needed.
 * mtxUnblockLock is for LEVEL-2 synch. LEVEL-2 is the
 * state where the external mutex is not necessarily locked by
 * any thread, ie. between cond_wait unlocking and re-acquiring
 * the lock after having been signaled or a timeout or
 * cancellation.
 *
 * Uses the following CV elements:
 *   nWaitersBlocked
 *   nWaitersToUnblock
 *   nWaitersGone
 *   mtxUnblockLock
 *   semBlockLock
 *   semBlockQueue
 */
{
    int result;
    pthread_cond_t cv;
    int nSignalsToIssue;

    if (cond == NULL || *cond == NULL) {
        return EINVAL;
    }

    cv = *cond;

    /*
     * No-op if the CV is static and hasn't been initialised yet.
     * Assuming that any race condition is harmless.
     */
    if (cv == PTHREAD_COND_INITIALIZER) {
        return 0;
    }

    if ((result = pthread_mutex_lock(&(cv->mtxUnblockLock))) != 0) {
        return result;
    }

    if (0 != cv->nWaitersToUnblock) {
        if (0 == cv->nWaitersBlocked) {
            return pthread_mutex_unlock(&(cv->mtxUnblockLock));
        }
        if (unblockAll) {
            cv->nWaitersToUnblock += (nSignalsToIssue = cv->nWaitersBlocked);
            cv->nWaitersBlocked = 0;
        } else {
            nSignalsToIssue = 1;
            cv->nWaitersToUnblock++;
            cv->nWaitersBlocked--;
        }
    } else if (cv->nWaitersBlocked > cv->nWaitersGone) {
        /* Use the non-cancellable version of sem_wait() */
        if (ptw32_semwait(&(cv->semBlockLock)) != 0) {
            result = errno;
            (void) pthread_mutex_unlock(&(cv->mtxUnblockLock));
            return result;
        }
        if (0 != cv->nWaitersGone) {
            cv->nWaitersBlocked -= cv->nWaitersGone;
            cv->nWaitersGone = 0;
        }
        if (unblockAll) {
            nSignalsToIssue = cv->nWaitersToUnblock = cv->nWaitersBlocked;
            cv->nWaitersBlocked = 0;
        } else {
            nSignalsToIssue = cv->nWaitersToUnblock = 1;
            cv->nWaitersBlocked--;
        }
    } else {
        return pthread_mutex_unlock(&(cv->mtxUnblockLock));
    }

    if ((result = pthread_mutex_unlock(&(cv->mtxUnblockLock))) == 0) {
        if (sem_post_multiple(&(cv->semBlockQueue), nSignalsToIssue) != 0) {
            result = errno;
        }
    }

    return result;

} /* ptw32_cond_unblock */

int pthread_cond_signal(pthread_cond_t* cond)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function signals a condition variable, waking
 *      one waiting thread.
 *      If SCHED_FIFO or SCHED_RR policy threads are waiting
 *      the highest priority waiter is awakened; otherwise,
 *      an unspecified waiter is awakened.
 *
 * PARAMETERS
 *      cond
 *              pointer to an instance of pthread_cond_t
 *
 *
 * DESCRIPTION
 *      This function signals a condition variable, waking
 *      one waiting thread.
 *      If SCHED_FIFO or SCHED_RR policy threads are waiting
 *      the highest priority waiter is awakened; otherwise,
 *      an unspecified waiter is awakened.
 *
 *      NOTES:
 *
 *      1)      Use when any waiter can respond and only one need
 *              respond (all waiters being equal).
 *
 * RESULTS
 *              0               successfully signaled condition,
 *              EINVAL          'cond' is invalid,
 *
 * ------------------------------------------------------
 */
{
    /*
     * The '0'(FALSE) unblockAll arg means unblock ONE waiter.
     */
    return (ptw32_cond_unblock(cond, 0));

} /* pthread_cond_signal */

int pthread_cond_broadcast(pthread_cond_t* cond)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function broadcasts the condition variable,
 *      waking all current waiters.
 *
 * PARAMETERS
 *      cond
 *              pointer to an instance of pthread_cond_t
 *
 *
 * DESCRIPTION
 *      This function signals a condition variable, waking
 *      all waiting threads.
 *
 *      NOTES:
 *
 *      1)      Use when more than one waiter may respond to
 *              predicate change or if any waiting thread may
 *              not be able to respond
 *
 * RESULTS
 *              0               successfully signalled condition to all
 *                              waiting threads,
 *              EINVAL          'cond' is invalid
 *              ENOSPC          a required resource has been exhausted,
 *
 * ------------------------------------------------------
 */
{
    /*
     * The TRUE unblockAll arg means unblock ALL waiters.
     */
    return (ptw32_cond_unblock(cond, PTW32_TRUE));

} /* pthread_cond_broadcast */
