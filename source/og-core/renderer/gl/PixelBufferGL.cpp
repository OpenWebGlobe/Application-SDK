/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "renderer/gl/PixelBufferGL.h"
#include "renderer/gl/TextureGL.h"

namespace GL
{

//-----------------------------------------------------------------------------

PixelBufferGL::PixelBufferGL()
{
   _bHasHardware = false;
   _pboId = 0;
   _nBufferSize = 0;
}

//-----------------------------------------------------------------------------

PixelBufferGL::~PixelBufferGL()
{

}

//-----------------------------------------------------------------------------

void PixelBufferGL::Destroy()
{
   if (_pboId != 0 /*&& _bHasHardware*/)
   {
      glDeleteBuffersARB(1, &_pboId);
      _nBufferSize = 0;
      _pboId = 0;
   }
}

//-----------------------------------------------------------------------------

void PixelBufferGL::CreateBuffer(size_t nBytes)
{
   if (GLEW_ARB_pixel_buffer_object)
   {
      if (_pboId != 0)
      {
         Destroy();
      }
      _nBufferSize = nBytes;

      _bHasHardware = true;
      glGenBuffersARB(1, &_pboId);
      glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _pboId);
      glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, nBytes, 0, GL_STREAM_DRAW_ARB);
      glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
   }
}

//-----------------------------------------------------------------------------

size_t PixelBufferGL::GetBufferSize()
{
   return _nBufferSize;
}

//-----------------------------------------------------------------------------

void* PixelBufferGL::Lock(BufferUsage bu)
{
   if (_nBufferSize == 0)
      return 0;

   if (bu == PIXELBUFFER_READ)
   {
      return 0;
   }
   else if (bu == PIXELBUFFER_WRITE)
   {
      glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _pboId);
      glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, _nBufferSize, 0, GL_STREAM_DRAW_ARB);

      GLubyte* ptr = (GLubyte*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);

      return (void*) ptr;
   }
   else
   {
      assert(false);
   }

   return 0;
}

//-----------------------------------------------------------------------------

void PixelBufferGL::Unlock()
{
   if (_nBufferSize == 0)
      return;

   // release pointer to mapping buffer
   glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB); 


   // it is good idea to release PBOs with ID 0 after use.
   // Once bound with 0, all pixel operations behave normal ways.
   glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
}

//-----------------------------------------------------------------------------

bool PixelBufferGL::CopyToTexture(shared_ptr<ITexture>& iTexture)
{
   if (!iTexture)
   {
      assert(false);
      return false;
   }

   int width = iTexture->GetWidth();
   int height = iTexture->GetHeight();

   GLenum ePixelFormat;
   GLenum eInternalFormat;
   GLenum eType;
   _ConvertTexturePixelFormat(iTexture->GetPixelFormat(), 
      iTexture->GetPixelDataType(), ePixelFormat, eInternalFormat, eType);

   iTexture->BindTexture(0);

   glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _pboId);

   // copy pixels from PBO to texture object
   // Use offset instead of ponter.


   switch(iTexture->GetTextureTarget())
   {
   case Texture::GFX_TEXTURE_2D:
       glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, ePixelFormat, eType, 0); 
       break;
   case Texture::GFX_TEXTURE_RECTANGLE:
       glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, width, height, ePixelFormat, eType, 0); 
      break;
   default:
      assert(false); // texture target not supported, stupid!!!
      return false;
   }
  
   return true;
}

//-----------------------------------------------------------------------------

} // namespace

