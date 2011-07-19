/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _ELEVATION_TILE_H
#define _ELEVATION_TILE_H

#include "og.h"
#include <math/vec3.h>
#include <math/vec2.h>
#include <math/ElevationPoint.h>
#include <vector>
#include <math/delaunay/DelaunayTriangulation.h>

//------------------------------------------------------------------------------

template <class T>
void sortpoints(std::vector<T>& sites)
{
   int nsites = (int)sites.size();

   for (int gap = nsites/2; gap > 0; gap /= 2)
   {
      for (int i = gap; i < nsites; i++)
      {
         for (int j = i-gap; 
            j >= 0 && (sites[j].x != sites[j+gap].x ? (sites[j].x > sites[j+gap].x) : (sites[j].y > sites[j+gap].y));j -= gap)

         {
            std::swap(sites[j], sites[j+gap]);
         }
      }
   }
}
//------------------------------------------------------------------------------

//! \class ElevationTile
//! \author Martin Christen, martin.christen@fhnw.ch
class OPENGLOBE_API ElevationTile
{
public:
   ElevationTile();
   virtual ~ElevationTile();

   bool LoadElevationTile(const std::string& sQuadcode, const std::string& sRepository, const std::string& sLayer, bool bLayer = true);
   bool SaveElevationTile(const std::string& sQuadcode, const std::string& sRepository, const std::string& sLayer);
   
   // bCornerTile is true if Tile is at corner of dataset!
   void CategorizePoints(double x0, double y0, double x1, double y1, bool bCornerTile, std::vector<ElevationPoint>& pts);

   // Create Triangulation (only valid after LoadElevationTile or CategorizePoints)
   boost::shared_ptr<math::DelaunayTriangulation> CreateTriangulation(bool bBorderOnly =false);

   // Returns 
   size_t GetNumPoints();

   std::vector<ElevationPoint>& GetCornerPoints(){return _ptsCorner;} 
   std::vector<ElevationPoint>& GetNorthPoints(){return _ptsNorth;}
   std::vector<ElevationPoint>& GetEastPoints(){return _ptsEast;}
   std::vector<ElevationPoint>& GetSouthPoints(){return _ptsSouth;}
   std::vector<ElevationPoint>& GetWestPoints(){return _ptsWest;}
   std::vector<ElevationPoint>& GetMiddlePoints(){return _ptsMiddle;}

   // Corer Points order: 0=NW, 1=NE, 2=SE, 3=SW
   void SetCornerPoints(std::vector<ElevationPoint>& pts){ _ptsCorner = pts; _x0 = _ptsCorner[0].x; _y0 = _ptsCorner[0].y;_x1 = _ptsCorner[2].x;_y1 = _ptsCorner[2].y;} 
   void SetNorthPoints(std::vector<ElevationPoint>& pts){ _ptsNorth = pts;}
   void SetEastPoints(std::vector<ElevationPoint>& pts){ _ptsEast = pts;}
   void SetSouthPoints(std::vector<ElevationPoint>& pts){ _ptsSouth = pts;}
   void SetWestPoints(std::vector<ElevationPoint>& pts){ _ptsWest = pts;}
   void SetMiddlePoints(std::vector<ElevationPoint>& pts){ _ptsMiddle = pts;}

   // Sort Points:
   void Sort();

   void SetHierarchy(bool b){_bHierarchy = b;}

   std::vector< vec3<float> >& GetPointsWGS84() {return _lstElevationPointWGS84;}  
   std::vector< vec2<float> >& GetTexCoord() {return _lstTexCoord;}             
   std::vector<int>& GetIndices() {return _lstIndices;}              
   vec3<double> GetOffset() {return _vOffset; }                

   void ElevationTile::_PrecomputeTriangulation();

protected:
   std::vector<ElevationPoint> _ptsCorner; // 0: NW, 1: NE, 2: SE, 3:SW
   std::vector<ElevationPoint> _ptsNorth;
   std::vector<ElevationPoint> _ptsEast;
   std::vector<ElevationPoint> _ptsSouth; 
   std::vector<ElevationPoint> _ptsWest;
   std::vector<ElevationPoint> _ptsMiddle;
   double _x0, _y0, _x1, _y1;
   bool _bHierarchy;

   // Pre-Triangulation:
   std::vector< vec3<float> >   _lstElevationPointWGS84;  // Elevation Points (Cartesian WGS84 minus offset)
   std::vector< vec2<float> >   _lstTexCoord;             //
   std::vector<int>             _lstIndices;              // Indices
   vec3<double>                 _vOffset;                 // Proposed offset for virtual camera
};


#endif