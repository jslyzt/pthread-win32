#include "pthread.h"
#include "implement.h"


void ptw32_threadDestroy(pthread_t thread) {
    ptw32_thread_t* tp = (ptw32_thread_t*) thread.p;
    ptw32_thread_t threadCopy;

    if (tp != NULL) {
        /*
         * Copy thread state so that the thread can be atomically NULLed.
         */
        memcpy(&threadCopy, tp, sizeof(threadCopy));

        /*
         * Thread ID structs are never freed. They're NULLed and reused.
         * This also sets the thread to PThreadStateInitial (invalid).
         */
        ptw32_threadReusePush(thread);

        /* Now work on the copy. */
        if (threadCopy.cancelEvent != NULL) {
            CloseHandle(threadCopy.cancelEvent);
        }

#if ! (defined(__MINGW64__) || defined(__MINGW32__)) || defined (__MSVCRT__) || defined (__DMC__)
        /*
         * See documentation for endthread vs endthreadex.
         */
        if (threadCopy.threadH != 0) {
            CloseHandle(threadCopy.threadH);
        }
#endif

    }
} /* ptw32_threadDestroy */

