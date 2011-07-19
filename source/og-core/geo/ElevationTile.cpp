/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "ElevationTile.h"
#include <io/IStreamIn.h>
#include <io/StreamOutFile.h>
#include <io/StreamInFile.h>
#include <float.h>
#include <cmath>
#include <io/FileSystem.h>
#include <string/FilenameUtils.h>
#include "geo/MercatorTransformation.h"
#include <string/StringUtils.h>
#include "math/GeoCoord.h"

//-----------------------------------------------------------------------------
 
inline bool _WriteElevationPoint(IStreamOut* pStream, const ElevationPoint& pt)
{
   pStream->WriteDouble(pt.x);
   pStream->WriteDouble(pt.y);
   pStream->WriteDouble(pt.elevation);
   pStream->WriteDouble(pt.weight);
   pStream->WriteDouble(pt.error);
   return true;
}

//--------------------------------------------------------------------------

inline bool _ReadElevationPoint(IStreamIn* pStream, ElevationPoint& pt)
{
   if (pStream->ReadDouble(&pt.x) == 0)
      return false;
   if (pStream->ReadDouble(&pt.y) == 0)
      return false;
   if (pStream->ReadDouble(&pt.elevation) == 0)
      return false;
   if (pStream->ReadDouble(&pt.weight) == 0)
      return false;
   if (pStream->ReadDouble(&pt.error) == 0)
      return false;

   return true;
}

//-----------------------------------------------------------------------------

// Eliminate Point that is close (epsilon) to rect (x0,y0,x1,y1)
inline void EliminateCloseToCorner(double x0, double y0, double x1, double y1, std::vector<ElevationPoint>& PointList, const double epsilon)
{
   std::vector<ElevationPoint>::iterator it = PointList.begin();

   bool advance = true;

   while (it!= PointList.end())
   {
      double dx0 = fabs(x0 - (*it).x);
      double dy0 = fabs(y0 - (*it).y);
      double dx1 = fabs(x1 - (*it).x);
      double dy1 = fabs(y1 - (*it).y);

      if (dx0 < epsilon || dy0 < epsilon || dx1 < epsilon || dy1 < epsilon)
      {
        
         it = PointList.erase(it);
      }
      else
      {
         it++;
      }
   }
}

//-----------------------------------------------------------------------------
// for performance reasons this only works on a sorted list!
inline void EliminateDoubleEntries(std::vector<ElevationPoint>& PointList, const double epsilon)
{
   return; // #fix NEVER CALL THIS!
   std::vector<ElevationPoint>::iterator it = PointList.begin();

   bool advance = true;
   ElevationPoint current;

   while (it!= PointList.end())
   {
      if (advance)
      {
         current = *it;
         advance = false;
         it++;
      }

      if (it!= PointList.end())
      {
         double dx = fabs((*it).x - current.x);
         double dy = fabs((*it).y - current.y);

         if (dx < epsilon && dy < epsilon)
         {
            it = PointList.erase(it);
         }
         else
         {
            advance = true;
         }
      }
   }
}


//-----------------------------------------------------------------------------

ElevationTile::ElevationTile()
{
   _bHierarchy = true;
}

//-----------------------------------------------------------------------------

ElevationTile::~ElevationTile()
{

}

//-----------------------------------------------------------------------------

bool ElevationTile::LoadElevationTile(const std::string& sQuadcode, const std::string& sRepository, const std::string& sLayer, bool bLayer)
{
   _ptsCorner.clear(); _ptsNorth.clear(); _ptsEast.clear(); 
   _ptsSouth.clear(); _ptsWest.clear(); _ptsMiddle.clear();
   _lstElevationPointWGS84.clear(); 
   _lstTexCoord.clear();            
   _lstIndices.clear(); 

   std::string sFilename;

   if (bLayer)
   {
      sFilename = sRepository + "/" + sLayer + "_tiles/" + sQuadcode + ".elv";
   }
   else
   {
      sFilename = sRepository + "/" + sQuadcode + ".elv";
   }

   std::string sFilenameW(sFilename);

   if (!FileSystem::FileExists(sFilenameW))
   {
      
      if (_bHierarchy)
      {
         sFilenameW = FilenameUtils::MakeHierarchicalFileName(sFilenameW, 2);
      }
      else
      {
         return false;
      }
      
      if (!FileSystem::FileExists(sFilenameW))
      {
         return false;
      }

   }

   StreamInFile si;

   try
   {
      si.Open(sFilenameW);

      char a,b,c,d;
      si.ReadByte(&a); si.ReadByte(&b); si.ReadByte(&c); si.ReadByte(&d);
      if (a != 'T' || b !='I' || c != 'L' || d != 'E')
         return false; // wrong header!!

      unsigned int nVersion = 0;

      si.ReadUInt(&nVersion);

      if (nVersion == 0) // compute TIN after loading!!
      {
         ElevationPoint pt;

         // Read Corner Points:
         _ReadElevationPoint(&si, pt); _ptsCorner.push_back(pt);
         _ReadElevationPoint(&si, pt); _ptsCorner.push_back(pt);
         _ReadElevationPoint(&si, pt); _ptsCorner.push_back(pt);
         _ReadElevationPoint(&si, pt); _ptsCorner.push_back(pt);

         // Get Boundary!
         _x0 = _ptsCorner[0].x;
         _y0 = _ptsCorner[0].y;
         _x1 = _ptsCorner[2].x;
         _y1 = _ptsCorner[2].y;

         unsigned int nNorthPoints;
         unsigned int nEastPoints;
         unsigned int nSouthPoints;
         unsigned int nWestPoints;
         unsigned int nMiddlePoints;

         si.ReadUInt(&nNorthPoints);

         for (unsigned int i=0;i<nNorthPoints;i++)
         {
            _ReadElevationPoint(&si, pt); _ptsNorth.push_back(pt);
         }

         si.ReadUInt(&nEastPoints);

         for (unsigned int i=0;i<nEastPoints;i++)
         {
            _ReadElevationPoint(&si, pt); _ptsEast.push_back(pt);
         }

         si.ReadUInt(&nSouthPoints);

         for (unsigned int i=0;i<nSouthPoints;i++)
         {
            _ReadElevationPoint(&si, pt); _ptsSouth.push_back(pt);
         }

         si.ReadUInt(&nWestPoints);

         for (unsigned int i=0;i<nWestPoints;i++)
         {
            _ReadElevationPoint(&si, pt); _ptsWest.push_back(pt);
         }

         si.ReadUInt(&nMiddlePoints);

         for (unsigned int i=0;i<nMiddlePoints;i++)
         {
            _ReadElevationPoint(&si, pt); _ptsMiddle.push_back(pt);
         }

         //*********************************************************
         // FATAL ERROR
         // DO NOT USE THIS ELEVATION DATA ANYMORE
         // PLEASE PROCESS THIS DATA AGAIN USING LATEST VERSION OF
         // ELEVATION DATA PROCESSOR!!!!
         assert(false); // PLEASE CREATE NEW DATASET
         // PROCESS DATA AGAIN!!
         // For more information contact martin.christen@fhnw.ch
         //*********************************************************
         
         _PrecomputeTriangulation(); // slow!

      }
      else if (nVersion == 1) // format: precomputed TIN!
      {
         ElevationPoint pt;

         // Read Corner Points:
         _ReadElevationPoint(&si, pt); _ptsCorner.push_back(pt);
         _ReadElevationPoint(&si, pt); _ptsCorner.push_back(pt);
         _ReadElevationPoint(&si, pt); _ptsCorner.push_back(pt);
         _ReadElevationPoint(&si, pt); _ptsCorner.push_back(pt);

         // Get Boundary!
         _x0 = _ptsCorner[0].x;
         _y0 = _ptsCorner[0].y;
         _x1 = _ptsCorner[2].x;
         _y1 = _ptsCorner[2].y;

         unsigned int nNorthPoints;
         unsigned int nEastPoints;
         unsigned int nSouthPoints;
         unsigned int nWestPoints;
         unsigned int nMiddlePoints;

         si.ReadUInt(&nNorthPoints);

         for (unsigned int i=0;i<nNorthPoints;i++)
         {
            _ReadElevationPoint(&si, pt); _ptsNorth.push_back(pt);
         }

         si.ReadUInt(&nEastPoints);

         for (unsigned int i=0;i<nEastPoints;i++)
         {
            _ReadElevationPoint(&si, pt); _ptsEast.push_back(pt);
         }

         si.ReadUInt(&nSouthPoints);

         for (unsigned int i=0;i<nSouthPoints;i++)
         {
            _ReadElevationPoint(&si, pt); _ptsSouth.push_back(pt);
         }

         si.ReadUInt(&nWestPoints);

         for (unsigned int i=0;i<nWestPoints;i++)
         {
            _ReadElevationPoint(&si, pt); _ptsWest.push_back(pt);
         }

         si.ReadUInt(&nMiddlePoints);

         for (unsigned int i=0;i<nMiddlePoints;i++)
         {
            _ReadElevationPoint(&si, pt); _ptsMiddle.push_back(pt);
         }

         // Position and TexCoord and Normals (reserved)
         unsigned int nElevationPoints;
         si.ReadUInt(&nElevationPoints);

         float px, py, pz;
         float nx, ny, nz; 
         float tx, ty;
         for (unsigned int i=0;i<nElevationPoints;i++)
         {
            si.ReadFloat(&px);
            si.ReadFloat(&py);
            si.ReadFloat(&pz);
            _lstElevationPointWGS84.push_back(vec3<float>(px, py, pz));

            si.ReadFloat(&tx);
            si.ReadFloat(&ty);
            _lstTexCoord.push_back(vec2<float>(tx, ty));

            si.ReadFloat(&nx); si.ReadFloat(&ny); si.ReadFloat(&nz);
            // currently unsupported!
         }

         // Index
         unsigned int nIndexSize;
         si.ReadUInt(&nIndexSize);
         int idx;
         for (unsigned int i=0;i<nIndexSize;i++)
         {
            si.ReadInt(&idx);
            _lstIndices.push_back(idx);
         }

         // Offset
         si.ReadDouble(&_vOffset.x);
         si.ReadDouble(&_vOffset.y);
         si.ReadDouble(&_vOffset.z);

      }
      else
      {
         si.Close();
         return false;
      }
      
      si.Close();
   }
   catch (ogException e)
   {
      si.Close();
      return false;
   }

   return true;
}

//-----------------------------------------------------------------------------

void ElevationTile::_PrecomputeTriangulation()
{
   // Precompute Triangulation
   GeoCoord                      oGeoCoord;
   GeoCoord                      oGeoCoordNorm;

   double lng, lat;
   double x,y,z;

   boost::shared_ptr<math::DelaunayTriangulation> qTriangulation;
   qTriangulation = CreateTriangulation();
   std::vector<ElevationPoint>   lstElevationPoint;

   _lstIndices.clear();
   _lstTexCoord.clear();
   _lstElevationPointWGS84.clear();

   qTriangulation->GetPointVec(lstElevationPoint);
   qTriangulation->GetTriangleIndices(_lstIndices);
   

   double TexCoordOffsetX = _ptsCorner[0].x;
   double TexCoordOffsetY = _ptsCorner[0].y;
   double TexCoordDX = fabs(_ptsCorner[1].x - _ptsCorner[0].x);
   double TexCoordDY = fabs(_ptsCorner[3].y - _ptsCorner[0].y);

   // Virtual Camera Offset (STORE)

   //**********************************************************
   // Calc virtual camera offset:
   //**********************************************************
   _vOffset.x = _ptsCorner[0].x + TexCoordDX/2.0;
   _vOffset.y = _ptsCorner[0].y + TexCoordDY/2.0;
   _vOffset.z = _ptsCorner[0].elevation;

   GeoRef::Mercator::ReverseCustom(_vOffset.x, _vOffset.y, lng, lat, 0.0);
   oGeoCoord.SetLatitude(lat);
   oGeoCoord.SetLongitude(lng);
   oGeoCoord.SetEllipsoidHeight(_vOffset.z);
   oGeoCoord.ToCartesian(&_vOffset.x,&_vOffset.y,&_vOffset.z);
   //**********************************************************

   std::vector<ElevationPoint>::iterator it  = lstElevationPoint.begin();
   while (it!=lstElevationPoint.end())
   {
      GeoRef::Mercator::ReverseCustom((*it).x, (*it).y, lng, lat, 0.0);
      oGeoCoord.SetLatitude(lat);
      oGeoCoord.SetLongitude(lng);
      oGeoCoord.SetEllipsoidHeight((*it).elevation);
      oGeoCoord.ToCartesian(&x,&y,&z);

      double u = ((*it).x - TexCoordOffsetX) / TexCoordDX;
      double v = ((*it).y - TexCoordOffsetY) / TexCoordDY;

      _lstTexCoord.push_back(vec2<float>((float)u,(float)v));
      _lstElevationPointWGS84.push_back(vec3<float>( 
                                                      (float) (x-_vOffset.x), 
                                                      (float) (y-_vOffset.y), 
                                                      (float) (z-_vOffset.z)));

      it++;
     
      //i++;
   }

}

//-----------------------------------------------------------------------------

bool ElevationTile::SaveElevationTile(const std::string& sQuadcode, const std::string& sRepository, const std::string& sLayer)
{
   std::string sFilename = sRepository + "/" + sLayer + "_tiles/" + sQuadcode + ".elv";

   if (_bHierarchy)
   {
      sFilename = FilenameUtils::MakeHierarchicalFileName(sFilename, 2);
      FileSystem::makeallsubdirs(sFilename);
   }

   _PrecomputeTriangulation(); // Create Vertex / Index Buffer coordinates (WGS84 Ellipsoid)

   if (_lstElevationPointWGS84.size() != _lstTexCoord.size())
   {
      assert(false);
      return false;
   }

   StreamOutFile so;

   // a tile requires atleast 4 corner points!
   if (_ptsCorner.size()!=4)
   {
      assert(false);
      return false;
   }

   try
   {
      so.Open(sFilename);

      // Write Header:
      so.WriteByte('T'); so.WriteByte('I'); so.WriteByte('L'); so.WriteByte('E');

      // Write Version:
      unsigned int nVersion = 1; // Version 1 has precomputed TIN
      so.WriteUInt(&nVersion); 

      _WriteElevationPoint(&so, _ptsCorner[0]);
      _WriteElevationPoint(&so, _ptsCorner[1]);
      _WriteElevationPoint(&so, _ptsCorner[2]);
      _WriteElevationPoint(&so, _ptsCorner[3]);

      unsigned int nNorthPoints = (unsigned int) _ptsNorth.size();
      unsigned int nEastPoints = (unsigned int) _ptsEast.size();
      unsigned int nSouthPoints = (unsigned int) _ptsSouth.size();
      unsigned int nWestPoints = (unsigned int) _ptsWest.size();
      unsigned int nMiddlePoints = (unsigned int) _ptsMiddle.size();

      so.WriteUInt(&nNorthPoints);
      for (size_t i=0;i<_ptsNorth.size();i++)
      {
         _WriteElevationPoint(&so, _ptsNorth[i]);
      }

      so.WriteUInt(&nEastPoints);
      for (size_t i=0;i<_ptsEast.size();i++)
      {
         _WriteElevationPoint(&so, _ptsEast[i]);
      }

      so.WriteUInt(&nSouthPoints);
      for (size_t i=0;i<_ptsSouth.size();i++)
      {
         _WriteElevationPoint(&so, _ptsSouth[i]);
      }

      so.WriteUInt(&nWestPoints);
      for (size_t i=0;i<_ptsWest.size();i++)
      {
         _WriteElevationPoint(&so, _ptsWest[i]);
      }

      so.WriteUInt(&nMiddlePoints);
      for (size_t i=0;i<_ptsMiddle.size();i++)
      {
         _WriteElevationPoint(&so, _ptsMiddle[i]);
      }

      // Position and TexCoord and Normals (reserved)
      unsigned int nElevationPoints = _lstElevationPointWGS84.size();
      so.WriteUInt(&nElevationPoints);
      float nx, ny, nz; 
      nx = ny = nz = 0.0f;
      for (size_t i=0;i<_lstElevationPointWGS84.size();i++)
      {
         so.WriteFloat(&_lstElevationPointWGS84[i].x);
         so.WriteFloat(&_lstElevationPointWGS84[i].y);
         so.WriteFloat(&_lstElevationPointWGS84[i].z);
         so.WriteFloat(&_lstTexCoord[i].x);
         so.WriteFloat(&_lstTexCoord[i].y);
         so.WriteFloat(&nx);
         so.WriteFloat(&ny);
         so.WriteFloat(&nz);
      }

      // Index
      unsigned int nIndexSize = _lstIndices.size();
      so.WriteUInt(&nIndexSize);
      for (size_t i=0;i<_lstIndices.size();i++)
      {
         so.WriteInt(&_lstIndices[i]);
      }
   
      // Offset
      so.WriteDouble(&_vOffset.x);
      so.WriteDouble(&_vOffset.y);
      so.WriteDouble(&_vOffset.z);


      so.Close();
   }
   catch (ogException)
   {
      return false;
   }

   return true;
}

//-----------------------------------------------------------------------------
#define MINELEVATION -1e20
// 
//   (x0,y1) +-----+ (x1,y1)
//           |     |
//           |     |
//   (x0,y0) +-----+ (x1,y0)
//   
void ElevationTile::CategorizePoints(double x0, double y0, double x1, double y1, bool bCornerTile, std::vector<ElevationPoint>& pts)
{
   //-----------------------------------
   // EPSILON SETTINGS:
   const double EDGE_EPSILON = 1e-12;
   //-----------------------------------

   _ptsCorner.clear(); _ptsNorth.clear(); _ptsEast.clear(); 
   _ptsSouth.clear(); _ptsWest.clear(); _ptsMiddle.clear();

   _x0 = x0; 
   _y0 = y0; 
   _x1 = x1; 
   _y1 = y1;

   assert(_x1>_x0);
   assert(_y1>_y0);

   // Setup default corner points (with elevation = 0)
   ElevationPoint c0, c1, c2, c3;
   c0.x = x0;
   c0.y = y0;
   c0.elevation = MINELEVATION;
   c0.weight = -3;

   c1.x = x1;
   c1.y = y0;
   c1.elevation = MINELEVATION;
   c1.weight = -3;

   c2.x = x1;
   c2.y = y1;
   c2.elevation = MINELEVATION;
   c2.weight = -3;

   c3.x = x0;
   c3.y = y1;
   c3.elevation = MINELEVATION;
   c3.weight = -3;

   _ptsCorner.push_back(c0);
   _ptsCorner.push_back(c1);
   _ptsCorner.push_back(c2);
   _ptsCorner.push_back(c3);

   // for precision reasons, the edge is decided using mid-point
   double midx = x0 + 0.5*fabs(x1-x0);
   double midy = y0 + 0.5*fabs(y1-y0);
   const double boundaryEpsilon = 1e-13;

   for (size_t i=0;i<pts.size();i++)
   {
      // make sure point really is in boundary!
      if (pts[i].x > _x1)
      {
         assert(false);
         pts[i].x = _x1;
      }

      if (pts[i].y > _y1)
      {
         assert(false);
         pts[i].y = _y1;  
      }
      
      if (pts[i].x < _x0)
      {
         assert(false);
         pts[i].x = _x0;
      }

      if (pts[i].y < _y0)
      {
         assert(false);
         pts[i].y = _y0;
      }

      bool bReject = false;

      if (!bReject)
      {
         double w = pts[i].weight;
         
         bool bAdded = false;
         const double CORNER_EPSILON = DBL_EPSILON;
         
         for (size_t k=0;k<4;k++)
         {
            if (fabs(_ptsCorner[k].x - pts[i].x)<CORNER_EPSILON &&
                fabs(_ptsCorner[k].y - pts[i].y)<CORNER_EPSILON)
            {
               _ptsCorner[k].elevation = pts[i].elevation;
               bAdded = true;
            }
         }
         
         
         // Classify EDGE points (-2)
         if (!bAdded)
         {
            
            // it is very important to adjust axis of edge point to EXACT value!
            if (fabs(pts[i].x - x0) < EDGE_EPSILON) // west
            {
               ElevationPoint pt;
               pt = pts[i];
               pt.weight = -2;
               pt.x = x0;
               _ptsWest.push_back(pt);
               bAdded = true;
            }
            else if (fabs(pts[i].x - x1) < EDGE_EPSILON) // east
            {
               ElevationPoint pt;
               pt = pts[i];
               pt.weight = -2;
               pt.x = x1;
               _ptsEast.push_back(pt);
               bAdded = true;
            }
            else if ((fabs(pts[i].y - y0) < EDGE_EPSILON)) // south
            {
               ElevationPoint pt;
               pt = pts[i];
               pt.weight = -2;
               pt.y = y0;
               _ptsSouth.push_back(pt);
               bAdded = true;
            }
            else if ((fabs(pts[i].y - y1) < EDGE_EPSILON)) // north
            {
               ElevationPoint pt;
               pt = pts[i];
               pt.weight = -2;
               pt.y = y1;
               _ptsNorth.push_back(pt);
               bAdded = true;
            }
         }
         
         // Classify: Middle
         if (!bAdded)
         {
            _ptsMiddle.push_back(pts[i]);
         }


      }
   }

   sortpoints<ElevationPoint>(_ptsNorth);
   sortpoints<ElevationPoint>(_ptsEast);
   sortpoints<ElevationPoint>(_ptsSouth);
   sortpoints<ElevationPoint>(_ptsWest);
   sortpoints<ElevationPoint>(_ptsMiddle);

   EliminateCloseToCorner(_x0, DBL_MAX, _x1, DBL_MAX, _ptsNorth, fabs(_x1-_x0)/100.0);
   EliminateCloseToCorner(_x0, DBL_MAX, _x1, DBL_MAX, _ptsSouth, fabs(_x1-_x0)/100.0);
   EliminateCloseToCorner(DBL_MAX, _y0, DBL_MAX, _y1, _ptsEast, fabs(_y1-_y0)/100.0);
   EliminateCloseToCorner(DBL_MAX, _y0, DBL_MAX, _y1, _ptsWest, fabs(_y1-_y0)/100.0);
   EliminateCloseToCorner(_x0, _y0, _x1, _y1, _ptsMiddle, fabs(_y1-_y0)/200.0);

   EliminateDoubleEntries(_ptsNorth, fabs(_x1-_x0)/100.0);
   EliminateDoubleEntries(_ptsEast, fabs(_y1-_y0)/100.0);
   EliminateDoubleEntries(_ptsSouth, fabs(_x1-_x0)/100.0);
   EliminateDoubleEntries(_ptsWest, fabs(_y1-_y0)/100.0);
   
   // fix border tiles:
   
   //---------------------------------------------------------------------------
   // NEW 2010 - added by chm
   // if there are no middle tiles, don't connect edge tiles, just remove
   // edge tiles!!
   if (_ptsMiddle.size() == 0)
   {
      _ptsWest.clear();
      _ptsNorth.clear();
      _ptsSouth.clear();
      _ptsEast.clear();
   }
   //---------------------------------------------------------------------------
   
   for (int i=0;i<4;i++)
   {
      if (_ptsCorner[i].elevation == MINELEVATION)
      {
         _ptsCorner[i].elevation = 0.0;
      }
   }
}

//------------------------------------------------------------------------------

boost::shared_ptr<math::DelaunayTriangulation> ElevationTile::CreateTriangulation(bool bBorderOnly)
{  
   if (_ptsCorner.size()!=4)
   {  
      return boost::shared_ptr<math::DelaunayTriangulation>();
   }

   double eps = fabs(_x1 - _x0);
   boost::shared_ptr<math::DelaunayTriangulation> qTriangulation = boost::shared_ptr<math::DelaunayTriangulation>(new math::DelaunayTriangulation(_x0-eps, _y0-eps, _x1+eps, _y1+eps));
   qTriangulation->SetEpsilon(DBL_EPSILON);

   qTriangulation->InsertPoint(_ptsCorner[0]);
   qTriangulation->InsertPoint(_ptsCorner[1]);
   qTriangulation->InsertPoint(_ptsCorner[2]);
   qTriangulation->InsertPoint(_ptsCorner[3]);

   if (!bBorderOnly)
   {
      for (size_t i=0;i<_ptsNorth.size();i++)
      {
         qTriangulation->InsertPoint(_ptsNorth[i]);
      }

      for (size_t i=0;i<_ptsEast.size();i++)
      {
         qTriangulation->InsertPoint(_ptsEast[i]);
      }

      for (size_t i=0;i<_ptsSouth.size();i++)
      {
         qTriangulation->InsertPoint(_ptsSouth[i]);
      }

      for (size_t i=0;i<_ptsWest.size();i++)
      {
         qTriangulation->InsertPoint(_ptsWest[i]);
      }

      for (size_t i=0;i<_ptsMiddle.size();i++)
      {
         qTriangulation->InsertPoint(_ptsMiddle[i]);
      }
   }

   return qTriangulation;
}

//-----------------------------------------------------------------------------

size_t ElevationTile::GetNumPoints()
{
   return (_ptsCorner.size() + _ptsEast.size() 
            + _ptsMiddle.size() + _ptsNorth.size()
            + _ptsSouth.size() + _ptsWest.size());

}

//-----------------------------------------------------------------------------


void ElevationTile::Sort()
{
   sortpoints<ElevationPoint>(_ptsNorth);
   sortpoints<ElevationPoint>(_ptsEast);
   sortpoints<ElevationPoint>(_ptsSouth);
   sortpoints<ElevationPoint>(_ptsWest);
   sortpoints<ElevationPoint>(_ptsMiddle);
}