/*******************************************************************************
Project       : i3D Studio  
Purpose       : vec4
Creation Date : 18.9.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef A_VEC4_H
#define A_VEC4_H

#include <cmath>
#include <iostream>

#define vec4f vec4<float>
#define vec4d vec4<double>

//! \ingroup math linalg
template <typename T>
class vec4
{
public:
   typedef T TYPE;

   vec4() : x(0), y(0), z(0), w(0) {}
   vec4(T xx, T yy, T zz, T ww) : x(xx), y(yy), z(zz), w(ww) {}
   vec4(const vec4<T>& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}


   void set(const T xx, const T yy, const T zz) {x=xx, y=yy, z=zz;}
   void set(const vec4<T>& p) { x=p.x; y=p.y; z=p.z;}

   void print(void) const {std::cout << "(x,y,z,w)=(" << x << ", " << y << ", " << z << ", " << w << ")" << std::endl; }

   union
   {
      T x;
      T r;
   };
   union
   {
      T y;
      T g;
   };
   union
   {
      T z;
      T b;
   };
   union
   {
      T w;
      T a;
   };
};


#endif