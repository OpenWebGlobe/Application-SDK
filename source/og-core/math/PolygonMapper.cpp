

#include "PolygonMapper.h"
#include "math/mat4.h"
#include "math/mathutils.h"
#include "math/bestfit.h"



//bool PolygonMapper::ProjectPolygon(const std::vector<vec3<double> >& vecPosition, std::vector<vec2<double> >& vecOutputPosition)
//{
//   vecOutputPosition.clear();
//
//   if (vecPosition.size()<3)
//      return false;
//
//   vec3<double> v0;
//   vec3<double> v1;
//   vec3<double> v2;
//
//   double* points = new double[3*vecPosition.size()];
//   double plane[4];
//
//   for (size_t i=0;i<vecPosition.size();i++)
//   {
//      points[i*3+0] = vecPosition[i].x;
//      points[i*3+1] = vecPosition[i].y;
//      points[i*3+2] = vecPosition[i].z;
//   }
//
//   getBestFitPlane(vecPosition.size(),points,sizeof(double)*3,0,0,plane);
//
//   delete[] points;
//
//   if ((double)fabs(plane[3])<DBL_EPSILON)
//   {
//      plane[0] /= plane[3];
//      plane[1] /= plane[3];
//      plane[2] /= plane[3];
//   }
//
//   v2.x = plane[0];
//   v2.y = plane[1];
//   v2.z = plane[2];
//   
//   double inv_length = 1.0 / v2.Normalize();
//
//   if( math::Abs<double>(v2.x) >= math::Abs<double>(v2.y) )
//   {
//      // W.x or W.z is the largest magnitude component, swap them
//      v0.x = -v2.z * inv_length;
//      v0.y = 0;
//      v0.z = +v2.x * inv_length;
//   }
//   else
//   {
//      // W.y or W.z is the largest magnitude component, swap them
//      v0.x = 0;
//      v0.y = +v2.z * inv_length;
//      v0.z = -v2.y * inv_length;
//   }
//
//   v1 = cross_dbl(v0, v2);
//
//   v0.Normalize();
//   v1.Normalize();
//   v2.Normalize();
//
//   mat4<double> M;
//
//   M.Set(v0.x, v1.x, v2.x, vecPosition[0].x,
//      v0.y, v1.y, v2.y, vecPosition[0].y,
//      v0.z, v1.z, v2.z, vecPosition[0].z,
//      0,    0,    0,   1);
//
//   mat4<double> Minv = M.Inverse();
//
//   for (size_t i=0;i<vecPosition.size(); i++)
//   {
//      vec3<double> vOut = Minv.vec3mul(vecPosition[i]);
//      vec2<double> vOut2; 
//      vOut2.x = vOut.x; 
//      vOut2.y = vOut.y;
//      vecOutputPosition.push_back(vOut2);
//   }
//
//   return true;
//}



bool PolygonMapper::ProjectPolygon(const std::vector<vec3<double> >& vecPosition, std::vector<vec2<double> >& vecOutputPosition)
{
   vecOutputPosition.clear();

   if (vecPosition.size()<3)
      return false;

   // Convention:
   // the x-axis the the axis that is most conformal to the new x-axis
   //
   // Example:
   //
   //     C
   //     /\
   //    /  \      -> AB should be new x-Axis because AB is already close to x-Axis
   //   /    \
   // A ------ B  

   vec3<double> A;
   vec3<double> B;
   vec3<double> C;

   int i,j,k;
   double maxPlane = -1e20;
   i = 0;
   j = 1;
   k = 2;

   for (size_t q = 0;q<vecPosition.size();q++)
   {
      for (size_t r = 0;r<vecPosition.size();r++)
      {
         if (r>q)
         {
            for (size_t s = 0;s<vecPosition.size();s++)
            {
               if (s>r)
               {
                  A = vecPosition[q];
                  B = vecPosition[r];
                  C = vecPosition[s];

                  //double area = math::triangle_area_3d(A.x, A.y, A.z, B.x, B.y, B.z, C.x, C.y, C.z);
                  // Area with herons formula:
                  double ha = length<double>(B-C);
                  double hb = length<double>(A-C);
                  double hc = length<double>(B-A);
                  double hs = (ha + hb + hc) / 2.0;
                  double area = sqrt(hs*(hs-ha)*(hs-hb)*(hs-hc));
                  if (area > 0 && area > maxPlane)
                  {
                     maxPlane = area;
                     i = q;
                     j = r;
                     k = s;
                  }                  
               }
            }
         }

      }
   }
   A = vecPosition[i];
   B = vecPosition[j];
   C = vecPosition[k];

   vec3<double> v0,v1,v2;
   vec3<double> a;
   vec3<double> b;


   vec3<double> vecX(1,0,0);

   double abx = dot<double>(B-A,vecX);
   double bcx = dot<double>(C-B,vecX);
   double cax = dot<double>(A-C,vecX);
   vec3<double> P;

   double res = math::Max<double>(abx,bcx);
   res = math::Max<double>(res, cax);
   
   if (res == abx)    // Case 1: AB is new x-Axis
   {
      a = B-A;
      b = C-A;
      P = A;
   }
   else if (res == bcx)   // Case 2: BC is new x-Axis
   {
      a = C-B;
      b = A-B;
      P = B;
   }
   else if (res == cax)  // Case 3: CA is new x-Axis
   {
      a = A-C;
      b = B-C;
      P = C;
   }

   v2 = cross_dbl(a,b);
   v0 = a;
   v1 = cross_dbl(v2,v0);

   v0.Normalize();
   v1.Normalize();
   v2.Normalize();

   mat4<double> M;

   M.Set(v0.x, v1.x, v2.x, P.x,
      v0.y, v1.y, v2.y, P.y,
      v0.z, v1.z, v2.z, P.z,
      0,    0,    0,   1);

   mat4<double> Minv = M.Inverse();

   for (size_t i=0;i<vecPosition.size(); i++)
   {
      vec3<double> vOut = Minv.vec3mul(vecPosition[i]);
      vec2<double> vOut2; 
      vOut2.x = vOut.x; 
      vOut2.y = vOut.y;
      vecOutputPosition.push_back(vOut2);
   }

   return true;
}



bool PolygonMapper::Triangulate(const std::vector<vec3<double> >& vecPosition, std::vector<int>& vecTriangleIndices)
{

   vecTriangleIndices.clear();

   if (vecPosition.size()<3)
      return false;
   else if (vecPosition.size() == 3)
   {
      // if input is already a triangle, return it...
      vecTriangleIndices.push_back(0);
      vecTriangleIndices.push_back(1);
      vecTriangleIndices.push_back(2);
      return true; 
   }

   std::vector<vec2<double> > vpos2;
   if (!ProjectPolygon(vecPosition, vpos2))
      return false;


   // only 1st quadrant for triangulation:
   double minx = 1e20;
   double miny = 1e20;
   for (size_t i=0;i<vpos2.size();i++)
   {
      minx = math::Min<double>(minx, vpos2[i].x);
      miny = math::Min<double>(miny, vpos2[i].y);
   }

   for (size_t i=0;i<vpos2.size();i++)
   {
      if (minx<0)
      {
         vpos2[i].x = vpos2[i].x - minx; 
      }
      if (miny<0)
      {
         vpos2[i].y = vpos2[i].y - miny; 
      }
   }

   //TriangulateEC<double> tri(vpos2, Query::QT_REAL, Math<double>::EPSILON, vecTriangleIndices);

   return true;
}


// Calculate area of polygon
double PolygonMapper::CalcArea(const std::vector<vec3<double> > &vecPosition)
{
   std::vector<vec2<double> > vec2DPosition;

   ProjectPolygon(vecPosition, vec2DPosition);

   double A = 0.0;
   for (size_t i=0;i<vec2DPosition.size()-1;i++)
   {
      A += vec2DPosition[i].x  * vec2DPosition[i+1].y - vec2DPosition[i+1].x * vec2DPosition[i].y;
   }
   A = A / 2.0;

   return A;
}
