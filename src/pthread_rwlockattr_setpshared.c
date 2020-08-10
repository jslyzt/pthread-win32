#include <limits.h>

#include "pthread.h"
#include "implement.h"

int pthread_rwlockattr_setpshared(pthread_rwlockattr_t* attr, int pshared)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      Rwlocks created with 'attr' can be shared between
 *      processes if pthread_rwlock_t variable is allocated
 *      in memory shared by these processes.
 *
 * PARAMETERS
 *      attr
 *              pointer to an instance of pthread_rwlockattr_t
 *
 *      pshared
 *              must be one of:
 *
 *                      PTHREAD_PROCESS_SHARED
 *                              May be shared if in shared memory
 *
 *                      PTHREAD_PROCESS_PRIVATE
 *                              Cannot be shared.
 *
 * DESCRIPTION
 *      Rwlocks creatd with 'attr' can be shared between
 *      processes if pthread_rwlock_t variable is allocated
 *      in memory shared by these processes.
 *
 *      NOTES:
 *              1)      pshared rwlocks MUST be allocated in shared
 *                      memory.
 *
 *              2)      The following macro is defined if shared rwlocks
 *                      are supported:
 *                              _POSIX_THREAD_PROCESS_SHARED
 *
 * RESULTS
 *              0               successfully set attribute,
 *              EINVAL          'attr' or pshared is invalid,
 *              ENOSYS          PTHREAD_PROCESS_SHARED not supported,
 *
 * ------------------------------------------------------
 */
{
    int result;

    if ((attr != NULL && *attr != NULL) &&
            ((pshared == PTHREAD_PROCESS_SHARED) ||
             (pshared == PTHREAD_PROCESS_PRIVATE))) {
        if (pshared == PTHREAD_PROCESS_SHARED) {

#if !defined( _POSIX_THREAD_PROCESS_SHARED )

            result = ENOSYS;
            pshared = PTHREAD_PROCESS_PRIVATE;

#else

            result = 0;

#endif /* _POSIX_THREAD_PROCESS_SHARED */

        } else {
            result = 0;
        }

        (*attr)->pshared = pshared;
    } else {
        result = EINVAL;
    }

    return (result);

} /* pthread_rwlockattr_setpshared */
