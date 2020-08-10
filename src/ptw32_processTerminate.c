#include "pthread.h"
#include "implement.h"


void ptw32_processTerminate(void)
/*
 * ------------------------------------------------------
 * DOCPRIVATE
 *      This function performs process wide termination for
 *      the pthread library.
 *
 * PARAMETERS
 *      N/A
 *
 * DESCRIPTION
 *      This function performs process wide termination for
 *      the pthread library.
 *      This routine sets the global variable
 *      ptw32_processInitialized to FALSE
 *
 * RESULTS
 *              N/A
 *
 * ------------------------------------------------------
 */
{
    if (ptw32_processInitialized) {
        ptw32_thread_t* tp, * tpNext;
        ptw32_mcs_local_node_t node;

        if (ptw32_selfThreadKey != NULL) {
            /*
             * Release ptw32_selfThreadKey
             */
            pthread_key_delete(ptw32_selfThreadKey);

            ptw32_selfThreadKey = NULL;
        }

        if (ptw32_cleanupKey != NULL) {
            /*
             * Release ptw32_cleanupKey
             */
            pthread_key_delete(ptw32_cleanupKey);

            ptw32_cleanupKey = NULL;
        }

        ptw32_mcs_lock_acquire(&ptw32_thread_reuse_lock, &node);

        tp = ptw32_threadReuseTop;
        while (tp != PTW32_THREAD_REUSE_EMPTY) {
            tpNext = tp->prevReuse;
            free(tp);
            tp = tpNext;
        }

        ptw32_mcs_lock_release(&node);

        ptw32_processInitialized = PTW32_FALSE;
    }

} /* processTerminate */
