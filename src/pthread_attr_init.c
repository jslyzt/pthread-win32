#include "pthread.h"
#include "implement.h"


int pthread_attr_init(pthread_attr_t* attr)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      Initializes a thread attributes object with default
 *      attributes.
 *
 * PARAMETERS
 *      attr
 *              pointer to an instance of pthread_attr_t
 *
 *
 * DESCRIPTION
 *      Initializes a thread attributes object with default
 *      attributes.
 *
 *      NOTES:
 *              1)      Used to define thread attributes
 *
 * RESULTS
 *              0               successfully initialized attr,
 *              ENOMEM          insufficient memory for attr.
 *
 * ------------------------------------------------------
 */
{
    pthread_attr_t attr_result;

    if (attr == NULL) {
        /* This is disallowed. */
        return EINVAL;
    }

    attr_result = (pthread_attr_t) malloc(sizeof(*attr_result));

    if (attr_result == NULL) {
        return ENOMEM;
    }

#if defined(_POSIX_THREAD_ATTR_STACKSIZE)
    /*
     * Default to zero size. Unless changed explicitly this
     * will allow Win32 to set the size to that of the
     * main thread.
     */
    attr_result->stacksize = 0;
#endif

#if defined(_POSIX_THREAD_ATTR_STACKADDR)
    /* FIXME: Set this to something sensible when we support it. */
    attr_result->stackaddr = NULL;
#endif

    attr_result->detachstate = PTHREAD_CREATE_JOINABLE;

#if defined(HAVE_SIGSET_T)
    memset(&(attr_result->sigmask), 0, sizeof(sigset_t));
#endif /* HAVE_SIGSET_T */

    /*
     * Win32 sets new threads to THREAD_PRIORITY_NORMAL and
     * not to that of the parent thread. We choose to default to
     * this arrangement.
     */
    attr_result->param.sched_priority = THREAD_PRIORITY_NORMAL;
    attr_result->inheritsched = PTHREAD_EXPLICIT_SCHED;
    attr_result->contentionscope = PTHREAD_SCOPE_SYSTEM;

    attr_result->valid = PTW32_ATTR_VALID;

    *attr = attr_result;

    return 0;
}
