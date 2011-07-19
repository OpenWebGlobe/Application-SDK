/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "og.h"
#include "image/ImageLoader.h"
#include "lodepng/lodepng.h"
#include "string/StringUtils.h"
#include "Object.h"
#include <fstream>
#include <cassert>

#define STBI_NO_HDR
#include "stb_image.c"

#define LODEPNG_COMPILE_ZLIB
#define LODEPNG_COMPILE_ENCODER
#include "lodepng/lodepng.cpp"

#include "data/DiskCache.h"

//------------------------------------------------------------------------------

struct SImageLoadData
{
   Img::FileFormat      eFormat;
   Img::PixelFormat     eDestPixelFormat;
   std::string          sUrl;
   void*                userdata;
   CallBack_ImageReady  cbf;
   CallBack_ImageFailed cbf_failed;
   ImageObject*         imageobject;
};

//------------------------------------------------------------------------------

void _OnLoadImage(const std::string& sUrl, MemoryEntry* pMemoryEntry, void* userdata)
{
   if (userdata)
   {
      SImageLoadData* pData =  (SImageLoadData*)userdata;

      if (pMemoryEntry->IsFailed())
      {
         pData->cbf_failed(pData->userdata);
         return;
      }

      ImageObject* pImageObject = new ImageObject();
      if (ImageLoader::LoadFromMemory(pData->eFormat, pMemoryEntry->Data(), pMemoryEntry->DataSize(), pData->eDestPixelFormat, *pImageObject))
      {
         pData->cbf(pImageObject, pData->userdata);
      }
      else
      {
         std::cout << "failed unpacking " << sUrl << "\n";
      }

      delete pData;
   }
}

//------------------------------------------------------------------------------

void ImageLoader::LoadFromUrl(Img::FileFormat eFormat, const std::string& sUrl, Img::PixelFormat eDestPixelFormat, CallBack_ImageReady cbf, CallBack_ImageFailed cbf_failed, void* userdata)
{
   Cache& cache = OpenGlobe::_ogCache();
   SImageLoadData* pUserData = new SImageLoadData;
   pUserData->eFormat = eFormat;
   pUserData->eDestPixelFormat = eDestPixelFormat;
   pUserData->sUrl = sUrl;
   pUserData->userdata = userdata;
   pUserData->cbf = cbf;
   pUserData->cbf_failed = cbf_failed;
   pUserData->imageobject = 0;

   cache.RequestDataAsync(sUrl, &_OnLoadImage, (void*)pUserData);
}

//------------------------------------------------------------------------------

void _OnLoadImageThreaded(const std::string& sUrl, MemoryEntry* pMemoryEntry, void* userdata)
{
   if (userdata)
   {
      SImageLoadData* pData =  (SImageLoadData*)userdata;

      if (pMemoryEntry->IsFailed() || pData->imageobject->GetRawData() == 0)
      {
         pData->cbf_failed(pData->userdata);
         delete pData;
         return;
      }

      pData->cbf(pData->imageobject, pData->userdata);
      delete pData;
   }
}

//------------------------------------------------------------------------------

void _OnUnpackImage(const std::string& sUrl, MemoryEntry* pMemoryEntry, void* userdata)
{
   if (userdata)
   {
      SImageLoadData* pData =  (SImageLoadData*)userdata;

      if (pMemoryEntry->IsFailed())
      {
         return; // failed!! can't do anything
      }

      // Create new Image 
      if (ImageLoader::LoadFromMemory(pData->eFormat, pMemoryEntry->Data(), pMemoryEntry->DataSize(), pData->eDestPixelFormat, *pData->imageobject))
      {
         return; // all good!
      }
      else
      {
         std::cout << "WARNING: failed unpacking " << sUrl << "\n";
      }
   }
}

//------------------------------------------------------------------------------

void ImageLoader::LoadFromUrl_ThreadedUnpack(Img::FileFormat eFormat, const std::string& sUrl, Img::PixelFormat eDestPixelFormat, CallBack_ImageReady cbf, CallBack_ImageFailed cbf_failed, void* userdata)
{
   Cache& cache = OpenGlobe::_ogCache();
   SImageLoadData* pUserData = new SImageLoadData;
   pUserData->eFormat = eFormat;
   pUserData->eDestPixelFormat = eDestPixelFormat;
   pUserData->sUrl = sUrl;
   pUserData->userdata = userdata;
   pUserData->cbf = cbf;
   pUserData->cbf_failed = cbf_failed;
   pUserData->imageobject = new ImageObject();

   cache.RequestDataThreaded(sUrl, &_OnLoadImageThreaded, &_OnUnpackImage, (void*)pUserData);
}

//------------------------------------------------------------------------------

bool ImageLoader::LoadFromDisk(Img::FileFormat eFormat, const std::string& sFilename, Img::PixelFormat eDestPixelFormat, ImageObject& outputimage)
{
   std::vector<unsigned char> vecData;
   std::ifstream ifs;
   
#ifdef OS_WINDOWS
   std::wstring sFilenameW = StringUtils::Utf8_To_wstring(sFilename);
   ifs.open(sFilenameW.c_str(), std::ios::in | std::ios::binary);
#else
   ifs.open(sFilename.c_str(), std::ios::in | std::ios::binary);
#endif
   if (ifs.good())
   {
      unsigned char s;
      while (!ifs.eof())
      {  
         ifs.read((char*)&s, 1);
         vecData.push_back(s);  
      }
   }
   else
   {
      return false;
   }
   
   return ImageLoader::LoadFromMemory(eFormat, &vecData[0], vecData.size(), eDestPixelFormat, outputimage);
}

//------------------------------------------------------------------------------

bool ImageLoader::LoadFromMemory(Img::FileFormat eFormat, const unsigned char* pData, const unsigned int nSize, Img::PixelFormat eDestPixelFormat, ImageObject& outputimage)
{
   unsigned int w,h;

   switch(eFormat)
   {
      case Img::Format_PNG:
         {
            std::vector<unsigned char> out;
            if (LodePNG::decode(out, w, h, pData, nSize) == 0)
            {
               outputimage.AllocateImage(w,h, eDestPixelFormat);
               outputimage.FillFromRGBA(&(out[0]));
               return true;
            }
            else
            {     
               return false; // failed!
            }
         }
         break;
      case Img::Format_JPG:
      case Img::Format_GIF:
      case Img::Format_TGA:
      {
         int x,y,comp;
         unsigned char* pImage = stbi_load_from_memory((unsigned char*)pData, (int)nSize, &x, &y, &comp, 0);
         if (x>0 && y>0)
         {
            if (comp == 3)
            {
               outputimage.AllocateImage(x,y, eDestPixelFormat);
               outputimage.FillFromRGB(pImage);
               stbi_image_free(pImage);
               return true;
            }
            else if (comp == 4)
            {
               outputimage.AllocateImage(x,y, eDestPixelFormat);
               outputimage.FillFromRGBA(pImage);
               stbi_image_free(pImage);
               return true;
            }
         }
         break;
      }
      default:
         assert(false);
   }

   return false;
}

//------------------------------------------------------------------------------
