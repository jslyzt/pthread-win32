#include "pthread.h"
#include "implement.h"


int pthread_condattr_destroy(pthread_condattr_t* attr)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      Destroys a condition variable attributes object.
 *      The object can no longer be used.
 *
 * PARAMETERS
 *      attr
 *              pointer to an instance of pthread_condattr_t
 *
 *
 * DESCRIPTION
 *      Destroys a condition variable attributes object.
 *      The object can no longer be used.
 *
 *      NOTES:
 *      1)      Does not affect condition variables created
 *              using 'attr'
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
        (void) free(*attr);

        *attr = NULL;
        result = 0;
    }

    return result;

} /* pthread_condattr_destroy */
