#include "pthread.h"
#include "implement.h"


int pthread_condattr_init(pthread_condattr_t* attr)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      Initializes a condition variable attributes object
 *      with default attributes.
 *
 * PARAMETERS
 *      attr
 *              pointer to an instance of pthread_condattr_t
 *
 *
 * DESCRIPTION
 *      Initializes a condition variable attributes object
 *      with default attributes.
 *
 *      NOTES:
 *              1)      Use to define condition variable types
 *              2)      It is up to the application to ensure
 *                      that it doesn't re-init an attribute
 *                      without destroying it first. Otherwise
 *                      a memory leak is created.
 *
 * RESULTS
 *              0               successfully initialized attr,
 *              ENOMEM          insufficient memory for attr.
 *
 * ------------------------------------------------------
 */
{
    pthread_condattr_t attr_result;
    int result = 0;

    attr_result = (pthread_condattr_t) calloc(1, sizeof(*attr_result));

    if (attr_result == NULL) {
        result = ENOMEM;
    }

    *attr = attr_result;

    return result;

} /* pthread_condattr_init */
