#include "pthread.h"
#include "implement.h"


#if defined(NEED_FTIME)

/*
 * time between jan 1, 1601 and jan 1, 1970 in units of 100 nanoseconds
 */
#define PTW32_TIMESPEC_TO_FILETIME_OFFSET \
	  ( ((int64_t) 27111902 << 32) + (int64_t) 3577643008 )

INLINE void ptw32_timespec_to_filetime(const struct timespec* ts, FILETIME* ft)
/*
 * -------------------------------------------------------------------
 * converts struct timespec
 * where the time is expressed in seconds and nanoseconds from Jan 1, 1970.
 * into FILETIME (as set by GetSystemTimeAsFileTime), where the time is
 * expressed in 100 nanoseconds from Jan 1, 1601,
 * -------------------------------------------------------------------
 */
{
    *(int64_t*) ft = ts->tv_sec * 10000000
                     + (ts->tv_nsec + 50) / 100 + PTW32_TIMESPEC_TO_FILETIME_OFFSET;
}

INLINE void ptw32_filetime_to_timespec(const FILETIME* ft, struct timespec* ts)
/*
 * -------------------------------------------------------------------
 * converts FILETIME (as set by GetSystemTimeAsFileTime), where the time is
 * expressed in 100 nanoseconds from Jan 1, 1601,
 * into struct timespec
 * where the time is expressed in seconds and nanoseconds from Jan 1, 1970.
 * -------------------------------------------------------------------
 */
{
    ts->tv_sec =
        (int)((*(int64_t*) ft - PTW32_TIMESPEC_TO_FILETIME_OFFSET) / 10000000);
    ts->tv_nsec =
        (int)((*(int64_t*) ft - PTW32_TIMESPEC_TO_FILETIME_OFFSET -
               ((int64_t) ts->tv_sec * (int64_t) 10000000)) * 100);
}

#endif /* NEED_FTIME */
