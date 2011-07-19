#ifndef _BOX_H_
#define _BOX_H_

#ifndef I3D_USE_PRECOMPILED_HEADERS
#include "vec3.h"
#endif

#include "ray.h"

//! \class box
//! \deprecated don't use, switch to aabb<T>
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup math linalg
template<typename T>
class box
{
  public:
    box() { }
    box(const vec3<T> &min, const vec3<T> &max) 
    {
      parameters[0] = min;
      parameters[1] = max;
    }

    // (t0, t1) is the interval for valid hits
    bool intersect(const ray<T> &r, T t0, T t1) const
    {
       T tmin, tmax, tymin, tymax, tzmin, tzmax;

       tmin = (parameters[r.sign[0]].x - r.origin.x) * r.inv_direction.x;
       tmax = (parameters[1-r.sign[0]].x - r.origin.x) * r.inv_direction.x;
       tymin = (parameters[r.sign[1]].y - r.origin.y) * r.inv_direction.y;
       tymax = (parameters[1-r.sign[1]].y - r.origin.y) * r.inv_direction.y;
       if ( (tmin > tymax) || (tymin > tmax) ) 
          return false;
       if (tymin > tmin)
          tmin = tymin;
       if (tymax < tmax)
          tmax = tymax;
       tzmin = (parameters[r.sign[2]].z - r.origin.z) * r.inv_direction.z;
       tzmax = (parameters[1-r.sign[2]].z - r.origin.z) * r.inv_direction.z;
       if ( (tmin > tzmax) || (tzmin > tmax) ) 
          return false;
       if (tzmin > tmin)
          tmin = tzmin;
       if (tzmax < tmax)
          tmax = tzmax;
       return ( (tmin < t1) && (tmax > t0) );
    }

    // corners
    vec3<T> parameters[2];
};

#endif 
