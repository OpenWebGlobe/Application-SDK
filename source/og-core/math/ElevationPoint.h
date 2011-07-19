/*******************************************************************************
Project       : i3D Studio  
Purpose       : Point representing elevation
Creation Date : 13.11.2008
Author        : Martin Christen
Copyright     : Copyright (c) 2008 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _ELEVATIONPOINT_H
#define _ELEVATIONPOINT_H

#include "og.h"

class OPENGLOBE_API ElevationPoint
{
public:
   ElevationPoint();
   ElevationPoint(const ElevationPoint& cp);

   virtual ~ElevationPoint();
   
   double x,y;
   double elevation;
   double weight;
   double error;
   
};



#endif
