#include "test.h"

void* func(void* arg) {
    int i = (int)(size_t)arg;

    Sleep(i * 100);

    pthread_exit(arg);

    /* Never reached. */
    exit(1);
}

int main(int argc, char* argv[]) {
    pthread_t id[4];
    int i;
    void* result = (void*) -1;

    /* Create a few threads and then exit. */
    for (i = 0; i < 4; i++) {
        assert(pthread_create(&id[i], NULL, func, (void*)(size_t)i) == 0);
    }

    /* Some threads will finish before they are joined, some after. */
    Sleep(2 * 100 + 50);

    for (i = 0; i < 4; i++) {
        assert(pthread_join(id[i], &result) == 0);
        assert((int)(size_t)result == i);
    }

    /* Success. */
    return 0;
}
