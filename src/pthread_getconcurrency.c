#include "pthread.h"
#include "implement.h"

int pthread_getconcurrency(void) {
    return ptw32_concurrency;
}
