#include "pthread.h"
#include "semaphore.h"
#include "implement.h"


int sem_getvalue(sem_t* sem, int* sval)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function stores the current count value of the
 *      semaphore.
 * RESULTS
 *
 * Return value
 *
 *       0                  sval has been set.
 *      -1                  failed, error in errno
 *
 *  in global errno
 *
 *      EINVAL              'sem' is not a valid semaphore,
 *      ENOSYS              this function is not supported,
 *
 *
 * PARAMETERS
 *
 *      sem                 pointer to an instance of sem_t
 *
 *      sval                pointer to int.
 *
 * DESCRIPTION
 *      This function stores the current count value of the semaphore
 *      pointed to by sem in the int pointed to by sval.
 */
{
    if (sem == NULL || *sem == NULL || sval == NULL) {
        errno = EINVAL;
        return -1;
    } else {
        long value;
        register sem_t s = *sem;
        int result = 0;

        if ((result = pthread_mutex_lock(&s->lock)) == 0) {
            /* See sem_destroy.c
             */
            if (*sem == NULL) {
                (void) pthread_mutex_unlock(&s->lock);
                errno = EINVAL;
                return -1;
            }

            value = s->value;
            (void) pthread_mutex_unlock(&s->lock);
            *sval = value;
        }

        return result;
    }

} /* sem_getvalue */
