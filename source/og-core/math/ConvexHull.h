/*******************************************************************************
Project       : foundation
Purpose       : compute convex hull using graham scan.
Creation Date : 16.1.2009
Author        : Martin Christen
Copyright     : This source code file is copyright (c) FHNW 2009
*******************************************************************************/

#ifndef _CONVEXHULL_H
#define _CONVEXHULL_H

#include "og.h"
#include <iostream>
#include <deque>
#include <vector>
#include <algorithm>
#include <math.h>
#include <math/ElevationPoint.h>

namespace math
{
   void OPENGLOBE_API graham_scan(std::vector<ElevationPoint>& points, std::vector<ElevationPoint> &chull);
}


#endif
