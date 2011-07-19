/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "DatasetInfo.h"
#include "xml/xml.h"

BeginPropertyMap(DatasetInfo);
   XMLProperty(DatasetInfo, "Layer"           ,_sLayerName);      
   XMLProperty(DatasetInfo, "Copyright"        ,_sLayerCopyright); 
   XMLProperty(DatasetInfo, "SRS"              ,_nSRS);            
   XMLProperty(DatasetInfo, "BoundingBox"      ,_vBoundingBox);    
   XMLProperty(DatasetInfo, "LevelOfDetail"    ,_nLevelofDetail);  
   XMLProperty(DatasetInfo, "ImageWidth"       ,_nImageWidth);     
   XMLProperty(DatasetInfo, "ImageHeight"      ,_nImageHeight);    
   XMLProperty(DatasetInfo, "TileSize"         ,_nTileSize);       
   XMLProperty(DatasetInfo, "TileLayout"       ,_vTileLayout);     
   XMLProperty(DatasetInfo, "TileFormat"       ,_sTileFormat);     
   XMLProperty(DatasetInfo, "Bounds"           ,_vBounds);         
   XMLProperty(DatasetInfo, "Center"           ,_vCenterCoord);    
   XMLProperty(DatasetInfo, "FlatFileHierarchy",_bFlat);   
EndPropertyMap(DatasetInfo)


//-----------------------------------------------------------------------------

DatasetInfo::DatasetInfo()
{
   // Set Default Values:
   _sLayerName = "unnamed";
   _sLayerCopyright = "(c) 2010 by FHNW. All Rights Reserved.";
   _nSRS = 3785;
   _nLevelofDetail = 0;
   _nImageWidth = 0;
   _nImageHeight = 0;
   _nTileSize = 256;
   _sTileFormat = "image/png";
   _bFlat = true;
}

//------------------------------------------------------------------------------

DatasetInfo::~DatasetInfo()
{

}

//------------------------------------------------------------------------------

void DatasetInfo::GetBoundingBox(double& ulx, double& uly, double& lrx, double& lry)
{
   if (_vBoundingBox.size() == 4)
   {
      ulx = _vBoundingBox[0];
      uly = _vBoundingBox[1];
      lrx = _vBoundingBox[2];
      lry = _vBoundingBox[3];
   }
   else
   {
      ulx = uly = lrx = lry = 0;
   }
}

//------------------------------------------------------------------------------

void DatasetInfo::GetTileLayout(unsigned int& TileX, unsigned int& TileY)
{
   if (_vTileLayout.size() == 2)
   {
      TileX = _vTileLayout[0];
      TileY = _vTileLayout[1];
   }
   else
   {
      TileX = 0;
      TileY = 0;
   }
}

//------------------------------------------------------------------------------

void DatasetInfo::SetBounds(unsigned int nMaxZoom, unsigned int TileX0, unsigned int TileY0, unsigned int TileX1, unsigned int TileY1)
{
   _vBounds.clear();
   _vBounds.push_back(nMaxZoom);
   _vBounds.push_back(TileX0);
   _vBounds.push_back(TileY0);
   _vBounds.push_back(TileX1);
   _vBounds.push_back(TileY1);
}

//------------------------------------------------------------------------------

void DatasetInfo::GetBounds(unsigned int& nMaxZoom, unsigned int& TileX0, unsigned int& TileY0, unsigned int& TileX1, unsigned int& TileY1)
{
   if (_vBounds.size() == 5)
   {
      nMaxZoom = _vBounds[0];
      TileX0 = _vBounds[1];
      TileY0 = _vBounds[2];
      TileX1 = _vBounds[3];
      TileY1 = _vBounds[4];
   }
   else
   {
      nMaxZoom = 0;
      TileX0 = 0;
      TileY0 = 0;
      TileX1 = 0;
      TileY1 = 0;
   }
}

//------------------------------------------------------------------------------

void DatasetInfo::SetCenter(double lng, double lat)
{
   _vCenterCoord.clear();
   _vCenterCoord.push_back(lng);
   _vCenterCoord.push_back(lat);
}

//------------------------------------------------------------------------------

void DatasetInfo::GetCenter(double& lng, double& lat)
{
   if (_vCenterCoord.size() == 2)
   {
      lng = _vCenterCoord[0];
      lat = _vCenterCoord[1];
   }
   else
   {
      lng = 0;
      lat = 0;
   }
}

//------------------------------------------------------------------------------

std::string DatasetInfo::ToXML()
{
   std::ostringstream oss;
   access::Class::ToXML(oss, "DatasetInfo", this);
   return oss.str();
}

//------------------------------------------------------------------------------

bool DatasetInfo::FromXML(const std::string& str, DatasetInfo& oDatasetInfo)
{
   std::istringstream iss(str);

   DatasetInfo* pDataset = (DatasetInfo*) access::Class::FromXML(iss, "DatasetInfo");
   if (pDataset)
   {
      oDatasetInfo = *pDataset;
      return true;
   }

   return false;
}

//------------------------------------------------------------------------------
