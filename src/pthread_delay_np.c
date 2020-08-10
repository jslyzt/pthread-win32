#include "pthread.h"
#include "implement.h"

/*
 * pthread_delay_np
 *
 * DESCRIPTION
 *
 *       This routine causes a thread to delay execution for a specific period of time.
 *       This period ends at the current time plus the specified interval. The routine
 *       will not return before the end of the period is reached, but may return an
 *       arbitrary amount of time after the period has gone by. This can be due to
 *       system load, thread priorities, and system timer granularity.
 *
 *       Specifying an interval of zero (0) seconds and zero (0) nanoseconds is
 *       allowed and can be used to force the thread to give up the processor or to
 *       deliver a pending cancelation request.
 *
 *       The timespec structure contains the following two fields:
 *
 *            tv_sec is an integer number of seconds.
 *            tv_nsec is an integer number of nanoseconds.
 *
 *  Return Values
 *
 *  If an error condition occurs, this routine returns an integer value indicating
 *  the type of error. Possible return values are as follows:
 *
 *  0
 *           Successful completion.
 *  [EINVAL]
 *           The value specified by interval is invalid.
 *
 * Example
 *
 * The following code segment would wait for 5 and 1/2 seconds
 *
 *  struct timespec tsWait;
 *  int      intRC;
 *
 *  tsWait.tv_sec  = 5;
 *  tsWait.tv_nsec = 500000000L;
 *  intRC = pthread_delay_np(&tsWait);
 */
int pthread_delay_np(struct timespec* interval) {
    DWORD wait_time;
    DWORD secs_in_millisecs;
    DWORD millisecs;
    DWORD status;
    pthread_t self;
    ptw32_thread_t* sp;

    if (interval == NULL) {
        return EINVAL;
    }

    if (interval->tv_sec == 0L && interval->tv_nsec == 0L) {
        pthread_testcancel();
        Sleep(0);
        pthread_testcancel();
        return (0);
    }

    /* convert secs to millisecs */
    secs_in_millisecs = (DWORD)interval->tv_sec * 1000L;

    /* convert nanosecs to millisecs (rounding up) */
    millisecs = (interval->tv_nsec + 999999L) / 1000000L;

#if defined(__WATCOMC__)
#pragma disable_message (124)
#endif

    /*
     * Most compilers will issue a warning 'comparison always 0'
     * because the variable type is unsigned, but we need to keep this
     * for some reason I can't recall now.
     */
    if (0 > (wait_time = secs_in_millisecs + millisecs)) {
        return EINVAL;
    }

#if defined(__WATCOMC__)
#pragma enable_message (124)
#endif

    if (NULL == (self = pthread_self()).p) {
        return ENOMEM;
    }

    sp = (ptw32_thread_t*) self.p;

    if (sp->cancelState == PTHREAD_CANCEL_ENABLE) {
        /*
         * Async cancelation won't catch us until wait_time is up.
         * Deferred cancelation will cancel us immediately.
         */
        if (WAIT_OBJECT_0 ==
                (status = WaitForSingleObject(sp->cancelEvent, wait_time))) {
            ptw32_mcs_local_node_t stateLock;
            /*
             * Canceling!
             */
            ptw32_mcs_lock_acquire(&sp->stateLock, &stateLock);
            if (sp->state < PThreadStateCanceling) {
                sp->state = PThreadStateCanceling;
                sp->cancelState = PTHREAD_CANCEL_DISABLE;
                ptw32_mcs_lock_release(&stateLock);

                ptw32_throw(PTW32_EPS_CANCEL);
            }

            ptw32_mcs_lock_release(&stateLock);
            return ESRCH;
        } else if (status != WAIT_TIMEOUT) {
            return EINVAL;
        }
    } else {
        Sleep(wait_time);
    }

    return (0);
}
