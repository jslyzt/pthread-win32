#include <sched.h>
#include "test.h"

enum {
    NUM_THREADS = 100
};

static pthread_key_t key = NULL;
static int accesscount[NUM_THREADS];
static int thread_set[NUM_THREADS];
static int thread_destroyed[NUM_THREADS];
static pthread_barrier_t startBarrier;

static void destroy_key(void* arg) {
    int* j = (int*) arg;

    (*j)++;

    assert(*j == 2);

    thread_destroyed[j - accesscount] = 1;
}

static void setkey(void* arg) {
    int* j = (int*) arg;

    thread_set[j - accesscount] = 1;

    assert(*j == 0);

    assert(pthread_getspecific(key) == NULL);

    assert(pthread_setspecific(key, arg) == 0);
    assert(pthread_setspecific(key, arg) == 0);
    assert(pthread_setspecific(key, arg) == 0);

    assert(pthread_getspecific(key) == arg);

    (*j)++;

    assert(*j == 1);
}

static void* mythread(void* arg) {
    (void) pthread_barrier_wait(&startBarrier);

    setkey(arg);

    return 0;

    /* Exiting the thread will call the key destructor. */
}

int main() {
    int i;
    int fail = 0;
    pthread_t thread[NUM_THREADS];

    assert(pthread_barrier_init(&startBarrier, NULL, NUM_THREADS / 2) == 0);

    for (i = 1; i < NUM_THREADS / 2; i++) {
        accesscount[i] = thread_set[i] = thread_destroyed[i] = 0;
        assert(pthread_create(&thread[i], NULL, mythread, (void*)&accesscount[i]) == 0);
    }

    /*
     * Here we test that existing threads will get a key created
     * for them.
     */
    assert(pthread_key_create(&key, destroy_key) == 0);

    (void) pthread_barrier_wait(&startBarrier);

    /*
     * Test main thread key.
     */
    accesscount[0] = 0;
    setkey((void*) &accesscount[0]);

    /*
     * Here we test that new threads will get a key created
     * for them.
     */
    for (i = NUM_THREADS / 2; i < NUM_THREADS; i++) {
        accesscount[i] = thread_set[i] = thread_destroyed[i] = 0;
        assert(pthread_create(&thread[i], NULL, mythread, (void*)&accesscount[i]) == 0);
    }

    /*
     * Wait for all threads to complete.
     */
    for (i = 1; i < NUM_THREADS; i++) {
        assert(pthread_join(thread[i], NULL) == 0);
    }

    assert(pthread_key_delete(key) == 0);

    assert(pthread_barrier_destroy(&startBarrier) == 0);

    for (i = 1; i < NUM_THREADS; i++) {
        /*
         * The counter is incremented once when the key is set to
         * a value, and again when the key is destroyed. If the key
         * doesn't get set for some reason then it will still be
         * NULL and the destroy function will not be called, and
         * hence accesscount will not equal 2.
         */
        if (accesscount[i] != 2) {
            fail++;
            fprintf(stderr, "Thread %d key, set = %d, destroyed = %d\n",
                    i, thread_set[i], thread_destroyed[i]);
        }
    }

    fflush(stderr);

    return (fail);
}
