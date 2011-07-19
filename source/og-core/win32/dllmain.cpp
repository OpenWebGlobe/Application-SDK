/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "og.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

//------------------------------------------------------------------------------

void DoInit();
void DoExit();

//------------------------------------------------------------------------------

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
      DoInit();
      break;
	case DLL_THREAD_ATTACH:
      
      break;
	case DLL_THREAD_DETACH:
      
      break;
	case DLL_PROCESS_DETACH:
      DoExit();
      break;
	}
	return TRUE;
}

//------------------------------------------------------------------------------

void DoInit()
{
   //FreeImage_Initialise(false);
}

//------------------------------------------------------------------------------

void DoExit()
{
   //FreeImage_DeInitialise();
}

