#include "pthread.h"
#include "implement.h"


int pthread_mutexattr_init(pthread_mutexattr_t* attr)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      Initializes a mutex attributes object with default
 *      attributes.
 *
 * PARAMETERS
 *      attr
 *              pointer to an instance of pthread_mutexattr_t
 *
 *
 * DESCRIPTION
 *      Initializes a mutex attributes object with default
 *      attributes.
 *
 *      NOTES:
 *              1)      Used to define mutex types
 *
 * RESULTS
 *              0               successfully initialized attr,
 *              ENOMEM          insufficient memory for attr.
 *
 * ------------------------------------------------------
 */
{
    int result = 0;
    pthread_mutexattr_t ma;

    ma = (pthread_mutexattr_t) calloc(1, sizeof(*ma));

    if (ma == NULL) {
        result = ENOMEM;
    } else {
        ma->pshared = PTHREAD_PROCESS_PRIVATE;
        ma->kind = PTHREAD_MUTEX_DEFAULT;
    }

    *attr = ma;

    return (result);
} /* pthread_mutexattr_init */
