#include <limits.h>

#include "pthread.h"
#include "implement.h"

int pthread_rwlockattr_init(pthread_rwlockattr_t* attr)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      Initializes a rwlock attributes object with default
 *      attributes.
 *
 * PARAMETERS
 *      attr
 *              pointer to an instance of pthread_rwlockattr_t
 *
 *
 * DESCRIPTION
 *      Initializes a rwlock attributes object with default
 *      attributes.
 *
 * RESULTS
 *              0               successfully initialized attr,
 *              ENOMEM          insufficient memory for attr.
 *
 * ------------------------------------------------------
 */
{
    int result = 0;
    pthread_rwlockattr_t rwa;

    rwa = (pthread_rwlockattr_t) calloc(1, sizeof(*rwa));

    if (rwa == NULL) {
        result = ENOMEM;
    } else {
        rwa->pshared = PTHREAD_PROCESS_PRIVATE;
    }

    *attr = rwa;

    return (result);
} /* pthread_rwlockattr_init */
