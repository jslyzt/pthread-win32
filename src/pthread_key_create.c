#include "pthread.h"
#include "implement.h"


/* TLS_OUT_OF_INDEXES not defined on WinCE */
#if !defined(TLS_OUT_OF_INDEXES)
#define TLS_OUT_OF_INDEXES 0xffffffff
#endif

int pthread_key_create(pthread_key_t* key, void (PTW32_CDECL* destructor)(void*))
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function creates a thread-specific data key visible
 *      to all threads. All existing and new threads have a value
 *      NULL for key until set using pthread_setspecific. When any
 *      thread with a non-NULL value for key terminates, 'destructor'
 *      is called with key's current value for that thread.
 *
 * PARAMETERS
 *      key
 *              pointer to an instance of pthread_key_t
 *
 *
 * DESCRIPTION
 *      This function creates a thread-specific data key visible
 *      to all threads. All existing and new threads have a value
 *      NULL for key until set using pthread_setspecific. When any
 *      thread with a non-NULL value for key terminates, 'destructor'
 *      is called with key's current value for that thread.
 *
 * RESULTS
 *              0               successfully created semaphore,
 *              EAGAIN          insufficient resources or PTHREAD_KEYS_MAX
 *                              exceeded,
 *              ENOMEM          insufficient memory to create the key,
 *
 * ------------------------------------------------------
 */
{
    int result = 0;
    pthread_key_t newkey;

    if ((newkey = (pthread_key_t) calloc(1, sizeof(*newkey))) == NULL) {
        result = ENOMEM;
    } else if ((newkey->key = TlsAlloc()) == TLS_OUT_OF_INDEXES) {
        result = EAGAIN;

        free(newkey);
        newkey = NULL;
    } else if (destructor != NULL) {
        /*
         * Have to manage associations between thread and key;
         * Therefore, need a lock that allows competing threads
         * to gain exclusive access to the key->threads list.
         *
         * The mutex will only be created when it is first locked.
         */
        newkey->keyLock = 0;
        newkey->destructor = destructor;
    }

    *key = newkey;

    return (result);
}
