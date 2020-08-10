#include "pthread.h"
#include "implement.h"

/*
 * Notes on handling system time adjustments (especially negative ones).
 * ---------------------------------------------------------------------
 *
 * This solution was suggested by Alexander Terekhov, but any errors
 * in the implementation are mine - [Ross Johnson]
 *
 * 1) The problem: threads doing a timedwait on a CV may expect to timeout
 *    at a specific absolute time according to a system timer. If the
 *    system clock is adjusted backwards then those threads sleep longer than
 *    expected. Also, pthreads-win32 converts absolute times to intervals in
 *    order to make use of the underlying Win32, and so waiting threads may
 *    awake before their proper abstimes.
 *
 * 2) We aren't able to distinquish between threads on timed or untimed waits,
 *    so we wake them all at the time of the adjustment so that they can
 *    re-evaluate their conditions and re-compute their timeouts.
 *
 * 3) We rely on correctly written applications for this to work. Specifically,
 *    they must be able to deal properly with spurious wakeups. That is,
 *    they must re-test their condition upon wakeup and wait again if
 *    the condition is not satisfied.
 */

void* pthread_timechange_handler_np(void* arg)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      Broadcasts all CVs to force re-evaluation and
 *      new timeouts if required.
 *
 * PARAMETERS
 *      NONE
 *
 *
 * DESCRIPTION
 *      Broadcasts all CVs to force re-evaluation and
 *      new timeouts if required.
 *
 *      This routine may be passed directly to pthread_create()
 *      as a new thread in order to run asynchronously.
 *
 *
 * RESULTS
 *              0               successfully broadcast all CVs
 *              EAGAIN          Not all CVs were broadcast
 *
 * ------------------------------------------------------
 */
{
    int result = 0;
    pthread_cond_t cv;
    ptw32_mcs_local_node_t node;

    ptw32_mcs_lock_acquire(&ptw32_cond_list_lock, &node);

    cv = ptw32_cond_list_head;

    while (cv != NULL && 0 == result) {
        result = pthread_cond_broadcast(&cv);
        cv = cv->next;
    }

    ptw32_mcs_lock_release(&node);

    return (void*)(size_t)(result != 0 ? EAGAIN : 0);
}
