#include "pthread.h"
#include "implement.h"

/*
 * pthread_getw32threadhandle_np()
 *
 * Returns the win32 thread handle that the POSIX
 * thread "thread" is running as.
 *
 * Applications can use the win32 handle to set
 * win32 specific attributes of the thread.
 */
HANDLE pthread_getw32threadhandle_np(pthread_t thread) {
    return ((ptw32_thread_t*)thread.p)->threadH;
}

/*
 * pthread_getw32threadid_np()
 *
 * Returns the win32 thread id that the POSIX
 * thread "thread" is running as.
 */
DWORD pthread_getw32threadid_np(pthread_t thread) {
    return ((ptw32_thread_t*)thread.p)->thread;
}
