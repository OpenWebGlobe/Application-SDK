/*******************************************************************************
Project       : foundation
Purpose       : compute convex hull using graham scan.
Creation Date : 16.1.2009
Author        : Martin Christen
Copyright     : This source code file is copyright (c) FHNW 2009
*******************************************************************************/

#include "ConvexHull.h"

#include <iostream>
#include <algorithm>
#include <cmath>
#include <float.h>
#include <vector>
#include <map>
#include <iterator>


// constatns we need for ccw function later
#define CCW_LEFT_TURN 1
#define CCW_RIGHT_TURN -1
#define CCW_COLINEAR 0

namespace math
{
   double OPENGLOBE_API dist(const ElevationPoint &a, const ElevationPoint &b);
   void OPENGLOBE_API FirstPointToLowerLeft(std::vector<ElevationPoint> &points);
   int OPENGLOBE_API ccw(const ElevationPoint &a, const ElevationPoint &b, const ElevationPoint &origin);

   void OPENGLOBE_API graham_scan(std::vector<ElevationPoint>& points, std::vector<ElevationPoint> &chull);


   struct point_angle_functor 
   {
      ElevationPoint p0;

      point_angle_functor() {} 
      point_angle_functor(const ElevationPoint &p) : p0(p) {} 

      bool operator()(const ElevationPoint &a, const ElevationPoint &b) 
      {
         int res;

         res = ccw(a, b, p0);

         if (res == CCW_LEFT_TURN)
         {
            return true;
         }
         else if (res == CCW_COLINEAR) 
         {
            double da, db;

            da = dist(p0, a);
            db = dist(p0, b);

            if (da < db)
               return true;
         }

         return false;
      }
   };

//-----------------------------------------------------------------------------

   void graham_scan(std::vector<ElevationPoint>& points, std::vector<ElevationPoint> &chull)
   {
      if (points.size() <= 3)
      {
         std::copy(points.begin(), points.end(), std::back_inserter(chull));
         return;
      }

      bool done;
      ElevationPoint next2last, last;

      FirstPointToLowerLeft(points);
      sort(points.begin()+1, points.end(), point_angle_functor(points[0]));

      chull.push_back(points[0]);
      chull.push_back(points[1]);

      for (size_t i = 2; i < points.size(); i++) 
      {
         done = false;

         while (!done && chull.size() > 1) 
         {
            last = chull.back();
            next2last = chull [chull.size()-2];

            if (ccw(last, points[i], next2last) != CCW_LEFT_TURN)
               chull.pop_back();
            else
               done = true;
         }

         chull.push_back(points[i]);
      }
   }

//-----------------------------------------------------------------------------
// runs through the points vector to find the lowest, left-most point and sets first points with it.

   void FirstPointToLowerLeft(std::vector<ElevationPoint> &points)
   {

      for (size_t i = 1; i < points.size(); i++) 
      {
         if (points[i].y < points[0].y)
         {
            std::swap(points[0], points[i]);
         }
         else if (points[i].y == points[0].y && points[i].x < points[0].x)
         {
            std::swap(points[0], points[i]);
         }
      }
   }

//-----------------------------------------------------------------------------

   int ccw(const ElevationPoint &a, const ElevationPoint &b, const ElevationPoint &origin)
   {
      ElevationPoint v1, v2;
      double cp_z;

      v1.x = a.x - origin.x;
      v1.y = a.y - origin.y;

      v2.x = b.x - origin.x;
      v2.y = b.y - origin.y;

      cp_z = v1.x * v2.y - v1.y * v2.x;

      if (cp_z > DBL_EPSILON) 
      {
         return CCW_LEFT_TURN;
      }
      else if (cp_z < -DBL_EPSILON)
      {
         return CCW_RIGHT_TURN;
      }

      return CCW_COLINEAR;
   }

//-----------------------------------------------------------------------------

   double dist(const ElevationPoint &a, const ElevationPoint &b)
   {
      double x, y;

      x = a.x - b.x;
      y = a.y - b.y;

      return (sqrt(double(x*x + y*y)));
   }

//-----------------------------------------------------------------------------

}
