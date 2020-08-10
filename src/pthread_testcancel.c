#include "pthread.h"
#include "implement.h"


void pthread_testcancel(void)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function creates a deferred cancellation point
 *      in the calling thread. The call has no effect if the
 *      current cancelability state is
 *              PTHREAD_CANCEL_DISABLE
 *
 * PARAMETERS
 *      N/A
 *
 *
 * DESCRIPTION
 *      This function creates a deferred cancellation point
 *      in the calling thread. The call has no effect if the
 *      current cancelability state is
 *              PTHREAD_CANCEL_DISABLE
 *
 *      NOTES:
 *      1)      Cancellation is asynchronous. Use pthread_join
 *              to wait for termination of thread if necessary
 *
 * RESULTS
 *              N/A
 *
 * ------------------------------------------------------
 */
{
    ptw32_mcs_local_node_t stateLock;
    pthread_t self = pthread_self();
    ptw32_thread_t* sp = (ptw32_thread_t*) self.p;

    if (sp == NULL) {
        return;
    }

    /*
     * Pthread_cancel() will have set sp->state to PThreadStateCancelPending
     * and set an event, so no need to enter kernel space if
     * sp->state != PThreadStateCancelPending - that only slows us down.
     */
    if (sp->state != PThreadStateCancelPending) {
        return;
    }

    ptw32_mcs_lock_acquire(&sp->stateLock, &stateLock);

    if (sp->cancelState != PTHREAD_CANCEL_DISABLE) {
        ResetEvent(sp->cancelEvent);
        sp->state = PThreadStateCanceling;
        sp->cancelState = PTHREAD_CANCEL_DISABLE;
        ptw32_mcs_lock_release(&stateLock);
        ptw32_throw(PTW32_EPS_CANCEL);
        /* Never returns here */
    }

    ptw32_mcs_lock_release(&stateLock);
} /* pthread_testcancel */
