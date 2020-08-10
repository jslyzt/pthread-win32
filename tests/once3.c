#include "test.h"

#define NUM_THREADS 100 /* Targeting each once control */
#define NUM_ONCE    10

static pthread_once_t o = PTHREAD_ONCE_INIT;
static pthread_once_t once[NUM_ONCE];

typedef struct {
    int i;
    CRITICAL_SECTION cs;
} sharedInt_t;

static sharedInt_t numOnce = {0, {0}};
static sharedInt_t numThreads = {0, {0}};

void myfunc(void) {
    EnterCriticalSection(&numOnce.cs);
    numOnce.i++;
    assert(numOnce.i > 0);
    LeaveCriticalSection(&numOnce.cs);
    /* Simulate slow once routine so that following threads pile up behind it */
    Sleep(10);
    /* test for cancelation late so we're sure to have waiters. */
    pthread_testcancel();
}

void* mythread(void* arg) {
    /*
     * Cancel every thread. These threads are deferred cancelable only, so
     * only the thread performing the once routine (my_func) will see it (there are
     * no other cancelation points here). The result will be that every thread
     * eventually cancels only when it becomes the new 'once' thread.
     */
    assert(pthread_cancel(pthread_self()) == 0);
    assert(pthread_once(&once[(int)(size_t)arg], myfunc) == 0);
    EnterCriticalSection(&numThreads.cs);
    numThreads.i++;
    LeaveCriticalSection(&numThreads.cs);
    return (void*)(size_t)0;
}

int main() {
    pthread_t t[NUM_THREADS][NUM_ONCE];
    int i, j;

    InitializeCriticalSection(&numThreads.cs);
    InitializeCriticalSection(&numOnce.cs);

    for (j = 0; j < NUM_ONCE; j++) {
        once[j] = o;

        for (i = 0; i < NUM_THREADS; i++) {
            /* GCC build: create was failing with EAGAIN after 790 threads */
            while (0 != pthread_create(&t[i][j], NULL, mythread, (void*)(size_t)j)) {
                sched_yield();
            }
        }
    }

    for (j = 0; j < NUM_ONCE; j++)
        for (i = 0; i < NUM_THREADS; i++)
            if (pthread_join(t[i][j], NULL) != 0) {
                printf("Join failed for [thread,once] = [%d,%d]\n", i, j);
            }

    /*
     * All threads will cancel, none will return normally from
     * pthread_once and so numThreads should never be incremented. However,
     * numOnce should be incremented by every thread (NUM_THREADS*NUM_ONCE).
     */
    assert(numOnce.i == NUM_ONCE * NUM_THREADS);
    assert(numThreads.i == 0);

    DeleteCriticalSection(&numOnce.cs);
    DeleteCriticalSection(&numThreads.cs);

    return 0;
}
