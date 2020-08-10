#include "pthread.h"
#include "implement.h"


int pthread_attr_setdetachstate(pthread_attr_t* attr, int detachstate)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function specifies whether threads created with
 *      'attr' will run detached.
 *
 * PARAMETERS
 *      attr
 *              pointer to an instance of pthread_attr_t
 *
 *      detachstate
 *              an integer containing one of:
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
 *      This function specifies whether threads created with
 *      'attr' will run detached.
 *
 *      NOTES:
 *              1)      You cannot join or cancel detached threads.
 *
 * RESULTS
 *              0               successfully set detach state,
 *              EINVAL          'attr' or 'detachstate' is invalid
 *
 * ------------------------------------------------------
 */
{
    if (ptw32_is_attr(attr) != 0) {
        return EINVAL;
    }

    if (detachstate != PTHREAD_CREATE_JOINABLE &&
            detachstate != PTHREAD_CREATE_DETACHED) {
        return EINVAL;
    }

    (*attr)->detachstate = detachstate;
    return 0;
}
