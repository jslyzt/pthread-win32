#include "pthread.h"
#include "implement.h"


INLINE int ptw32_cond_check_need_init(pthread_cond_t* cond) {
    int result = 0;
    ptw32_mcs_local_node_t node;

    /*
     * The following guarded test is specifically for statically
     * initialised condition variables (via PTHREAD_OBJECT_INITIALIZER).
     */
    ptw32_mcs_lock_acquire(&ptw32_cond_test_init_lock, &node);

    /*
     * We got here possibly under race
     * conditions. Check again inside the critical section.
     * If a static cv has been destroyed, the application can
     * re-initialise it only by calling pthread_cond_init()
     * explicitly.
     */
    if (*cond == PTHREAD_COND_INITIALIZER) {
        result = pthread_cond_init(cond, NULL);
    } else if (*cond == NULL) {
        /*
         * The cv has been destroyed while we were waiting to
         * initialise it, so the operation that caused the
         * auto-initialisation should fail.
         */
        result = EINVAL;
    }

    ptw32_mcs_lock_release(&node);

    return result;
}
