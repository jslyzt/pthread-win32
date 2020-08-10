#include "pthread.h"
#include "implement.h"

#if defined(HAVE_SIGSET_T)

static void ptw32_signal_thread() {
}

static void ptw32_signal_callhandler() {
}

int pthread_sigmask(int how, sigset_t const* set, sigset_t* oset) {
    pthread_t thread = pthread_self();

    if (thread.p == NULL) {
        return ENOENT;
    }

    /* Validate the `how' argument. */
    if (set != NULL) {
        switch (how) {
            case SIG_BLOCK:
                break;
            case SIG_UNBLOCK:
                break;
            case SIG_SETMASK:
                break;
            default:
                /* Invalid `how' argument. */
                return EINVAL;
        }
    }

    /* Copy the old mask before modifying it. */
    if (oset != NULL) {
        memcpy(oset, &(thread.p->sigmask), sizeof(sigset_t));
    }

    if (set != NULL) {
        unsigned int i;

        /* FIXME: this code assumes that sigmask is an even multiple of
           the size of a long integer. */

        unsigned long* src = (unsigned long const*) set;
        unsigned long* dest = (unsigned long*) & (thread.p->sigmask);

        switch (how) {
            case SIG_BLOCK:
                for (i = 0; i < (sizeof(sigset_t) / sizeof(unsigned long)); i++) {
                    /* OR the bit field longword-wise. */
                    *dest++ |= *src++;
                }
                break;
            case SIG_UNBLOCK:
                for (i = 0; i < (sizeof(sigset_t) / sizeof(unsigned long)); i++) {
                    /* XOR the bitfield longword-wise. */
                    *dest++ ^= *src++;
                }
            case SIG_SETMASK:
                /* Replace the whole sigmask. */
                memcpy(&(thread.p->sigmask), set, sizeof(sigset_t));
                break;
        }
    }

    return 0;
}

int sigwait(const sigset_t* set, int* sig) {
    /* This routine is a cancellation point */
    pthread_test_cancel();
}

int sigaction(int signum, const struct sigaction* act, struct sigaction* oldact) {
}

#endif /* HAVE_SIGSET_T */
