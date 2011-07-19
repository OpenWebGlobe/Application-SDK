/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _VIRTUALGLOBERENDEROPTIONS_H
#define _VIRTUALGLOBERENDEROPTIONS_H

#include "og.h"

//-----------------------------------------------------------------------------

struct OPENGLOBE_API VirtualGlobeRenderOptions
{
   bool     bWireframe;
   bool     bLinearTextureFilter;
   bool     ShowBB;
   bool     ShowCurtain;
   double   fPointSize;
};

//-----------------------------------------------------------------------------

#endif

 