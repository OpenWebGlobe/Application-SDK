/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _MERCATORTRANSFORMATION_H_
#define _MERCATORTRANSFORMATION_H_

#include "og.h"
#include <string>

namespace GeoRef
{
   //! \class Mercator
   //! \author Martin Christen, martin.christen@fhnw.ch
   class OPENGLOBE_API Mercator
   {
   public:
      Mercator();
      virtual ~Mercator();

      //! Forward Projection on WGS84 Ellipsoid
      static bool Forward(const double   lng,  const double lat,   double& out_x,   double& out_y);

      //! Reverse Projection on WGS84 Ellipsoid
      static bool Reverse(const double&  in_x, const double& in_y, double& out_lng, double& out_lat);

      //! Forward Projection using Ellipsoid with paramater e
      static bool ForwardCustom(const double   lng,  const double lat,   double& out_x,   double& out_y, const double e);

      //! Forward Projection using Ellipsoid with paramater e
      static bool ReverseCustom(const double&  in_x, const double& in_y, double& out_lng, double& out_lat, const double e);
   };
}

using namespace GeoRef;

#endif

