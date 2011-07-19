/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _ITEXTURE_H
#define _ITEXTURE_H

#include "og.h"
//-----------------------------------------------------------------------------

namespace Texture
{
   enum Target
   {
      GFX_TEXTURE_2D,         
      GFX_TEXTURE_RECTANGLE,
   };

   enum PixelFormat
   {
      GFX_RED,
      GFX_GREEN,
      GFX_BLUE,
      GFX_ALPHA,
      GFX_RGB,
      GFX_BGR,
      GFX_RGBA,
      GFX_BGRA,
      GFX_LUMINANCE,
      GFX_LUMINANCE_ALPHA,
   };

   enum PixelDataType
   {
      GFX_UNSIGNED_BYTE,
      GFX_FLOAT,
   };
}

//-----------------------------------------------------------------------------
// Class for 2D Textures
class OPENGLOBE_API ITexture
{
public:
   ITexture(){}
   virtual ~ITexture(){}

   //! \brief Create a 2D texture from memory.
   //! \param target power of 2 or rectangle texture
   //! \param width Width of texture
   //! \return true if succeeded
   virtual bool   CreateTexture( Texture::Target         target,  
                                 int  	                  width, 
                                 int  	                  height, 
                                 Texture::PixelFormat  	format, 
                                 Texture::PixelDataType  type, 
                                 const void *  	         data) = 0;

   virtual void UpdateTexture(int xoffset, int yoffset, int width, int height, void* data) = 0;


   //! \brief Free Memory of the texture
   virtual void Destroy() = 0;

   //! \brief Get Width of texture
   virtual int    GetWidth() = 0;

   //! \brief Get Height of texture
   virtual int    GetHeight() = 0;

   //! \brief Get pixel format of texture
   virtual Texture::PixelFormat GetPixelFormat() = 0;

   //! \brief Get pixel data type of texture
   virtual Texture::PixelDataType GetPixelDataType() = 0;

   //! \brief bind this texture to specified texture unit
   virtual void BindTexture(int nTextureUnit = 0) = 0;

   //! \brief Query Maximal Texture size
   virtual unsigned int GetMaxTextureSize() = 0; 

   //! \brief Retrieve Texture Target
   virtual Texture::Target GetTextureTarget() = 0;
};

//-----------------------------------------------------------------------------


#endif

