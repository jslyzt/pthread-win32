#include "pthread.h"
#include "implement.h"


int pthread_mutexattr_settype(pthread_mutexattr_t* attr, int kind)
/*
 * ------------------------------------------------------
 *
 * DOCPUBLIC
 * The pthread_mutexattr_settype() and
 * pthread_mutexattr_gettype() functions  respectively set and
 * get the mutex type  attribute. This attribute is set in  the
 * type parameter to these functions.
 *
 * PARAMETERS
 *      attr
 *              pointer to an instance of pthread_mutexattr_t
 *
 *      type
 *              must be one of:
 *
 *                      PTHREAD_MUTEX_DEFAULT
 *
 *                      PTHREAD_MUTEX_NORMAL
 *
 *                      PTHREAD_MUTEX_ERRORCHECK
 *
 *                      PTHREAD_MUTEX_RECURSIVE
 *
 * DESCRIPTION
 * The pthread_mutexattr_settype() and
 * pthread_mutexattr_gettype() functions  respectively set and
 * get the mutex type  attribute. This attribute is set in  the
 * type  parameter to these functions. The default value of the
 * type  attribute is  PTHREAD_MUTEX_DEFAULT.
 *
 * The type of mutex is contained in the type  attribute of the
 * mutex attributes. Valid mutex types include:
 *
 * PTHREAD_MUTEX_NORMAL
 *          This type of mutex does  not  detect  deadlock.  A
 *          thread  attempting  to  relock  this mutex without
 *          first unlocking it will  deadlock.  Attempting  to
 *          unlock  a  mutex  locked  by  a  different  thread
 *          results  in  undefined  behavior.  Attempting   to
 *          unlock  an  unlocked  mutex  results  in undefined
 *          behavior.
 *
 * PTHREAD_MUTEX_ERRORCHECK
 *          This type of  mutex  provides  error  checking.  A
 *          thread  attempting  to  relock  this mutex without
 *          first unlocking it will return with  an  error.  A
 *          thread  attempting to unlock a mutex which another
 *          thread has locked will return  with  an  error.  A
 *          thread attempting to unlock an unlocked mutex will
 *          return with an error.
 *
 * PTHREAD_MUTEX_DEFAULT
 *          Same as PTHREAD_MUTEX_NORMAL.
 *
 * PTHREAD_MUTEX_RECURSIVE
 *          A thread attempting to relock this  mutex  without
 *          first  unlocking  it  will  succeed in locking the
 *          mutex. The relocking deadlock which can occur with
 *          mutexes of type  PTHREAD_MUTEX_NORMAL cannot occur
 *          with this type of mutex. Multiple  locks  of  this
 *          mutex  require  the  same  number  of  unlocks  to
 *          release  the  mutex  before  another  thread   can
 *          acquire the mutex. A thread attempting to unlock a
 *          mutex which another thread has locked will  return
 *          with  an  error. A thread attempting to  unlock an
 *          unlocked mutex will return  with  an  error.  This
 *          type  of mutex is only supported for mutexes whose
 *          process        shared         attribute         is
 *          PTHREAD_PROCESS_PRIVATE.
 *
 * RESULTS
 *              0               successfully set attribute,
 *              EINVAL          'attr' or 'type' is invalid,
 *
 * ------------------------------------------------------
 */
{
    int result = 0;

    if ((attr != NULL && *attr != NULL)) {
        switch (kind) {
            case PTHREAD_MUTEX_FAST_NP:
            case PTHREAD_MUTEX_RECURSIVE_NP:
            case PTHREAD_MUTEX_ERRORCHECK_NP:
                (*attr)->kind = kind;
                break;
            default:
                result = EINVAL;
                break;
        }
    } else {
        result = EINVAL;
    }

    return (result);
} /* pthread_mutexattr_settype */
