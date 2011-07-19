/*******************************************************************************
Project       : i3D Studio  
Purpose       : Vertex and index buffer management
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2004-2007 by Martin Christen. All rights reserved.
*******************************************************************************/


#ifndef A_BUFFERSOGL_H
#define A_BUFFERSOGL_H

#include "og.h"
#include <GL/glew.h>

#include "renderer/IIndexBuffer.h"
#include "renderer/IVertexBuffer.h"
   
//! \brief Reset Statistics.
void OPENGLOBE_API GfxResetStatistics();

//! \brief Retrieve Number of triangles drawn since last call to ResetStatistics
size_t OPENGLOBE_API GfxGetNumTriangles();


//! \brief Specify Vertex Buffer to be drawn.
//! \ingroup vb
void OPENGLOBE_API SetVertexBuffer(IVertexBuffer* pVB, bool ClearLastState=true);
//! \brief Draw Indexed Primitive
//! \param nPolyType Polygon Type (OpenGL enum like GL_TRIANGLES)
//! \param pIB Pointer to index buffer
//! \ingroup vb
void OPENGLOBE_API DrawIndexedPrimitive(unsigned int nPolyType, IIndexBuffer* pIB);
//! \ingroup vb
//! \param nPolyType Polygon Type (OpenGL enum like GL_TRIANGLES)
//! \param pVB Pointer to Vertex Buffer
void OPENGLOBE_API DrawPrimitive(unsigned int nPolyType, IVertexBuffer* pVB); // draw (non indexed) primitive


void OPENGLOBE_API ClearVertexBufferStates();


//! \class VertexAttributeBufferOGL
//! \brief Vertex Attribute Buffer for OpenGL.
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup vb
class OPENGLOBE_API VertexAttributeBufferOGL : public IVertexAttributeBuffer
{
public:
   VertexAttributeBufferOGL();
   virtual ~VertexAttributeBufferOGL() { Release(); }

   virtual int                Create(unsigned int Size, EVertexAttribute nVertexAttribute, EVertexDataType edecltype);
   virtual void               Release(void);
   virtual void*              GetDataPtr(void);  
   virtual void               SetValue(unsigned int index, float a=0, float b=0, float c=0, float d=0);
   virtual void               SetPackedValue(unsigned int index, unsigned int a);
   virtual unsigned int       GetPackedValue(unsigned int index);
   virtual unsigned int       GetChannelSizeInBytes();
   virtual unsigned int       GetNumComponents(); 
   virtual int                GetAttributeDataType(); // data type of system memory buffer: 1: FLOAT, 2: UINT
   virtual unsigned int       GetSize(void);

   virtual unsigned int	      CreateVRAMObject(void);
   virtual unsigned int       CopyToVRAM(bool bDynamic);
   virtual unsigned int       StreamToVRAM(); 

   virtual EVertexAttribute   GetAttributeType(void) { return _nVertexAttribute;}

   // OpenGL specific methods:
   GLuint                     GetGLName(EVertexAttribute nVertexAttribute) { return _nStreamName;}
   bool                       CheckMem(void) {return _mem;}

   void*                      GetData() {return _pVertexData;}

protected:
   EVertexDataType            _declt;
   EVertexAttribute           _nVertexAttribute;
   void*                      _pVertexData;
   unsigned int               _size;
   bool                       _mem;       // Engine specific memory reserved ?
   bool                       _bVBO;

   // OpenGL specific attributes
   GLuint                     _nStreamName;   
};

//! \class VertexBufferOGL
//! \brief Vertex Buffer for OpenGL.
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup vb
class OPENGLOBE_API VertexBufferOGL : public IVertexBuffer
{
public:
                              VertexBufferOGL();
   virtual                    ~VertexBufferOGL();

   virtual void               Initialize(unsigned int size);;
   virtual void               FreeResources(); 

   virtual void               AddVertexAttributeBuffer(boost::shared_ptr<IVertexAttributeBuffer> pVertexAttributeBuffer);
   virtual IVertexAttributeBuffer* GetVertexAttributeBuffer(EVertexAttribute eAttribute);

   virtual void               Flush();

   VertexAttributeBufferOGL*  GetBuffer(int i) {return (VertexAttributeBufferOGL*)_buffers[i].get();}

   virtual unsigned int	      CreateVRAMObject();
   virtual unsigned int	      CopyToVRAM(bool bDynamic);
   virtual unsigned int       StreamToVRAM(); 

   virtual unsigned int       GetSize(void) { return _size; }

   static long               GetNumBuffers(){return _numBuffers;}

protected:
   void                       _Destroy(); 
   unsigned int               _size;
   boost::shared_ptr<IVertexAttributeBuffer>     _buffers[MAX_VERTEX_ATTRIB];
   bool                       _bVBO;
   static long                _numBuffers;
};

//! \class IndexBufferOGL
//! \brief Index Buffer for OpenGL.
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup vb
class OPENGLOBE_API IndexBufferOGL : public IIndexBuffer
{
public:
                              IndexBufferOGL();
   virtual                    ~IndexBufferOGL();

   virtual void               Initialize(unsigned int size);

   virtual unsigned int       CreateVRAMObject(void);
   virtual unsigned int       CopyToVRAM(bool bDynamic);
   virtual unsigned int       StreamToVRAM(); 

   virtual void               FreeResources(); 

   GLuint                     GetGLName(void) { return nIndexName;}
   unsigned int		         GetMaxIndexValue() { return _nMaxIndexValue; }

   virtual unsigned int*      GetIndicesPtr(void);   //!< retrieve data pointer with index data
   virtual int*               GetFaceIdPtr(void);
   
   virtual void               SetValue(unsigned int index, unsigned int value); //!< set index buffer value directly
   virtual void               SetValue(unsigned int index, unsigned int value, int faceId); //!< set index buffer value and user defined faceID


   //! Return size of index buffer (number of index entries)
   unsigned int               GetSize(void) { return _size;}

   //! Return number of currently allocated Buffers
   static long               GetNumBuffers(){return _numBuffers;}

protected:
   void                       _Destroy(); 
   unsigned int               CreateIndexBuffer(void);
   GLuint                     nIndexName;
   bool                       _glMem;
   bool                       _bVBO;
   unsigned int	            _nMaxIndexValue;
   unsigned int*              _pIndexData;
   int*                       _pFaceIdData;
   unsigned int               _size;   
   static long                _numBuffers;
};

#endif
