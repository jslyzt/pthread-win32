#include "pthread.h"
#include "implement.h"


int pthread_barrierattr_init(pthread_barrierattr_t* attr)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      Initializes a barrier attributes object with default
 *      attributes.
 *
 * PARAMETERS
 *      attr
 *              pointer to an instance of pthread_barrierattr_t
 *
 *
 * DESCRIPTION
 *      Initializes a barrier attributes object with default
 *      attributes.
 *
 *      NOTES:
 *              1)      Used to define barrier types
 *
 * RESULTS
 *              0               successfully initialized attr,
 *              ENOMEM          insufficient memory for attr.
 *
 * ------------------------------------------------------
 */
{
    pthread_barrierattr_t ba;
    int result = 0;

    ba = (pthread_barrierattr_t) calloc(1, sizeof(*ba));

    if (ba == NULL) {
        result = ENOMEM;
    } else {
        ba->pshared = PTHREAD_PROCESS_PRIVATE;
    }

    *attr = ba;

    return (result);
} /* pthread_barrierattr_init */
