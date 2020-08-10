#include "test.h"
#ifndef _UWIN
#include <process.h>
#endif

/*
 * Create NUMTHREADS threads in addition to the Main thread.
 */
enum {
    NUMTHREADS = 4
};

typedef struct bag_t_ bag_t;
struct bag_t_ {
    int threadnum;
    int started;
    /* Add more per-thread state variables here */
    int count;
    pthread_t self;
};

static bag_t threadbag[NUMTHREADS + 1];

#if ! defined (__MINGW32__) || defined (__MSVCRT__)
unsigned __stdcall
#else
void
#endif
Win32thread(void* arg) {
    int i;
    bag_t* bag = (bag_t*) arg;

    assert(bag == &threadbag[bag->threadnum]);
    assert(bag->started == 0);
    bag->started = 1;

    assert((bag->self = pthread_self()).p != NULL);
    assert(pthread_kill(bag->self, 0) == 0);

    for (i = 0; i < 100; i++) {
        Sleep(100);
        pthread_testcancel();
    }

#if ! defined (__MINGW32__) || defined (__MSVCRT__)
    return 0;
#endif
}

int main() {
    int failed = 0;
    int i;
    HANDLE h[NUMTHREADS + 1];
    unsigned thrAddr; /* Dummy variable to pass a valid location to _beginthreadex (Win98). */

    for (i = 1; i <= NUMTHREADS; i++) {
        threadbag[i].started = 0;
        threadbag[i].threadnum = i;
#if ! defined (__MINGW32__) || defined (__MSVCRT__)
        h[i] = (HANDLE) _beginthreadex(NULL, 0, Win32thread, (void*) &threadbag[i], 0, &thrAddr);
#else
        h[i] = (HANDLE) _beginthread(Win32thread, 0, (void*) &threadbag[i]);
#endif
    }

    /*
     * Code to control or munipulate child threads should probably go here.
     */
    Sleep(500);

    /*
     * Cancel all threads.
     */
    for (i = 1; i <= NUMTHREADS; i++) {
        assert(pthread_kill(threadbag[i].self, 0) == 0);
        assert(pthread_cancel(threadbag[i].self) == 0);
    }

    /*
     * Give threads time to run.
     */
    Sleep(NUMTHREADS * 100);

    /*
     * Standard check that all threads started.
     */
    for (i = 1; i <= NUMTHREADS; i++) {
        if (!threadbag[i].started) {
            failed |= !threadbag[i].started;
            fprintf(stderr, "Thread %d: started %d\n", i, threadbag[i].started);
        }
    }

    assert(!failed);

    /*
     * Check any results here. Set "failed" and only print output on failure.
     */
    failed = 0;
    for (i = 1; i <= NUMTHREADS; i++) {
        int fail = 0;
        int result = 0;

#if ! defined (__MINGW32__) || defined (__MSVCRT__)
        assert(GetExitCodeThread(h[i], (LPDWORD) &result) == TRUE);
#else
        /*
         * Can't get a result code.
         */
        result = (int)(size_t)PTHREAD_CANCELED;
#endif

        assert(threadbag[i].self.p != NULL);
        assert(pthread_kill(threadbag[i].self, 0) == ESRCH);

        fail = (result != (int)(size_t)PTHREAD_CANCELED);

        if (fail) {
            fprintf(stderr, "Thread %d: started %d: count %d\n",
                    i,
                    threadbag[i].started,
                    threadbag[i].count);
        }
        failed = (failed || fail);
    }

    assert(!failed);

    /*
     * Success.
     */
    return 0;
}

