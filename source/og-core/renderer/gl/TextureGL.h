/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _TEXTUREGL_H
#define _TEXTUREGL_H

#include "renderer/ITexture.h"
#include <GL/glew.h>

namespace GL
{
   class OPENGLOBE_API TextureGL : public ITexture
   {
   public:
      TextureGL();
      virtual ~TextureGL();

      //! \brief Create a standard 2D texture
      //! \brief Create a texture
      //! \param target power of 2 or rectangle texture
      virtual bool   CreateTexture( Texture::Target target,  
         int  	width, 
         int  	height, 
         Texture::PixelFormat    format, 
         Texture::PixelDataType  type, 
         const void *  	data);

      virtual void UpdateTexture(int xoffset, int yoffset, int width, int height, void* data);

      //! \brief Free Memory of the texture
      virtual void Destroy();

      //! \brief Get Width of texture
      virtual int    GetWidth();

      //! \brief Get Height of texture
      virtual int    GetHeight();

      //! \brief Get pixel format of texture
      virtual Texture::PixelFormat GetPixelFormat();

      //! \brief Get pixel data type of texture
      virtual Texture::PixelDataType GetPixelDataType();

      //! \brief bind this texture to specified texture unit
      virtual void BindTexture(int nTextureUnit = 0);

      //! \brief unbind texture.
      virtual void UnbindTexture();

      //! \brief Retrieve Maximal Texture Size (hardware specific)
      virtual unsigned int GetMaxTextureSize();

      //! \brief Retrieve Maximal Supported Texture Units
      virtual unsigned int GetMaxTextureUnits();

      Texture::Target GetTextureTarget() {return _eTarget;}

   protected:
      int                        _nWidth, _nHeight;
      Texture::Target            _eTarget; 
      Texture::PixelFormat  	   _eFormat; 
      Texture::PixelDataType  	_eType;
      int                        _curTexUnit;

   private:
      unsigned int               _textureId;   // Texture Id (OpenGL)
   };


   bool OPENGLOBE_API _ConvertTexturePixelFormat(Texture::PixelFormat ePf, Texture::PixelDataType ePDT, GLenum& out, GLenum& eInternalFormat, GLenum& eType);

}

#endif
