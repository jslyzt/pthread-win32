#include "pthread.h"
#include "implement.h"


pthread_t ptw32_new(void) {
    pthread_t t;
    pthread_t nil = {NULL, 0};
    ptw32_thread_t* tp;

    /*
     * If there's a reusable pthread_t then use it.
     */
    t = ptw32_threadReusePop();

    if (NULL != t.p) {
        tp = (ptw32_thread_t*) t.p;
    } else {
        /* No reuse threads available */
        tp = (ptw32_thread_t*) calloc(1, sizeof(ptw32_thread_t));

        if (tp == NULL) {
            return nil;
        }

        /* ptHandle.p needs to point to it's parent ptw32_thread_t. */
        t.p = tp->ptHandle.p = tp;
        t.x = tp->ptHandle.x = 0;
    }

    /* Set default state. */
    tp->seqNumber = ++ptw32_threadSeqNumber;
    tp->sched_priority = THREAD_PRIORITY_NORMAL;
    tp->detachState = PTHREAD_CREATE_JOINABLE;
    tp->cancelState = PTHREAD_CANCEL_ENABLE;
    tp->cancelType = PTHREAD_CANCEL_DEFERRED;
    tp->stateLock = 0;
    tp->threadLock = 0;
    tp->robustMxListLock = 0;
    tp->robustMxList = NULL;
    tp->cancelEvent = CreateEvent(0, (int) PTW32_TRUE,	/* manualReset  */
                                  (int) PTW32_FALSE,	/* setSignaled  */
                                  NULL);

    if (tp->cancelEvent == NULL) {
        ptw32_threadReusePush(tp->ptHandle);
        return nil;
    }

    return t;

}
