/*******************************************************************************
Project       : i3D Studio  
Purpose       : Point representing elevation
Creation Date : 13.11.2008
Author        : Martin Christen
Copyright     : Copyright (c) 2008 by FHNW. All rights reserved.
*******************************************************************************/

#include "og.h"
#include "ElevationPoint.h"
#include "xml/xml.h"


//-----------------------------------------------------------------------------
// Conversion of Elevation Point from/to string (for XML serialization)

std::string ElevationPointToString(void* pAddress)
{
   std::ostringstream os;
   ElevationPoint* data = (ElevationPoint*)(pAddress);

   os << data->x << " ";
   os << data->y << " ";
   os << data->elevation << " ";
   os << data->weight << " ";
   os << data->error;

   return os.str();
}
TYPE_TO_STRING_CONVERSION(ElevationPoint, ElevationPointToString)

//-----------------------------------------------------------------------------

void StringToElevationPoint(void* pAddress, std::string sValue)
{
   ElevationPoint* pData = (ElevationPoint*)(pAddress);

   std::vector<float> vElevationPoint;
   Tokenize(sValue, ' ', vElevationPoint);
   if (vElevationPoint.size() == 5)
   {
      pData->x = vElevationPoint[0]; 
      pData->y = vElevationPoint[1];
      pData->elevation = vElevationPoint[2];
      pData->weight = vElevationPoint[3];
      pData->error = vElevationPoint[4];
   }
}
STRING_TO_TYPE_CONVERSION(ElevationPoint, StringToElevationPoint)

//-----------------------------------------------------------------------------

ElevationPoint::ElevationPoint()
{
   x = y = 0.0;
   elevation = 0.0;
   weight = 0.0;
   error = 0.0;
}

//-----------------------------------------------------------------------------

ElevationPoint::~ElevationPoint()
{

}

//-----------------------------------------------------------------------------

ElevationPoint::ElevationPoint(const ElevationPoint& cp)
{
   x = cp.x;
   y = cp.y;
   elevation = cp.elevation;
   weight = cp.weight;
   error = cp.error;
}

