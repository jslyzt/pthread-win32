#include "pthread.h"
#include "implement.h"

/*
 * Not needed yet, but defining it should indicate clashes with build target
 * environment that should be fixed.
 */
#if !defined(WINCE)
#  include <signal.h>
#endif

int pthread_kill(pthread_t thread, int sig)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function requests that a signal be delivered to the
 *      specified thread. If sig is zero, error checking is
 *      performed but no signal is actually sent such that this
 *      function can be used to check for a valid thread ID.
 *
 * PARAMETERS
 *      thread  reference to an instances of pthread_t
 *      sig     signal. Currently only a value of 0 is supported.
 *
 *
 * DESCRIPTION
 *      This function requests that a signal be delivered to the
 *      specified thread. If sig is zero, error checking is
 *      performed but no signal is actually sent such that this
 *      function can be used to check for a valid thread ID.
 *
 * RESULTS
 *              ESRCH           the thread is not a valid thread ID,
 *              EINVAL          the value of the signal is invalid
 *                              or unsupported.
 *              0               the signal was successfully sent.
 *
 * ------------------------------------------------------
 */
{
    int result = 0;
    ptw32_thread_t* tp;
    ptw32_mcs_local_node_t node;

    ptw32_mcs_lock_acquire(&ptw32_thread_reuse_lock, &node);

    tp = (ptw32_thread_t*) thread.p;

    if (NULL == tp
            || thread.x != tp->ptHandle.x
            || NULL == tp->threadH) {
        result = ESRCH;
    }

    ptw32_mcs_lock_release(&node);

    if (0 == result && 0 != sig) {
        /*
         * Currently does not support any signals.
         */
        result = EINVAL;
    }

    return result;

} /* pthread_kill */
