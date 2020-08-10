#include "pthread.h"
#include "implement.h"


int pthread_barrierattr_destroy(pthread_barrierattr_t* attr)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      Destroys a barrier attributes object. The object can
 *      no longer be used.
 *
 * PARAMETERS
 *      attr
 *              pointer to an instance of pthread_barrierattr_t
 *
 *
 * DESCRIPTION
 *      Destroys a barrier attributes object. The object can
 *      no longer be used.
 *
 *      NOTES:
 *              1)      Does not affect barrieres created using 'attr'
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
        pthread_barrierattr_t ba = *attr;

        *attr = NULL;
        free(ba);
    }

    return (result);
} /* pthread_barrierattr_destroy */
