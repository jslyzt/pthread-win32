#include "pthread.h"
#include "implement.h"
#if !defined(_UWIN)
/*#   include <process.h> */
#endif

void pthread_exit(void* value_ptr)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function terminates the calling thread, returning
 *      the value 'value_ptr' to any joining thread.
 *
 * PARAMETERS
 *      value_ptr
 *              a generic data value (i.e. not the address of a value)
 *
 *
 * DESCRIPTION
 *      This function terminates the calling thread, returning
 *      the value 'value_ptr' to any joining thread.
 *      NOTE: thread should be joinable.
 *
 * RESULTS
 *              N/A
 *
 * ------------------------------------------------------
 */
{
    ptw32_thread_t* sp;

    /*
     * Don't use pthread_self() to avoid creating an implicit POSIX thread handle
     * unnecessarily.
     */
    sp = (ptw32_thread_t*) pthread_getspecific(ptw32_selfThreadKey);

#if defined(_UWIN)
    if (--pthread_count <= 0) {
        exit((int) value_ptr);
    }
#endif

    if (NULL == sp) {
        /*
         * A POSIX thread handle was never created. I.e. this is a
         * Win32 thread that has never called a pthreads-win32 routine that
         * required a POSIX handle.
         *
         * Implicit POSIX handles are cleaned up in ptw32_throw() now.
         */

#if ! (defined (__MINGW64__) || defined(__MINGW32__)) || defined (__MSVCRT__)  || defined (__DMC__)
        _endthreadex((unsigned)(size_t) value_ptr);
#else
        _endthread();
#endif

        /* Never reached */
    }

    sp->exitStatus = value_ptr;

    ptw32_throw(PTW32_EPS_EXIT);

    /* Never reached. */

}
