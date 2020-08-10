#include "pthread.h"
#include "implement.h"

/* ignore warning "unreferenced formal parameter" */
#if defined(_MSC_VER)
#pragma warning( disable : 4100 )
#endif

int pthread_attr_getstackaddr(const pthread_attr_t* attr, void** stackaddr)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function determines the address of the stack
 *      on which threads created with 'attr' will run.
 *
 * PARAMETERS
 *      attr
 *              pointer to an instance of pthread_attr_t
 *
 *      stackaddr
 *              pointer into which is returned the stack address.
 *
 *
 * DESCRIPTION
 *      This function determines the address of the stack
 *      on which threads created with 'attr' will run.
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
 * RESULTS
 *              0               successfully retreived stack address,
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

    *stackaddr = (*attr)->stackaddr;
    return 0;

#else

    return ENOSYS;

#endif /* _POSIX_THREAD_ATTR_STACKADDR */
}
