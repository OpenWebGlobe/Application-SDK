/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _OPENGLOBE_C_H
#define _OPENGLOBE_C_H

#ifdef _MSC_VER
#include "win32/og_config.h"
#pragma warning(disable:4251)
#pragma warning(disable:4275)
#ifndef OPENGLOBE_API
 #ifdef BUILD_OPENGLOBELIB
 #define OPENGLOBE_API __declspec(dllexport)
 #else
 #define OPENGLOBE_API __declspec(dllimport)
 #endif
#endif
#else
#define OPENGLOBE_API
#endif

#ifndef LLCONST
#ifdef _MSC_EXTENSIONS
typedef __int64	            int64;
typedef unsigned __int64      uint64;
#define LLCONST(a) (a##i64)
#define ULLCONST(a) (a##ui64)
//#elif !defined(_MSC_VER)
//typedef int64_t  	            int64;
//typedef uint64_t              uint64;
//#define LLCONST(a) (a##i64)
//#define ULLCONST(a) (a##ui64)
#else
typedef long long	            int64;
typedef unsigned long long    uint64;
typedef long long __int64;  // do not use
#define LLCONST(a) (a##ll)
#define ULLCONST(a) (a##ull)
#endif
#endif

//------------------------------------------------------------------------------
namespace math
{
   template<typename T>
   inline T Min(T a, T b)    //!< Minimum (a,b)
   {
      return (a < b ? a : b);
   }
   //---------------------------------------------------------------------------
   template<typename T>
   inline T Max(T a, T b)    //!< Maximum (a,b)
   {
      return (a < b ? b : a);
   }

};
//------------------------------------------------------------------------------
#include "OpenGlobe.h"


#endif
