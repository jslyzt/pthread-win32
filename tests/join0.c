#include "test.h"

void* func(void* arg) {
    Sleep(2000);

    pthread_exit(arg);

    /* Never reached. */
    exit(1);
}

int main(int argc, char* argv[]) {
    pthread_t id;
    void* result = (void*)0;

    /* Create a single thread and wait for it to exit. */
    assert(pthread_create(&id, NULL, func, (void*) 123) == 0);

    assert(pthread_join(id, &result) == 0);

    assert((int)(size_t)result == 123);

    /* Success. */
    return 0;
}
