#include "pthread.h"
#include "implement.h"


int pthread_barrier_wait(pthread_barrier_t* barrier) {
    int result;
    pthread_barrier_t b;

    ptw32_mcs_local_node_t node;

    if (barrier == NULL || *barrier == (pthread_barrier_t) PTW32_OBJECT_INVALID) {
        return EINVAL;
    }

    ptw32_mcs_lock_acquire(&(*barrier)->lock, &node);

    b = *barrier;
    if (--b->nCurrentBarrierHeight == 0) {
        /*
         * We are the last thread to arrive at the barrier before it releases us.
         * Move our MCS local node to the global scope barrier handle so that the
         * last thread out (not necessarily us) can release the lock.
         */
        ptw32_mcs_node_transfer(&b->proxynode, &node);

        /*
         * Any threads that have not quite entered sem_wait below when the
         * multiple_post has completed will nevertheless continue through
         * the semaphore (barrier).
         */
        result = (b->nInitialBarrierHeight > 1
                  ? sem_post_multiple(&(b->semBarrierBreeched),
                                      b->nInitialBarrierHeight - 1) : 0);
    } else {
        ptw32_mcs_lock_release(&node);
        /*
         * Use the non-cancelable version of sem_wait().
         *
         * It is possible that all nInitialBarrierHeight-1 threads are
         * at this point when the last thread enters the barrier, resets
         * nCurrentBarrierHeight = nInitialBarrierHeight and leaves.
         * If pthread_barrier_destroy is called at that moment then the
         * barrier will be destroyed along with the semas.
         */
        result = ptw32_semwait(&(b->semBarrierBreeched));
    }

    if ((PTW32_INTERLOCKED_LONG)PTW32_INTERLOCKED_INCREMENT_LONG((PTW32_INTERLOCKED_LONGPTR)&b->nCurrentBarrierHeight)
            == (PTW32_INTERLOCKED_LONG)b->nInitialBarrierHeight) {
        /*
         * We are the last thread to cross this barrier
         */
        ptw32_mcs_lock_release(&b->proxynode);
        if (0 == result) {
            result = PTHREAD_BARRIER_SERIAL_THREAD;
        }
    }

    return (result);
}
