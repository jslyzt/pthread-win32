#include "pthread.h"
#include "implement.h"


int pthread_setcanceltype(int type, int* oldtype)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function atomically sets the calling thread's
 *      cancelability type to 'type' and returns the previous
 *      cancelability type at the location referenced by
 *      'oldtype'
 *
 * PARAMETERS
 *      type,
 *      oldtype
 *              PTHREAD_CANCEL_DEFERRED
 *                      only deferred cancelation is allowed,
 *
 *              PTHREAD_CANCEL_ASYNCHRONOUS
 *                      Asynchronous cancellation is allowed
 *
 *
 * DESCRIPTION
 *      This function atomically sets the calling thread's
 *      cancelability type to 'type' and returns the previous
 *      cancelability type at the location referenced by
 *      'oldtype'
 *
 *      NOTES:
 *      1)      Use with caution; most code is not safe for use
 *              with asynchronous cancelability.
 *
 * COMPATIBILITY ADDITIONS
 *      If 'oldtype' is NULL then the previous type is not returned
 *      but the function still succeeds. (Solaris)
 *
 * RESULTS
 *              0               successfully set cancelability type,
 *              EINVAL          'type' is invalid
 *
 * ------------------------------------------------------
 */
{
    ptw32_mcs_local_node_t stateLock;
    int result = 0;
    pthread_t self = pthread_self();
    ptw32_thread_t* sp = (ptw32_thread_t*) self.p;

    if (sp == NULL
            || (type != PTHREAD_CANCEL_DEFERRED
                && type != PTHREAD_CANCEL_ASYNCHRONOUS)) {
        return EINVAL;
    }

    /*
     * Lock for async-cancel safety.
     */
    ptw32_mcs_lock_acquire(&sp->stateLock, &stateLock);

    if (oldtype != NULL) {
        *oldtype = sp->cancelType;
    }

    sp->cancelType = type;

    /*
     * Check if there is a pending asynchronous cancel
     */
    if (sp->cancelState == PTHREAD_CANCEL_ENABLE
            && type == PTHREAD_CANCEL_ASYNCHRONOUS
            && WaitForSingleObject(sp->cancelEvent, 0) == WAIT_OBJECT_0) {
        sp->state = PThreadStateCanceling;
        sp->cancelState = PTHREAD_CANCEL_DISABLE;
        ResetEvent(sp->cancelEvent);
        ptw32_mcs_lock_release(&stateLock);
        ptw32_throw(PTW32_EPS_CANCEL);

        /* Never reached */
    }

    ptw32_mcs_lock_release(&stateLock);

    return (result);

} /* pthread_setcanceltype */
