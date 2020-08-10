#include "pthread.h"
#include "implement.h"

/* ignore warning "unreferenced formal parameter" */
#if defined(_MSC_VER)
#pragma warning( disable : 4100 )
#endif

int pthread_attr_getstacksize(const pthread_attr_t* attr, size_t* stacksize)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function determines the size of the stack on
 *      which threads created with 'attr' will run.
 *
 * PARAMETERS
 *      attr
 *              pointer to an instance of pthread_attr_t
 *
 *      stacksize
 *              pointer to size_t into which is returned the
 *              stack size, in bytes.
 *
 *
 * DESCRIPTION
 *      This function determines the size of the stack on
 *      which threads created with 'attr' will run.
 *
 *      NOTES:
 *              1)      Function supported only if this macro is
 *                      defined:
 *
 *                              _POSIX_THREAD_ATTR_STACKSIZE
 *
 *              2)      Use on newly created attributes object to
 *                      find the default stack size.
 *
 * RESULTS
 *              0               successfully retrieved stack size,
 *              EINVAL          'attr' is invalid
 *              ENOSYS          function not supported
 *
 * ------------------------------------------------------
 */
{
#if defined(_POSIX_THREAD_ATTR_STACKSIZE)

    if (ptw32_is_attr(attr) != 0) {
        return EINVAL;
    }

    /* Everything is okay. */
    *stacksize = (*attr)->stacksize;
    return 0;

#else

    return ENOSYS;

#endif /* _POSIX_THREAD_ATTR_STACKSIZE */

}
