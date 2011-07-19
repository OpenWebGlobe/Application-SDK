/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "geo/MercatorQuadtreeElevation.h"
#include "math/mathutils.h"
#include "MercatorTransformation.h"
#include <sstream>
#include <utility>
#include <string/StringUtils.h>

//-----------------------------------------------------------------------------

MercatorQuadtreeElevation::MercatorQuadtreeElevation(double ellipsoid_a, double ellipsoid_e)
{
   _EarthRadius = ellipsoid_a;
   //_MinLatitude = -85.084059050110696;
   //_MaxLatitude = 85.084059050110696;
   _MinLongitude = -180.0;
   _MaxLongitude = 180.0;
   _ellipsoid_e = ellipsoid_e;

   double out_lng;
   double out_lat;
   GeoRef::Mercator::ReverseCustom(0, 1.0, out_lng, out_lat, _ellipsoid_e);

   _MaxLatitude = out_lat;
   _MinLatitude = -_MaxLatitude;

}

//-----------------------------------------------------------------------------

MercatorQuadtreeElevation::~MercatorQuadtreeElevation()
{

}

//-----------------------------------------------------------------------------

void MercatorQuadtreeElevation::Setup(double ellipsoid_a, double ellipsoid_e)
{
   _EarthRadius = ellipsoid_a;
   _MinLongitude = -180.0;
   _MaxLongitude = 180.0;
   _ellipsoid_e = ellipsoid_e;

   double out_lng;
   double out_lat;
   GeoRef::Mercator::ReverseCustom(0, 1.0, out_lng, out_lat, _ellipsoid_e);

   _MaxLatitude = out_lat;
   _MinLatitude = -_MaxLatitude;
}

//-----------------------------------------------------------------------------

void MercatorQuadtreeElevation::SetupSphericWGS84()
{
   Setup(6378137.0, 0.0);
}

//-----------------------------------------------------------------------------

void MercatorQuadtreeElevation::SetupEllipsoidWGS84()
{
   Setup(6378137.0,0.081819190842961775161887117288255);
}

//-----------------------------------------------------------------------------

std::string  MercatorQuadtreeElevation::TileCoordToQuadkey(int64 TileX, int64 TileY, int levelofdetail)
{
   std::string sQuadkey;
   std::ostringstream  oss;

   for (int i=levelofdetail; i>0;i--)
   {
      char digit = '0';
      int mask = 1 << (i-1);
      if ((TileX & mask) != 0)
      {
         digit++;
      }
      if ((TileY & mask) != 0)
      {
         digit++;
         digit++;
      }

      oss << digit;
   }

   sQuadkey = oss.str();
   return sQuadkey;
}

//-----------------------------------------------------------------------------

bool MercatorQuadtreeElevation::QuadKeyToTileCoord(const std::string& quadKey, int64& out_tileX, int64& out_tileY, int& out_levelOfDetail)
{
   out_tileX = out_tileY = 0;
   out_levelOfDetail = quadKey.length();
   for (int i = out_levelOfDetail; i > 0; i--)
   {
      int64 mask = 1 << (i - 1);
      switch (quadKey[out_levelOfDetail - i])
      {
      case '0':
         break;

      case '1':
         out_tileX |= mask;
         break;

      case '2':
         out_tileY |= mask;
         break;

      case '3':
         out_tileX |= mask;
         out_tileY |= mask;
         break;

      default:
         //wrong quadkey!
         return false;   
      }
   }

   return true;
}

//-----------------------------------------------------------------------------

static inline void _QuadCoordTranslate(vec3<double>& oVec, double fX, double fY)
{
   oVec.x += fX;
   oVec.y += fY;
}

//-----------------------------------------------------------------------------

// Convert Quadkey to mercator coordinate without floating point problems!
//
// It is very important to do this over quadkey and *not* over pixels, because
// of precision problems between int (or int64) and double as pixel coordinates
// are very big at lower resolutions.

bool MercatorQuadtreeElevation::QuadKeyToMercatorCoord(const std::string& quadKey, double& x0, double& y0, double &x1, double &y1)
{
   QuadKeyToNormalizedCoord(quadKey, x0, y0, x1, y1);

   // Create Mercator Coordinates [-pi,-pi]-[pi,pi]

   x0 = 2.0*x0 - 1.0;
   y0 = 2.0*y0 - 1.0;
   x1 = 2.0*x1 - 1.0 ;
   y1 = 2.0*y1 - 1.0;

   return true;
}

//-----------------------------------------------------------------------------

bool MercatorQuadtreeElevation::QuadKeyToNormalizedCoord(const std::string& quadKey, double& x0, double& y0, double &x1, double &y1)
{
   double scale = 1.0;
   vec3<double> vec(0,0,0);

   int nlevelOfDetail =  quadKey.length();

   for (int i = 0; i < nlevelOfDetail; i++)
   {
      scale /= 2.0;

      switch (quadKey[i])
      {
      case '0':
         _QuadCoordTranslate(vec, 0, scale);
         break;
      case '1':
         _QuadCoordTranslate(vec, scale, scale);
         break;
      case '2':
         break;
      case '3':
         _QuadCoordTranslate(vec, scale, 0);
         break;

      default:
         //wrong quadkey!
         return false;   
      }
   }


   // This is in the correct "cartesian" way, but now it must be converted to "pixel" based
   // representation (TL, BR) coordinates:
   // +-----------##
   // |            |
   // |            |
   // |            |   Bottom Left -->   Top Left
   // |            |   Top Right   -->   Bottom Right
   // |            |
   // |            |   in other words: swap(y0,y1)
   // ##-----------+

   x0 = vec.x;
   y0 = vec.y;
   x1 = x0+scale;
   y1 = y0+scale;

   std::swap(y0,y1);


   return true;
}




//-----------------------------------------------------------------------------

int  MercatorQuadtreeElevation::QuadtreePosition(const std::string& quadKey)
{
   if (quadKey.length()<1)
   {
      assert(false);
      return -1;
   }

   std::string s = StringUtils::Right(quadKey, 1);

   if (s.length()<1)
   {
      assert(false);
      return -1;
   }

   switch (s[0])
   {
   case '0':
      return 0;
      break;
   case '1':
      return 1;
      break;
   case '2':
      return 2;
      break;
   case '3':
      return 3;
      break;
   default:
      return -1;
   }

   return -1;
}

//-----------------------------------------------------------------------------

std::string  MercatorQuadtreeElevation::GetParent(const std::string& quadKey)
{
    if (quadKey.length()<2)
    {
       assert(false);
       return std::string();
    }

    return StringUtils::Left(quadKey, quadKey.length()-1);
}

//-----------------------------------------------------------------------------

int MercatorQuadtreeElevation::GetQuad(const std::string& quadKey, int pos)
{
   int l = (int)quadKey.length();
   if (pos>l-1 || pos<0)
   {
      assert(false);
      return -1;
   }

   switch (quadKey[pos])
   {
   case '0':
      return 0;
      break;
   case '1':
      return 1;
      break;
   case '2':
      return 2;
      break;
   case '3':
      return 3;
      break;
   default:
      return -1;
   }
}

//-----------------------------------------------------------------------------

void MercatorQuadtreeElevation::TileCoordToMercator(int64 TileX, int64 TileY, int levelofdetail, double& out_x0, double& out_y0, double& out_x1, double& out_y1)
{
   double b = 1.0 / double(pow(2.0, double(levelofdetail)));
   double tx = double(TileX);
   double ty = double(TileY);

   out_x0 = tx * b;
   out_y0 = 1.0 - ((ty + 1.0) * b);
   out_x1 = (tx + 1.0) * b;
   out_y1 = 1.0 - (ty * b);

   out_x0 = 2.0*1.0*out_x0 - 1.0;
   out_y0 = 2.0*1.0*out_y0 - 1.0;
   out_x1 = 2.0*1.0*out_x1 - 1.0;
   out_y1 = 2.0*1.0*out_y1 - 1.0;

}
//-------------------------------
// #BUG gefunden am 28.5.2010 chm 
//-------------------------------
std::list< std::pair<int64, int64> > MercatorQuadtreeElevation::MercatorToTileCoord(double x, double y, int levelofdetail)
{
   std::list< std::pair<int64, int64> > lResult;

   double px, py;
   double tx, ty;
   double pdet = pow(2.0, double(levelofdetail));
   int64 pdeti = 1 << levelofdetail;
   int64 nMaxTileCoord = int64(pdet-1);
   int txi, tyi;
   //double b = 1.0 / double(pdet);


   px = 0.5*(x + 1.0);
   py = 0.5*(y + 1.0);

   double ftx, fty;
   double pyy;

   ftx = px * pdet;
   fty = (1.0-py) * pdet;
   pyy = 1.0-py;

   tx = floor(ftx);
   ty = floor(fty);
   
   txi = int(px  * pdeti);
   tyi = int(pyy * pdeti);

   const double epsilon = DBL_EPSILON;

   bool A = fabs(tx-ftx) < epsilon; 
   bool B = fabs(ty-fty) < epsilon;

   if (A && B)
   {
      // Point on corner
      std::pair<int64, int64> Solution1 = std::make_pair(int64(tx), int64(ty));
      std::pair<int64, int64> Solution2 = std::make_pair(int64(tx-1), int64(ty-1));
      std::pair<int64, int64> Solution3 = std::make_pair(int64(tx), int64(ty-1));
      std::pair<int64, int64> Solution4 = std::make_pair(int64(tx-1), int64(ty));

      if (Solution1.first >= 0 && Solution1.first <= nMaxTileCoord 
         && Solution1.second >=0 && Solution1.second <= nMaxTileCoord)
      {
        lResult.push_back(Solution1);
      }

      if (Solution2.first >= 0 && Solution2.first <= nMaxTileCoord 
         && Solution2.second >=0 && Solution2.second <= nMaxTileCoord)
      {
         lResult.push_back(Solution2);
      }

      if (Solution3.first >= 0 && Solution3.first <= nMaxTileCoord 
         && Solution3.second >=0 && Solution3.second <= nMaxTileCoord)
      {
         lResult.push_back(Solution3);
      }

      if (Solution4.first >= 0 && Solution4.first <= nMaxTileCoord 
         && Solution4.second >=0 && Solution4.second <= nMaxTileCoord)
      {
         lResult.push_back(Solution4);
      }
      
   }
   else if (A)
   {
      // Point on Vertical edge

      std::pair<int64, int64> Solution1 = std::make_pair(int64(tx), int64(ty));
      std::pair<int64, int64> Solution2 = std::make_pair(int64(tx-1), int64(ty));

      if (Solution1.first >= 0 && Solution1.first <= nMaxTileCoord 
         && Solution1.second >=0 && Solution1.second <= nMaxTileCoord)
      {
         lResult.push_back(Solution1);
      }

      if (Solution2.first >= 0 && Solution2.first <= nMaxTileCoord 
         && Solution2.second >=0 && Solution2.second <= nMaxTileCoord)
      {
         lResult.push_back(Solution2);
      }


   }
   else if (B)
   {
      // point on horizontal edge

      std::pair<int64, int64> Solution1 = std::make_pair(int64(tx), int64(ty));
      std::pair<int64, int64> Solution2 = std::make_pair(int64(tx), int64(ty-1));

      if (Solution1.first >= 0 && Solution1.first <= nMaxTileCoord 
         && Solution1.second >=0 && Solution1.second <= nMaxTileCoord)
      {
         lResult.push_back(Solution1);
      }

      if (Solution2.first >= 0 && Solution2.first <= nMaxTileCoord 
         && Solution2.second >=0 && Solution2.second <= nMaxTileCoord)
      {
         lResult.push_back(Solution2);
      }
   }
   else
   {
      std::pair<int64, int64> Solution1 = std::make_pair(int64(tx), int64(ty));
      if (Solution1.first >= 0 && Solution1.first <= nMaxTileCoord 
         && Solution1.second >=0 && Solution1.second <= nMaxTileCoord)
      {
         lResult.push_back(Solution1);
      }
   }

   return lResult;
}

//-----------------------------------------------------------------------------

void MercatorQuadtreeElevation::MercatorToWGS84(double& x, double& y)
{
   double outx, outy;
   GeoRef::Mercator::ReverseCustom(x,y, outx, outy, _ellipsoid_e);
   x = outx;
   y = outy;
}

//-----------------------------------------------------------------------------

void MercatorQuadtreeElevation::WGS84ToTileCoord(double lng, double lat, int nLevelOfDetail, int64& TileX, int64& TileY)
{
   // lng / lat -> Mercator
   double outx, outy;
   GeoRef::Mercator::ForwardCustom(lng, lat, outx, outy, _ellipsoid_e);

   std::list< std::pair<int64, int64> > keyList;
   keyList =  MercatorToTileCoord(outx,outy, nLevelOfDetail);

   if (keyList.size()>0)
   {
      std::list< std::pair<int64, int64> >::iterator it = keyList.begin();

      TileX = (*it).first;
      TileY = (*it).second;
   }
   else
   {
      assert(false);
      TileX = 0;
      TileY = 0;
   }

}



//-----------------------------------------------------------------------------