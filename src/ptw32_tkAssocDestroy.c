#include "pthread.h"
#include "implement.h"


void ptw32_tkAssocDestroy(ThreadKeyAssoc* assoc)
/*
 * -------------------------------------------------------------------
 * This routine releases all resources for the given ThreadKeyAssoc
 * once it is no longer being referenced
 * ie) either the key or thread has stopped referencing it.
 *
 * Parameters:
 *              assoc
 *                      an instance of ThreadKeyAssoc.
 * Returns:
 *      N/A
 * -------------------------------------------------------------------
 */
{

    /*
     * Both key->keyLock and thread->threadLock are locked before
     * entry to this routine.
     */
    if (assoc != NULL) {
        ThreadKeyAssoc* prev, * next;

        /* Remove assoc from thread's keys chain */
        prev = assoc->prevKey;
        next = assoc->nextKey;
        if (prev != NULL) {
            prev->nextKey = next;
        }
        if (next != NULL) {
            next->prevKey = prev;
        }

        if (assoc->thread->keys == assoc) {
            /* We're at the head of the thread's keys chain */
            assoc->thread->keys = next;
        }
        if (assoc->thread->nextAssoc == assoc) {
            /*
             * Thread is exiting and we're deleting the assoc to be processed next.
             * Hand thread the assoc after this one.
             */
            assoc->thread->nextAssoc = next;
        }

        /* Remove assoc from key's threads chain */
        prev = assoc->prevThread;
        next = assoc->nextThread;
        if (prev != NULL) {
            prev->nextThread = next;
        }
        if (next != NULL) {
            next->prevThread = prev;
        }

        if (assoc->key->threads == assoc) {
            /* We're at the head of the key's threads chain */
            assoc->key->threads = next;
        }

        free(assoc);
    }

} /* ptw32_tkAssocDestroy */
