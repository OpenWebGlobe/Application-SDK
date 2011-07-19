/*******************************************************************************
Project       : i3D
Purpose       : Math Utils.
Creation Date : 01.03.2007
Author        : Martin Christen
Copyright     : This source code file is copyright (c) FHNW 2007
*******************************************************************************/


#ifndef _MATHUTILS_H
#define _MATHUTILS_H

#include "og.h"
#include "math/vec3.h"
#include "math/vec2.h"
#include <cmath>
#include "math/plane3.h"
#include "math/GeoCoord.h"
#include <float.h>
#include <vector>

namespace math
{
   

   template<class T>
   bool TriangleIsCCW(vec2<T>& v0, vec2<T>& v1, vec2<T>& v2)
   {
      // >0 Counterclockwise
      // =0 degenerate
      // <0 clockwise
      return ((v1.x-v0.x)*(v2.y-v0.y)-(v2.x-v0.x)*(v1.y-v0.y)) >0;
   }

   //--------------------------------------------------------------------------

   // Test if point is in convex polyon O(log n)
   template<class T>
   bool PointInConvexPolygon(vec2<T>& point, std::vector< vec2<T> >& polygon)
   {
      size_t low = 0; size_t high = polygon.size()-1;
      do 
      {
         size_t mid = (low + high) / 2;
         if (TriangleIsCCW<T>(polygon[0], polygon[mid], point))
            low = mid;
         else
            high = mid;
      } while (low + 1 < high);

      return TriangleIsCCW<T>(polygon[low], polygon[high], point);
   }

   //--------------------------------------------------------------------------

   template<class T>
   bool PointInPolygon(vec2<T>& point, std::vector< vec2<T> >& polygon)
   {
      int i, j;
      bool c = false;
      for (i = 0, j = polygon.size()-1; i < polygon.size(); j = i++) 
      {
         if ( ((polygon[i].y>point.y) != (polygon[j].y>point.y)) &&
            (point.x < (polygon[j].x-polygon[i].x) * (point.y-polygon[i].y) / (polygon[j].y-polygon[i].y) + polygon[i].x) )
            c = !c;
      }
      return c;
   }

   //--------------------------------------------------------------------------

   //--------------------------------------------------------------------------
   //! \brief Multidimensional aa-box point-intersection
   //! \ingroup math intersections
   template <class vec_t, int dimension>
   inline bool pointboxtest(const vec_t& pt, const vec_t& vMin, const vec_t& vMax)
   {
      bool bResult = true;
      
      for (int i=0;i<dimension;i++)
      {
         bResult = bResult && (pt[i] >= vMin[i] && pt[i]<= vMax[i]); 
      }

      return bResult;
   }

   //--------------------------------------------------------------------------
   //! \brief Create Circle from 3 points.
   //! input points: (x1,y1), (x2,y2) and  (x3, y3)
   //! output circle: out_radius, Center: (out_cx, out_cy)
   /*template <class T>
   bool Circumcenter(T x1, T y1, T x2, T y2, T x3, T y3, T& out_radius, T& out_cx, T& out_cy)
   {
      // Use coordinates relative to point n1 of the triangle
      T x21 = x2 - x1;
      T y21 = y2 - y1;
      T x31 = x3 - x1;
      T y31 = y3 - y1;

      T denominator = 0.5 / (x21 * y31 - y21 * x31);

      if (denominator == 0) 
      {
         // Triangle is degenerate
         out_cx = out_cy = out_radius = T(0.0);
         return false;
      }

      // Squares of the lengths of the edges incident to n1
      T length21 = x21 * x21 + y21 * y21;
      T length31 = x31 * x31 + y31 * y31;

      // Calculate offset from n1 of circumcenter
      T x = (y31 * length21 - y21 * length31) * denominator;  
      T y = (x21 * length31 - x31 * length21) * denominator;

      // Create a node at the circumcenter
      out_cx = x + x1;
      out_cy = y + y1;
      out_radius = sqrt((x1-out_cx)*(x1-out_cx) + (y1-out_cy)*(y1-out_cy));

      // Circumcenter calculated
      return true;
   }
   */

   inline bool circumcenter(double x0, double y0,
                     double x1, double y1,
                     double x2, double y2,
                     double& out_radius,
                     double& out_cx, double& out_cy)
   {
       double D;
       double x0m2, y1m2, x1m2, y0m2;
       double x0p2, y1p2, x1p2, y0p2;
       x0m2 = x0 - x2;
       y1m2 = y1 - y2;
       x1m2 = x1 - x2;
       y0m2 = y0 - y2;
       x0p2 = x0 + x2;
       y1p2 = y1 + y2;
       x1p2 = x1 + x2;
       y0p2 = y0 + y2;

       D = x0m2*y1m2 - x1m2*y0m2;
       if ((D < DBL_EPSILON) && (D > -DBL_EPSILON)) return false;

       out_cx = (((x0m2*x0p2 + y0m2*y0p2)/2*y1m2) 
                 - (x1m2*x1p2 + y1m2*y1p2)/2*y0m2) / D;
       out_cy = (((x1m2*x1p2 + y1m2*y1p2)/2*x0m2)
                 - (x0m2*x0p2 + y0m2*y0p2)/2*x1m2) / D;

       out_radius = sqrt((x0-out_cx)*(x0-out_cx) + (y0-out_cy)*(y0-out_cy));

       return true;
   }

   //--------------------------------------------------------------------------
   //! \brief Fast Circle-Rectangle Intersection Checking
   //! by Clifford A. Shaffer
   //! from "Graphics Gems", Academic Press, 1990 
   //! \param rminx min x-coordinate of rectangle
   //! \param rminy min y-coordinate of rectangle
   //! \param rmaxx max x-coordinate of rectangle
   //! \param rmaxy max y-coordinate of rectangle
   //! \param cx x-coordinate of center of circle
   //! \param cy y-coordinate of center of circle
   //! \param radius radius of circle
   template <class T>
   bool TestIntersectRectCircle(T rminx, T rminy, T rmaxx, T rmaxy, T cx, T cy, T radius)
   {
      T Rad2;

      Rad2 = radius * radius;
      rmaxx -= cx;  rmaxy -= cy;
      rminx -= cx;  rminy -= cy;

      if (rmaxx < 0) 			
         if (rmaxy < 0) 		
            return ((rmaxx * rmaxx + rmaxy * rmaxy) < Rad2);
         else if (rminy > 0) 	
            return ((rmaxx * rmaxx + rminy * rminy) < Rad2);
         else 					
            return(math::Abs<T>(rmaxx) < radius);
      else if (rminx > 0)  	
         if (rmaxy < 0) 	
            return ((rminx * rminx + rmaxy * rmaxy) < Rad2);
         else if (rminy > 0)  	
            return ((rminx * rminx + rminy * rminy) < Rad2);
         else 				
            return (rminx < radius);
      else				
         if (rmaxy < 0) 	
            return (math::Abs<T>(rmaxy) < radius);
         else if (rminy > 0)  	
            return (rminy < radius);
         else 				
            return(true);
   } 	


   //--------------------------------------------------------------------------
   template <class T>
   static bool TestIntersectRectCircle2(T rminx, T rminy, T rmaxx, T rmaxy, T cx, T cy, T radius)
   {
      T r_diffx, r_diffy;
      T Rad2 = radius * radius;

      if (rmaxx < (cx - DBL_EPSILON)) // R to left of circle center
      {
         r_diffx = (cx - DBL_EPSILON) - rmaxx;
         if (rmaxy < (cy - DBL_EPSILON)) // R in lower left corner
         {
            r_diffy = (cy - DBL_EPSILON) - rmaxy;
            return ((r_diffx * r_diffx + r_diffy * r_diffy) < Rad2);
         }
         else if (rminy > (cy + DBL_EPSILON)) // R in upper left corner
         {
            r_diffy = rminy - (cy + DBL_EPSILON);
            return ((r_diffx * r_diffx + r_diffy * r_diffy) < Rad2);
         }
         else // R due West of circle
         {
            return (r_diffx < radius);
         }
      }
      else if (rminx > (cx + DBL_EPSILON)) // R to right of circle center
      {
         r_diffx = rminx - (cx + DBL_EPSILON);
         if (rmaxy < (cy - DBL_EPSILON)) // R in lower right corner
         {
            r_diffy = (cy - DBL_EPSILON) - rmaxy;
            return ((r_diffx * r_diffx + r_diffy * r_diffy) < Rad2);
         }
         else if (rminy > (cy + DBL_EPSILON)) // R in upper right corner
         {
            r_diffy = rminy - (cy + DBL_EPSILON);
            return ((r_diffx * r_diffx + r_diffy * r_diffy) < Rad2);
         }
         else // R due East of circle
         {
            return (r_diffx < radius);
         }
      }
      else // R on circle vertical centerline
      {
         if (rmaxy < (cy - DBL_EPSILON)) // R due South of circle
         {
            r_diffy = (cy - DBL_EPSILON) - rmaxy;
            return (r_diffy < radius);
         }
         else if (rminy > (cy + DBL_EPSILON)) // R due North of circle
         {
            r_diffy = rminy - (cy + DBL_EPSILON);
            return (r_diffy < radius);
         }
         else // R contains circle centerpoint
         {
            return true;
         }
      }
   }
 

   //--------------------------------------------------------------------------
   //! \brief Multidimensional aa-box aa-box-intersection:
   //! \brief 1d-AAB: Point
   //! \brief 2d-AAB: Rectangle
   //! \brief 3d-AAB: Cuboid
   //! \brief 4d-AAB: Hyperbox
   //! \ingroup math intersections
   template <class vec_t, int dimension>
   inline bool boxboxtest(const vec_t& vMin_A, const vec_t& vMax_A, const vec_t& vMin_B, const vec_t& vMax_B)
   {
      for (int i=0;i<dimension;i++)
      {
         if (vMax_A[i] <= vMin_B[i])
            return false;

         if (vMax_B[i] <= vMin_A[i])
            return false;
      }

      return true;
   };

   //--------------------------------------------------------------------------
   //! \ingroup math linalg
   //! \deprecated Do not use anymore.
   template<typename T>
   class Rectangle3
   {
      // Points are R(s,t) = C+s0*U0+s1*U1, where C is the center of the
      // rectangle, U0 and U1 are unit-length and perpendicular axes.  The
      // parameters s0 and s1 are constrained by |s0| <= e0 and |s1| <= e1,
      // where e0 > 0 and e1 > 0 are called the extents of the rectangle.
   public:
      Rectangle3 ()
      {}

      Rectangle3 (const vec3<T>& rkCenter, const vec3<T>& rkAxis0, const vec3<T>& rkAxis1, T fExtent0, T fExtent1)
      {
         Center = rkCenter;
         Axis[0] = rkAxis0;
         Axis[1] = rkAxis1;
         Extent[0] = fExtent0;
         Extent[1] = fExtent1;
      }

      //-----------------------------------------------------------------------

      //----------------------------------
      //  0            1 (ignored)
      //   *----------*
      //   |          |
      //   |          |
      //   *----------*
      //  3            2
      //----------------------------------

      void FromDegreeRect(vec3<double> inGeoCoord[4])
      {
         GeoCoord ul(inGeoCoord[0].x, inGeoCoord[0].y, inGeoCoord[0].z);
         //GeoCoord ur(inGeoCoord[1].x, inGeoCoord[1].y, inGeoCoord[1].z);
         GeoCoord lr(inGeoCoord[2].x, inGeoCoord[2].y, inGeoCoord[2].z);
         GeoCoord ll(inGeoCoord[3].x, inGeoCoord[3].y, inGeoCoord[3].z);

         //ul, ur, lr, ll must be in a plane
         vec3<double> ulc, urc, lrc, llc;
         ul.GetCartesian(ulc);
         //ur.GetCartesian(urc);
         lr.GetCartesian(lrc);
         ll.GetCartesian(llc);

         Center = ((ulc + lrc)*0.5);
         Axis[0] = ulc - llc;
         Axis[1] = lrc - llc;
         Extent[0] = (Axis[0] * 0.5).length();
         Extent[1] = (Axis[1] * 0.5).length();
      }

      //-----------------------------------------------------------------------

      //----------------------------------
      //  0            1 (ignored)
      //   *----------*
      //   |          |
      //   |          |
      //   *----------*
      //  3            2
      //----------------------------------

      void FromCartesianRect(vec3<double> inGeoCoord[4])
      {
         Center = ((inGeoCoord[0] + inGeoCoord[2])*0.5);
         Axis[0] = inGeoCoord[0] - inGeoCoord[3];
         Axis[1] = inGeoCoord[2] - inGeoCoord[3];
         Extent[0] = (Axis[0] * 0.5).Length();
         Extent[1] = (Axis[1] * 0.5).Length();
      }

      // Retrieve 4 Corners of Rectangle
      void ComputeVertices (vec3<T> vVertex[4]) const
      {
         vec3<T> akEAxis[2] = { Extent[0]*Axis[0], Extent[1]*Axis[1] };

         vVertex[0] = Center - akEAxis[0] - akEAxis[1];
         vVertex[1] = Center + akEAxis[0] - akEAxis[1];
         vVertex[2] = Center + akEAxis[0] + akEAxis[1];
         vVertex[3] = Center - akEAxis[0] + akEAxis[1];
      }

      vec3<T> GetPPCorner () const  // C + e0*A0 + e1*A1
      {  return Center + Extent[0]*Axis[0] + Extent[1]*Axis[1]; }
      vec3<T> GetPMCorner () const  // C + e0*A0 - e1*A1
      {  return Center + Extent[0]*Axis[0] - Extent[1]*Axis[1]; }
      vec3<T> GetMPCorner () const  // C - e0*A0 + e1*A1
      {  return Center - Extent[0]*Axis[0] + Extent[1]*Axis[1]; }
      vec3<T> GetMMCorner () const  // C - e0*A0 - e1*A1
      {  return Center - Extent[0]*Axis[0] - Extent[1]*Axis[1]; }

      vec3<T> Center;
      vec3<T> Axis[2];
      T Extent[2];
   };

   //--------------------------------------------------------------------------
   //! \ingroup math linalg
   template<typename T>
   double DistVec3Rectangle3Squared(const vec3<T>& rkVector,
                                    const Rectangle3<T>& rkRectangle)
   {
      vec3<T> kDiff = rkRectangle.Center - rkVector;

      T fB0 = kDiff.Dot(rkRectangle.Axis[0]);
      T fB1 = kDiff.Dot(rkRectangle.Axis[1]);
      T fS0 = -fB0, fS1 = -fB1;
      T fSqrDistance = kDiff.SquaredLength();

      if (fS0 < -rkRectangle.Extent[0])
      {
         fS0 = -rkRectangle.Extent[0];
      }
      else if (fS0 > rkRectangle.Extent[0])
      {
         fS0 = rkRectangle.Extent[0];
      }
      fSqrDistance += fS0*(fS0 + ((T)2.0)*fB0);

      if (fS1 < -rkRectangle.Extent[1])
      {
         fS1 = -rkRectangle.Extent[1];
      }
      else if (fS1 > rkRectangle.Extent[1])
      {
         fS1 = rkRectangle.Extent[1];
      }
      fSqrDistance += fS1*(fS1 + ((T)2.0)*fB1);

      // account for numerical round-off error
      if (fSqrDistance < (T)0)
      {
         fSqrDistance = (T)0;
      }

      /*m_kClosestPoint0 = m_rkVector;
      m_kClosestPoint1 = m_rkRectangle.Center + fS0*m_rkRectangle.Axis[0] +
         fS1*m_rkRectangle.Axis[1];
      m_afRectCoord[0] = fS0;
      m_afRectCoord[1] = fS1;*/

      return fSqrDistance;
   }

   //! \ingroup math linalg
   template<typename T>
   vec3<T> NearestPointVec3Rectangle3(const vec3<T>& rkVector,
      const Rectangle3<T>& rkRectangle)
   {
      vec3<T> kDiff = rkRectangle.Center - rkVector;

      T fB0 = kDiff.Dot(rkRectangle.Axis[0]);
      T fB1 = kDiff.Dot(rkRectangle.Axis[1]);
      T fS0 = -fB0, fS1 = -fB1;
      T fSqrDistance = kDiff.SquaredLength();

      if (fS0 < -rkRectangle.Extent[0])
      {
         fS0 = -rkRectangle.Extent[0];
      }
      else if (fS0 > rkRectangle.Extent[0])
      {
         fS0 = rkRectangle.Extent[0];
      }
      fSqrDistance += fS0*(fS0 + ((T)2.0)*fB0);

      if (fS1 < -rkRectangle.Extent[1])
      {
         fS1 = -rkRectangle.Extent[1];
      }
      else if (fS1 > rkRectangle.Extent[1])
      {
         fS1 = rkRectangle.Extent[1];
      }
      fSqrDistance += fS1*(fS1 + ((T)2.0)*fB1);

      // account for numerical round-off error
      if (fSqrDistance < (T)0)
      {
         fSqrDistance = (T)0;
      }

      vec3<T> vClosestPoint = rkRectangle.Center + rkRectangle.Axis[0]*fS0 + rkRectangle.Axis[1]*fS1;
      return vClosestPoint;

   }

   //--------------------------------------------------------------------------
   //! \ingroup math shader
   template<typename T>
   bool Flteq(T x, T y)
   {
      const T eps = std::numeric_limits<T>::epsilon();
      if (std::fabs(x) < eps && std::fabs(y) < eps)
         return true;
      else
         return (std::fabs(x-y) < eps*(std::fabs(x) + std::fabs(y)));
   }

   //! \ingroup math shader
   inline void Seed(long k)
   {
      srand(k);
   }
   
   //! \ingroup math shader
   template<typename T>
   inline T Rand(void)
   {
      return (T)rand()/(T)RAND_MAX;
   }

   //! \ingroup math shader
   template<typename T>
   inline T Rand01(void)
   {
      return (T)(::abs(rand()))/(T)RAND_MAX;
   }

   //--------------------------------------------------------------------------
   //! \ingroup math shader
   template<typename T>
   inline T Abs(T x)             //!< returns x if x>=0, otherwise returns -x
   {
      return (x < 0 ? -x : x);
   }

   template<typename T>
   inline bool IsZero( T a );

   template<>
   inline bool IsZero<double>(double a)
   {
      return ( math::Abs<double>(a) < DBL_EPSILON);
   }   

   template<>
   inline bool IsZero<float>(float a)
   {
      return ( math::Abs<float>(a) < FLT_EPSILON);
   }


   //--------------------------------------------------------------------------
   //! \ingroup math shader
   template<typename T>
   inline T Sign(T x)            //!< returns 1 if x > 0, returns 0 if x=0, returns -1 if x < 0
   {
      if (x>0) return 1;
      if (x<0) return -1;
      return 0;
   }
   //--------------------------------------------------------------------------
   //! \ingroup math shader
   template<typename T>
   inline T Floor(T x)           //!< value equal to nearest Integer that is less than or equal to x
   {
      //return (T)((int)x - ((x < 0 && x != (int)(x))));
      return (T)floor((T)x);
   }

   //--------------------------------------------------------------------------
   //! \ingroup math shader
   template<typename T>
   inline double Ceil(T x)            //!< value equal to nearest Integer that is greater than or equal to x
   {
      //return (T)((int)x + ((x > 0 && x != (int)(x))));
      return (T)ceil(x);
   }

   //--------------------------------------------------------------------------
   //! \ingroup math shader
   template<typename T>
   inline void Round(T &x, int nDecimal)     // round values   
   {
      if (nDecimal < 0)
         return;

      // move the decimal place nDecimal number of times
      x *= (T)std::pow((T)10, (T)nDecimal);

      T fDiff;
      if (x > 0)       // check for -ve or +ve
      {
         fDiff = x - std::floor(x);
         if (fDiff >= 0.5)
            x = std::ceil(x);      // e.g. 2.6 -> 3.0
         else
            x = std::floor(x);     // e.g. 2.2 -> 2.0
      }
      else
      {
         fDiff = std::fabs(x) - std::floor(std::fabs(x));
         if (fDiff >= 0.5)
            x = std::floor(x);    // e.g. -2.6 -> -3.0
         else
            x = std::ceil(x);     // e.g. -2.2 -> -2.0
      }

      // move back the decimal place to the original position
      x /= (T)std::pow((double)10, (T)nDecimal);
   }

   

   //--------------------------------------------------------------------------
   //! \ingroup math shader
   template<typename T>
   inline T PrevInt(T x)    //!< returns previous Integer, example: -3.1 -> -4 and 1.1 -> 1
   {
      T r = Floor<T>(x);
      return r;
   }
     
   //! \ingroup math shader
   template<typename T>
   inline T NextInt(T x)       //!< returns next Integer, example: -3.1 -> -3 and 1.1 -> 2
   {
      T r = Ceil<T>(x);
      return r;
   }

  
   // nächster ganzzahliger Vektor!
   //! \ingroup math shader
   template<typename T>
   inline vec3<T> gnNextInt(const vec3<T>& v)       //!< returns next Integer of all components. -3.1 -> 4 and 1.1 -> 2
   {
      vec3<T> r;
      r.x = NextInt(v.x);
      r.y = NextInt(v.y);
      r.z = NextInt(v.z);    
      return r;
   }
     
   //! \ingroup math shader
   template<typename T>
   inline vec3<T> gnPrevInt(const vec3<T>& v)       //!< returns next Integer of all components. -3.1 -> 4 and 1.1 -> 2
   {
      vec3<T> r;
      r.x = PrevInt(v.x);
      r.y = PrevInt(v.y);
      r.z = PrevInt(v.z);    
      return r;
   }
     
   //--------------------------------------------------------------------------
   //! \ingroup math shader
   template<typename T>
   inline T Fract(T x)           //!< x-floor(x) or mod(x,1);
   {
      return x-Floor<T>(x);
   }

   //--------------------------------------------------------------------------

   template <class T> void Swap(T &a, T &b)
   {
      T temp;
      temp = a;
      a = b;
      b = temp;
   }

   //--------------------------------------------------------------------------
   //! \ingroup math shader
   template<typename T>
   inline T Mod(T x, T y)    //!< x-y*floor(x/y)
   {
      return x-y*Floor<T>(x/y);   
   }  
   //--------------------------------------------------------------------------
   // Min and Max was moved to base
   /*template<typename T>
   inline T Min(T a, T b)    //!< Minimum (a,b)
   {
      return (a < b ? a : b);
   }

   //--------------------------------------------------------------------------
   template<typename T>
   inline T Max(T a, T b)    //!< Maximum (a,b)
   {
      return (a < b ? b : a);
   }*/
   //--------------------------------------------------------------------------
   template <class T>
   inline T Clip(T n, T minValue, T maxValue)
   {
      return math::Min<T>(math::Max<T>(n, minValue), maxValue);
   }

   //--------------------------------------------------------------------------
   //! \ingroup math shader
   template<typename T>
   inline T Clamp(T x, T minval, T maxval)
   {
      return (x < minval ? minval : (x > maxval ? maxval : x));
   }
   //--------------------------------------------------------------------------
   //! \ingroup math shader
   template<typename T>
   inline T Mix(T x, T y, T a)  //!< linear blend: x*(1-a) + y*a 
   {
      return (1-a)*x + a*y;
   }

   //--------------------------------------------------------------------------
   //! \ingroup math shader
   template<typename T>
   inline T Step(T edge, T x)
   {
      return (T) (x >= edge);
   }

   //--------------------------------------------------------------------------
   //! \ingroup math shader
   template<typename T>
   inline T Lerp(T x, T a, T b)
   {
      return (a + x * (b - a));
   }
   
   //! \ingroup math shader
   template<typename T>
   inline vec3<T> Lerp(T x, vec3<T>& a, vec3<T>& b)
   {
      return vec3<T>(Lerp(x, a.x, b.x), 
                  Lerp(x, a.y, b.y),
                  Lerp(x, a.z, b.z));
   }
     
   //--------------------------------------------------------------------------
   
   //! \ingroup math shader
   template<typename T>
	inline T SCurve(T x)
   {
      return ( x * x * (3.0 - 2.0 * x));
   }

   /*!
    * \brief This function calculates bilinear interpolation (2x2)
    * \param tx input coordinate in x direction
    * \param ty input coordinate in y direction
    * \param P00 input value at (0,0)
    * \param P01 input value at (0,1)
    * \param P10 input value at (1,0)
    * \param P11 input value at (1,1)
    * \return returns bilinear interpolation
    * \ingroup math shader
    */
   template <typename T> 
   inline T Bilinear(
      T tx, T ty,
      T P00, T P01, T P10, T P11)
   {
      T u = Fract<T>(tx);
      T v = Fract<T>(ty);

      return (1-u)*(1-v)*P01 + u*(1-v)*P11 + (1-u)*v * P00 + u*v*P10;
   }
   
   /*!
    * \brief clamps a value [0,inf]
    * \param x The value to be zero clamped 
    * \return clamped value
    * \ingroup math shader
    */
   template<typename T>
   inline T ClampZero(T x)
   {
      return (x>0.0)?x:0.0;
   }
   /*!
    * \brief calculates binomial for bicubic interpolation 
    * \param x value
    * \return bicubic binomial
    * \ingroup math shader
    */
   template<typename T>
   inline T BCurve(T x)
   {
      T p0=ClampZero<T>(x+2);
      T p1=ClampZero<T>(x+1);
      T p2=ClampZero<T>(x);
      T p3=ClampZero<T>(x-1);
      return ((p0*p0*p0) - 4*(p1*p1*p1) + 6*(p2*p2*p2) - 4*(p3*p3*p3) ) / 6.0; 
   }

   /*!
    * \brief This function calculates bicubic interpolation (4x4)
    * \param tx input coordinate in x direction
    * \param ty input coordinate in y direction
    * \param P00 input value at (0,0) 
    * \param P01 input value at (0,1)
    * \param P02 input value at (0,2)
    * \param P03 input value at (0,3)
    * \param P10 input value at (1,0)
    * \param P11 input value at (1,1)
    * \param P12 input value at (1,2)
    * \param P13 input value at (1,3)
    * \param P20 input value at (2,0)
    * \param P21 input value at (2,1)
    * \param P22 input value at (2,2)
    * \param P23 input value at (2,3)
    * \param P30 input value at (3,0)
    * \param P31 input value at (3,1)
    * \param P32 input value at (3,2)
    * \param P33 input value at (3,3)
    * \return
    * \ingroup math shader
    */
   template <typename T>
   inline T BiCubic(
      T tx, T ty,
      T P00, T P01, T P02, T P03,
      T P10, T P11, T P12, T P13,
      T P20, T P21, T P22, T P23,
      T P30, T P31, T P32, T P33)
   {
      T u=Fract<T>(tx);
      T v=Fract<T>(ty);

      T Rdx[4],Rdy[4];
      for(int n=0;n<=3;n++)
      {
         Rdx[n]=BCurve<T>(n-1-u);
         Rdy[n]=BCurve<T>(n-1-v);
      }

      float s;

      s =P00*Rdx[0]*Rdy[0];
      s+=P01*Rdx[1]*Rdy[0];
      s+=P02*Rdx[2]*Rdy[0];
      s+=P03*Rdx[3]*Rdy[0];

      s+=P10*Rdx[0]*Rdy[1];
      s+=P11*Rdx[1]*Rdy[1];
      s+=P12*Rdx[2]*Rdy[1];
      s+=P13*Rdx[3]*Rdy[1];

      s+=P20*Rdx[0]*Rdy[2];
      s+=P21*Rdx[1]*Rdy[2];
      s+=P22*Rdx[2]*Rdy[2];
      s+=P23*Rdx[3]*Rdy[2];

      s+=P30*Rdx[0]*Rdy[3];
      s+=P31*Rdx[1]*Rdy[3];
      s+=P32*Rdx[2]*Rdy[3];
      s+=P33*Rdx[3]*Rdy[3];

      return s; 
   }

   //--------------------------------------------------------------------------

   //! \ingroup math shader
   template<typename T>
   inline T Smoothstep(T a, T b, T x) //!< 3*x^2 - 2*x^3 Cubic Approximation, crashes if a==b
   {
      if (x<a) return 0.0;
      if (x>=b) return 1.0;
      x = (x-a) / (b-a);
      return SCurve<T>(x);    
   }

   //--------------------------------------------------------------------------
  
   //! \ingroup math shader
   template<typename T>
   inline T Pulse(T a, T b, T x) //!< step(a,x) - step(b,x)
   {
      return (Step<T>(a,x) - Step<T>(b,x));
   }

   //--------------------------------------------------------------------------

   //! \ingroup math shader
   template<typename T>
   inline T Gammacorrect(T gamma, T x)
   {
      return (T)pow((T)x, (T)1.0f/gamma);
   }

   //--------------------------------------------------------------------------
   //! \ingroup math shader
   template<typename T>
   inline T Bias(T b, T x)
   {
      return (T)pow(x,(T)log(b)/(T)log(0.5));
   }

   //--------------------------------------------------------------------------
   //! \ingroup math shader
   template<typename T>
   inline T Gain(T a, T b)
   {
      T p = log(1.0 - b) / log(0.5);
     
      if (a < 0.001)
         return 0.0;
      else if (a > 0.999)
         return 1.0;
      if (a < 0.5)
         return (T)pow((T)2.0 * a, (T)p) / 2.0;
      else
         return 1.0 - (T)pow((float)2.0 * (1.0 - a), (T)p) / 2.0;
   }

   //--------------------------------------------------------------------------
   // Todo: create templates of those functions:
   OPENGLOBE_API float Noise(vec3<float>& f);
   OPENGLOBE_API float Noise(vec2<float>& f);
   OPENGLOBE_API float Noise(float f);
   
   OPENGLOBE_API float Turbulence(float t, float freq);
   OPENGLOBE_API float Turbulence2(vec2<float> v, float freq);
   OPENGLOBE_API float Turbulence3(vec2<float> v, float freq);

   template<typename T>
   inline T Cloud(vec3<T> Q, T cloudsize = 0.15f, T fm = 2.5f)
   {
      T value = 0.0;
      T i;

      for (i=0.0; i<4.0; i+=1.0)
      {
         value += (Noise(vec3<float>(Q * (cloudsize * 0.25f)))) / cloudsize ;
         cloudsize = cloudsize*fm; 
      }
      return value;
   }

   //--------------------------------------------------------------------------
   //#define Odd(x) ((x)&1)
   //#define Even(x) (!((x)&1))
   //--------------------------------------------------------------------------
   //--------------------------------------------------------------------------
   //! \ingroup math shader
   inline unsigned long Pow2(int k)
   {
      return	(unsigned long(1) << (k));
      /*int r = 1;
      for (int i=0;i<k;i++) r <<= 1;
      return r;*/
   }
   //--------------------------------------------------------------------------
   //! \ingroup math shader
   inline int Nextpow2(int n) //!< next power of 2: 2^A of n.
   {
      int p2=1; if (n==0) return 1; for (; p2<n; p2<<=1); return p2;
   }
   //--------------------------------------------------------------------------
   //! \ingroup math shader
   template<typename T>
   bool IsPower2(T i)
   {
      int	nExp;
      double mant = frexp((double)i, &nExp);
      return (mant == 0.5);
   }

   //*************************
   // INTERSECTIONS
   //*************************
   //! \ingroup math intersections
   template<typename T>
   vec3<T> ClosestPtPointTriangle(vec3<T>& p, vec3<T>& a, vec3<T>& b, vec3<T>& c)
   {
      vec3<T> ab = b - a;
      vec3<T> ac = c - a;
      vec3<T> ap = p - a;
      T d1 = dot<T>(ab, ap);
      T d2 = dot<T>(ac, ap);
      if (d1 <= 0.0f && d2 <= 0.0f) return a; // barycentric coordinates (1,0,0)

      // Check if P in vertex region outside B
      vec3<T> bp = p - b;
      T d3 = dot<T>(ab, bp);
      T d4 = dot<T>(ac, bp);
      if (d3 >= 0.0f && d4 <= d3) return b; // barycentric coordinates (0,1,0)

      T vc = d1*d4 - d3*d2;
      if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
         T v = d1 / (d1 - d3);
         return a + v * ab; // barycentric coordinates (1-v,v,0)
      }

      Vector cp = p - c;
      T d5 = dot<T>(ab, cp);
      T d6 = dot<T>(ac, cp);
      if (d6 >= 0.0f && d5 <= d6) return c; // barycentric coordinates (0,0,1)

      T vb = d5*d2 - d1*d6;
      if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
         T w = d2 / (d2 - d6);
         return a + w * ac; // barycentric coordinates (1-w,0,w)
      }

      T va = d3*d6 - d5*d4;
      if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
         T w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
         return b + w * (c - b); // barycentric coordinates (0,1-w,w)
      }

      T denom = 1.0f / (va + vb + vc);
      T v = vb * denom;
      T w = vc * denom;
      return a + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
   }

   //--------------------------------------------------------------------------
   //! \ingroup math intersections
   template<typename T>
   int TestSphereTriangle(vec3<T> sphereCenter, T sphereRadius, vec3<T>& a, vec3<T>& b, vec3<T>& c, vec3<T>& p)
   {
      p = ClosestPtPointTriangle(sphereCenter, a, b, c);
      Vector v = p - sphereCenter;
      return dot<T>(v, v) <= sphereRadius * sphereRadius;
   }

   //----------------------
   // Intersect 3 Planes
   //----------------------

   //! \ingroup math intersections
   template<typename T>
   std::pair<bool, vec3<T> > intersect(const plane3<T> & p1, const plane3<T> & p2, const plane3<T> & p3)
   {
      double d = dot<T>(p1.Normal, cross<T>(p2.Normal, p3.Normal));
      if (d == 0.0) 
      {
         return std::make_pair(false, vec3<T>(0,0,0));
      }
      vec3<T> v = ((cross<T>(p2.Normal,p3.Normal)* -p1.D) - 
         (cross<T>(p3.Normal,p1.Normal)*p2.D) - 
         (cross<T>(p1.Normal,p2.Normal)*p3.D)) / d;

      vec3<T> vResult(v.x, v.y, v.z);
      return std::make_pair(true, vResult);
   }
   
   //! \ingroup math intersections
   //! \brief Intersect triangle
   //!
   //! Code based on: "Ray-Triangle Intersection Test Routines".\n
   //! Different optimizations of Tomas Moller and Ben Trumbore's 
   //! code from journals of graphics tools (JGT) http://www.acm.org/jgt/                                                 
   OPENGLOBE_API int IntersectTriangle( double orig[3], double dir[3],
      double vert0[3], double vert1[3], double vert2[3],
      double *t, double *u, double *v);

   //! \ingroup math intersections
   OPENGLOBE_API int IntersectTriangle(const vec3<double>& orig, const vec3<double>& dir,
      const vec3<double>& vert0, const vec3<double>& vert1, const vec3<double>& vert2,
      double /*out*/ *t, double /*out*/ *u, double /*out*/ *v);

   //! \ingroup math intersections
   OPENGLOBE_API int IntersectTriangle_flt(const vec3<float>& orig, const vec3<float>& dir,
      const vec3<float>& vert0, const vec3<float>& vert1, const vec3<float>& vert2,
      float /*out*/ *t, float /*out*/ *u, float /*out*/ *v);

   //! \ingroup math intersections
   OPENGLOBE_API void GetTrianglePos( vec3<double>& vPos0, vec3<double>& vPos1, vec3<double>& vPos2,
      double dU, double dV, vec3<double>* pResult);
   
   //--------------------------------------------------------------------------

   // 3D Object Intersection
   // List at: http://www.realtimerendering.com/int/

   //--------------------------------------------------------------------------
   /*
   A Simple Method for Box-Sphere Intersection Testing
   by Jim Arvo
   from "Graphics Gems", Academic Press, 1990
   */

   /*!
   *  Axis Aligned Box <-> Sphere Intersection
   *
   *  This routine tests for intersection between an n-dimensional             
   *  axis-aligned box and an n-dimensional sphere.  The mode argument       
   *  indicates whether the objects are to be regarded as surfaces or        
   *  solids.  The values are:                                               
   *                                                                         
   *     mode                                                                
   *                                                                         
   *       0   Hollow Box, Hollow Sphere                                     
   *       1   Hollow Box, Solid  Sphere
   *       2   Solid  Box, Hollow Sphere                                     
   *       3   Solid  Box, Solid  Sphere                                     
   *                                                                         
   */          
   //! \param Bmin;   The minimum of the box for each axis.  
   //! \param Bmax;   The maximum of the box for each axis. 
   //! \param C;      The sphere center in n-space.         
   //! \param r;      The radius of the sphere.             
   //! \param mode;   Selects hollow or solid.              
   //! \ingroup math intersections
   template<typename T, int dimension>
   bool BoxSphereIntersect(T Bmin[dimension], T Bmax[dimension], T C[dimension], T r, int mode)
   {
      T  a, b;
      T  dmin, dmax;
      T  r2 = (T)sqrt(r);
      int   i;
      bool  face;


      switch( mode )
      {
      case 0: /* Hollow Box - Hollow Sphere */
         dmin = 0;
         dmax = 0;
         face = false;
         for( i = 0; i < dimension; i++ ) {
            a = (T)sqrt( C[i] - Bmin[i] );
            b = (T)sqrt( C[i] - Bmax[i] );
            dmax += math::Max<T>( a, b );
            if( C[i] < Bmin[i] ) {
               face = true;
               dmin += a;
            }
            else if( C[i] > Bmax[i] ) {
               face = true;
               dmin += b;
            }
            else if( math::Min<T>( a, b ) <= r2 ) face = true;
         }
         if(face && ( dmin <= r2 ) && ( r2 <= dmax)) return(true);
         break;

      case 1: /* Hollow Box - Solid Sphere */
         dmin = 0;
         face = false;
         for( i = 0; i < dimension; i++ ) {
            if( C[i] < Bmin[i] ) {
               face = true;
               dmin += (T)sqrt( C[i] - Bmin[i] );
            }
            else if( C[i] > Bmax[i] ) {
               face = true;
               dmin += (T)sqrt( C[i] - Bmax[i] );     
            }
            else if( C[i] - Bmin[i] <= r ) face = true;
            else if( Bmax[i] - C[i] <= r ) face = true;
         }
         if( face && ( dmin <= r2 ) ) return( true );
         break;


      case 2: /* Solid Box - Hollow Sphere */
         dmax = 0;
         dmin = 0;
         for( i = 0; i < dimension; i++ ) {
            a = (T)sqrt( C[i] - Bmin[i] );
            b = (T)sqrt( C[i] - Bmax[i] );
            dmax += math::Max<T>( a, b );
            if( C[i] < Bmin[i] ) dmin += a; else
               if( C[i] > Bmax[i] ) dmin += b;
         }
         if( dmin <= r2 && r2 <= dmax ) return( true );
         break;

      case 3: /* Solid Box - Solid Sphere */
         dmin = 0;
         for( i = 0; i < dimension; i++ ) {
            if( C[i] < Bmin[i] ) dmin += (T)sqrt(C[i] - Bmin[i] ); else
               if( C[i] > Bmax[i] ) dmin += (T)sqrt( C[i] - Bmax[i] );     
         }
         if( dmin <= r2 ) return( true );
         break;

      } /* end switch */

      return false;
   }



} // namespace math


#endif