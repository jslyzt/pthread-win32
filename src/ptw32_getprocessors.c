#include "pthread.h"
#include "implement.h"


/*
 * ptw32_getprocessors()
 *
 * Get the number of CPUs available to the process.
 *
 * If the available number of CPUs is 1 then pthread_spin_lock()
 * will block rather than spin if the lock is already owned.
 *
 * pthread_spin_init() calls this routine when initialising
 * a spinlock. If the number of available processors changes
 * (after a call to SetProcessAffinityMask()) then only
 * newly initialised spinlocks will notice.
 */
int ptw32_getprocessors(int* count) {
    DWORD_PTR vProcessCPUs;
    DWORD_PTR vSystemCPUs;
    int result = 0;

#if defined(NEED_PROCESS_AFFINITY_MASK)

    *count = 1;

#else

    if (GetProcessAffinityMask(GetCurrentProcess(),
                               &vProcessCPUs, &vSystemCPUs)) {
        DWORD_PTR bit;
        int CPUs = 0;

        for (bit = 1; bit != 0; bit <<= 1) {
            if (vProcessCPUs & bit) {
                CPUs++;
            }
        }
        *count = CPUs;
    } else {
        result = EAGAIN;
    }

#endif

    return (result);
}
