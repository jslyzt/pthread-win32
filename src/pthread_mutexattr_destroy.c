#include "pthread.h"
#include "implement.h"


int pthread_mutexattr_destroy(pthread_mutexattr_t* attr)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      Destroys a mutex attributes object. The object can
 *      no longer be used.
 *
 * PARAMETERS
 *      attr
 *              pointer to an instance of pthread_mutexattr_t
 *
 *
 * DESCRIPTION
 *      Destroys a mutex attributes object. The object can
 *      no longer be used.
 *
 *      NOTES:
 *              1)      Does not affect mutexes created using 'attr'
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
        pthread_mutexattr_t ma = *attr;

        *attr = NULL;
        free(ma);
    }

    return (result);
} /* pthread_mutexattr_destroy */
