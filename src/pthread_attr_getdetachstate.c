#include "pthread.h"
#include "implement.h"


int pthread_attr_getdetachstate(const pthread_attr_t* attr, int* detachstate)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function determines whether threads created with
 *      'attr' will run detached.
 *
 * PARAMETERS
 *      attr
 *              pointer to an instance of pthread_attr_t
 *
 *      detachstate
 *              pointer to an integer into which is returned one
 *              of:
 *
 *              PTHREAD_CREATE_JOINABLE
 *                              Thread ID is valid, must be joined
 *
 *              PTHREAD_CREATE_DETACHED
 *                              Thread ID is invalid, cannot be joined,
 *                              canceled, or modified
 *
 *
 * DESCRIPTION
 *      This function determines whether threads created with
 *      'attr' will run detached.
 *
 *      NOTES:
 *              1)      You cannot join or cancel detached threads.
 *
 * RESULTS
 *              0               successfully retrieved detach state,
 *              EINVAL          'attr' is invalid
 *
 * ------------------------------------------------------
 */
{
    if (ptw32_is_attr(attr) != 0 || detachstate == NULL) {
        return EINVAL;
    }

    *detachstate = (*attr)->detachstate;
    return 0;
}
