/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef A_IVERTEXBUFFER_H
#define A_IVERTEXBUFFER_H

#include "og.h"
#include <boost/shared_ptr.hpp>

#include "renderer/IVertexAttributeBuffer.h"

class OPENGLOBE_API IVertexBuffer
{   
public:
   //! \brief Destructor
   virtual ~IVertexBuffer(){}

   //! \brief Initialize buffer size for specified vertex stream.
   //! \param size Number of vertices.
   virtual void Initialize(unsigned int size) = 0;   
   //! \brief Free all resources occupied by this vertex stream.
   virtual void FreeResources() = 0;  

   //! \brief Add an attribute buffer.
   //! \param pVertexAttributeBuffer shared 
   virtual void AddVertexAttributeBuffer(boost::shared_ptr<IVertexAttributeBuffer> pVertexAttributeBuffer) = 0;

   //! \brief Return attribute buffer.
   //! \param eAttribute Attribute type.
   //! \return Pointer to attribute buffer
   virtual IVertexAttributeBuffer* GetVertexAttributeBuffer(EVertexAttribute eAttribute) = 0;

   //! Allocate Memory on GPU.
   virtual unsigned int CreateVRAMObject() = 0; 
   //! Copy Vertex Data to GPU.
   //! \param bDynamic specifies if memory should be treated as dynamic. Set to true if this vertex buffer changes alot.
   virtual unsigned int CopyToVRAM(bool bDynamic) = 0; 
   //! Stream Vertex Data to GPU. This should be used if data changes several times per frame.
   virtual unsigned int StreamToVRAM() = 0;      

   //! Make sure this stream is fully uploaded to the graphics card.
   virtual void Flush() = 0;   

   //! Return number of vertices in buffer.
   //! \return number of vertices
   virtual unsigned int GetSize (void) = 0;
};


#endif

