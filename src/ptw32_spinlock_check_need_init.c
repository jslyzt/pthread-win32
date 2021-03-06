#include "pthread.h"
#include "implement.h"


INLINE int ptw32_spinlock_check_need_init(pthread_spinlock_t* lock) {
    int result = 0;
    ptw32_mcs_local_node_t node;

    /*
     * The following guarded test is specifically for statically
     * initialised spinlocks (via PTHREAD_SPINLOCK_INITIALIZER).
     */
    ptw32_mcs_lock_acquire(&ptw32_spinlock_test_init_lock, &node);

    /*
     * We got here possibly under race
     * conditions. Check again inside the critical section
     * and only initialise if the spinlock is valid (not been destroyed).
     * If a static spinlock has been destroyed, the application can
     * re-initialise it only by calling pthread_spin_init()
     * explicitly.
     */
    if (*lock == PTHREAD_SPINLOCK_INITIALIZER) {
        result = pthread_spin_init(lock, PTHREAD_PROCESS_PRIVATE);
    } else if (*lock == NULL) {
        /*
         * The spinlock has been destroyed while we were waiting to
         * initialise it, so the operation that caused the
         * auto-initialisation should fail.
         */
        result = EINVAL;
    }

    ptw32_mcs_lock_release(&node);

    return (result);
}
