#ifndef __SERVICE_WORK__
#define __SERVICE_WORK__

#include "Threads.h"

DWORD __stdcall StartFuncReader( void * params );
DWORD __stdcall StartFuncWriter( void * params );

#endif