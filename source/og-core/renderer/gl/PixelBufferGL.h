/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _PIXELBUFFER_GL
#define _PIXELBUFFER_GL

#include "og.h"
#include "renderer/IPixelBuffer.h"

namespace GL
{
   class OPENGLOBE_API PixelBufferGL : public IPixelBuffer
   {
   public:
      PixelBufferGL();
      virtual ~PixelBufferGL();

      virtual void Destroy();

      //! Create Pixel Buffer with specified size
      virtual void CreateBuffer(size_t nBytes);

      //! Get size of buffer
      virtual size_t GetBufferSize();

      //! Lock Buffer and retrieve Data pointer to it
      // \param bu BufferUsage is read if you want to get data from device. If you set PIXELBUFFER_WRITE then you can write to buffer. if PIXELBUFFER_READWRITE you can read and write to buffer.      
      virtual void* Lock(BufferUsage bu = PIXELBUFFER_WRITE);

      //! Unlock Buffer (data previously retrieved is now updated!)
      virtual void Unlock();

      //! Copy Buffer to Texture, please note that the Buffer must fit the size of the texture!
      //! \param iTexture the texture to copy to
      //! \return true if copy succeeded
      virtual bool CopyToTexture(shared_ptr<ITexture>& iTexture);

   protected:
      bool           _bHasHardware;
      unsigned int   _pboId;
      size_t         _nBufferSize;

   };

}

#endif