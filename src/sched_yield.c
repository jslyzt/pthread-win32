#include "pthread.h"
#include "implement.h"
#include "sched.h"

int sched_yield(void)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function indicates that the calling thread is
 *      willing to give up some time slices to other threads.
 *
 * PARAMETERS
 *      N/A
 *
 *
 * DESCRIPTION
 *      This function indicates that the calling thread is
 *      willing to give up some time slices to other threads.
 *      NOTE: Since this is part of POSIX 1003.1b
 *                (realtime extensions), it is defined as returning
 *                -1 if an error occurs and sets errno to the actual
 *                error.
 *
 * RESULTS
 *              0               successfully created semaphore,
 *              ENOSYS          sched_yield not supported,
 *
 * ------------------------------------------------------
 */
{
    Sleep(0);

    return 0;
}
