/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/


#ifndef _MERCATORQUADTREEELEVATION_H
#define _MERCATORQUADTREEELEVATION_H


#include "og.h"
#include <boost/shared_ptr.hpp>
#include "math/vec3.h"
#include "math/vec2.h"
#include <string>
#include <map>
#include <list>
#include "math/GeoCoord.h"

//! \class MercatorQuadtreeElevation
//! \author Martin Christen, martin.christen@fhnw.ch
class OPENGLOBE_API MercatorQuadtreeElevation
{
public:
   // For Earth:
   // +-------------------------------------------------------------+
   // |   WGS 84: ellipsoid_e = 0.081819190842961775161887117288255 |
   // |         ellipsoid_a = 6378137.0                             |
   // |                                                             |
   // |   Spherical: ellipsoid_e = 0                                |
   // |            ellipsoid_a = 6378137.0                          |
   // +-------------------------------------------------------------+

   MercatorQuadtreeElevation(double ellipsoid_a = 6378137.0, double ellipsoid_e=0.0);
   virtual ~MercatorQuadtreeElevation();

   //! \brief Change Ellipsoid parameters
   void Setup(double ellipsoid_a, double ellipsoid_e);
   
   //! \brief Change Ellipsoid parameters to Spherical Mercator Projection (WGS84)
   void SetupSphericWGS84();

   //! \brief Change Ellipsoid parameters to Ellipsoid Mercator Projection (WGS84)
   void SetupEllipsoidWGS84();

   //! \brief Convertes Mercator Value to WGS84 (x to longitude and y to latitude)
   void MercatorToWGS84(double& x, double& y);


   void WGS84ToTileCoord(double lng, double lat, int nLevelOfDetail, int64& TileX, int64& TileY);

   //----------------------------------------------------------------
   // STATIC FUNCTIONS , ALSO AVAILABLE IN CLASS MercatorQuadtree!!!
   // This will change!!!
   //----------------------------------------------------------------


   //-------------------------------------
   // Mercator Coord:
   //            
   //  +---------+ (x1, y1) 
   //  | Tile    |
   //  | (tx, ty)|
   //  | lod     |
   //  |         |
   //  +---------+
   // (x0, y0)
   //
   //! \brief Fast conversion from Tilecoord to Mercator Coord. Same result could be achived by calling
   //!        TileCoordToQuadkey(...) and then QuadKeyToMercatorCoord(...) but that would be much slower.
   //! \param TileX tile coordinate X
   //! \param TileY tile coordinate Y
   //! \param levelofdetail Level of detail
   //! \param out_x0 output minimum x-coordinate of mercator projection
   //! \param out_y0 output minimum y-coordinate of mercator projection
   //! \param out_x1 output maximum x-coordinate of mercator projection
   //! \param out_y1 output maximum y-coordinate of mercator projection
   static void TileCoordToMercator(int64 TileX, int64 TileY, int levelofdetail, double& out_x0, double& out_y0, double& out_x1, double& out_y1);


   //! \brief Convert Mercator coord to tile coord
   static std::list< std::pair<int64, int64> > MercatorToTileCoord(double x, double y, int levelofdetail);


   //! \brief Convert TileXY Coordinate to Quadkey at specified level of detail
   //! \param TileX tile coordinate X
   //! \param TileY tile coordinate Y
   //! \param levelofdetail Level of detail
   //! \return std::string containing quad tree
   static std::string TileCoordToQuadkey(int64 TileX, int64 TileY, int levelofdetail);

   //! \brief Convert Quadkey to tile coordinates
   //! \param quadKey the quadkey
   //! \param out_tileX output tile coordinate X
   //! \param out_tileY output tile coordinate Y
   //! \param out_levelOfDetail output Level of detail
   //! \return true on success
   static bool QuadKeyToTileCoord(const std::string& quadKey, int64& out_tileX, int64& out_tileY, int& out_levelOfDetail);

   //! \brief Convert Quadkey to mercator coordinates
   static bool QuadKeyToMercatorCoord(const std::string& quadKey, double& x0, double& y0, double &x1, double &y1);


   //! \brief Convert Quadkey to normalized coordinates [0,0]-[1,1]
   static bool QuadKeyToNormalizedCoord(const std::string& quadKey, double& x0, double& y0, double &x1, double &y1);


   //! \brief Returns relation to parent element in quadtree (0,1,2,3). Returns -1 on invalid queries.
   //! \param quadKey the quad key ASCII string
   static int  QuadtreePosition(const std::string& quadKey);

   //! \brief Returns quadkey of parent quadtree element
   //! \param quadKey the quad key ASCII string
   static std::string  GetParent(const std::string& quadKey);

   //! \brief Returns quad value at specified position
   static int   GetQuad(const std::string& quadKey, int pos);

  
private:
   double _EarthRadius;
   double _ellipsoid_e;
   double _MinLatitude;
   double _MaxLatitude;
   double _MinLongitude;
   double _MaxLongitude;
};



#endif