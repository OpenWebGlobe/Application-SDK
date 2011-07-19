/*******************************************************************************
Project       : i3D Studio  
Purpose       : Sphere for Collision Detection
Creation Date : 6.7.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef A_SPHERE_H
#define A_SPHERE_H

#ifndef I3D_USE_PRECOMPILED_HEADERS
#include "vec3.h"
#include "ray.h"
#include "mathutils.h"
#endif

// Todo: TEST.... und INTERSECT... Funktionen unterscheiden!
//! \ingroup math linalg
template<typename T>
class sphere3
{
public:
   sphere3();
   sphere3(const vec3<T>& vCenter, T fRadius);
   virtual ~sphere3();

   // returns true if sphere intersects with given axis aligned box
   bool IntersectBox(vec3<T>& vMin, vec3<T>& vMax);

   // returns true if sphere intersects with given ray
   std::vector<double> IntersectRay(vec3<T>& vStart, vec3<T>& vDir);

   // returns true if sphere intersects with triangle
   bool IntersectTriangle(vec3<T>& v0, vec3<T>& v1, vec3<T>& v2, vec3<T>& result);

   // returns true if sphere intersects with sphere
   bool IntersectSphere(vec3<T>& vCenter, T fRadius);

   // retrieve center
   vec3<T>&    Center();

   // retrieve radius
   T&          Radius();

protected:
   vec3<T>     _vCenter;
   T           _fRadius;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// inline code:

template<typename T>
sphere3<T>::sphere3(){}

//-----------------------------------------------------------------------------

template<typename T>
sphere3<T>::sphere3(const vec3<T>& vCenter, T fRadius)
{
   _vCenter = vCenter;
   _fRadius = fRadius;
}

//-----------------------------------------------------------------------------

template<typename T>
sphere3<T>::~sphere3()
{

}

//-----------------------------------------------------------------------------

template<typename T>
vec3<T>&    sphere3<T>::Center()
{
   return _vCenter;
}

//-----------------------------------------------------------------------------

template<typename T>
T&          sphere3<T>::Radius()
{
   return _fRadius;
}

//-----------------------------------------------------------------------------

template<typename T>
bool sphere3<T>::IntersectBox(vec3<T>& vMin, vec3<T>& vMax)
{
   T Bmin[3];
   T Bmax[3];
   T C[3];

   Bmin[0] = vMin.x; Bmin[1] = vMin.y; Bmin[2] = vMin.z;
   Bmax[0] = vMax.x; Bmax[1] = vMax.y; Bmax[2] = vMax.z;
   C[0] = _vCenter.x; C[1] = _vCenter.y; C[2] = _vCenter.z;
 
   return math::BoxSphereIntersect<double,3>(Bmin, Bmax, C, _fRadius, 3);
}

//-----------------------------------------------------------------------------

// returns true if sphere intersects with given ray
template<typename T>
std::vector<double> sphere3<T>::IntersectRay(vec3<T>& vStart, vec3<T>& vDir)
{
      std::vector<double> vResult;

      double dx = vDir.x;
      double dy = vDir.y;
      double dz = vDir.z;

      double a = dx*dx + dy*dy + dz*dz;
      double b = 2.0*dx*(vStart.x-_vCenter.x) +  2*dy*(vStart.y-_vCenter.y) +  2*dz*(vStart.z-_vCenter.z);
      double c = _vCenter.x*_vCenter.x + _vCenter.y*_vCenter.y + _vCenter.z*_vCenter.z 
                  + vStart.x*vStart.x + vStart.y*vStart.y + vStart.z*vStart.z +
                    -2.0*(_vCenter.x*vStart.x + _vCenter.y*vStart.y + _vCenter.z*vStart.z) 
                    - _fRadius*_fRadius; 

      double D = b*b - 4.0*a*c; // Diskriminante

      if (D<0.0)
         return vResult;   // keine Lösung  

      // Lösung:        __________
      /*                /
                -b +- \/ b^2 - 4ac
            t = -------------------
                        2 a

      */

      if (D==0)
      {
         double t = (-b - sqrt(b*b-4*a*c) ) / 2*a;
         vResult.push_back(t);
      }
      else
      {
         double t = (-b - sqrt(b*b-4*a*c) ) / 2*a;
         vResult.push_back(t);
         t = (-b + sqrt(b*b-4*a*c) ) / 2*a;
         vResult.push_back(t);
      }

  
   return vResult;
}

//-----------------------------------------------------------------------------

// returns true if sphere intersects with triangle
template<typename T>
bool sphere3<T>::IntersectTriangle(vec3<T>& v0, vec3<T>& v1, vec3<T>& v2, vec3<T>& result)
{
   return math::TestSphereTriangle<T>(_vCenter, _fRadius, v0, v1, v2, result);
}

//-----------------------------------------------------------------------------

// returns true if sphere intersects with sphere
template<typename T>
bool sphere3<T>::IntersectSphere(vec3<T>& vCenter, T fRadius)
{
   vec3<T> diff = vCenter - _vCenter;
   T rsum = fRadius + _fRadius;
   return dot<T>(diff, diff) <= rsum*rsum;
}

//-----------------------------------------------------------------------------


#endif