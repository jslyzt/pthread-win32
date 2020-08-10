#include "test.h"

void* func(void* arg) {
    sched_yield();
    return arg;
}

int main(int argc, char* argv[]) {
    pthread_t id[4];
    int i;
    void* result = (void*) -1;

    /* Create a few threads and then exit. */
    for (i = 0; i < 4; i++) {
        assert(pthread_create(&id[i], NULL, func, (void*)(size_t)i) == 0);
    }

    /*
     * Let threads exit before we join them.
     * We should still retrieve the exit code for the threads.
     */
    Sleep(1000);

    for (i = 0; i < 4; i++) {
        assert(pthread_join(id[i], &result) == 0);
        assert((int)(size_t)result == i);
    }

    /* Success. */
    return 0;
}
