/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _IMAGELOADER_H
#define _IMAGELOADER_H

#include "ImageHandler.h"

/*
   Example Code:
   -------------
   
   unsigned char* pData = { compressed image }
   unsigned int nSize = { size of compressed image }
   
   ImageLoader loader;
   ImageObject data;
   
   loader.ImageFromMemory(Img::Format_PNG, pData, nSize, PixelFormat_RGB, data);
   
   // ... now data contains RGB data of image...
   

*/


//----------------------------------------------------------------------
namespace Img
{

   enum FileFormat
   {
      Format_PNG,    // PNG File Format
      Format_JPG,    // JPG File Format (baseline only)
      Format_TGA,    // TGA File Format
      Format_GIF,    // GIF File Format
   };
}



//------------------------------------------------------------------------------

typedef void (*CallBack_ImageReady)(ImageObject* pImage, void* userdata);
typedef void (*CallBack_ImageFailed)(void* userdata);

/*static*/ class OPENGLOBE_API ImageLoader
{
public:
   ImageLoader() {}
   virtual ~ImageLoader() {}
   
   // async loading image from URL (lousing the openwebglobe cache, a local file is ok too...)
   static void LoadFromUrl(Img::FileFormat eFormat, const std::string& sUrl, Img::PixelFormat eDestPixelFormat, CallBack_ImageReady cbf, CallBack_ImageFailed cbf_failed, void* userdata = 0);

   // async loading and decompression of an image (image decompression is in download thread)
   static void LoadFromUrl_ThreadedUnpack(Img::FileFormat eFormat, const std::string& sUrl, Img::PixelFormat eDestPixelFormat, CallBack_ImageReady cbf, CallBack_ImageFailed cbf_failed, void* userdata = 0);
   
   // synchrousous loading from disk
   static bool LoadFromDisk(Img::FileFormat eFormat, const std::string& sFilename, Img::PixelFormat eDestPixelFormat, ImageObject& outputimage);
   
   // decompress from memory
   static bool LoadFromMemory(Img::FileFormat eFormat, const unsigned char* pData, const unsigned int nSize, Img::PixelFormat eDestPixelFormat, ImageObject& outputimage);   
   
};

//------------------------------------------------------------------------------

#endif