#include "pthread.h"
#include "implement.h"

/*
 * Not needed yet, but defining it should indicate clashes with build target
 * environment that should be fixed.
 */
#if !defined(WINCE)
#  include <signal.h>
#endif


int pthread_detach(pthread_t thread)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function detaches the given thread.
 *
 * PARAMETERS
 *      thread
 *              an instance of a pthread_t
 *
 *
 * DESCRIPTION
 *      This function detaches the given thread. You may use it to
 *      detach the main thread or to detach a joinable thread.
 *      NOTE:   detached threads cannot be joined;
 *              storage is freed immediately on termination.
 *
 * RESULTS
 *              0               successfully detached the thread,
 *              EINVAL          thread is not a joinable thread,
 *              ENOSPC          a required resource has been exhausted,
 *              ESRCH           no thread could be found for 'thread',
 *
 * ------------------------------------------------------
 */
{
    int result;
    BOOL destroyIt = PTW32_FALSE;
    ptw32_thread_t* tp = (ptw32_thread_t*) thread.p;
    ptw32_mcs_local_node_t node;

    ptw32_mcs_lock_acquire(&ptw32_thread_reuse_lock, &node);

    if (NULL == tp
            || thread.x != tp->ptHandle.x) {
        result = ESRCH;
    } else if (PTHREAD_CREATE_DETACHED == tp->detachState) {
        result = EINVAL;
    } else {
        ptw32_mcs_local_node_t stateLock;
        /*
         * Joinable ptw32_thread_t structs are not scavenged until
         * a join or detach is done. The thread may have exited already,
         * but all of the state and locks etc are still there.
         */
        result = 0;

        ptw32_mcs_lock_acquire(&tp->stateLock, &stateLock);
        if (tp->state != PThreadStateLast) {
            tp->detachState = PTHREAD_CREATE_DETACHED;
        } else if (tp->detachState != PTHREAD_CREATE_DETACHED) {
            /*
             * Thread is joinable and has exited or is exiting.
             */
            destroyIt = PTW32_TRUE;
        }
        ptw32_mcs_lock_release(&stateLock);
    }

    ptw32_mcs_lock_release(&node);

    if (result == 0) {
        /* Thread is joinable */

        if (destroyIt) {
            /* The thread has exited or is exiting but has not been joined or
             * detached. Need to wait in case it's still exiting.
             */
            (void) WaitForSingleObject(tp->threadH, INFINITE);
            ptw32_threadDestroy(thread);
        }
    }

    return (result);

} /* pthread_detach */
