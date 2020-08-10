#include "test.h"


enum {
    NUMTHREADS = 100
};

void* func(void* arg) {
    int i = (int)(size_t)arg;

    Sleep(i * 10);

    pthread_exit(arg);

    /* Never reached. */
    exit(1);
}

int main(int argc, char* argv[]) {
    pthread_t id[NUMTHREADS];
    int i;

    /* Create a few threads and then exit. */
    for (i = 0; i < NUMTHREADS; i++) {
        assert(pthread_create(&id[i], NULL, func, (void*)(size_t)i) == 0);
    }

    /* Some threads will finish before they are detached, some after. */
    Sleep(NUMTHREADS / 2 * 10 + 50);

    for (i = 0; i < NUMTHREADS; i++) {
        assert(pthread_detach(id[i]) == 0);
    }

    Sleep(NUMTHREADS * 10 + 100);

    /*
     * Check that all threads are now invalid.
     * This relies on unique thread IDs - e.g. works with
     * pthreads-w32 or Solaris, but may not work for Linux, BSD etc.
     */
    for (i = 0; i < NUMTHREADS; i++) {
        assert(pthread_kill(id[i], 0) == ESRCH);
    }

    /* Success. */
    return 0;
}
