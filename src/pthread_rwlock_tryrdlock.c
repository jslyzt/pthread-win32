#include <limits.h>

#include "pthread.h"
#include "implement.h"

int pthread_rwlock_tryrdlock(pthread_rwlock_t* rwlock) {
    int result;
    pthread_rwlock_t rwl;

    if (rwlock == NULL || *rwlock == NULL) {
        return EINVAL;
    }

    /*
     * We do a quick check to see if we need to do more work
     * to initialise a static rwlock. We check
     * again inside the guarded section of ptw32_rwlock_check_need_init()
     * to avoid race conditions.
     */
    if (*rwlock == PTHREAD_RWLOCK_INITIALIZER) {
        result = ptw32_rwlock_check_need_init(rwlock);
        if (result != 0 && result != EBUSY) {
            return result;
        }
    }

    rwl = *rwlock;

    if (rwl->nMagic != PTW32_RWLOCK_MAGIC) {
        return EINVAL;
    }

    if ((result = pthread_mutex_trylock(&(rwl->mtxExclusiveAccess))) != 0) {
        return result;
    }

    if (++rwl->nSharedAccessCount == INT_MAX) {
        if ((result = pthread_mutex_lock(&(rwl->mtxSharedAccessCompleted))) != 0) {
            (void) pthread_mutex_unlock(&(rwl->mtxExclusiveAccess));
            return result;
        }

        rwl->nSharedAccessCount -= rwl->nCompletedSharedAccessCount;
        rwl->nCompletedSharedAccessCount = 0;

        if ((result = pthread_mutex_unlock(&(rwl->mtxSharedAccessCompleted))) != 0) {
            (void) pthread_mutex_unlock(&(rwl->mtxExclusiveAccess));
            return result;
        }
    }

    return (pthread_mutex_unlock(&rwl->mtxExclusiveAccess));
}
