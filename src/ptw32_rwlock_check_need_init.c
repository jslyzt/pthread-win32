#include "pthread.h"
#include "implement.h"

INLINE int ptw32_rwlock_check_need_init(pthread_rwlock_t* rwlock) {
    int result = 0;
    ptw32_mcs_local_node_t node;

    /*
     * The following guarded test is specifically for statically
     * initialised rwlocks (via PTHREAD_RWLOCK_INITIALIZER).
     */
    ptw32_mcs_lock_acquire(&ptw32_rwlock_test_init_lock, &node);

    /*
     * We got here possibly under race
     * conditions. Check again inside the critical section
     * and only initialise if the rwlock is valid (not been destroyed).
     * If a static rwlock has been destroyed, the application can
     * re-initialise it only by calling pthread_rwlock_init()
     * explicitly.
     */
    if (*rwlock == PTHREAD_RWLOCK_INITIALIZER) {
        result = pthread_rwlock_init(rwlock, NULL);
    } else if (*rwlock == NULL) {
        /*
         * The rwlock has been destroyed while we were waiting to
         * initialise it, so the operation that caused the
         * auto-initialisation should fail.
         */
        result = EINVAL;
    }

    ptw32_mcs_lock_release(&node);

    return result;
}
