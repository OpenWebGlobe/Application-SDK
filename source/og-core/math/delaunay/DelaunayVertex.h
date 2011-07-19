
#ifndef DELAUNAY_VERTEX_H
#define DELAUNAY_VERTEX_H

#include "og.h"
#include "math/ElevationPoint.h"
#include "math/ElevationPointUtils.h"
#include "memory/ReferenceCounter.h"

namespace math
{
   template <class T>
   void SortPoints(std::vector<T>& sites)
   {
      int nsites = (int)sites.size();

      for (int gap = nsites/2; gap > 0; gap /= 2)
      {
         for (int i = gap; i < nsites; i++)
         {
            for (int j = i-gap; 
               j >= 0 && 
               (sites[j].x != sites[j+gap].x ? 
               (sites[j].x > sites[j+gap].x) : 
            (sites[j].y > sites[j+gap].y));j -= gap)

            {
               std::swap(sites[j], sites[j+gap]);
            }
         }
      }
   }



   class OPENGLOBE_API DelaunayVertex : public ReferenceCounter
   {
   public:
      DelaunayVertex();
      DelaunayVertex(double x, double y, double elevation = 0.0, double weight = 0.0);
      DelaunayVertex(const ElevationPoint& pt);
      virtual ~DelaunayVertex();

      double x() const { return _pt.x; }  
      double y() const { return _pt.y; }
      double elevation() const {return _pt.elevation; }
      double weight() const {return _pt.weight; }

      void SetInifite();
      bool IsInfinite();

      virtual void IncRef();
      virtual bool DecRef();
      virtual int GetRefCount();

      void SetId(int nId);
      int  GetId();

      ElevationPoint&   GetElevationPoint() {return _pt;}
      ElevationPoint    GetElevationPointCopy() {return _pt;}

   private:
      void              _Init();

   private:
      ElevationPoint    _pt;
      int               _nRef;
      int               _nId;
      bool              _bInfinite;
   };
}


#endif