#include "pthread.h"
#include "implement.h"


int pthread_attr_destroy(pthread_attr_t* attr)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      Destroys a thread attributes object.
 *
 * PARAMETERS
 *      attr
 *              pointer to an instance of pthread_attr_t
 *
 *
 * DESCRIPTION
 *      Destroys a thread attributes object.
 *
 *      NOTES:
 *              1)      Does not affect threads created with 'attr'.
 *
 * RESULTS
 *              0               successfully destroyed attr,
 *              EINVAL          'attr' is invalid.
 *
 * ------------------------------------------------------
 */
{
    if (ptw32_is_attr(attr) != 0) {
        return EINVAL;
    }

    /*
     * Set the attribute object to a specific invalid value.
     */
    (*attr)->valid = 0;
    free(*attr);
    *attr = NULL;

    return 0;
}
