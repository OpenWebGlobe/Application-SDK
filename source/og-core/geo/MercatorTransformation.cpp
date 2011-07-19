/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include <cmath>
#include <string>
#include <cassert>

#include "MercatorTransformation.h"

using namespace GeoRef;

#define AGEPI       3.1415926535897932384626433832795028841971693993751
#define AGEHALFPI   1.5707963267948966192313216916395
#define RAD2DEG(x)  ((180.0*x)/AGEPI)
#define DEG2RAD(x)  ((x*AGEPI)/180.0)

#define WGS84_a            6378137.0          
#define WGS84_b            6356752.314245     
#define WGS84_F_INV        298.257223563  
#define WGS84_E_SQUARED    0.006694379990197 
#define WGS84_E            0.081819190842961775161887117288255
#define WGS84_E_SQUARED2   0.006739496742
#define WGS84_RN_POLE      6.399593625758673e+006


//-----------------------------------------------------------------------------

Mercator::Mercator()
{
}
//-----------------------------------------------------------------------------
Mercator::~Mercator()
{
}
//-----------------------------------------------------------------------------
bool Mercator::Forward(const double lng, const double lat, double& out_x, double& out_y)
{
   const double WGS84_a_uniform = 1.0;
   const double lngRad0 = 0;

   double lngRad = DEG2RAD(lng);
   double latRad = DEG2RAD(lat);

   // #Todo: make sure lng/lat is in correct range!

   out_x = WGS84_a_uniform*(lngRad-lngRad0);
   out_y = WGS84_a_uniform*log(tan(AGEPI/4.0+latRad/2.0)*pow((1.0-WGS84_E*sin(latRad))/(1.0+WGS84_E*sin(latRad)),0.5*WGS84_E));

   out_x /= AGEPI;
   out_y /= AGEPI;

   return true;
}  

//-----------------------------------------------------------------------------

bool Mercator::Reverse(const double&  xx, const double& yy, double& out_lng, double& out_lat)
{
   double x = xx * AGEPI;
   double y = yy * AGEPI;

   const double WGS84_a_uniform = 1.0;
   const double lngRad0 = 0;

   double t = exp(-y/WGS84_a_uniform);
   out_lat = AGEHALFPI-2.0*atan(t);  // initial value for iteration...

   for (int i=0;i<10;i++)
   {
      double F= pow((1.0-WGS84_E*sin(out_lat))/(1.0+WGS84_E*sin(out_lat)),0.5*WGS84_E);
      out_lat = AGEHALFPI - 2.0*atan(t*F);
   }

   out_lng = x / WGS84_a_uniform + lngRad0;

   out_lat = RAD2DEG(out_lat);
   out_lng = RAD2DEG(out_lng);

   while (out_lng>180)
      out_lng -=180;

   while (out_lng<-180)
      out_lng +=180;

   while (out_lat>90)
      out_lat-=180;

   while (out_lat<-90)
      out_lat+=180;

   return true;
}

//-----------------------------------------------------------------------------

bool Mercator::ForwardCustom(const double lng, const double lat, double& out_x, double& out_y, const double e)
{
   const double a_uniform = 1.0;
   const double lngRad0 = 0;

   double lngRad = DEG2RAD(lng);
   double latRad = DEG2RAD(lat);

   // #Todo: make sure lng/lat is in correct range!


   if (e==0.0)
   {
      out_x = a_uniform*(lngRad-lngRad0);
      out_y = log(tan(AGEPI/4.0 + latRad/2));
   }
   else
   {
      out_x = a_uniform*(lngRad-lngRad0);
      out_y = a_uniform*log(tan(AGEPI/4.0+latRad/2.0)*pow((1.0-e*sin(latRad))/(1.0+e*sin(latRad)),0.5*e));
   }

   out_x /= AGEPI;
   out_y /= AGEPI;

   return true;
}

//-----------------------------------------------------------------------------

bool Mercator::ReverseCustom(const double&  xx, const double& yy, double& out_lng, double& out_lat, const double e)
{
   double x = xx * AGEPI;
   double y = yy * AGEPI;

   const double a_uniform = 1.0;
   const double lngRad0 = 0;

   double t = exp(-y/a_uniform);
   out_lat = AGEHALFPI-2.0*atan(t);  // initial value for iteration...

   if (e==0.0)
   {
      out_lat = AGEHALFPI - 2 * atan(t);     
   }
   else
   { 
      for (int i=0;i<10;i++)
      {
         double F= pow((1.0-e*sin(out_lat))/(1.0+e*sin(out_lat)),0.5*e);
         out_lat = AGEHALFPI - 2.0*atan(t*F);
      }
   }

   out_lng = x / a_uniform + lngRad0;

   out_lat = RAD2DEG(out_lat);
   out_lng = RAD2DEG(out_lng);

   while (out_lng>180)
      out_lng -=180;

   while (out_lng<-180)
      out_lng +=180;

   while (out_lat>90)
      out_lat-=180;

   while (out_lat<-90)
      out_lat+=180;

   return true;
}

//-----------------------------------------------------------------------------
