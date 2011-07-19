/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "geo/layer/ElevationLayeri3d.h"
#include "geo/MercatorQuadtree.h"
#include "string/FilenameUtils.h"
#include "string/StringUtils.h"
#include "image/ImageLoader.h"

//------------------------------------------------------------------------------

ElevationLayeri3d::ElevationLayeri3d()
   : _bIsFailed(false), _bIsReady(false)
{

}

//------------------------------------------------------------------------------

ElevationLayeri3d::~ElevationLayeri3d()
{
}

//------------------------------------------------------------------------------

void ElevationLayeri3d::_OnLoadDatasetinfo(const std::string& sFilename, MemoryEntry* pEntry, void* userdata)
{
   if (pEntry->IsFailed())
   {
   }
   else
   {
      if (pEntry->IsReady())
      {
         std::string sXML;
        
         unsigned char* data = pEntry->Data();
         for (size_t i=0;i<pEntry->DataSize();i++)
         {
            sXML += data[i];
         }

         if (!DatasetInfo::FromXML(sXML,_oDatasetInfo))
         {
            _bIsFailed = true;
            return;
         }

         _bIsReady=true;
         _bIsFailed = false;

         if (_cbNotify) 
         {
            _cbNotify(this);
         }
         return;
      }
   }

   _bIsFailed = true;
   if (_cbNotify) 
   {  
      _cbNotify(this);
   }
   return;
}

//------------------------------------------------------------------------------

void ElevationLayeri3d::Setup(const std::string& server, const std::string& layer, boost::function<void(IElevationLayer*)> cbNotify)
{
   _cbNotify = cbNotify;
   _servername = FilenameUtils::DelimitPath(server);
   _layername = layer;

   std::string datasetfile = _servername + layer + "_meta.xml";

   // Download Dataset info:
   OpenGlobe::_ogCache().RequestDataAsync(datasetfile, boost::bind(&ElevationLayeri3d::_OnLoadDatasetinfo, this, _1, _2, _3), 0);

}

//------------------------------------------------------------------------------

bool ElevationLayeri3d::Ready()
{
   return _bIsReady;
}

//------------------------------------------------------------------------------

bool ElevationLayeri3d::Failed()
{
   return _bIsFailed;
}

//------------------------------------------------------------------------------

struct SElevationTileRequestI3D
{
   ElevationLayeri3d* pLayer;
   std::string    sQuadcode;
   boost::function<void(const std::string&, ElevationTile*)> cbfReady;
   boost::function<void(const std::string&)> cbfFailed;
   bool bInterpolate;
   unsigned int nLod;
   std::string    sQuadcodeParent;
};

//------------------------------------------------------------------------------

void ElevationLayeri3d::RequestTile(
   const std::string& sQuadcode, 
   boost::function<void(const std::string&, ElevationTile*)> cbfReady, 
   boost::function<void(const std::string&)> cbfFailed)
{
   if (Failed())
   {
      assert(false); // attempting to request tile from a failed image layer!!
      return;
   }

   if (!Ready())
   {
      assert(false); // attempting to request a tile from a non ready image layer!
      return;
   }

   // File-Format;
   std::string sExtension = ".elv";

   if (_oDatasetInfo.GetTileFormat() == "image/png")
   {
     
   }

   //  Check lod:
   
   unsigned int nRequestedLod = sQuadcode.length();
   unsigned int nLod = _oDatasetInfo.GetLevelOfDetail();
   bool bInterpolate = false;

   SElevationTileRequestI3D* pTileRequest = new SElevationTileRequestI3D;
   pTileRequest->cbfFailed = cbfFailed;
   pTileRequest->cbfReady = cbfReady;
   pTileRequest->pLayer = this;
   pTileRequest->sQuadcode = sQuadcode;

   if (nRequestedLod>nLod)
   {
      delete pTileRequest;
      // can't get!
   }
   else
   {
      pTileRequest->bInterpolate = false;
      pTileRequest->nLod = 0;
      std::string sFilename = FilenameUtils::DelimitPath(_servername + _layername);
      sFilename += sQuadcode;
      sFilename += sExtension;
      sFilename = FilenameUtils::MakeHierarchicalFileName(sFilename, 2);

      //ImageLoader::LoadFromUrl_ThreadedUnpack(eFormat, sFilename, ePixelformat, _OnImageReadyi3d, _OnImageFailedi3d, (void*) pTileRequest);

   }

  }

//------------------------------------------------------------------------------

int ElevationLayeri3d::GetMinLod()
{
   return 0;
} 

//------------------------------------------------------------------------------

int ElevationLayeri3d::GetMaxLod()
{
   if (!Ready())
   {
      return 0;
   }

   return (int)_oDatasetInfo.GetLevelOfDetail();
}

//------------------------------------------------------------------------------