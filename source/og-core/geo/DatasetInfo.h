/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _DATASETINFO_H
#define _DATASETINFO_H

#include "og.h"
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include "math/vec3.h"
#include "math/vec2.h"
#include <vector>
#include <string>
#include <map>


//-----------------------------------------------------------------------------
// DatasetInfo 
//! \class DatasetInfo
//! \author Martin Christen, martin.christen@fhnw.ch
class OPENGLOBE_API DatasetInfo
{
public:
   DatasetInfo();
   virtual ~DatasetInfo();

   void SetLayerName(std::string sLayerName){ _sLayerName = sLayerName;}
   void SetLayerCopyright(std::string sLayerCopyright) { _sLayerCopyright = sLayerCopyright;}
   void SetSRS(int nEPSG){_nSRS = nEPSG;}
   void SetBoundingBox(double ulx, double uly, double lrx, double lry){_vBoundingBox.clear(); _vBoundingBox.push_back(ulx); _vBoundingBox.push_back(uly); _vBoundingBox.push_back(lrx); _vBoundingBox.push_back(lry); }
   void SetLevelOfDetail(unsigned int nLod){ _nLevelofDetail = nLod;}
   void SetImageWidth(unsigned int nWidth){_nImageWidth = nWidth;}
   void SetImageHeight(unsigned int nHeight){_nImageHeight = nHeight;}
   void SetTileSize(unsigned int nTileSize){_nTileSize = nTileSize;}
   void SetTileLayout(unsigned int TileX, unsigned int TileY){_vTileLayout.clear(); _vTileLayout.push_back(TileX); _vTileLayout.push_back(TileY);}
   void SetTileFormat(std::string sTileFormat){_sTileFormat = sTileFormat;}
   void SetBounds(unsigned int nMaxZoom, unsigned int TileX0, unsigned int TileY0, unsigned int TileX1, unsigned int TileY1);
   void SetCenter(double lng, double lat);
   void SetFlat(bool f){_bFlat = f;}

   std::string    GetLayerName(){return _sLayerName;}
   std::string    GetLayerCopyright(){return _sLayerCopyright;}
   unsigned int   GetSRS(){return _nSRS;}
   void           GetBoundingBox(double& ulx, double& uly, double& lrx, double& lry);
   unsigned int   GetLevelOfDetail(){return _nLevelofDetail;}
   unsigned int   GetWidth(){return _nImageWidth;}
   unsigned int   GetHeight(){return _nImageHeight;}
   unsigned int   GetTileSize(){return _nTileSize;}
   void           GetTileLayout(unsigned int& TileX, unsigned int& TileY);
   std::string    GetTileFormat(){return _sTileFormat;}
   void           GetBounds(unsigned int& nMaxZoom, unsigned int& TileX0, unsigned int& TileY0, unsigned int& TileX1, unsigned int& TileY1);
   void           GetCenter(double& lng, double& lat);
   bool           IsFlat(){return _bFlat;}

   // Serialize this class from XML
   std::string    ToXML();
   static bool    FromXML(const std::string&, DatasetInfo& oDatasetInfo);

protected:
   std::string                _sLayerName;         // Layer Name
   std::string                _sLayerCopyright;    // Layer Copyright String
   unsigned int               _nSRS;               // Spatial Reference System: 3395 or 3785
   std::vector<double>        _vBoundingBox;       // Bounding Box: ulx uly lrx lry
   int                        _nLevelofDetail;     // Level of Detail
   unsigned int               _nImageWidth;
   unsigned int               _nImageHeight;
   unsigned int               _nTileSize;
   std::vector<unsigned int>  _vTileLayout;        // Tile Dimension: x y
   std::string                _sTileFormat;        // "image/png"

   std::vector<unsigned int>  _vBounds;            // Zoom, TileX0, TileY0, TileX1, TileY1
   std::vector<double>        _vCenterCoord;       // Dataset Center coord in WGS84
   bool                       _bFlat; // true if flat file hierarchy
};




#endif

