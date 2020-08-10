#include "pthread.h"
#include "implement.h"


int ptw32_processInitialize(void)
/*
 * ------------------------------------------------------
 * DOCPRIVATE
 *      This function performs process wide initialization for
 *      the pthread library.
 *
 * PARAMETERS
 *      N/A
 *
 * DESCRIPTION
 *      This function performs process wide initialization for
 *      the pthread library.
 *      If successful, this routine sets the global variable
 *      ptw32_processInitialized to TRUE.
 *
 * RESULTS
 *              TRUE    if successful,
 *              FALSE   otherwise
 *
 * ------------------------------------------------------
 */
{
    if (ptw32_processInitialized) {
        /*
         * Ignore if already initialized. this is useful for
         * programs that uses a non-dll pthread
         * library. Such programs must call ptw32_processInitialize() explicitly,
         * since this initialization routine is automatically called only when
         * the dll is loaded.
         */
        return PTW32_TRUE;
    }

    ptw32_processInitialized = PTW32_TRUE;

    /*
     * Initialize Keys
     */
    if ((pthread_key_create(&ptw32_selfThreadKey, NULL) != 0) ||
            (pthread_key_create(&ptw32_cleanupKey, NULL) != 0)) {

        ptw32_processTerminate();
    }

    return (ptw32_processInitialized);

} /* processInitialize */
