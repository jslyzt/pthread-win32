#include "test.h"

int main() {
    pthread_t NullThread = PTW32_THREAD_NULL_ID;

    assert(pthread_kill(NullThread, 0) == ESRCH);

    return 0;
}
