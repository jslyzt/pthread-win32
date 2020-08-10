#include "pthread.h"
#include "implement.h"

pthread_t pthread_self(void)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function returns a reference to the current running
 *      thread.
 *
 * PARAMETERS
 *      N/A
 *
 *
 * DESCRIPTION
 *      This function returns a reference to the current running
 *      thread.
 *
 * RESULTS
 *              pthread_t       reference to the current thread
 *
 * ------------------------------------------------------
 */
{
    pthread_t self;
    pthread_t nil = {NULL, 0};
    ptw32_thread_t* sp;

#if defined(_UWIN)
    if (!ptw32_selfThreadKey) {
        return nil;
    }
#endif

    sp = (ptw32_thread_t*) pthread_getspecific(ptw32_selfThreadKey);

    if (sp != NULL) {
        self = sp->ptHandle;
    } else {
        /*
         * Need to create an implicit 'self' for the currently
         * executing thread.
         */
        self = ptw32_new();
        sp = (ptw32_thread_t*) self.p;

        if (sp != NULL) {
            /*
             * This is a non-POSIX thread which has chosen to call
             * a POSIX threads function for some reason. We assume that
             * it isn't joinable, but we do assume that it's
             * (deferred) cancelable.
             */
            sp->implicit = 1;
            sp->detachState = PTHREAD_CREATE_DETACHED;
            sp->thread = GetCurrentThreadId();

#if defined(NEED_DUPLICATEHANDLE)
            /*
             * DuplicateHandle does not exist on WinCE.
             *
             * NOTE:
             * GetCurrentThread only returns a pseudo-handle
             * which is only valid in the current thread context.
             * Therefore, you should not pass the handle to
             * other threads for whatever purpose.
             */
            sp->threadH = GetCurrentThread();
#else
            if (!DuplicateHandle(GetCurrentProcess(),
                                 GetCurrentThread(),
                                 GetCurrentProcess(),
                                 &sp->threadH,
                                 0, FALSE, DUPLICATE_SAME_ACCESS)) {
                /*
                 * Should not do this, but we have no alternative if
                 * we can't get a Win32 thread handle.
                 * Thread structs are never freed.
                 */
                ptw32_threadReusePush(self);
                /*
                 * As this is a win32 thread calling us and we have failed,
                 * return a value that makes sense to win32.
                 */
                return nil;
            }
#endif

            /*
             * No need to explicitly serialise access to sched_priority
             * because the new handle is not yet public.
             */
            sp->sched_priority = GetThreadPriority(sp->threadH);
            pthread_setspecific(ptw32_selfThreadKey, (void*) sp);
        }
    }

    return (self);

} /* pthread_self */
