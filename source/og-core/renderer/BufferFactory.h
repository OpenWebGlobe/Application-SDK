/*******************************************************************************
Project       : i3D Studio  
Purpose       : Vertex/Index Buffer Creation
Creation Date : 30.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2004-2007 by Martin Christen. All rights reserved.
*******************************************************************************/

#ifndef A_BUFFERFACTORY_H
#define A_BUFFERFACTORY_H

#include "og.h"
#include <GL/glew.h>
#include <boost/shared_ptr.hpp>
#include "IIndexBuffer.h"
#include "IVertexBuffer.h"


//! \class BufferFactory
//! \brief Create VertexBuffers, Index Buffers and Vertex Attribute Buffers.
//! This will create buffers for the engine implementation (currently OpenGL only).
//! It is abstracted so that other APIs could be used. Never assume buffers to be OpenGL only.
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup vb
class OPENGLOBE_API BufferFactory
{
public:
   //! \brief Create Vertex Attribute Buffer
   //! \param nSize Number of Components (for example 4 for xyzw)
   //! \param nVertexAttribute Attribute Type (check EVertexAttribute specification)
   //! \param decltype Attribute Data Type (check EVertexDataType specification)
   static boost::shared_ptr<IVertexAttributeBuffer> CreateVertexAttributeBuffer(unsigned int Size, EVertexAttribute nVertexAttribute, EVertexDataType edecltype);
   //! \brief Create Index Buffer
   //! \param size Size of index buffer (number of indices)
   static boost::shared_ptr<IIndexBuffer> CreateIndexBuffer(unsigned int size); 
   //! \brief Create Vertex Buffer
   //! \param size Size of vertex buffer (number of vertices)
   //! \param bUseOneStream Set to true if only one stream must be used. This setting has no use for OpenGL versions of vertex buffers. In doubt, set to false.
   static boost::shared_ptr<IVertexBuffer> CreateVertexBuffer(unsigned int size, bool bUseOneStream=false);
};


#endif