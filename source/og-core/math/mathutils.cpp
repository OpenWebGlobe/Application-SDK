/*******************************************************************************
Project       : i3D
Purpose       : Math Utils.
Creation Date : 01.03.2007
Author        : Martin Christen
Copyright     : This source code file is copyright (c) FHNW 2007
*******************************************************************************/

#include "math/mathutils.h"

#include <cstdlib>
#include <cstdio>




/*!
* \defgroup math Math
* \brief The Math module contains math algorithms. Primary focus are computer graphics related math, but it isn't limited to it.
* Because many math-functions were taken from the cwc library (programmed by Martin Christen),
* The coding standard for Foundation is not used correctly, i.e. class names, methods should begin with capital letters).
* This could change in future.
*/
/*@{*/
/*!
* \defgroup linalg Linear Algebra
*     \brief This group contains classes like vectors, ray, boxes, matrices, quaternions, ... 
* \defgroup shader Shader Math
*     \brief This group contains classes and functions which can be used for procedural materials, shaders (and other things).
*      Functions like Noise, Abs, Sign, Floor, Ceil, Round, Mix, Mod, Clamp, Smoothstep can be found here. 
* \defgroup geotoolbox Geo
*     \brief This group contains geo-related math classes like DegreeConverter and GeoCoord. 
* \defgroup gfxtoolbox Graphics
*     \brief This group contains math classes directly related to graphics. Things like Billboard matrix calculations can be found here.
* \defgroup spatial Spatial Datastructures
*     \brief This group contains classes for spatial data structures. Kd-trees, BVH, Quadtree etc. belongs to here.
* \defgroup intersections Intersections
*     \brief This group contains functions for intersection tests like ray hits triangles, ray hits box, triangle hits triangle etc.
*      This would also fit to \ref linalg, but because of its importance it is listed separately.
*     \todo many intersections are still missing
*/
/*@}*/


using namespace math;

namespace math
{
   // Perlin Noise / turbulence function
   float  noise1(float arg);
   float  noise2(float vec[]);
   float  noise3(float vec[]);

   float  noise(float vec[], int len)
   {
      switch (len) 
      {
      case 0:
         return 0.;
      case 1:
         return noise1(vec[0]);
      case 2:
         return noise2(vec);
      default:
         return noise3(vec);
      }
   }
   float  Turbulence(float v, float freq)
   {
      float t;
      vec2<float> vec;

      for (t = 0. ; freq >= 1. ; freq /= 2) 
      {
         v = freq * v;
         t += fabs(math::Noise(vec)) / freq;
      }
      return t;
   }


   float  Turbulence2(vec2<float> v, float freq)
   {
      float t;
      vec2<float> vec;

      for (t = 0. ; freq >= 1. ; freq /= 2) 
      {
         vec[0] = freq * v[0];
         vec[1] = freq * v[1];
         //vec[2] = freq * v[2];
         t += fabs(math::Noise(vec)) / freq;
      }
      return t;
   }

   float  Turbulence3(vec2<float> v, float freq)
   {
      float t;
      vec3<float> vec;

      for (t = 0. ; freq >= 1. ; freq /= 2) 
      {
         vec[0] = freq * v[0];
         vec[1] = freq * v[1];
         vec[2] = freq * v[2];
         t += fabs(math::Noise(vec)) / freq;
      }
      return t;
   }

   /* noise functions over 1, 2, and 3 dimensions */

   #define B 0x100
   #define BM 0xff

   #define N 0x1000
   #define NP 12   /* 2^N */
   #define NM 0xfff

   static unsigned int p[B + B + 2];
   static float g3[B + B + 2][3];
   static float g2[B + B + 2][2];
   static float g1[B + B + 2];
   static unsigned int start = 1;

   static void init(void);

   #define s_curve(t) ( t * t * (3.0f - 2.0f * t) )

   #define lerp(t, a, b) ( a + t * (b - a) )

   #define setup(i,b0,b1,r0,r1)\
      t = vec[i] + N;\
      b0 = ((int)t) & BM;\
      b1 = (b0+1) & BM;\
      r0 = t - (int)t;\
      r1 = r0 - 1.0f;

   float  noise1(float arg)
   {
      int bx0, bx1;
      float rx0, rx1, sx, t, u, v, vec[1];

      vec[0] = arg;
      if (start) 
      {
         start = 0;
         init();
      }

      setup(0, bx0,bx1, rx0,rx1);

      sx = s_curve(rx0);

      u = rx0 * g1[ p[ bx0 ] ];
      v = rx1 * g1[ p[ bx1 ] ];

      return lerp(sx, u, v);
   }

   float  noise2(float vec[2])
   {
      int bx0, bx1, by0, by1, b00, b10, b01, b11;
      float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
      unsigned int i, j;

      if (start) {
         start = 0;
         init();
      }

      setup(0, bx0,bx1, rx0,rx1);
      setup(1, by0,by1, ry0,ry1);

      i = p[ bx0 ];
      j = p[ bx1 ];

      b00 = p[ i + by0 ];
      b10 = p[ j + by0 ];
      b01 = p[ i + by1 ];
      b11 = p[ j + by1 ];

      sx = s_curve(rx0);
      sy = s_curve(ry0);

   #define at2(rx,ry) ( rx * q[0] + ry * q[1] )

      q = g2[ b00 ] ; u = at2(rx0,ry0);
      q = g2[ b10 ] ; v = at2(rx1,ry0);
      a = lerp(sx, u, v);

      q = g2[ b01 ] ; u = at2(rx0,ry1);
      q = g2[ b11 ] ; v = at2(rx1,ry1);
      b = lerp(sx, u, v);

      return lerp(sy, a, b);
   }

   float  noise3(float vec[3])
   {
      int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
      float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
      unsigned int i, j;

      if (start) {
         start = 0;
         init();
      }

      setup(0, bx0,bx1, rx0,rx1);
      setup(1, by0,by1, ry0,ry1);
      setup(2, bz0,bz1, rz0,rz1);

      i = p[ bx0 ];
      j = p[ bx1 ];

      b00 = p[ i + by0 ];
      b10 = p[ j + by0 ];
      b01 = p[ i + by1 ];
      b11 = p[ j + by1 ];

      t  = s_curve(rx0);
      sy = s_curve(ry0);
      sz = s_curve(rz0);

   #define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

      q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
      q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
      a = lerp(t, u, v);

      q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
      q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
      b = lerp(t, u, v);

      c = lerp(sy, a, b);

      q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
      q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
      a = lerp(t, u, v);

      q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
      q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
      b = lerp(t, u, v);

      d = lerp(sy, a, b);

      return lerp(sz, c, d);
   }

   static void normalize2(float v[2])
   {
      float s;

      s = sqrt(v[0] * v[0] + v[1] * v[1]);
      v[0] = v[0] / s;
      v[1] = v[1] / s;
   }

   static void normalize3(float v[3])
   {
      float s;

      s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
      v[0] = v[0] / s;
      v[1] = v[1] / s;
      v[2] = v[2] / s;
   }

   static void init(void)
   {
      int i, j, k;

      for (i = 0 ; i < B ; i++) {
         p[i] = i;

         g1[i] = (float)((rand() % (B + B)) - B) / B;

         for (j = 0 ; j < 2 ; j++)
            g2[i][j] = (float)((rand() % (B + B)) - B) / B;
         normalize2(g2[i]);

         for (j = 0 ; j < 3 ; j++)
            g3[i][j] = (float)((rand() % (B + B)) - B) / B;
         normalize3(g3[i]);
      }

      while (--i) {
         k = p[i];
         p[i] = p[j = rand() % B];
         p[j] = k;
      }

      for (i = 0 ; i < B + 2 ; i++) {
         p[B + i] = p[i];
         g1[B + i] = g1[i];
         for (j = 0 ; j < 2 ; j++)
            g2[B + i][j] = g2[i][j];
         for (j = 0 ; j < 3 ; j++)
            g3[B + i][j] = g3[i][j];
      }
   }
   //##########################################################################

   float  Noise(vec3<float>& f)
   {
      float v[3];
      v[0] = f.x; v[1] = f.y; v[2] = f.z;
      return noise3(v);
   }
   //--------------------------------------------------------------------------
   float  Noise(vec2<float>& f)
   {
      float v[2];
      v[0] = f.x; v[1] = f.y;
      return noise2(v);
   }
   //--------------------------------------------------------------------------
   float  Noise(float f)
   {
      return noise1(f);
   }
   //--------------------------------------------------------------------------

 
   //##########################################################################

   /* Ray-Triangle Intersection Test Routines          */
   /* Different optimizations of my and Ben Trumbore's */
   /* code from journals of graphics tools (JGT)       */
   /* http://www.acm.org/jgt/                          */
   /* by Tomas Moller, May 2000                        */

#include <cmath>
#include <float.h>

#define EPSILON DBL_EPSILON //1e-12
#define CROSS(dest,v1,v2) \
   dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
   dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
   dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
#define SUB(dest,v1,v2) \
   dest[0]=v1[0]-v2[0]; \
   dest[1]=v1[1]-v2[1]; \
   dest[2]=v1[2]-v2[2]; 

   /* the original jgt code */
   int IntersectTriangle_0(double orig[3], double dir[3],
      double vert0[3], double vert1[3], double vert2[3],
      double *t, double *u, double *v)
   {
      double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
      double det,inv_det;

      /* find vectors for two edges sharing vert0 */
      SUB(edge1, vert1, vert0);
      SUB(edge2, vert2, vert0);

      /* begin calculating determinant - also used to calculate U parameter */
      CROSS(pvec, dir, edge2);

      /* if determinant is near zero, ray lies in plane of triangle */
      det = DOT(edge1, pvec);

      if (det > -EPSILON && det < EPSILON)
         return 0;
      inv_det = 1.0 / det;

      /* calculate distance from vert0 to ray origin */
      SUB(tvec, orig, vert0);

      /* calculate U parameter and test bounds */
      *u = DOT(tvec, pvec) * inv_det;
      if (*u < 0.0 || *u > 1.0)
         return 0;

      /* prepare to test V parameter */
      CROSS(qvec, tvec, edge1);

      /* calculate V parameter and test bounds */
      *v = DOT(dir, qvec) * inv_det;
      if (*v < 0.0 || *u + *v > 1.0)
         return 0;

      /* calculate t, ray intersects triangle */
      *t = DOT(edge2, qvec) * inv_det;

      return 1;
   }


   /* code rewritten to do tests on the sign of the determinant */
   /* the division is at the end in the code                    */
   int IntersectTriangle_1(double orig[3], double dir[3],
      double vert0[3], double vert1[3], double vert2[3],
      double *t, double *u, double *v)
   {
      double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
      double det,inv_det;

      /* find vectors for two edges sharing vert0 */
      SUB(edge1, vert1, vert0);
      SUB(edge2, vert2, vert0);

      /* begin calculating determinant - also used to calculate U parameter */
      CROSS(pvec, dir, edge2);

      /* if determinant is near zero, ray lies in plane of triangle */
      det = DOT(edge1, pvec);

      if (det > EPSILON)
      {
         /* calculate distance from vert0 to ray origin */
         SUB(tvec, orig, vert0);

         /* calculate U parameter and test bounds */
         *u = DOT(tvec, pvec);
         if (*u < 0.0 || *u > det)
            return 0;

         /* prepare to test V parameter */
         CROSS(qvec, tvec, edge1);

         /* calculate V parameter and test bounds */
         *v = DOT(dir, qvec);
         if (*v < 0.0 || *u + *v > det)
            return 0;

      }
      else if(det < -EPSILON)
      {
         /* calculate distance from vert0 to ray origin */
         SUB(tvec, orig, vert0);

         /* calculate U parameter and test bounds */
         *u = DOT(tvec, pvec);
         /*      printf("*u=%f\n",(float)*u); */
         /*      printf("det=%f\n",det); */
         if (*u > 0.0 || *u < det)
            return 0;

         /* prepare to test V parameter */
         CROSS(qvec, tvec, edge1);

         /* calculate V parameter and test bounds */
         *v = DOT(dir, qvec) ;
         if (*v > 0.0 || *u + *v < det)
            return 0;
      }
      else return 0;  /* ray is parallell to the plane of the triangle */


      inv_det = 1.0 / det;

      /* calculate t, ray intersects triangle */
      *t = DOT(edge2, qvec) * inv_det;
      (*u) *= inv_det;
      (*v) *= inv_det;

      return 1;
   }

   /* code rewritten to do tests on the sign of the determinant */
   /* the division is before the test of the sign of the det    */
   int IntersectTriangle_2(double orig[3], double dir[3],
      double vert0[3], double vert1[3], double vert2[3],
      double *t, double *u, double *v)
   {
      double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
      double det,inv_det;

      /* find vectors for two edges sharing vert0 */
      SUB(edge1, vert1, vert0);
      SUB(edge2, vert2, vert0);

      /* begin calculating determinant - also used to calculate U parameter */
      CROSS(pvec, dir, edge2);

      /* if determinant is near zero, ray lies in plane of triangle */
      det = DOT(edge1, pvec);

      /* calculate distance from vert0 to ray origin */
      SUB(tvec, orig, vert0);
      inv_det = 1.0 / det;

      if (det > EPSILON)
      {
         /* calculate U parameter and test bounds */
         *u = DOT(tvec, pvec);
         if (*u < 0.0 || *u > det)
            return 0;

         /* prepare to test V parameter */
         CROSS(qvec, tvec, edge1);

         /* calculate V parameter and test bounds */
         *v = DOT(dir, qvec);
         if (*v < 0.0 || *u + *v > det)
            return 0;

      }
      else if(det < -EPSILON)
      {
         /* calculate U parameter and test bounds */
         *u = DOT(tvec, pvec);
         if (*u > 0.0 || *u < det)
            return 0;

         /* prepare to test V parameter */
         CROSS(qvec, tvec, edge1);

         /* calculate V parameter and test bounds */
         *v = DOT(dir, qvec) ;
         if (*v > 0.0 || *u + *v < det)
            return 0;
      }
      else return 0;  /* ray is parallell to the plane of the triangle */

      /* calculate t, ray intersects triangle */
      *t = DOT(edge2, qvec) * inv_det;
      (*u) *= inv_det;
      (*v) *= inv_det;

      return 1;
   }

   /* code rewritten to do tests on the sign of the determinant */
   /* the division is before the test of the sign of the det    */
   /* and one CROSS has been moved out from the if-else if-else */
   int IntersectTriangle_3(double orig[3], double dir[3],
      double vert0[3], double vert1[3], double vert2[3],
      double *t, double *u, double *v)
   {
      double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
      double det,inv_det;

      /* find vectors for two edges sharing vert0 */
      SUB(edge1, vert1, vert0);
      SUB(edge2, vert2, vert0);

      /* begin calculating determinant - also used to calculate U parameter */
      CROSS(pvec, dir, edge2);

      /* if determinant is near zero, ray lies in plane of triangle */
      det = DOT(edge1, pvec);

      /* calculate distance from vert0 to ray origin */
      SUB(tvec, orig, vert0);
      inv_det = 1.0 / det;

      CROSS(qvec, tvec, edge1);

      if (det > EPSILON)
      {
         *u = DOT(tvec, pvec);
         if (*u < 0.0 || *u > det)
            return 0;

         /* calculate V parameter and test bounds */
         *v = DOT(dir, qvec);
         if (*v < 0.0 || *u + *v > det)
            return 0;

      }
      else if(det < -EPSILON)
      {
         /* calculate U parameter and test bounds */
         *u = DOT(tvec, pvec);
         if (*u > 0.0 || *u < det)
            return 0;

         /* calculate V parameter and test bounds */
         *v = DOT(dir, qvec) ;
         if (*v > 0.0 || *u + *v < det)
            return 0;
      }
      else return 0;  /* ray is parallell to the plane of the triangle */

      *t = DOT(edge2, qvec) * inv_det;
      (*u) *= inv_det;
      (*v) *= inv_det;

      return 1;
   }

   int IntersectTriangle(double orig[3], double dir[3],
      double vert0[3], double vert1[3], double vert2[3],
      double *t, double *u, double *v)
   {
      return IntersectTriangle_3(orig, dir, vert0, vert1, vert2, t,u,v);
   }

   //--------------------------------------------------------------------------
   // OFFICIAL i3D Intersect Test without SSE extensions
   /* Code (IntersectTriangle_3) rewritten to support vec3 type of i3d */

   int IntersectTriangle(const vec3<double>& orig, const vec3<double>& dir,
      const vec3<double>& vert0, const vec3<double>& vert1, const vec3<double>& vert2,
      double /*out*/ *t, double /*out*/ *u, double /*out*/ *v)
   {
      double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
      double det,inv_det;

      /* find vectors for two edges sharing vert0 */
      SUB(edge1, vert1, vert0);
      SUB(edge2, vert2, vert0);

      /* begin calculating determinant - also used to calculate U parameter */
      CROSS(pvec, dir, edge2);

      /* if determinant is near zero, ray lies in plane of triangle */
      det = DOT(edge1, pvec);

      /* calculate distance from vert0 to ray origin */
      SUB(tvec, orig, vert0);
      inv_det = 1.0 / det;

      CROSS(qvec, tvec, edge1);

      if (det > EPSILON)
      {
         *u = DOT(tvec, pvec);
         if (*u < 0.0 || *u > det)
            return 0;

         /* calculate V parameter and test bounds */
         *v = DOT(dir, qvec);
         if (*v < 0.0 || *u + *v > det)
            return 0;

      }
      else if(det < -EPSILON)
      {
         /* calculate U parameter and test bounds */
         *u = DOT(tvec, pvec);
         if (*u > 0.0 || *u < det)
            return 0;

         /* calculate V parameter and test bounds */
         *v = DOT(dir, qvec) ;
         if (*v > 0.0 || *u + *v < det)
            return 0;
      }
      else return 0;  /* ray is parallel to the plane of the triangle */

      *t = DOT(edge2, qvec) * inv_det;
      (*u) *= inv_det;
      (*v) *= inv_det;

      return 1;
   }


   int IntersectTriangle_flt(const vec3<float>& orig, const vec3<float>& dir,
      const vec3<float>& vert0, const vec3<float>& vert1, const vec3<float>& vert2,
      float /*out*/ *t, float /*out*/ *u, float /*out*/ *v)
   {
      float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
      float det,inv_det;

      /* find vectors for two edges sharing vert0 */
      SUB(edge1, vert1, vert0);
      SUB(edge2, vert2, vert0);

      /* begin calculating determinant - also used to calculate U parameter */
      CROSS(pvec, dir, edge2);

      /* if determinant is near zero, ray lies in plane of triangle */
      det = DOT(edge1, pvec);

      /* calculate distance from vert0 to ray origin */
      SUB(tvec, orig, vert0);
      inv_det = 1.0f / det;

      CROSS(qvec, tvec, edge1);

      if (det > EPSILON)
      {
         *u = DOT(tvec, pvec);
         if (*u < 0.0f || *u > det)
            return 0;

         /* calculate V parameter and test bounds */
         *v = DOT(dir, qvec);
         if (*v < 0.0f || *u + *v > det)
            return 0;

      }
      else if(det < -EPSILON)
      {
         /* calculate U parameter and test bounds */
         *u = DOT(tvec, pvec);
         if (*u > 0.0f || *u < det)
            return 0;

         /* calculate V parameter and test bounds */
         *v = DOT(dir, qvec) ;
         if (*v > 0.0 || *u + *v < det)
            return 0;
      }
      else return 0;  /* ray is parallel to the plane of the triangle */

      *t = DOT(edge2, qvec) * inv_det;
      (*u) *= inv_det;
      (*v) *= inv_det;

      return 1;
   }

   //--------------------------------------------------------------------------
   
   void GetTrianglePos(vec3<double>& vPos0, vec3<double>& vPos1, vec3<double>& vPos2, double dU, double dV, vec3<double>* pResult)
   {
      double dW = 1.0 - dU - dV;

      if(pResult)
      {
         vec3<double> vTemp = vPos0 * dW + vPos1 * dU + vPos2 * dV;
         pResult->x = vTemp.x;
         pResult->y = vTemp.y;
         pResult->z = vTemp.z;         
      }
   }

   //--------------------------------------------------------------------------


}