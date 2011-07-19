
#include "System.h"
#include "process.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//------------------------------------------------------------------------------

int System::GetProcessId()
{ 
   return _getpid();
}

//------------------------------------------------------------------------------

int System::GetThreadId()
{ 
   return (int) ::GetCurrentThreadId();
}

//------------------------------------------------------------------------------
