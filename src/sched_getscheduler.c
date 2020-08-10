#include "pthread.h"
#include "implement.h"
#include "sched.h"

int sched_getscheduler(pid_t pid) {
    /*
     * Win32 only has one policy which we call SCHED_OTHER.
     * However, we try to provide other valid side-effects
     * such as EPERM and ESRCH errors.
     */
    if (0 != pid) {
        int selfPid = (int) GetCurrentProcessId();

        if (pid != selfPid) {
            HANDLE h =
                OpenProcess(PROCESS_QUERY_INFORMATION, PTW32_FALSE, (DWORD) pid);

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

    return SCHED_OTHER;
}
