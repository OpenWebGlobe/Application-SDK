#ifndef _YAWPITCHROLL_H
#define _YAWPITCHROLL_H

#include "og.h"
#include <cmath>
#include <iostream>
#include <cassert>

//! \ingroup math linalg
//! \internal
enum EYPROrder
{
   YPR_ORDER_ZXY,
   YPR_ORDER_ZYX,
   YPR_ORDER_XYZ,
   YPR_ORDER_XZY,
   YPR_ORDER_YXZ,
   YPR_ORDER_YZX,
};

//-------------------------------------------------------------------------
//! \ingroup math linalg
//! \bug this class needs to be revised. Do not use if possible.
template<typename TType>
struct yawpitchroll
{
public:

   TType		fYaw, fPitch, fRoll;
   vec2<TType> vec;

   yawpitchroll() {};
   yawpitchroll(TType yaw, TType pitch, TType roll)	
   { 
      fYaw = yaw;
      fPitch = pitch;
      fRoll = roll;
   };
   yawpitchroll& operator= (const yawpitchroll& ypr)		
   {
      fYaw = ypr.fYaw;
      fPitch = ypr.fPitch;
      fRoll = ypr.fRoll;
      return *this;
   };
};

#endif

