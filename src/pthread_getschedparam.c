#include "pthread.h"
#include "implement.h"
#include "sched.h"

int pthread_getschedparam(pthread_t thread, int* policy,
                      struct sched_param* param) {
    int result;

    /* Validate the thread id. */
    result = pthread_kill(thread, 0);
    if (0 != result) {
        return result;
    }

    /*
     * Validate the policy and param args.
     * Check that a policy constant wasn't passed rather than &policy.
     */
    if (policy <= (int*) SCHED_MAX || param == NULL) {
        return EINVAL;
    }

    /* Fill out the policy. */
    *policy = SCHED_OTHER;

    /*
     * This function must return the priority value set by
     * the most recent pthread_setschedparam() or pthread_create()
     * for the target thread. It must not return the actual thread
     * priority as altered by any system priority adjustments etc.
     */
    param->sched_priority = ((ptw32_thread_t*)thread.p)->sched_priority;

    return 0;
}
