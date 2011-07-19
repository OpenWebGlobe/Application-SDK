/*******************************************************************************
Project       : Foundation
Purpose       : Utils for Elevation Points
Creation Date : 23.1.2009
Author        : Martin Christen
Copyright     : This source code file is copyright (c) FHNW 2009
*******************************************************************************/

#ifndef _ELEVATIONPOINT_UTILS_H
#define _ELEVATIONPOINT_UTILS_H 

#include "math/ElevationPoint.h"
#include <float.h>
#include <io/IStreamIn.h>
#include <io/IStreamOut.h>
#include <vector>


namespace math
{ 
   //--------------------------------------------------------------------------
   inline bool PointInPolygon(const ElevationPoint& point, const std::vector<ElevationPoint>& polygon)
   {
      size_t i, j;
      bool c = false;
      for (i = 0, j = polygon.size()-1; i < polygon.size(); j = i++) 
      {
         if ( ((polygon[i].y>=point.y) != (polygon[j].y>=point.y)) &&
            (point.x <= (polygon[j].x-polygon[i].x) * (point.y-polygon[i].y) / (polygon[j].y-polygon[i].y) + polygon[i].x) )
            c = !c;
      }
      return c;
   }

   //--------------------------------------------------------------------------
}

#endif
