
#ifndef _MATH_H
#define _MATH_H

#include "og.h"
#include <cmath>
#include <float.h>

//-----------------------------------------------------------------------------

// T must be float or double!
template <class T>
class Math
{
public:
   // Wrappers to hide implementations of functions.  The ACos and ASin
   // functions clamp the input argument to [-1,1] to avoid NaN issues
   // when the input is slightly larger than 1 or slightly smaller than -1.
   // Other functions have the potential for using a fast and approximate
   // algorithm rather than calling the standard math library functions.
   static T ACos (T fValue);
   static T ASin (T fValue);
   static T ATan (T fValue);
   static T ATan2 (T fY, T fX);
   static T Ceil (T fValue);
   static T Cos (T fValue);
   static T Exp (T fValue);
   static T FAbs (T fValue);
   static T Floor (T fValue);
   static T FMod (T fX, T fY);
   static T InvSqrt (T fValue);
   static T Log (T fValue);
   static T Log2 (T fValue);
   static T Log10 (T fValue);
   static T Pow (T fBase, T fExponent);
   static T Sin (T fValue);
   static T Sqr (T fValue);
   static T Sqrt (T fValue);
   static T Tan (T fValue);

   // Return -1 if the input is negative, 0 if the input is zero, and +1
   // if the input is positive.
   static int Sign (int iValue);
   static T Sign (T fValue);

   // Generate a random number in [0,1).  The random number generator may
   // be seeded by a first call to UnitRandom with a positive seed.
   static T UnitRandom (unsigned int uiSeed = 0);

   // Generate a random number in [-1,1).  The random number generator may
   // be seeded by a first call to SymmetricRandom with a positive seed.
   static T SymmetricRandom (unsigned int uiSeed = 0);

   // Generate a random number in [min,max).  The random number generator may
   // be seeded by a first call to IntervalRandom with a positive seed.
   static T IntervalRandom (T fMin, T fMax, unsigned int uiSeed = 0);

   // Fast evaluation of trigonometric and inverse trigonometric functions
   // using polynomial approximations:

   // The input must be in [0,pi/2].
   // max error sin0 = 1.7e-04, speed up = 4.0
   // max error sin1 = 1.9e-08, speed up = 2.8
   static T FastSin0 (T fAngle);
   static T FastSin1 (T fAngle);

   // The input must be in [0,pi/2]
   // max error cos0 = 1.2e-03, speed up = 4.5
   // max error cos1 = 6.5e-09, speed up = 2.8
   static T FastCos0 (T fAngle);
   static T FastCos1 (T fAngle);

   // The input must be in [0,pi/4].
   // max error tan0 = 8.1e-04, speed up = 5.6
   // max error tan1 = 1.9e-08, speed up = 3.4
   static T FastTan0 (T fAngle);
   static T FastTan1 (T fAngle);

   // The input must be in [0,1].
   // max error invsin0 = 6.8e-05, speed up = 7.5
   // max error invsin1 = 1.4e-07, speed up = 5.5
   static T FastInvSin0 (T fValue);
   static T FastInvSin1 (T fValue);

   // The input must be in [0,1].
   // max error invcos0 = 6.8e-05, speed up = 7.5
   // max error invcos1 = 1.4e-07, speed up = 5.7
   static T FastInvCos0 (T fValue);
   static T FastInvCos1 (T fValue);

   // The input must be in [-1,1]. 
   // max error invtan0 = 1.2e-05, speed up = 2.8
   // max error invtan1 = 2.3e-08, speed up = 1.8
   static T FastInvTan0 (T fValue);
   static T FastInvTan1 (T fValue);

   // A fast approximation to 1/sqrt.
   static T FastInvSqrt (T fValue);

   // Fast approximations to exp(-x).  The input x must be in [0,infinity).
   // max error negexp0 = 0.00024, speed up = 25.4
   // max error negexp1 = 0.000024, speed up = 25.4
   // max error negexp2 = 0.0000024, speed up = 20.5
   // max error negexp3 = 0.00000025, speed up = 17.3
   static T FastNegExp0 (T fValue);
   static T FastNegExp1 (T fValue);
   static T FastNegExp2 (T fValue);
   static T FastNegExp3 (T fValue);

   // constants
   static OPENGLOBE_API const T EPSILON;
   static OPENGLOBE_API const T ZERO_TOLERANCE;
   static OPENGLOBE_API const T MAX_REAL;
   static OPENGLOBE_API const T PI;
   static OPENGLOBE_API const T TWO_PI;
   static OPENGLOBE_API const T HALF_PI;
   static OPENGLOBE_API const T INV_PI;
   static OPENGLOBE_API const T INV_TWO_PI;
   static OPENGLOBE_API const T DEG_TO_RAD;
   static OPENGLOBE_API const T RAD_TO_DEG;
   static OPENGLOBE_API const T LN_2;
   static OPENGLOBE_API const T LN_10;
   static OPENGLOBE_API const T INV_LN_2;
   static OPENGLOBE_API const T INV_LN_10;
};

//-----------------------------------------------------------------------------

template <class T>
T Math<T>::ACos (T fValue)
{
   if (-(T)1.0 < fValue)
   {
      if (fValue < (T)1.0)
      {
         return (T)acos((double)fValue);
      }
      else
      {
         return (T)0.0;
      }
   }
   else
   {
      return PI;
   }
}
//-----------------------------------------------------------------------------
template <class T>
T Math<T>::ASin (T fValue)
{
   if (-(T)1.0 < fValue)
   {
      if (fValue < (T)1.0)
      {
         return (T)asin((double)fValue);
      }
      else
      {
         return HALF_PI;
      }
   }
   else
   {
      return -HALF_PI;
   }
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::ATan (T fValue)
{
   return (T)atan((double)fValue);
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::ATan2 (T fY, T fX)
{
   return (T)atan2((double)fY,(double)fX);
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::Ceil (T fValue)
{
   return (T)ceil((double)fValue);
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::Cos (T fValue)
{
   return (T)cos((double)fValue);
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::Exp (T fValue)
{
   return (T)exp((double)fValue);
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::FAbs (T fValue)
{
   return (T)fabs((double)fValue);
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::Floor (T fValue)
{
   return (T)floor((double)fValue);
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::FMod (T fX, T fY)
{
   return (T)fmod((double)fX,(double)fY);
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::InvSqrt (T fValue)
{
   return (T)(1.0/sqrt((double)fValue));
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::Log (T fValue)
{
   return (T)log((double)fValue);
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::Log2 (T fValue)
{
   return Math<T>::INV_LN_2 * (T)log((double)fValue);
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::Log10 (T fValue)
{
   return Math<T>::INV_LN_10 * (T)log((double)fValue);
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::Pow (T fBase, T fExponent)
{
   return (T)pow((double)fBase,(double)fExponent);
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::Sin (T fValue)
{
   return (T)sin((double)fValue);
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::Sqr (T fValue)
{
   return fValue*fValue;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::Sqrt (T fValue)
{
   return (T)sqrt((double)fValue);
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::Tan (T fValue)
{
   return (T)tan((double)fValue);
}
//----------------------------------------------------------------------------
template <class T>
int Math<T>::Sign (int iValue)
{
   if (iValue > 0)
   {
      return 1;
   }

   if (iValue < 0)
   {
      return -1;
   }

   return 0;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::Sign (T fValue)
{
   if (fValue > (T)0.0)
   {
      return (T)1.0;
   }

   if (fValue < (T)0.0)
   {
      return -(T)1.0;
   }

   return (T)0.0;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::UnitRandom (unsigned int uiSeed )
{
   if (uiSeed > 0)
   {
      srand(uiSeed);
   }

   double dRatio = ((double)rand())/((double)(RAND_MAX));
   return (T)dRatio;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::SymmetricRandom (unsigned int uiSeed)
{
   if (uiSeed > 0.0)
   {
      srand(uiSeed);
   }

   double dRatio = ((double)rand())/((double)(RAND_MAX));
   return (T)(2.0*dRatio - 1.0);
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::IntervalRandom (T fMin, T fMax, unsigned int uiSeed)
{
   if (uiSeed > 0)
   {
      srand(uiSeed);
   }

   double dRatio = ((double)rand())/((double)(RAND_MAX));
   return fMin+(fMax-fMin)*((T)dRatio);
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::FastSin0 (T fAngle)
{
   T fASqr = fAngle*fAngle;
   T fResult = (T)7.61e-03;
   fResult *= fASqr;
   fResult -= (T)1.6605e-01;
   fResult *= fASqr;
   fResult += (T)1.0;
   fResult *= fAngle;
   return fResult;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::FastSin1 (T fAngle)
{
   T fASqr = fAngle*fAngle;
   T fResult = -(T)2.39e-08;
   fResult *= fASqr;
   fResult += (T)2.7526e-06;
   fResult *= fASqr;
   fResult -= (T)1.98409e-04;
   fResult *= fASqr;
   fResult += (T)8.3333315e-03;
   fResult *= fASqr;
   fResult -= (T)1.666666664e-01;
   fResult *= fASqr;
   fResult += (T)1.0;
   fResult *= fAngle;
   return fResult;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::FastCos0 (T fAngle)
{
   T fASqr = fAngle*fAngle;
   T fResult = (T)3.705e-02;
   fResult *= fASqr;
   fResult -= (T)4.967e-01;
   fResult *= fASqr;
   fResult += (T)1.0;
   return fResult;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::FastCos1 (T fAngle)
{
   T fASqr = fAngle*fAngle;
   T fResult = -(T)2.605e-07;
   fResult *= fASqr;
   fResult += (T)2.47609e-05;
   fResult *= fASqr;
   fResult -= (T)1.3888397e-03;
   fResult *= fASqr;
   fResult += (T)4.16666418e-02;
   fResult *= fASqr;
   fResult -= (T)4.999999963e-01;
   fResult *= fASqr;
   fResult += (T)1.0;
   return fResult;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::FastTan0 (T fAngle)
{
   T fASqr = fAngle*fAngle;
   T fResult = (T)2.033e-01;
   fResult *= fASqr;
   fResult += (T)3.1755e-01;
   fResult *= fASqr;
   fResult += (T)1.0;
   fResult *= fAngle;
   return fResult;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::FastTan1 (T fAngle)
{
   T fASqr = fAngle*fAngle;
   T fResult = (T)9.5168091e-03;
   fResult *= fASqr;
   fResult += (T)2.900525e-03;
   fResult *= fASqr;
   fResult += (T)2.45650893e-02;
   fResult *= fASqr;
   fResult += (T)5.33740603e-02;
   fResult *= fASqr;
   fResult += (T)1.333923995e-01;
   fResult *= fASqr;
   fResult += (T)3.333314036e-01;
   fResult *= fASqr;
   fResult += (T)1.0;
   fResult *= fAngle;
   return fResult;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::FastInvSin0 (T fValue)
{
   T fRoot = Math<T>::Sqrt(((T)1.0)-fValue);
   T fResult = -(T)0.0187293;
   fResult *= fValue;
   fResult += (T)0.0742610;
   fResult *= fValue;
   fResult -= (T)0.2121144;
   fResult *= fValue;
   fResult += (T)1.5707288;
   fResult = HALF_PI - fRoot*fResult;
   return fResult;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::FastInvSin1 (T fValue)
{
   T fRoot = Math<T>::Sqrt(FAbs(((T)1.0)-fValue));
   T fResult = -(T)0.0012624911;
   fResult *= fValue;
   fResult += (T)0.0066700901;
   fResult *= fValue;
   fResult -= (T)0.0170881256;
   fResult *= fValue;
   fResult += (T)0.0308918810;
   fResult *= fValue;
   fResult -= (T)0.0501743046;
   fResult *= fValue;
   fResult += (T)0.0889789874;
   fResult *= fValue;
   fResult -= (T)0.2145988016;
   fResult *= fValue;
   fResult += (T)1.5707963050;
   fResult = HALF_PI - fRoot*fResult;
   return fResult;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::FastInvCos0 (T fValue)
{
   T fRoot = Math<T>::Sqrt(((T)1.0)-fValue);
   T fResult = -(T)0.0187293;
   fResult *= fValue;
   fResult += (T)0.0742610;
   fResult *= fValue;
   fResult -= (T)0.2121144;
   fResult *= fValue;
   fResult += (T)1.5707288;
   fResult *= fRoot;
   return fResult;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::FastInvCos1 (T fValue)
{
   T fRoot = Math<T>::Sqrt(FAbs(((T)1.0)-fValue));
   T fResult = -(T)0.0012624911;
   fResult *= fValue;
   fResult += (T)0.0066700901;
   fResult *= fValue;
   fResult -= (T)0.0170881256;
   fResult *= fValue;
   fResult += (T)0.0308918810;
   fResult *= fValue;
   fResult -= (T)0.0501743046;
   fResult *= fValue;
   fResult += (T)0.0889789874;
   fResult *= fValue;
   fResult -= (T)0.2145988016;
   fResult *= fValue;
   fResult += (T)1.5707963050;
   fResult *= fRoot;
   return fResult;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::FastInvTan0 (T fValue)
{
   T fVSqr = fValue*fValue;
   T fResult = (T)0.0208351;
   fResult *= fVSqr;
   fResult -= (T)0.085133;
   fResult *= fVSqr;
   fResult += (T)0.180141;
   fResult *= fVSqr;
   fResult -= (T)0.3302995;
   fResult *= fVSqr;
   fResult += (T)0.999866;
   fResult *= fValue;
   return fResult;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::FastInvTan1 (T fValue)
{
   T fVSqr = fValue*fValue;
   T fResult = (T)0.0028662257;
   fResult *= fVSqr;
   fResult -= (T)0.0161657367;
   fResult *= fVSqr;
   fResult += (T)0.0429096138;
   fResult *= fVSqr;
   fResult -= (T)0.0752896400;
   fResult *= fVSqr;
   fResult += (T)0.1065626393;
   fResult *= fVSqr;
   fResult -= (T)0.1420889944;
   fResult *= fVSqr;
   fResult += (T)0.1999355085;
   fResult *= fVSqr;
   fResult -= (T)0.3333314528;
   fResult *= fVSqr;
   fResult += (T)1.0;
   fResult *= fValue;
   return fResult;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::FastNegExp0 (T fValue)
{
   T fResult = (T)0.0038278;
   fResult *= fValue;
   fResult += (T)0.0292732;
   fResult *= fValue;
   fResult += (T)0.2507213;
   fResult *= fValue;
   fResult += (T)1.0;
   fResult *= fResult;
   fResult *= fResult;
   fResult = ((T)1.0)/fResult;
   return fResult;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::FastNegExp1 (T fValue)
{
   T fResult = (T)0.00026695;
   fResult *= fValue;
   fResult += (T)0.00227723;
   fResult *= fValue;
   fResult += (T)0.03158565;
   fResult *= fValue;
   fResult += (T)0.24991035;
   fResult *= fValue;
   fResult += (T)1.0;
   fResult *= fResult;
   fResult *= fResult;
   fResult = ((T)1.0)/fResult;
   return fResult;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::FastNegExp2 (T fValue)
{
   T fResult = (T)0.000014876;
   fResult *= fValue;
   fResult += (T)0.000127992;
   fResult *= fValue;
   fResult += (T)0.002673255;
   fResult *= fValue;
   fResult += (T)0.031198056;
   fResult *= fValue;
   fResult += (T)0.250010936;
   fResult *= fValue;
   fResult += (T)1.0;
   fResult *= fResult;
   fResult *= fResult;
   fResult = ((T)1.0)/fResult;
   return fResult;
}
//----------------------------------------------------------------------------
template <class T>
T Math<T>::FastNegExp3 (T fValue)
{
   T fResult = (T)0.0000006906;
   fResult *= fValue;
   fResult += (T)0.0000054302;
   fResult *= fValue;
   fResult += (T)0.0001715620;
   fResult *= fValue;
   fResult += (T)0.0025913712;
   fResult *= fValue;
   fResult += (T)0.0312575832;
   fResult *= fValue;
   fResult += (T)0.2499986842;
   fResult *= fValue;
   fResult += (T)1.0;
   fResult *= fResult;
   fResult *= fResult;
   fResult = ((T)1.0)/fResult;
   return fResult;
}


#endif