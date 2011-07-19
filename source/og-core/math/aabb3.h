/*******************************************************************************
Project       : i3D Studio  
Purpose       : Axis aligned Bounding Box
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2006 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef A_AABB3_H
#define A_AABB3_H

#include "vec3.h"
#include "vec2.h"
#include "mat4.h"
#include "line3.h"
#include "plane3.h"
#include "sphere3.h"
#include "mathutils.h"

//! \class aabb3
//! \brief Axis aligned bounding box (AABB).
template <class T>
class aabb3
{
public:

  aabb3(): MinEdge(T(1e20),T(1e20),T(1e20)), MaxEdge(T(-1e20),T(-1e20),T(-1e20)) {};
  
  aabb3(const aabb3<T>& other): MinEdge(other.MinEdge), MaxEdge(other.MaxEdge) {};
  
  aabb3(const vec3<T>& min, const vec3<T>& max): MinEdge(min), MaxEdge(max) {};

  aabb3(T minx, T miny, T minz, T maxx, T maxy, T maxz): MinEdge(minx, miny, minz), MaxEdge(maxx, maxy, maxz) {};

  inline bool operator==(const aabb3<T>& other) const { return (MinEdge == other.MinEdge && other.MaxEdge == MaxEdge);};
  
  inline bool operator!=(const aabb3<T>& other) const { return !(MinEdge == other.MinEdge && other.MaxEdge == MaxEdge);};

  //! Adds a point to the bounding box and adjust AABB
  void addInternalPoint(const vec3<T>& p)
  {
     addInternalPoint(p.x, p.y, p.z);
  }

  //! Adds a point to the bounding box and adjust AABB
  void addInternalBox(const aabb3<T>& b)
  {
     addInternalPoint(b.MaxEdge);
     addInternalPoint(b.MinEdge);
  }

  //! Reset bounding box.
  void reset(T x, T y, T z)
  {
     MaxEdge.set(x,y,z);
     MinEdge = MaxEdge;
  }

  //! Reset bounding box.
  void reset(const aabb3<T>& initValue)
  {
     *this = initValue;
  }

  //! Reset the bounding box.
  void reset(const vec3<T>& initValue)
  {
     MaxEdge = initValue;
     MinEdge = initValue;
  }

  void addInternalPoint(T x, T y, T z)
  {
     if (x>MaxEdge.x) MaxEdge.x = x;
     if (y>MaxEdge.y) MaxEdge.y = y;
     if (z>MaxEdge.z) MaxEdge.z = z;

     if (x<MinEdge.x) MinEdge.x = x;
     if (y<MinEdge.y) MinEdge.y = y;
     if (z<MinEdge.z) MinEdge.z = z;
  }

  bool isPointInside(const vec3<T>& p) const
  {
     return (p.x >= MinEdge.x && p.x <= MaxEdge.x &&
        p.y >= MinEdge.y && p.y <= MaxEdge.y &&
        p.z >= MinEdge.z && p.z <= MaxEdge.z);
  };

  bool isPointTotalInside(const vec3<T>& p) const
  {
     return (p.x > MinEdge.x && p.x < MaxEdge.x &&
        p.y > MinEdge.y && p.y < MaxEdge.y &&
        p.z > MinEdge.z && p.z < MaxEdge.z);
  };

  bool intersectsWithBox(const aabb3<T>& other) const
  {
     return (MinEdge <= other.MaxEdge && MaxEdge >= other.MinEdge);
  }

  int IntersectWithRay(const vec3<T>& p, const vec3<T>& d, T& tmin, vec3<T>& q)
  {
     tmin = 0;
     T tmax = DBL_MAX;
     for (int i=0;i<3;i++)
     {
        if (fabs(d[i])<DBL_EPSILON)
        {
           if (p[i]<MinEdge[i] || p[i]>MaxEdge[i])
              return 0;
        }
        else
        {
           T ood = 1.0 / d[i];
           T t1 = (MinEdge[i] - p[i]) * ood;
           T t2 = (MaxEdge[i] - p[i]) * ood;

           if (t1>t2)
              std::swap(t1,t2);
           tmin = math::Max<T>(tmin, t1);
           tmax = math::Min<T>(tmax, t2);
           if (tmin>tmax)
              return 0;
        }
     }

     q = p + d * tmin;
     return 1;
  }

  bool intersect(const vec3<T> &pos, const vec3<T>& dir) const 
  {
     T tmin, tmax, tymin, tymax, tzmin, tzmax;
     if (dir.x >= 0) 
     {
        tmin = (MinEdge.x - pos.x) / dir.x;
        tmax = (MaxEdge.x - pos.x) / dir.x;
     }
     else 
     {
        tmin = (MaxEdge.x - pos.x) / dir.x;
        tmax = (MinEdge.x - pos.x) / dir.x;
     }
     if (dir.y >= 0) 
     {
        tymin = (MinEdge.y - pos.y) / dir.y;
        tymax = (MaxEdge.y - pos.y) / dir.y;
     }
     else 
     {
        tymin = (MaxEdge.y - pos.y) / dir.y;
        tymax = (MinEdge.y - pos.y) / dir.y;
     }
     if ( (tmin > tymax) || (tymin > tmax) )
        return false;

     if (tymin > tmin)
        tmin = tymin;
     if (tymax < tmax)
        tmax = tymax;
     if (dir.z >= 0) 
     {
        tzmin = (MinEdge.z - pos.z) / dir.z;
        tzmax = (MaxEdge.z - pos.z) / dir.z;
     }
     else 
     {
        tzmin = (MaxEdge.z - pos.z) / dir.z;
        tzmax = (MinEdge.z - pos.z) / dir.z;
     }
     if ( (tmin > tzmax) || (tzmin > tmax) )
        return false;
     if (tzmin > tmin)
        tmin = tzmin;
     if (tzmax < tmax)
        tmax = tzmax;
     return true;
  }

  bool intersectsWithLine(const line3<T>& line) const
  {
     return intersectsWithLine(line.getMiddle(), line.getVector().normalize(), 
        (T)(line.getLength() * (T)0.5));
  }

  bool intersectsWithLine(const vec3<T>& linemiddle, 
     const vec3<T>& linevect,
     T halflength) const
  {
     const vec3<T> e = getExtent() * (T)0.5;
     const vec3<T> t = getCenter() - linemiddle;
     float r;

     if ((fabs(t.x) > e.x + halflength * fabs(linevect.x)) || 
        (fabs(t.y) > e.y + halflength * fabs(linevect.y)) ||
        (fabs(t.z) > e.z + halflength * fabs(linevect.z)) )
        return false;

     r = e.y * (T)fabs(linevect.z) + e.z * (T)fabs(linevect.y);
     if (fabs(t.y*linevect.z - t.z*linevect.y) > r )
        return false;

     r = e.x * (T)fabs(linevect.z) + e.z * (T)fabs(linevect.x);
     if (fabs(t.z*linevect.x - t.x*linevect.z) > r )
        return false;

     r = e.x * (T)fabs(linevect.y) + e.y * (T)fabs(linevect.x);
     if (fabs(t.x*linevect.y - t.y*linevect.x) > r)
        return false;

     return true;
  }

  EIntersectionRelation3D classifyPlaneRelation(const plane3<T>& plane) const
  {
     vec3<T> nearPoint(MaxEdge);
     vec3<T> farPoint(MinEdge);

     if (plane.Normal.x > (T)0)
     {
        nearPoint.x = MinEdge.x;
        farPoint.x = MaxEdge.x;
     }

     if (plane.Normal.y > (T)0)
     {
        nearPoint.y = MinEdge.y;
        farPoint.y = MaxEdge.y;
     }

     if (plane.Normal.z > (T)0)
     {
        nearPoint.z = MinEdge.z;
        farPoint.z = MaxEdge.z;
     }

     if (plane.Normal.Dot(nearPoint) + plane.D > (T)0)
        return ISREL3D_FRONT;

     if (plane.Normal.Dot(farPoint) + plane.D > (T)0)
        return ISREL3D_CLIPPED;

     return ISREL3D_BACK;
  }
 
  vec3<T> getCenter() const
  {
     return (MinEdge + MaxEdge) / 2;
  }

  vec3<T> getExtent() const
  {
     return MaxEdge - MinEdge;
  }

  void getEdges(vec3<T> *edges) const
  {
     vec3<T> middle = getCenter();
     vec3<T> diag = middle - MaxEdge;

     /*
     Edges are stored in this way:

       /4--------/0
      /  |      / |
     /   |     /  |
     6---------2  |
     |   5- - -| -1
     |  /      |  /
     | /       | /
     7---------3/ 
     */

     edges[0].Set(middle.x + diag.x, middle.y + diag.y, middle.z + diag.z);
     edges[1].Set(middle.x + diag.x, middle.y - diag.y, middle.z + diag.z);
     edges[2].Set(middle.x + diag.x, middle.y + diag.y, middle.z - diag.z);
     edges[3].Set(middle.x + diag.x, middle.y - diag.y, middle.z - diag.z);
     edges[4].Set(middle.x - diag.x, middle.y + diag.y, middle.z + diag.z);
     edges[5].Set(middle.x - diag.x, middle.y - diag.y, middle.z + diag.z);
     edges[6].Set(middle.x - diag.x, middle.y + diag.y, middle.z - diag.z);
     edges[7].Set(middle.x - diag.x, middle.y - diag.y, middle.z - diag.z);
  }


  //! returns if the box is empty, which means that there is
  //! no space within the min and the max edge.
  bool isEmpty() const
  {
     vec3<T> d = MinEdge - MaxEdge;
     if (d.x < 0) d.x = -d.x;
     if (d.y < 0) d.y = -d.y;
     if (d.z < 0) d.z = -d.z;

     return (d.x < AGE_ROUNDING_ERROR && 
        d.y < AGE_ROUNDING_ERROR && 
        d.z < AGE_ROUNDING_ERROR);
  }

  aabb3<T> getInterpolated(const aabb3<T>& other, T d) const
  {
     T inv = T(1.0) - d;
     return aabb3<T>((other.MinEdge*inv) + (MinEdge*d),
        (other.MaxEdge*inv) + (MaxEdge*d));
  }

  double distanceToPoint(const vec3<T>& p, vec3<double>* hitPoint = 0)
  {
      // compute coordinates of point in aabb coordinate system
      vec3<double> diff = p - getCenter();
      vec3<double> length = getExtent();
  
      // project test point onto aabb
      double distance = 0.0;
      double delta;
  
      for( int i=0; i<3; i++ )
      {
          if ( diff[i] < -length[i] )
          {
              delta = diff[i] + length[i];
              distance += delta*delta;
              diff[i] = -length[i];
          }
          else if ( diff[i] > length[i] )
          {
              delta = diff[i] - length[i];
              distance += delta*delta;
              diff[i] = length[i];
          }
      }
      if ( hitPoint )
          *hitPoint = diff;
  
      return distance;
  }
  
  const aabb3<T> Transform(const mat4<T>& Matrix) const
  {
      vec3<T> edges[8];
      vec3<T> edges_transformed[8];
      aabb3<T> aabb_transformed(1e20,1e20,1e20,-1e20,-1e20,-1e20);

      // retrieve all 8 edges:
      getEdges(edges);

      for (int i=0;i<8;i++)
      {
         edges_transformed[i] = Matrix.vec3mul(edges[i]);
      }
      
      for (int i=0;i<8;i++)
      {
         if (aabb_transformed.MaxEdge.x < edges_transformed[i].x) aabb_transformed.MaxEdge.x = edges_transformed[i].x;
         if (aabb_transformed.MaxEdge.y < edges_transformed[i].y) aabb_transformed.MaxEdge.y = edges_transformed[i].y;
         if (aabb_transformed.MaxEdge.z < edges_transformed[i].z) aabb_transformed.MaxEdge.z = edges_transformed[i].z;

         if (aabb_transformed.MinEdge.x > edges_transformed[i].x) aabb_transformed.MinEdge.x = edges_transformed[i].x;
         if (aabb_transformed.MinEdge.y > edges_transformed[i].y) aabb_transformed.MinEdge.y = edges_transformed[i].y;
         if (aabb_transformed.MinEdge.z > edges_transformed[i].z) aabb_transformed.MinEdge.z = edges_transformed[i].z;
      }
 
      return aabb_transformed;
  }

  // member variables

  vec3<T> MinEdge;
  vec3<T> MaxEdge;
};

typedef aabb3<float>    aabb3f;
typedef aabb3<double>   aabb3d;

#endif