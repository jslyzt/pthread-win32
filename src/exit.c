#include "pthread.h"
#include "implement.h"
#if ! defined(_UWIN) && ! defined(WINCE)
#   include <process.h>
#endif

#include "pthread_exit.c"
