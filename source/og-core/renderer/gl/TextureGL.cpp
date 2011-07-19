/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "renderer/gl/TextureGL.h"
#include <cmath>
#include <cassert>
#include <GL/glew.h>
#include <iostream>
#include "glUtils.h"

namespace GL
{


//-----------------------------------------------------------------------------
// Utility functions:
//-----------------------------------------------------------------------------
// calculate next power of 2 of n.
inline int _Nextpow2(int n) 
{
   int p2=1; if (n==0) return 1; for (; p2<n; p2<<=1); return p2;
}
//-----------------------------------------------------------------------------
// test if specified number is power of 2
template<typename T>
bool _IsPower2(T i)
{
   int	nExp;
   double mant = frexp((double)i, &nExp);
   return (mant == 0.5);
}

//-----------------------------------------------------------------------------
// Convert Texture Target to OpenGL
inline GLenum _ConvertTextureTarget(Texture::Target eTarget)
{
   switch (eTarget)
   {
   case Texture::GFX_TEXTURE_2D:
      return GL_TEXTURE_2D;
   case Texture::GFX_TEXTURE_RECTANGLE:
      return GL_TEXTURE_RECTANGLE_ARB;
    
   default:
      assert(false);
   }
}
//-----------------------------------------------------------------------------
// Convert Pixel Format to OpenGL
bool _ConvertTexturePixelFormat(Texture::PixelFormat ePf, Texture::PixelDataType ePDT, GLenum& out, GLenum& eInternalFormat, GLenum& eType)
{
   switch(ePDT)
   {
   case Texture::GFX_UNSIGNED_BYTE:
      eType = GL_UNSIGNED_BYTE;
      break;
   case Texture::GFX_FLOAT:
      eType = GL_FLOAT;
      break;
   default:
      assert(false);
      return false;
   }

   switch (ePf)
   {
   case Texture::GFX_RED:
      out = GL_RED;
      eInternalFormat = 1;
      break;
   case Texture::GFX_GREEN:
      out = GL_GREEN;
      eInternalFormat = 1;
      break;
   case Texture::GFX_BLUE:
      out = GL_BLUE;
      eInternalFormat = 1;
      break;
   case Texture::GFX_ALPHA:
      out = GL_ALPHA;
      eInternalFormat = 1;
      break;
   case Texture::GFX_RGB:
      out = GL_RGB;
      eInternalFormat = 3;
      break;
   case Texture::GFX_BGR:
      out = GL_BGR;
      eInternalFormat = 3;
      break;
   case Texture::GFX_RGBA:
      out = GL_RGBA;
      eInternalFormat = 4;
      break;
   case Texture::GFX_BGRA:
      out = GL_BGRA;
      eInternalFormat = 4;
      break;
   case Texture::GFX_LUMINANCE:
      out = GL_LUMINANCE;
      eInternalFormat = 1;
      break;
   case Texture::GFX_LUMINANCE_ALPHA:
      out = GL_LUMINANCE_ALPHA;
      eInternalFormat = 2;
      break;
   default:
      assert(false);
      return false;
   }

   return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Implementation of TextureGL: OpenGL and OpenGL ES texture handling
//------------------------------------------------------------------------------
TextureGL::TextureGL()
{
   _nWidth = 0;
   _nHeight = 0;
   glGenTextures(1, &_textureId);
   _curTexUnit = 0;
}

//------------------------------------------------------------------------------

TextureGL::~TextureGL()
{
   
}

//------------------------------------------------------------------------------

void TextureGL::Destroy()
{
   if (_textureId != 0)
   {
      glDeleteTextures(1, &_textureId);
      _textureId = 0;
   }
}

//------------------------------------------------------------------------------

bool   TextureGL::CreateTexture( Texture::Target target,  
                     int  	width, 
                     int  	height, 
                     Texture::PixelFormat  	format, 
                     Texture::PixelDataType  	type, 
                     const void *  	pData)
{
   _nWidth = width;
   _nHeight = height;
   _eTarget = target; 
   _eFormat = format; 
   _eType = type;

   if (_textureId == 0) // previously destroyed ?
   {
      glGenTextures(1, &_textureId);
   }

   GLint texSize; 
   glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);

   if (target == Texture::GFX_TEXTURE_2D)
   {
      if (_nWidth > texSize || _nHeight > texSize)
      {
         std::cout << "** Error: texture size not supported!\n";
         return false;
      }

      if (_IsPower2<int>(width) && _IsPower2<int>(height))
      {
         // size is ok, create texture:

         glBindTexture(GL_TEXTURE_2D, _textureId);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
         
         GLenum ePixelFormat;
         GLenum eInternalFormat;
         GLenum eType;
         _ConvertTexturePixelFormat(_eFormat, _eType, ePixelFormat, eInternalFormat, eType);
        
         glTexImage2D(GL_TEXTURE_2D, 
            0, 
            eInternalFormat, 
            _nWidth, 
            _nHeight, 
            0, // border 
            ePixelFormat, 
            eType, 
            pData);

         glBindTexture(GL_TEXTURE_2D, 0);
      }
      else
      {
         std::cout << "** Error: not a power of 2 texure!\n";
         return false; // not a power of 2 texture!
      }
   }
   else if (target == Texture::GFX_TEXTURE_RECTANGLE)
   {

      ASSERT_ON_GLERROR;
      glBindTexture(GL_TEXTURE_RECTANGLE_ARB, _textureId);
      glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
      ASSERT_ON_GLERROR;

      GLenum ePixelFormat;
      GLenum eInternalFormat;
      GLenum eType;
      _ConvertTexturePixelFormat(_eFormat, _eType, ePixelFormat, eInternalFormat, eType);

      glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 
         0, 
         eInternalFormat, 
         _nWidth, 
         _nHeight, 
         0, // border 
         ePixelFormat, 
         eType, 
         pData);
      ASSERT_ON_GLERROR;

      glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
      ASSERT_ON_GLERROR;
   }
   else
   {
      assert(false);
   }


   return false;
}

//------------------------------------------------------------------------------

void TextureGL::UpdateTexture(int xoffset, int yoffset, int width, int height, void* data)
{
   GLenum ePixelFormat;
   GLenum eInternalFormat;
   GLenum eType;
   _ConvertTexturePixelFormat(_eFormat, _eType, ePixelFormat, eInternalFormat, eType);


   if (_eTarget == Texture::GFX_TEXTURE_2D)
   {
      glPushAttrib(GL_TEXTURE_BIT);
      BindTexture();
      glTexSubImage2D(GL_TEXTURE_2D,  0,  xoffset, yoffset, width, height,  ePixelFormat, eType, data);       
      ASSERT_ON_GLERROR;
      glPopAttrib();
   }
   else if (_eTarget == Texture::GFX_TEXTURE_RECTANGLE)
   {
      glPushAttrib(GL_TEXTURE_BIT);
      BindTexture();
      glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB,  0,  xoffset, yoffset, width, height,  ePixelFormat, eType, data);
      ASSERT_ON_GLERROR
      glPopAttrib();
   }
   else
   {
      assert(false);
   }
}

//------------------------------------------------------------------------------

int    TextureGL::GetWidth()
{
   return _nWidth;
}

//------------------------------------------------------------------------------

int    TextureGL::GetHeight()
{
   return _nHeight;
}

//------------------------------------------------------------------------------

Texture::PixelFormat TextureGL::GetPixelFormat()
{
   return _eFormat;
}

//------------------------------------------------------------------------------

Texture::PixelDataType TextureGL::GetPixelDataType()
{
   return _eType;
}

//------------------------------------------------------------------------------

void TextureGL::BindTexture(int nTextureUnit)
{
   _curTexUnit = nTextureUnit;

   if (GLEW_ARB_multitexture)
      glActiveTextureARB(GL_TEXTURE0_ARB+nTextureUnit);

   if (_eTarget == Texture::GFX_TEXTURE_2D)
   {
      glBindTexture(GL_TEXTURE_2D, _textureId);
   }
   else if (_eTarget == Texture::GFX_TEXTURE_RECTANGLE)
   {
      glBindTexture(GL_TEXTURE_RECTANGLE_ARB, _textureId);
   }
}

//-----------------------------------------------------------------------------

void TextureGL::UnbindTexture()
{
   if (GLEW_ARB_multitexture)
      glActiveTextureARB(GL_TEXTURE0_ARB+_curTexUnit);

   if (_eTarget == Texture::GFX_TEXTURE_2D)
   {
      glBindTexture(GL_TEXTURE_2D, 0);
   }
   else if (_eTarget == Texture::GFX_TEXTURE_RECTANGLE)
   {
      glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
   }
}

//-----------------------------------------------------------------------------

unsigned int TextureGL::GetMaxTextureSize()
{
   GLint texSize; 
   glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);
   return (unsigned int)texSize;
}

//-----------------------------------------------------------------------------

unsigned int TextureGL::GetMaxTextureUnits()
{
   if (GLEW_ARB_multitexture)
   {
      GLint maxTextureUnits;
      glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &maxTextureUnits);
      return (unsigned int)maxTextureUnits;
   }
   else
   {
      return 1;
   }
}

//-----------------------------------------------------------------------------


} // namespace


