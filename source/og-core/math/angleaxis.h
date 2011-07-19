#ifndef _ANGLEAXIS_H
#define _ANGLEAXIS_H

#include "og.h"
#include <cmath>
#include <iostream>
#include <cassert>
#include "vec3.h"
#include "yawpitchroll.h"

//-------------------------------------------------------------------------
//! \class angleaxis
//! \brief Angle Axis representation for rotations.
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup math linalg
template<typename TType>
struct angleaxis
{
public:
   //! the Angle in radiants
   TType					fAngle;

   //! the axis to rotate about
   vec3<TType>	      vAxis;

   //! Default constructor. Does not set anything.
   angleaxis() {};

   //! Constructor with an axis only. The angle is set to 0.
   angleaxis(vec3<TType> &axis)							{fAngle = 0; vAxis = axis;};		// zero angle rotation
   
   //! Constructor with angle [in radiants] and x,y,z components of the axis.   
   angleaxis(TType angle, TType ax, TType ay, TType az)	{fAngle = angle; vAxis.x = ax; vAxis.y = ay; vAxis.z = az;};
   
   //! Constructor with angle [in radiants] and axis as vec3.
   angleaxis(TType angle, const vec3<TType>& axis)	{fAngle = angle; vAxis = axis;};
   
   //! Assign operator.
   angleaxis& operator= (const angleaxis& aa){fAngle = aa.fAngle; vAxis = aa.vAxis;return *this;};
   
   //! Invert sign of angle. Axis remains the same.
   friend angleaxis operator - (const angleaxis& aa)
   {
      return angleaxis<TType>(-aa.fAngle, aa.vAxis);
   }
};


#endif

