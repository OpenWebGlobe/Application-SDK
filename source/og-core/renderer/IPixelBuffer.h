/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _IPIXELBUFFER_H
#define _IPIXELBUFFER_H

#include "og.h"
#include "renderer/ITexture.h"
#include "shared_ptr.h"

//-----------------------------------------------------------------------------

enum BufferUsage
{
   PIXELBUFFER_READ,          // Read Data from GPU to Buffer (DMA)
   PIXELBUFFER_WRITE,         // Write Data from Buffer to GPU (DMA)
};

//-----------------------------------------------------------------------------

class OPENGLOBE_API IPixelBuffer
{
public:
   IPixelBuffer(){}
   virtual ~IPixelBuffer(){}

   //! Destroy Buffer
   virtual void Destroy() = 0;

   //! Create Pixel Buffer with specified size
   virtual void CreateBuffer(size_t nBytes) = 0;

   //! Get size of buffer
   virtual size_t GetBufferSize() = 0;

   //! Lock Buffer and retrieve Data pointer to it
   // \param bu BufferUsage is read if you want to get data from device. If you set PIXELBUFFER_WRITE then you can write to buffer. if PIXELBUFFER_READWRITE you can read and write to buffer.      
   virtual void* Lock(BufferUsage bu = PIXELBUFFER_WRITE) = 0;
   
   //! Unlock Buffer (data previously retrieved is now updated!)
   virtual void Unlock() = 0;

   //! Copy Buffer to Texture, please note that the Buffer must fit the size of the texture!
   //! \param iTexture the texture to copy to
   //! \return true if copy succeeded
   virtual bool CopyToTexture(shared_ptr<ITexture>& iTexture) = 0;
};


#endif