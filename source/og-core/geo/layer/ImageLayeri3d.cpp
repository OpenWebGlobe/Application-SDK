/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "geo/layer/ImageLayeri3d.h"
#include "geo/MercatorQuadtree.h"
#include "string/FilenameUtils.h"
#include "string/StringUtils.h"
#include "image/ImageLoader.h"

//------------------------------------------------------------------------------

ImageLayeri3d::ImageLayeri3d()
   : _bIsFailed(false), _bIsReady(false)
{

}

//------------------------------------------------------------------------------

ImageLayeri3d::~ImageLayeri3d()
{
}

//------------------------------------------------------------------------------


void ImageLayeri3d::_OnLoadDatasetinfo(const std::string& sFilename, MemoryEntry* pEntry, void* userdata)
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

void ImageLayeri3d::Setup(const std::string& server, const std::string& layer, boost::function<void(IImageLayer*)> cbNotify)
{
   _cbNotify = cbNotify;
   _servername = FilenameUtils::DelimitPath(server);
   _layername = layer;

   std::string datasetfile = _servername + layer + "_meta.xml";

   // Download Dataset info:
   OpenGlobe::_ogCache().RequestDataAsync(datasetfile, boost::bind(&ImageLayeri3d::_OnLoadDatasetinfo, this, _1, _2, _3), 0);

}

//------------------------------------------------------------------------------

bool ImageLayeri3d::Ready()
{
   return _bIsReady;
}

//------------------------------------------------------------------------------

bool ImageLayeri3d::Failed()
{
   return _bIsFailed;
}

//------------------------------------------------------------------------------

struct STileRequestI3D
{
   ImageLayeri3d* pLayer;
   std::string    sQuadcode;
   boost::function<void(const std::string&, ImageTile*)> cbfReady;
   boost::function<void(const std::string&)> cbfFailed;
   bool bInterpolate;
   unsigned int nLod;
   std::string    sQuadcodeParent;
};

//------------------------------------------------------------------------------

void _OnImageReadyi3d(ImageObject* pImage, void* userdata)
{
   STileRequestI3D* pTileReq = (STileRequestI3D*)userdata;
   
   if (pTileReq)
   {
      if (pTileReq->bInterpolate)
      {
         // with and height should always be the same, but still doing it this way...
         int nParentTileWidth = pImage->GetWidth();
         int nParentTileHeight = pImage->GetHeight();

         // Create new Image Object (destination)
         ImageObject* pNewImage = new ImageObject();
         pNewImage->AllocateImage(nParentTileWidth,nParentTileHeight, pImage->GetPixelFormat());  
         unsigned char* pDestImageData = pNewImage->GetRawData().get();
         
         // interpolate...
         double x0_r, y0_r, x1_r, y1_r; // requested coord
         double x0_a, y0_a, x1_a, y1_a; // available area
         MercatorQuadtree::QuadKeyToMercatorCoord(pTileReq->sQuadcode, x0_r, y0_r, x1_r, y1_r);
         MercatorQuadtree::QuadKeyToMercatorCoord(pTileReq->sQuadcodeParent, x0_a, y0_a, x1_a, y1_a);
      
         if (pImage->GetPixelFormat() == Img::PixelFormat_RGB ||
             pImage->GetPixelFormat() == Img::PixelFormat_BGR)
         {
            assert(false); // currently, RGB is not supported for interpolation!
         }
         else if (pImage->GetPixelFormat() == Img::PixelFormat_RGBA ||
             pImage->GetPixelFormat() == Img::PixelFormat_BGRA)
         {
            double dh1 = double(nParentTileHeight-1);
            double dw1 = double(nParentTileWidth-1);
            double dw = double(nParentTileWidth);
            double dh = double(nParentTileHeight);
            unsigned char r,g,b,a;

            for (int y=0;y<nParentTileHeight;y++)
            {
               for (int x=0;x<nParentTileWidth;x++)
               {
                  double xx = x0_r + (x1_r-x0_r)/(dw1)*x;
                  double yy = y0_r + (y1_r-y0_r)/(dh1)*(dh1-y);

                  double xpos = (double(xx)-x0_a) * dw/(x1_a-x0_a);
                  double ypos = dh - (double(yy)-y0_a) * dh/(y1_a-y0_a);

                  pImage->ReadPixelBilinear4(xpos, ypos, r,g,b,a);

                  size_t srcadr = 4*y*nParentTileWidth+4*x;
                  pDestImageData[srcadr+0] = r;
                  pDestImageData[srcadr+1] = g;
                  pDestImageData[srcadr+2] = b;
                  pDestImageData[srcadr+3] = a;
               }
            }

            delete pImage;
            pTileReq->cbfReady(pTileReq->sQuadcode, pNewImage);

         }
         else
         {
            assert(false); // unknown pixel format...
         }
      
      }
      else
      {

            pTileReq->cbfReady(pTileReq->sQuadcode, pImage);
      }
      
      delete pTileReq;
   }
   else
   {
      delete pImage;
   }
}

//------------------------------------------------------------------------------

void _OnImageFailedi3d(void* userdata)
{
   STileRequestI3D* pTileReq = (STileRequestI3D*)userdata;

   if (pTileReq)
   {
      pTileReq->cbfFailed(pTileReq->sQuadcode);
      delete pTileReq;
   }
}

//------------------------------------------------------------------------------

void ImageLayeri3d::RequestTile(
   const std::string& sQuadcode, 
   boost::function<void(const std::string&, ImageTile*)> cbfReady, 
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
   std::string sExtension = ".png";
   Img::FileFormat eFormat = Img::Format_PNG;
   Img::PixelFormat ePixelformat = Img::PixelFormat_RGBA;

   if (_oDatasetInfo.GetTileFormat() == "image/png")
   {
     eFormat = Img::Format_PNG;
     ePixelformat = Img::PixelFormat_RGBA;
     sExtension = ".png";
   }
   else if (_oDatasetInfo.GetTileFormat() == "image/jpg")
   {
      eFormat = Img::Format_JPG;
      ePixelformat = Img::PixelFormat_RGB;
      sExtension = ".jpg";     
   }
   else
   {
      assert(false); // not supported image format! ignore
      return;
   }

   //  Check lod:
   
   unsigned int nRequestedLod = sQuadcode.length();
   unsigned int nLod = _oDatasetInfo.GetLevelOfDetail();
   bool bInterpolate = false;

   STileRequestI3D* pTileRequest = new STileRequestI3D;
   pTileRequest->cbfFailed = cbfFailed;
   pTileRequest->cbfReady = cbfReady;
   pTileRequest->pLayer = this;
   pTileRequest->sQuadcode = sQuadcode;

   if (nRequestedLod>nLod)
   {
      // the requested level of detail is not available in this dataset, it must be interpolated from a higher level
      // this is currently not available
      std::string sQuadcodeParent = StringUtils::Left(sQuadcode, nLod);

      pTileRequest->bInterpolate = true; // needs interpolation..
      pTileRequest->nLod = nLod; // interpolate to
      pTileRequest->sQuadcodeParent = sQuadcodeParent;

      std::string sFilename = FilenameUtils::DelimitPath(_servername + _layername);
      sFilename += sQuadcodeParent;
      sFilename += sExtension;
      sFilename = FilenameUtils::MakeHierarchicalFileName(sFilename, 2);

      ImageLoader::LoadFromUrl_ThreadedUnpack(eFormat, sFilename, ePixelformat, _OnImageReadyi3d, _OnImageFailedi3d, (void*) pTileRequest);
   }
   else
   {
      pTileRequest->bInterpolate = false;
      pTileRequest->nLod = 0;
      std::string sFilename = FilenameUtils::DelimitPath(_servername + _layername);
      sFilename += sQuadcode;
      sFilename += sExtension;
      sFilename = FilenameUtils::MakeHierarchicalFileName(sFilename, 2);

      ImageLoader::LoadFromUrl_ThreadedUnpack(eFormat, sFilename, ePixelformat, _OnImageReadyi3d, _OnImageFailedi3d, (void*) pTileRequest);

   }

  }

//------------------------------------------------------------------------------

int ImageLayeri3d::GetMinLod()
{
   return 0;
} 

//------------------------------------------------------------------------------

int ImageLayeri3d::GetMaxLod()
{
   if (!Ready())
   {
      return 0;
   }

   return (int)_oDatasetInfo.GetLevelOfDetail();
}

//------------------------------------------------------------------------------