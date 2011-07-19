/*******************************************************************************
Project       : i3D Studio  
Purpose       : Point representing elevation
Creation Date : 13.11.2008
Author        : Martin Christen
Copyright     : Copyright (c) 2008 by FHNW. All rights reserved.
*******************************************************************************/

#include "og.h"
#include "CloudPoint.h"
#include "xml/xml.h"


//-----------------------------------------------------------------------------
// Conversion of Elevation Point from/to string (for XML serialization)

std::string CloudPointToString(void* pAddress)
{
   std::ostringstream os;
   CloudPoint* data = (CloudPoint*)(pAddress);

   os << data->x << " ";
   os << data->y << " ";
   os << data->elevation << " ";
   os << data->ox << " ";
   os << data->oy << " ";
   os << data->oz << " ";
   os << data->intensity << " ";
   os << data->r << " ";
   os << data->g << " ";
   os << data->b;

   return os.str();
}
TYPE_TO_STRING_CONVERSION(CloudPoint, CloudPointToString)

//-----------------------------------------------------------------------------

void StringToCloudPoint(void* pAddress, std::string sValue)
{
   CloudPoint* pData = (CloudPoint*)(pAddress);

   std::vector<double> vCloudPoint;
   Tokenize(sValue, ' ', vCloudPoint);
   if (vCloudPoint.size() == 10)
   {
      pData->x =           vCloudPoint[0]; 
      pData->y =           vCloudPoint[1];
      pData->elevation =   vCloudPoint[2];
      pData->ox =          vCloudPoint[3];
      pData->oy =          vCloudPoint[4];
      pData->oz =          vCloudPoint[5];
      pData->intensity =   (int)vCloudPoint[6];
      pData->r =           (unsigned char)vCloudPoint[7]; 
      pData->g =           (unsigned char)vCloudPoint[8]; 
      pData->b =           (unsigned char)vCloudPoint[9];
   }
}
STRING_TO_TYPE_CONVERSION(CloudPoint, StringToCloudPoint)

//-----------------------------------------------------------------------------

CloudPoint::CloudPoint()
{
   x = y = 0.0;
   elevation = 0.0;
   ox = oy = oz = 0.0;
   intensity = 0;
   r = g = b = a = 255; // default color is white!
}

//-----------------------------------------------------------------------------

CloudPoint::~CloudPoint()
{

}

//-----------------------------------------------------------------------------

CloudPoint::CloudPoint(const CloudPoint& cp)
{
   x = cp.x;
   y = cp.y;
   elevation = cp.elevation;
   ox = cp.ox; oy = cp.oy; oz = cp.oz;
   intensity = cp.intensity;
   r = cp.r;
   g = cp.g;
   b = cp.b;
   a = 255;
}

