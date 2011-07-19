/*******************************************************************************
Project       : i3D Studio  
Purpose       : 3D Ray for ray-casting/intersection tests/collision detection
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef A_RAY_H
#define A_RAY_H

#ifndef I3D_USE_PRECOMPILED_HEADERS
#include "vec3.h"
#endif

//namespace cwc
//{
//! \ingroup math linalg
   template<typename T>
   class ray 
   {
   public:
      ray<T>() {}
      ray<T>(vec3<T> o, vec3<T> d) 
      {
         set(o,d);
      }

      ray<T>(const ray<T> &r) 
      {
         origin = r.origin;
         direction = r.direction;
         inv_direction = r.inv_direction;
         sign[0] = r.sign[0]; sign[1] = r.sign[1]; sign[2] = r.sign[2];
      }

      void set(const vec3<T>& vOrigin, const vec3<T>& vDirection)
      {
         origin = vOrigin;
         direction = vDirection;
         inv_direction = vec3<T>(1/vDirection.x, 1/vDirection.y, 1/vDirection.z);
         sign[0] = (inv_direction.x < 0);
         sign[1] = (inv_direction.y < 0);
         sign[2] = (inv_direction.z < 0);
      }

      vec3<T> origin;
      vec3<T> direction;
      vec3<T> inv_direction;
      int     sign[3];
   };
//};


#endif