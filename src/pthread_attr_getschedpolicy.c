#include "pthread.h"
#include "implement.h"
#include "sched.h"

int pthread_attr_getschedpolicy(const pthread_attr_t* attr, int* policy) {
    if (ptw32_is_attr(attr) != 0 || policy == NULL) {
        return EINVAL;
    }

    /*
     * Validate the policy arg.
     * Check that a policy constant wasn't passed rather than &policy.
     */
    if (policy <= (int*) SCHED_MAX) {
        return EINVAL;
    }

    *policy = SCHED_OTHER;

    return 0;
}
