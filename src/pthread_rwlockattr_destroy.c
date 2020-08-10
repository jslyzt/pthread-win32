#include <limits.h>

#include "pthread.h"
#include "implement.h"

int pthread_rwlockattr_destroy(pthread_rwlockattr_t* attr)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      Destroys a rwlock attributes object. The object can
 *      no longer be used.
 *
 * PARAMETERS
 *      attr
 *              pointer to an instance of pthread_rwlockattr_t
 *
 *
 * DESCRIPTION
 *      Destroys a rwlock attributes object. The object can
 *      no longer be used.
 *
 *      NOTES:
 *              1)      Does not affect rwlockss created using 'attr'
 *
 * RESULTS
 *              0               successfully released attr,
 *              EINVAL          'attr' is invalid.
 *
 * ------------------------------------------------------
 */
{
    int result = 0;

    if (attr == NULL || *attr == NULL) {
        result = EINVAL;
    } else {
        pthread_rwlockattr_t rwa = *attr;

        *attr = NULL;
        free(rwa);
    }

    return (result);
} /* pthread_rwlockattr_destroy */
