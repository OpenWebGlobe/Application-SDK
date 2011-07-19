/*******************************************************************************
Project       : i3D Studio  
Purpose       : Point representing elevation
Creation Date : 18.6.2009
Author        : Martin Christen
Copyright     : Copyright (c) 2009 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _CLOUDPOINT_H
#define _CLOUDPOINT_H

#include "og.h"

class OPENGLOBE_API CloudPoint
{
public:
   CloudPoint();
   CloudPoint(const CloudPoint& cp);
   ~CloudPoint();
   
   double         x,y;
   double         elevation;    // XYZ
   double         ox, oy, oz;   // Orientation
   int            intensity;    // intensity
   unsigned char  r,g,b,a;      // rgba color
};



#endif
