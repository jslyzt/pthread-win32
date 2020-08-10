#include "pthread.h"
#include "implement.h"


int ptw32_processInitialized = PTW32_FALSE;
ptw32_thread_t* ptw32_threadReuseTop = PTW32_THREAD_REUSE_EMPTY;
ptw32_thread_t* ptw32_threadReuseBottom = PTW32_THREAD_REUSE_EMPTY;
pthread_key_t ptw32_selfThreadKey = NULL;
pthread_key_t ptw32_cleanupKey = NULL;
pthread_cond_t ptw32_cond_list_head = NULL;
pthread_cond_t ptw32_cond_list_tail = NULL;

int ptw32_concurrency = 0;

/* What features have been auto-detected */
int ptw32_features = 0;

/*
 * Global [process wide] thread sequence Number
 */
unsigned __int64 ptw32_threadSeqNumber = 0;

/*
 * Function pointer to QueueUserAPCEx if it exists, otherwise
 * it will be set at runtime to a substitute routine which cannot unblock
 * blocked threads.
 */
DWORD (*ptw32_register_cancelation)(PAPCFUNC, HANDLE, DWORD) = NULL;

/*
 * Global lock for managing pthread_t struct reuse.
 */
ptw32_mcs_lock_t ptw32_thread_reuse_lock = 0;

/*
 * Global lock for testing internal state of statically declared mutexes.
 */
ptw32_mcs_lock_t ptw32_mutex_test_init_lock = 0;

/*
 * Global lock for testing internal state of PTHREAD_COND_INITIALIZER
 * created condition variables.
 */
ptw32_mcs_lock_t ptw32_cond_test_init_lock = 0;

/*
 * Global lock for testing internal state of PTHREAD_RWLOCK_INITIALIZER
 * created read/write locks.
 */
ptw32_mcs_lock_t ptw32_rwlock_test_init_lock = 0;

/*
 * Global lock for testing internal state of PTHREAD_SPINLOCK_INITIALIZER
 * created spin locks.
 */
ptw32_mcs_lock_t ptw32_spinlock_test_init_lock = 0;

/*
 * Global lock for condition variable linked list. The list exists
 * to wake up CVs when a WM_TIMECHANGE message arrives. See
 * w32_TimeChangeHandler.c.
 */
ptw32_mcs_lock_t ptw32_cond_list_lock = 0;

#if defined(_UWIN)
/*
 * Keep a count of the number of threads.
 */
int pthread_count = 0;
#endif
