
#include "TriangleMapper.h"
#include "math/mat4.h"
#include "math/mathutils.h"


void TriangleMapper::ProjectTriangle(const vec3<double>& A, const vec3<double>& B, const vec3<double>& C, vec3<double>& Aout, vec3<double>& Bout, vec3<double>& Cout)
{
   // Generate Orthonormal Basis from triangle: A->B is x-axis in new system

   vec3<double> v0,v1,v2;
   vec3<double> a = B-A;
   vec3<double> b = C-A;
   
   v2 = cross_dbl(a,b);
   v0 = a;
   v1 = cross_dbl(v2,v0);
   
   v0.Normalize();
   v1.Normalize();
   v2.Normalize();

   mat4<double> M;

   M.Set(v0.x, v1.x, v2.x, A.x,
         v0.y, v1.y, v2.y, A.y,
         v0.z, v1.z, v2.z, A.z,
            0,    0,    0,   1);

   mat4<double> Minv = M.Inverse();

   Aout = Minv.vec3mul(A);
   Bout = Minv.vec3mul(B);
   Cout = Minv.vec3mul(C);

   // translate triangle so that minimal value is at (0,0)

   double xmin = math::Min<double>(Aout.x, Bout.x);
   xmin = math::Min<double>(xmin, Cout.x);

   double ymin = math::Min<double>(Aout.y, Bout.y);
   ymin = math::Min<double>(ymin, Cout.y);

   // Possibly move triangle, so that there are no negative values!
   Aout.x -= xmin;
   Bout.x -= xmin;
   Cout.x -= xmin;

   Aout.y -= ymin;
   Bout.y -= ymin;
   Cout.y -= ymin;

   // elimita rounding errors in z, this is always exactly 0.0
   Aout.z = 0;
   Bout.z = 0;
   Cout.z = 0;
}


void TriangleMapper::ProjectTriangle_flt(const vec3<float>& A, const vec3<float>& B, const vec3<float>& C, vec3<float>& Aout, vec3<float>& Bout, vec3<float>& Cout)
{
   // Generate Orthonormal Basis from triangle: A->B is x-axis in new system

   vec3<float> v0,v1,v2;
   vec3<float> a;
   vec3<float> b;

   vec3<float> vecX(1,0,0);

   float ab = dot<float>(B-A,vecX);
   float bc = dot<float>(C-B,vecX);
   float ca = dot<float>(A-C,vecX);
   vec3<float> P;

   float res = math::Max<float>(ab,bc);
   res = math::Max<float>(res, ca);

   if (res == ab)    // Case 1: AB is new x-Axis
   {
      a = B-A;
      b = C-A;
      P = A;
   }
   else if (res == bc)   // Case 2: BC is new x-Axis
   {
      a = C-B;
      b = A-B;
      P = B;
   }
   else // res == ca  // Case 3: CA is new x-Axis
   {
      a = A-C;
      b = B-C;
      P = C;
   }
   
   v2 = cross_flt(a,b);
   v0 = a;
   v1 = cross_flt(v2,v0);
   
   v0.Normalize();
   v1.Normalize();
   v2.Normalize();

   mat4<float> M;

   M.Set(v0.x, v1.x, v2.x, P.x,
         v0.y, v1.y, v2.y, P.y,
         v0.z, v1.z, v2.z, P.z,
            0,    0,    0,   1);

   mat4<float> Minv = M.Inverse();

   Aout = Minv.vec3mul(A);
   Bout = Minv.vec3mul(B);
   Cout = Minv.vec3mul(C);

   // translate triangle so that minimal value is at (0,0)

   float xmin = math::Min<float>(Aout.x, Bout.x);
   xmin = math::Min<float>(xmin, Cout.x);

   float ymin = math::Min<float>(Aout.y, Bout.y);
   ymin = math::Min<float>(ymin, Cout.y);

   // Possibly move triangle, so that there are no negative values!
   Aout.x -= xmin;
   Bout.x -= xmin;
   Cout.x -= xmin;

   Aout.y -= ymin;
   Bout.y -= ymin;
   Cout.y -= ymin;

   // elimita rounding errors in z, this is always exactly 0.0
   Aout.z = 0.0f;
   Bout.z = 0.0f;
   Cout.z = 0.0f;
}
