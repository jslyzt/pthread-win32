#include "pthread.h"
#include "implement.h"

int pthread_mutexattr_setkind_np(pthread_mutexattr_t* attr, int kind) {
    return pthread_mutexattr_settype(attr, kind);
}
