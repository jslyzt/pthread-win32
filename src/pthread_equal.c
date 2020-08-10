#include "pthread.h"
#include "implement.h"


int pthread_equal(pthread_t t1, pthread_t t2)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function returns nonzero if t1 and t2 are equal, else
 *      returns zero
 *
 * PARAMETERS
 *      t1,
 *      t2
 *              thread IDs
 *
 *
 * DESCRIPTION
 *      This function returns nonzero if t1 and t2 are equal, else
 *      returns zero.
 *
 * RESULTS
 *              non-zero        if t1 and t2 refer to the same thread,
 *              0               t1 and t2 do not refer to the same thread
 *
 * ------------------------------------------------------
 */
{
    int result;

    /*
     * We also accept NULL == NULL - treating NULL as a thread
     * for this special case, because there is no error that we can return.
     */
    result = (t1.p == t2.p && t1.x == t2.x);

    return (result);

} /* pthread_equal */
