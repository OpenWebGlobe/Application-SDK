/*******************************************************************************
Project       : Vec2  
Purpose       : Basic Configuration.
Creation Date : 1.2.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2004-2007 by Martin Christen. All rights reserved.
*******************************************************************************/

// ***************************
// Version 1.4: April 28, 2008
// ***************************

#ifndef A_VEC2_H
#define A_VEC2_H

#include <cmath>
#include <iostream>
#include <cassert>
#include "math/mathdef.h"

//! \class vec2
//! \brief Rational (float, double) vector class with 2 components.
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup math
template <typename T>
class vec2
{
public:
   typedef T TYPE;

   vec2() {x=0; y=0; }
   vec2(T xx, T yy) {Set(xx,yy);}
   vec2(const vec2<T>& other) : x(other.x), y(other.y) {}

   void Set(T xx, T yy) {x=xx; y=yy;}

   // Access Coordinates:
   inline operator const T* () const;
   inline operator T* ();
   inline T operator[] (int i) const;
   inline T& operator[] (int i);
   inline T X () const;
   inline T& X ();
   inline T Y () const;
   inline T& Y ();

   // assignment
   inline vec2& operator= (const vec2& oVec);

   // comparison
   bool operator== (const vec2& oVec) const;
   bool operator!= (const vec2& oVec) const;
   bool operator<  (const vec2& oVec) const;
   bool operator<= (const vec2& oVec) const;
   bool operator>  (const vec2& oVec) const;
   bool operator>= (const vec2& oVec) const;

   // arithmetic operations
   inline vec2 operator+ (const vec2& oVec) const;
   inline vec2 operator- (const vec2& oVec) const;
   inline vec2 operator* (T fScalar) const;
   inline vec2 operator/ (T fScalar) const;
   inline vec2 operator- () const;

   // arithmetic updates
   inline vec2& operator+= (const vec2& oVec);
   inline vec2& operator-= (const vec2& oVec);
   inline vec2& operator*= (T fScalar);
   inline vec2& operator/= (T fScalar);


   // vector operations
   inline T Length () const;
   inline T SquaredLength () const;
   inline T Dot (const vec2& oVec) const;
   inline T Normalize();

   // returns (y,-x)
   inline vec2 Perp () const;

   // returns (y,-x)/sqrt(x*x+y*y)
   inline vec2 UnitPerp () const;

   // returns DotPerp((x,y),(V.x,V.y)) = x*V.y - y*V.x
   inline T DotPerp (const vec2& oVec) const;

   // Compute the barycentric coordinates of the point with respect to the
   // triangle <V0,V1,V2>, P = b0*V0 + b1*V1 + b2*V2, where b0 + b1 + b2 = 1.
   void GetBarycentrics (const vec2& oVec0, const vec2& oVec1,
      const vec2& oVec2, T afBary[3]) const;

   // Gram-Schmidt orthonormalization.  Take linearly independent vectors U
   // and V and compute an orthonormal set (unit length, mutually
   // perpendicular).
   static void Orthonormalize (vec2& rkU, vec2& oVec);

   // Input V must be a nonzero vector.  The output is an orthonormal basis
   // {U,V}.  The input V is normalized by this function.  If you know V is
   // already unit length, use U = V.Perp().
   static void GenerateOrthonormalBasis (vec2& rkU, vec2& oVec);

   // Compute the extreme values.
   static void ComputeExtremes(int iVQuantity, const vec2* pPointArray, vec2& rkMin, vec2& rkMax);


   /*static const vec2 ZERO;    // (0,0)
   static const vec2 UNIT_X;  // (1,0)
   static const vec2 UNIT_Y;  // (0,1)
   static const vec2 ONE;     // (1,1) */

   union
   {
      struct { T x, y; };
      struct { T xy[2]; };
   };

private:
   inline int CompareArrays (const vec2& oVec) const;

};

//--------------------------------------------------------------------------
template <class T>
inline vec2<T>::operator const T* () const
{
   return xy;
}
//--------------------------------------------------------------------------
template <class T>
inline vec2<T>::operator T* ()
{
   return xy;
}
//--------------------------------------------------------------------------
template <class T>
inline T vec2<T>::operator[] (int i) const
{
   return xy[i];
}
//--------------------------------------------------------------------------
template <class T>
inline T& vec2<T>::operator[] (int i)
{
   return xy[i];
}
//--------------------------------------------------------------------------
template <class T>
inline T vec2<T>::X () const
{
   return xy[0];
}
//--------------------------------------------------------------------------
template <class T>
inline T& vec2<T>::X ()
{
   return xy[0];
}
//--------------------------------------------------------------------------
template <class T>
inline T vec2<T>::Y () const
{
   return xy[1];
}
//--------------------------------------------------------------------------
template <class T>
inline T& vec2<T>::Y ()
{
   return xy[1];
}
//--------------------------------------------------------------------------
template <class T>
inline vec2<T>& vec2<T>::operator= (const vec2& oVec)
{
   xy[0] = oVec.xy[0];
   xy[1] = oVec.xy[1];
   return *this;
}
//--------------------------------------------------------------------------
template <class T>
bool vec2<T>::operator== (const vec2& oVec) const
{
   return CompareArrays(oVec) == 0;
}
//--------------------------------------------------------------------------
template <class T>
bool vec2<T>::operator!= (const vec2& oVec) const
{
   return CompareArrays(oVec) != 0;
}
//--------------------------------------------------------------------------
template <class T>
bool vec2<T>::operator< (const vec2& oVec) const
{
   return CompareArrays(oVec) < 0;
}
//--------------------------------------------------------------------------
template <class T>
bool vec2<T>::operator<= (const vec2& oVec) const
{
   return CompareArrays(oVec) <= 0;
}
//--------------------------------------------------------------------------
template <class T>
bool vec2<T>::operator> (const vec2& oVec) const
{
   return CompareArrays(oVec) > 0;
}
//--------------------------------------------------------------------------
template <class T>
bool vec2<T>::operator>= (const vec2& oVec) const
{
   return CompareArrays(oVec) >= 0;
}
//--------------------------------------------------------------------------
template <class T>
inline vec2<T> vec2<T>::operator+ (const vec2& oVec) const
{
   return vec2(
      xy[0]+oVec.xy[0],
      xy[1]+oVec.xy[1]);
}
//--------------------------------------------------------------------------
template <class T>
inline vec2<T> vec2<T>::operator- (const vec2& oVec) const
{
   return vec2(
      xy[0]-oVec.xy[0],
      xy[1]-oVec.xy[1]);
}
//--------------------------------------------------------------------------
template <class T>
inline vec2<T> vec2<T>::operator* (T fScalar) const
{
   return vec2(
      fScalar*xy[0],
      fScalar*xy[1]);
}
//--------------------------------------------------------------------------
template <class T>
inline vec2<T> vec2<T>::operator/ (T fScalar) const
{
   vec2 kQuot;

   if (fScalar != (T)0.0)
   {
      T fInvScalar = ((T)1.0)/fScalar;
      kQuot.xy[0] = fInvScalar*xy[0];
      kQuot.xy[1] = fInvScalar*xy[1];
   }
   else
   {
      kQuot.xy[0] = Math<T>::MAX_REAL;
      kQuot.xy[1] = Math<T>::MAX_REAL;
   }

   return kQuot;
}
//--------------------------------------------------------------------------
template <class T>
inline vec2<T> vec2<T>::operator- () const
{
   return vec2(
      -xy[0],
      -xy[1]);
}
//--------------------------------------------------------------------------
template <class T>
inline vec2<T> operator* (T fScalar, const vec2<T>& oVec)
{
   return vec2<T>(
      fScalar*oVec[0],
      fScalar*oVec[1]);
}
//--------------------------------------------------------------------------
template <class T>
inline vec2<T>& vec2<T>::operator+= (const vec2& oVec)
{
   xy[0] += oVec.xy[0];
   xy[1] += oVec.xy[1];
   return *this;
}
//--------------------------------------------------------------------------
template <class T>
inline vec2<T>& vec2<T>::operator-= (const vec2& oVec)
{
   xy[0] -= oVec.xy[0];
   xy[1] -= oVec.xy[1];
   return *this;
}
//--------------------------------------------------------------------------
template <class T>
inline vec2<T>& vec2<T>::operator*= (T fScalar)
{
   xy[0] *= fScalar;
   xy[1] *= fScalar;
   return *this;
}
//--------------------------------------------------------------------------
template <class T>
inline vec2<T>& vec2<T>::operator/= (T fScalar)
{
   if (fScalar != (T)0.0)
   {
      T fInvScalar = ((T)1.0)/fScalar;
      xy[0] *= fInvScalar;
      xy[1] *= fInvScalar;
   }
   else
   {
      xy[0] = Math<T>::MAX_REAL;
      xy[1] = Math<T>::MAX_REAL;
   }

   return *this;
}
//--------------------------------------------------------------------------

template <class T>
inline T vec2<T>::Length () const
{
   return Math<T>::Sqrt(xy[0]*xy[0] + xy[1]*xy[1]);
}
//----------------------------------------------------------------------------
template <class T>
inline T vec2<T>::SquaredLength () const
{
   return xy[0]*xy[0] + xy[1]*xy[1];
}
//----------------------------------------------------------------------------
template <class T>
inline T vec2<T>::Dot (const vec2& oVec) const
{
   return xy[0]*oVec.xy[0] + xy[1]*oVec.xy[1];
}
//----------------------------------------------------------------------------
template <class T>
inline T vec2<T>::Normalize()
{
   T fLength = Length();

   if (fLength > Math<T>::ZERO_TOLERANCE)
   {
      T fInvLength = ((T)1.0)/fLength;
      xy[0] *= fInvLength;
      xy[1] *= fInvLength;
   }
   else
   {
      fLength = (T)0.0;
      xy[0] = (T)0.0;
      xy[1] = (T)0.0;
   }

   return fLength;
}
//----------------------------------------------------------------------------
template <class T>
inline vec2<T> vec2<T>::Perp () const
{
   return vec2(xy[1],-xy[0]);
}
//----------------------------------------------------------------------------
template <class T>
inline vec2<T> vec2<T>::UnitPerp () const
{
   vec2 kPerp(xy[1],-xy[0]);
   kPerp.Normalize();
   return kPerp;
}
//----------------------------------------------------------------------------
template <class T>
inline T vec2<T>::DotPerp (const vec2& oVec) const
{
   return xy[0]*oVec.xy[1] - xy[1]*oVec.xy[0];
}
//----------------------------------------------------------------------------
template <class T>
void vec2<T>::GetBarycentrics (const vec2& oVec0, const vec2& oVec1,
                                     const vec2& oVec2, T afBary[3]) const
{
   // Compute the vectors relative to V2 of the triangle.
   vec2 akDiff[3] =
   {
      oVec0 - oVec2,
      oVec1 - oVec2,
      *this - oVec2
   };

   // If the vertices have large magnitude, the linear system of equations
   // for computing barycentric coordinates can be ill-conditioned.  To avoid
   // this, uniformly scale the triangle edges to be of order 1.  The scaling
   // of all differences does not change the barycentric coordinates.
   T fMax = (T)0.0;
   int i;
   for (i = 0; i < 2; i++)
   {
      for (int j = 0; j < 2; j++)
      {
         T fValue = Math<T>::FAbs(akDiff[i][j]);
         if (fValue > fMax)
         {
            fMax = fValue;
         }
      }
   }

   // Scale down only large data.
   T fInvMax = (T)0.0;
   if (fMax > (T)1.0)
   {
      fInvMax = ((T)1.0)/fMax;
      for (i = 0; i < 3; i++)
      {
         akDiff[i] *= fInvMax;
      }
   }

   T fDet = akDiff[0].DotPerp(akDiff[1]);
   if (Math<T>::FAbs(fDet) > Math<T>::ZERO_TOLERANCE)
   {
      T fInvDet = ((T)1.0)/fDet;
      afBary[0] = akDiff[2].DotPerp(akDiff[1])*fInvDet;
      afBary[1] = akDiff[0].DotPerp(akDiff[2])*fInvDet;
      afBary[2] = (T)1.0 - afBary[0] - afBary[1];
   }
   else
   {
      // The triangle is a sliver.  Determine the longest edge and
      // compute barycentric coordinates with respect to that edge.
      vec2 kE2 = oVec0 - oVec1;
      if (fInvMax != (T)0.0)
      {
         kE2 *= fInvMax;
      }

      T fMaxSqrLength = kE2.SquaredLength();
      int iMaxIndex = 2;
      T fSqrLength = akDiff[1].SquaredLength();
      if (fSqrLength > fMaxSqrLength)
      {
         iMaxIndex = 1;
         fMaxSqrLength = fSqrLength;
      }
      fSqrLength = akDiff[0].SquaredLength();
      if (fSqrLength > fMaxSqrLength)
      {
         iMaxIndex = 0;
         fMaxSqrLength = fSqrLength;
      }

      if (fMaxSqrLength > Math<T>::ZERO_TOLERANCE)
      {
         T fInvSqrLength = ((T)1.0)/fMaxSqrLength;
         if (iMaxIndex == 0)
         {
            // P-V2 = t(V0-V2)
            afBary[0] = akDiff[2].Dot(akDiff[0])*fInvSqrLength;
            afBary[1] = (T)0.0;
            afBary[2] = (T)1.0 - afBary[0];
         }
         else if (iMaxIndex == 1)
         {
            // P-V2 = t(V1-V2)
            afBary[0] = (T)0.0;
            afBary[1] = akDiff[2].Dot(akDiff[1])*fInvSqrLength;
            afBary[2] = (T)1.0 - afBary[1];
         }
         else
         {
            // P-V1 = t(V0-V1)
            akDiff[2] = *this - oVec1;
            if (fInvMax != (T)0.0)
            {
               akDiff[2] *= fInvMax;
            }

            afBary[0] = akDiff[2].Dot(kE2)*fInvSqrLength;
            afBary[1] = (T)1.0 - afBary[0];
            afBary[2] = (T)0.0;
         }
      }
      else
      {
         // The triangle is a nearly a point, just return equal weights.
         afBary[0] = ((T)1.0)/(T)3.0;
         afBary[1] = afBary[0];
         afBary[2] = afBary[0];
      }
   }
}
//----------------------------------------------------------------------------
template <class T>
void vec2<T>::Orthonormalize (vec2& rkU, vec2& oVec)
{
   // If the input vectors are v0 and v1, then the Gram-Schmidt
   // orthonormalization produces vectors u0 and u1 as follows,
   //
   //   u0 = v0/|v0|
   //   u1 = (v1-(u0*v1)u0)/|v1-(u0*v1)u0|
   //
   // where |A| indicates length of vector A and A*B indicates dot
   // product of vectors A and B.

   // compute u0
   rkU.Normalize();

   // compute u1
   T fDot0 = rkU.Dot(oVec); 
   oVec -= rkU*fDot0;
   oVec.Normalize();
}
//----------------------------------------------------------------------------
template <class T>
void vec2<T>::GenerateOrthonormalBasis (vec2& rkU, vec2& oVec)
{
   oVec.Normalize();
   rkU = oVec.Perp();
}
//----------------------------------------------------------------------------
template <class T>
void vec2<T>::ComputeExtremes (int iVQuantity, const vec2* pPointArray, vec2& rkMin, vec2& rkMax)
{
   assert(iVQuantity > 0 && pPointArray);

   rkMin = pPointArray[0];
   rkMax = rkMin;
   for (int i = 1; i < iVQuantity; i++)
   {
      const vec2<T>& rkPoint = pPointArray[i];
      for (int j = 0; j < 2; j++)
      {
         if (rkPoint[j] < rkMin[j])
         {
            rkMin[j] = rkPoint[j];
         }
         else if (rkPoint[j] > rkMax[j])
         {
            rkMax[j] = rkPoint[j];
         }
      }
   }
}
//-----------------------------------------------------------------------------
template <class T>
inline int vec2<T>::CompareArrays (const vec2& oOther) const
{
   return memcmp(xy,oOther.xy,2*sizeof(T));
}
//-----------------------------------------------------------------------------

#endif