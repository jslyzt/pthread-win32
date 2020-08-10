#include "test.h"

int main(int argc, char* argv[]) {
    /*
     * This should always succeed unless the system has no
     * resources (memory) left.
     */
    pthread_t self;

#if defined(PTW32_STATIC_LIB) && !(defined(_MSC_VER) || defined(__MINGW32__))
    pthread_win32_process_attach_np();
#endif

    self = pthread_self();

    assert(self.p != NULL);

#if defined(PTW32_STATIC_LIB) && !(defined(_MSC_VER) || defined(__MINGW32__))
    pthread_win32_process_detach_np();
#endif
    return 0;
}
