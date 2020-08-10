#if !defined(PTW32_STATIC_LIB)

#include "pthread.h"
#include "implement.h"

#if defined(_MSC_VER)
/*
 * lpvReserved yields an unreferenced formal parameter;
 * ignore it
 */
#pragma warning( disable : 4100 )
#endif

#if defined(__cplusplus)
/*
 * Dear c++: Please don't mangle this name. -thanks
 */
extern "C"
#endif				/* __cplusplus */
BOOL WINAPI
DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved) {
    BOOL result = PTW32_TRUE;

    switch (fdwReason) {

        case DLL_PROCESS_ATTACH:
            result = pthread_win32_process_attach_np();
            break;

        case DLL_THREAD_ATTACH:
            /*
             * A thread is being created
             */
            result = pthread_win32_thread_attach_np();
            break;

        case DLL_THREAD_DETACH:
            /*
             * A thread is exiting cleanly
             */
            result = pthread_win32_thread_detach_np();
            break;

        case DLL_PROCESS_DETACH:
            (void) pthread_win32_thread_detach_np();
            result = pthread_win32_process_detach_np();
            break;
    }

    return (result);

}				/* DllMain */

#endif /* PTW32_STATIC_LIB */
