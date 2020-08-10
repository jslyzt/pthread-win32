#if defined(NEED_ERRNO)

#include "pthread.h"
#include "implement.h"

static int reallyBad = ENOMEM;

/*
 * Re-entrant errno.
 *
 * Each thread has it's own errno variable in pthread_t.
 *
 * The benefit of using the pthread_t structure
 * instead of another TSD key is TSD keys are limited
 * on Win32 to 64 per process. Secondly, to implement
 * it properly without using pthread_t you'd need
 * to dynamically allocate an int on starting the thread
 * and store it manually into TLS and then ensure that you free
 * it on thread termination. We get all that for free
 * by simply storing the errno on the pthread_t structure.
 *
 * MSVC and Mingw32 already have their own thread-safe errno.
 *
 * #if defined( _REENTRANT ) || defined( _MT )
 * #define errno *_errno()
 *
 * int *_errno( void );
 * #else
 * extern int errno;
 * #endif
 *
 */

int* _errno(void) {
    pthread_t self;
    int* result;

    if ((self = pthread_self()).p == NULL) {
        /*
         * Yikes! unable to allocate a thread!
         * Throw an exception? return an error?
         */
        result = &reallyBad;
    } else {
        result = (int*)(&self.p->exitStatus);
    }

    return (result);

}				/* _errno */

#endif /* (NEED_ERRNO) */
