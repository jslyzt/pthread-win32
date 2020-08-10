#include "pthread.h"
#include "implement.h"
#include "sched.h"

int sched_setscheduler(pid_t pid, int policy) {
    /*
     * Win32 only has one policy which we call SCHED_OTHER.
     * However, we try to provide other valid side-effects
     * such as EPERM and ESRCH errors. Choosing to check
     * for a valid policy last allows us to get the most value out
     * of this function.
     */
    if (0 != pid) {
        int selfPid = (int) GetCurrentProcessId();

        if (pid != selfPid) {
            HANDLE h =
                OpenProcess(PROCESS_SET_INFORMATION, PTW32_FALSE, (DWORD) pid);

            if (NULL == h) {
                errno =
                    (GetLastError() ==
                     (0xFF & ERROR_ACCESS_DENIED)) ? EPERM : ESRCH;
                return -1;
            } else {
                CloseHandle(h);
            }
        }
    }

    if (SCHED_OTHER != policy) {
        errno = ENOSYS;
        return -1;
    }

    /*
     * Don't set anything because there is nothing to set.
     * Just return the current (the only possible) value.
     */
    return SCHED_OTHER;
}
