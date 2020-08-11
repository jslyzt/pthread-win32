#include "pthread.h"
#include "implement.h"

int ptw32_is_attr(const pthread_attr_t* attr) {
    /* Return 0 if the attr object is valid, non-zero otherwise. */
    return (attr == NULL || *attr == NULL || (*attr)->valid != PTW32_ATTR_VALID);
}
