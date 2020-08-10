#include "pthread.h"
#include "semaphore.h"
#include "implement.h"

int sem_init(sem_t* sem, int pshared, unsigned int value)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function initializes a semaphore. The
 *      initial value of the semaphore is 'value'
 *
 * PARAMETERS
 *      sem
 *              pointer to an instance of sem_t
 *
 *      pshared
 *              if zero, this semaphore may only be shared between
 *              threads in the same process.
 *              if nonzero, the semaphore can be shared between
 *              processes
 *
 *      value
 *              initial value of the semaphore counter
 *
 * DESCRIPTION
 *      This function initializes a semaphore. The
 *      initial value of the semaphore is set to 'value'.
 *
 * RESULTS
 *              0               successfully created semaphore,
 *              -1              failed, error in errno
 * ERRNO
 *              EINVAL          'sem' is not a valid semaphore, or
 *                              'value' >= SEM_VALUE_MAX
 *              ENOMEM          out of memory,
 *              ENOSPC          a required resource has been exhausted,
 *              ENOSYS          semaphores are not supported,
 *              EPERM           the process lacks appropriate privilege
 *
 * ------------------------------------------------------
 */
{
    int result = 0;
    sem_t s = NULL;

    if (pshared != 0) {
        /*
         * Creating a semaphore that can be shared between
         * processes
         */
        result = EPERM;
    } else if (value > (unsigned int)SEM_VALUE_MAX) {
        result = EINVAL;
    } else {
        s = (sem_t) calloc(1, sizeof(*s));

        if (NULL == s) {
            result = ENOMEM;
        } else {

            s->value = value;
            if (pthread_mutex_init(&s->lock, NULL) == 0) {

#if defined(NEED_SEM)

                s->sem = CreateEvent(NULL,
                                     PTW32_FALSE,	/* auto (not manual) reset */
                                     PTW32_FALSE,	/* initial state is unset */
                                     NULL);

                if (0 == s->sem) {
                    free(s);
                    (void) pthread_mutex_destroy(&s->lock);
                    result = ENOSPC;
                } else {
                    s->leftToUnblock = 0;
                }

#else /* NEED_SEM */

                if ((s->sem = CreateSemaphore(NULL,	/* Always NULL */
                                              (long) 0,	/* Force threads to wait */
                                              (long) SEM_VALUE_MAX,	/* Maximum value */
                                              NULL)) == 0) {	/* Name */
                    (void) pthread_mutex_destroy(&s->lock);
                    result = ENOSPC;
                }

#endif /* NEED_SEM */

            } else {
                result = ENOSPC;
            }

            if (result != 0) {
                free(s);
            }
        }
    }

    if (result != 0) {
        errno = result;
        return -1;
    }

    *sem = s;

    return 0;

} /* sem_init */
