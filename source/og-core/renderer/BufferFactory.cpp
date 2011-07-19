/*******************************************************************************
Project       : i3D Studio  
Purpose       : Vertex/Index Buffer Creation
Creation Date : 30.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2004-2007 by Martin Christen. All rights reserved.
*******************************************************************************/

#include "BufferFactory.h"
#include "renderer/gl/BuffersOGL.h"


boost::shared_ptr<IVertexAttributeBuffer> BufferFactory::CreateVertexAttributeBuffer(unsigned int Size, EVertexAttribute nVertexAttribute, EVertexDataType edecltype)
{
   boost::shared_ptr<IVertexAttributeBuffer> VABufOGL(new VertexAttributeBufferOGL());

   if (VABufOGL)
      VABufOGL->Create(Size, nVertexAttribute, edecltype);

   return VABufOGL;
}

boost::shared_ptr<IIndexBuffer> BufferFactory::CreateIndexBuffer(unsigned int size)
{
    boost::shared_ptr<IIndexBuffer> IBufOGL(new IndexBufferOGL());

   if (IBufOGL)
      IBufOGL->Initialize(size);

   return IBufOGL;
}

boost::shared_ptr<IVertexBuffer> BufferFactory::CreateVertexBuffer(unsigned int size, bool bUseOneStream)
{
   boost::shared_ptr<IVertexBuffer> VBufOGL(new VertexBufferOGL());

   if (VBufOGL)
      VBufOGL->Initialize(size);

   return VBufOGL;
}
