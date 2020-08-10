#include "pthread.h"
#include "implement.h"


int pthread_key_delete(pthread_key_t key)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function deletes a thread-specific data key. This
 *      does not change the value of the thread specific data key
 *      for any thread and does not run the key's destructor
 *      in any thread so it should be used with caution.
 *
 * PARAMETERS
 *      key
 *              pointer to an instance of pthread_key_t
 *
 *
 * DESCRIPTION
 *      This function deletes a thread-specific data key. This
 *      does not change the value of the thread specific data key
 *      for any thread and does not run the key's destructor
 *      in any thread so it should be used with caution.
 *
 * RESULTS
 *              0               successfully deleted the key,
 *              EINVAL          key is invalid,
 *
 * ------------------------------------------------------
 */
{
    ptw32_mcs_local_node_t keyLock;
    int result = 0;

    if (key != NULL) {
        if (key->threads != NULL && key->destructor != NULL) {
            ThreadKeyAssoc* assoc;
            ptw32_mcs_lock_acquire(&(key->keyLock), &keyLock);
            /*
             * Run through all Thread<-->Key associations
             * for this key.
             *
             * While we hold at least one of the locks guarding
             * the assoc, we know that the assoc pointed to by
             * key->threads is valid.
             */
            while ((assoc = (ThreadKeyAssoc*) key->threads) != NULL) {
                ptw32_mcs_local_node_t threadLock;
                ptw32_thread_t* thread = assoc->thread;

                if (assoc == NULL) {
                    /* Finished */
                    break;
                }

                ptw32_mcs_lock_acquire(&(thread->threadLock), &threadLock);
                /*
                 * Since we are starting at the head of the key's threads
                 * chain, this will also point key->threads at the next assoc.
                 * While we hold key->keyLock, no other thread can insert
                 * a new assoc via pthread_setspecific.
                 */
                ptw32_tkAssocDestroy(assoc);
                ptw32_mcs_lock_release(&threadLock);
                ptw32_mcs_lock_release(&keyLock);
            }
        }

        TlsFree(key->key);
        if (key->destructor != NULL) {
            /* A thread could be holding the keyLock */
            ptw32_mcs_lock_acquire(&(key->keyLock), &keyLock);
            ptw32_mcs_lock_release(&keyLock);
        }

#if defined( _DEBUG )
        memset((char*) key, 0, sizeof(*key));
#endif
        free(key);
    }

    return (result);
}
