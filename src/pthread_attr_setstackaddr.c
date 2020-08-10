#include "pthread.h"
#include "implement.h"


int pthread_attr_setstackaddr(pthread_attr_t* attr, void* stackaddr)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      Threads created with 'attr' will run on the stack
 *      starting at 'stackaddr'.
 *      Stack must be at least PTHREAD_STACK_MIN bytes.
 *
 * PARAMETERS
 *      attr
 *              pointer to an instance of pthread_attr_t
 *
 *      stackaddr
 *              the address of the stack to use
 *
 *
 * DESCRIPTION
 *      Threads created with 'attr' will run on the stack
 *      starting at 'stackaddr'.
 *      Stack must be at least PTHREAD_STACK_MIN bytes.
 *
 *      NOTES:
 *              1)      Function supported only if this macro is
 *                      defined:
 *
 *                              _POSIX_THREAD_ATTR_STACKADDR
 *
 *              2)      Create only one thread for each stack
 *                      address..
 *
 *              3)      Ensure that stackaddr is aligned.
 *
 * RESULTS
 *              0               successfully set stack address,
 *              EINVAL          'attr' is invalid
 *              ENOSYS          function not supported
 *
 * ------------------------------------------------------
 */
{
#if defined( _POSIX_THREAD_ATTR_STACKADDR )

    if (ptw32_is_attr(attr) != 0) {
        return EINVAL;
    }

    (*attr)->stackaddr = stackaddr;
    return 0;

#else

    return ENOSYS;

#endif /* _POSIX_THREAD_ATTR_STACKADDR */
}
