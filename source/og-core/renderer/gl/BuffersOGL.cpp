/*******************************************************************************
Project       : i3D Studio  
Purpose       : Vertex and index buffer management
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2004-2007 by Martin Christen. All rights reserved.
*******************************************************************************/

// Define DISABLE_VBO to disable using VBO
// (uses vertex arrays instead)
//#define DISABLE_VBO

#include "BuffersOGL.h"
#include "renderer/IVertexBuffer.h"
#include "renderer/IVertexAttributeBuffer.h"

#include <cassert>
#include <memory.h>
#include <stdlib.h>
#include <cmath>

//using namespace cwc;

long VertexBufferOGL::_numBuffers = 0;
long IndexBufferOGL::_numBuffers = 0;
static size_t _numTriangles = 0;


#pragma region UtilityFunctions

   void GfxResetStatistics()
   {
      _numTriangles = 0;
   }

   size_t GfxGetNumTriangles()
   {
      return _numTriangles;
   }



//namespace cwc
//{
   // Low Level Buffer creation/destruction
   /*unsigned int CreateVertexAttributeBuffer(IVertexAttributeBuffer** ppVA, unsigned int Size, EVertexAttribute nVertexAttribute, EVertexDataType edecltype)
   {
      *ppVA = new VertexAttributeBufferOGL();
      if (*ppVA)
         (*ppVA)->Create(Size, nVertexAttribute, decltype);
      else
         return 1;   // out of memory
      return 0;
   }

   void CreateIndexBuffer(IIndexBuffer** ppIB, unsigned int size)
   {
      *ppIB = new IndexBufferOGL();
      if (*ppIB)
         (*ppIB)->Initialize(size);
   }

   void CreateVertexBuffer(IVertexBuffer** ppVB, unsigned int size, bool bUseOneStream)
   {
      VertexBufferOGL* pVB=0;
      *ppVB = new VertexBufferOGL();
      (*ppVB)->Initialize(size);
   }

   void DestroyIndexBuffer(IIndexBuffer* pIB)
   {
      glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
      pIB->FreeResources();
      delete pIB;
   }

   void DestroyVertexBuffer(IVertexBuffer* pVB)
   {
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_NORMAL_ARRAY);
      glDisableClientState(GL_COLOR_ARRAY);
      glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      pVB->FreeResources();
      delete pVB;
   }*/

   void ClearVertexBufferStates()
   {
      if (!GLEW_ARB_vertex_buffer_object)
      {
         return;
      }

      for (unsigned int i=0;i<MAX_VERTEX_ATTRIB;i++)
      {
         switch(i)
         { 
         case Vertex:
            glDisableClientState(GL_VERTEX_ARRAY);
            break;
         case Normal:
            glDisableClientState(GL_NORMAL_ARRAY);
            break;
         case PrimaryColor:
            glDisableClientState(GL_COLOR_ARRAY);
            break;
         case SecondaryColor:
            glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
            break;
         case TexCoord0:
            glClientActiveTextureARB(GL_TEXTURE0_ARB);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            break;         
         }
      }
   }

   void ClearClientStates(IVertexBuffer* pVB)
   {
      if (!GLEW_ARB_vertex_buffer_object)
      {
         return;
      }

      VertexBufferOGL* pGLVB = (VertexBufferOGL*) pVB;
      if (pGLVB)
      {
         for (unsigned int i=0;i<MAX_VERTEX_ATTRIB;i++)
         {
            switch(i)
            { 
            case Vertex:
               glDisableClientState(GL_VERTEX_ARRAY);
               break;
            case Normal:
                glDisableClientState(GL_NORMAL_ARRAY);
               break;
            case PrimaryColor:
               glDisableClientState(GL_COLOR_ARRAY);
              break;
            case SecondaryColor:
                glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
               break;
            case TexCoord0:
               glClientActiveTextureARB(GL_TEXTURE0_ARB);
               glDisableClientState(GL_TEXTURE_COORD_ARRAY);
               break;         
            }
         }
      }
   }


   // Drawing Buffered objects
   void SetVertexBuffer(IVertexBuffer* pVB, bool ClearLastState)
   {
      if (ClearLastState)
         ClearClientStates(pVB);

      GLuint nName;
      int nComponents;
      GLenum type_gl;
      int  type_i;
      bool bHasVBO;
#ifdef DISABLE_VBO
      bHasVBO = false;
#else
      if (GLEW_ARB_vertex_buffer_object) 
         bHasVBO = true; 
      else 
         bHasVBO = false;
#endif

      VertexBufferOGL* pGLVB = (VertexBufferOGL*) pVB;
      if (pGLVB)
      {
         
         for (unsigned int i=0;i<MAX_VERTEX_ATTRIB;i++)
         {
            if (pGLVB->GetBuffer(i) && pGLVB->GetBuffer(i)->CheckMem())    
            {  
               nComponents = pGLVB->GetBuffer(i)->GetNumComponents();
               type_i = pGLVB->GetBuffer(i)->GetAttributeDataType();
               if (type_i == 1) type_gl = GL_FLOAT;
               if (type_i == 2)
               {
                  type_gl = GL_UNSIGNED_BYTE;  // 32 bit type
                  nComponents = 4;             // Some OpenGL implementations only work with 4 bytes alignment 
               }
               switch(i)
               { 
               case Vertex: 
                  if (bHasVBO)
                  {
                     nName = pGLVB->GetBuffer(i)->GetGLName(Vertex);
                     glBindBufferARB(GL_ARRAY_BUFFER_ARB, nName);
                     glEnableClientState(GL_VERTEX_ARRAY);
                     glVertexPointer(nComponents, type_gl, 0, 0); 
                  }
                  else
                  {
                     glEnableClientState(GL_VERTEX_ARRAY);
                     glVertexPointer( nComponents, type_gl, 0, pGLVB->GetBuffer(i)->GetDataPtr());
                  }
                  break;
               case Normal:
                  if (bHasVBO)
                  {
                     nName = pGLVB->GetBuffer(i)->GetGLName(Normal);
                     glBindBufferARB(GL_ARRAY_BUFFER_ARB, nName);
                     glEnableClientState(GL_NORMAL_ARRAY);
                     glNormalPointer(type_gl, 0, 0);
                  }
                  else
                  {
                     glEnableClientState(GL_NORMAL_ARRAY);
                     glNormalPointer(type_gl, 0, pGLVB->GetBuffer(i)->GetDataPtr());
                  }
                  break;
               case PrimaryColor:
                  if (bHasVBO)
                  {
                     nName = pGLVB->GetBuffer(i)->GetGLName(PrimaryColor);
                     glBindBufferARB(GL_ARRAY_BUFFER_ARB, nName);
                     glEnableClientState(GL_COLOR_ARRAY);
                     glColorPointer(nComponents, type_gl, 0, 0);
                  }
                  else
                  {
                     glEnableClientState(GL_COLOR_ARRAY);
                     glColorPointer(nComponents, type_gl, 0, pGLVB->GetBuffer(i)->GetDataPtr());
                  }
                  break;
               case SecondaryColor:
                  if (bHasVBO)
                  {
                     nName = pGLVB->GetBuffer(i)->GetGLName(SecondaryColor);
                     glBindBufferARB(GL_ARRAY_BUFFER_ARB,nName);
                     glEnableClientState(GL_SECONDARY_COLOR_ARRAY);
                     glSecondaryColorPointer(nComponents, type_gl, 0, 0);
                  }
                  else
                  {
                     glEnableClientState(GL_SECONDARY_COLOR_ARRAY);
                     glSecondaryColorPointer(nComponents, type_gl, 0, pGLVB->GetBuffer(i)->GetDataPtr());
                  }
                  break;
               case TexCoord0:
                  if (bHasVBO)
                  {
                     nName = pGLVB->GetBuffer(i)->GetGLName(TexCoord0);
                     glClientActiveTextureARB(GL_TEXTURE0_ARB);
                     glBindBufferARB(GL_ARRAY_BUFFER_ARB, nName);
                     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                     glTexCoordPointer(nComponents, type_gl, 0, 0);
                  }
                  else
                  {
                     glClientActiveTextureARB(GL_TEXTURE0_ARB);
                     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                     glTexCoordPointer(nComponents, type_gl, 0, pGLVB->GetBuffer(i)->GetDataPtr());
                  }
                  break;
               case TexCoord1:
                  if (bHasVBO)
                  {
                     nName = pGLVB->GetBuffer(i)->GetGLName(TexCoord1);
                     glClientActiveTextureARB(GL_TEXTURE1_ARB);
                     glBindBufferARB(GL_ARRAY_BUFFER_ARB, nName);
                     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                     glTexCoordPointer(nComponents, type_gl, 0, 0);
                  }
                  else
                  {
                     glClientActiveTextureARB(GL_TEXTURE1_ARB);
                     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                     glTexCoordPointer(nComponents, type_gl, 0, pGLVB->GetBuffer(i)->GetDataPtr());
                  }
                  break;
               case TexCoord2:
                  if (bHasVBO)
                  {
                     nName = pGLVB->GetBuffer(i)->GetGLName(TexCoord2);
                     glClientActiveTextureARB(GL_TEXTURE2_ARB);
                     glBindBufferARB(GL_ARRAY_BUFFER_ARB, nName);
                     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                     glTexCoordPointer(nComponents, type_gl, 0, 0);
                  }
                  else
                  {
                     glClientActiveTextureARB(GL_TEXTURE2_ARB);
                     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                     glTexCoordPointer(nComponents, type_gl, 0, pGLVB->GetBuffer(i)->GetDataPtr());
                  }
                  break;
               case TexCoord3:
                  if (bHasVBO)
                  {
                     nName = pGLVB->GetBuffer(i)->GetGLName(TexCoord3);
                     glClientActiveTextureARB(GL_TEXTURE3_ARB);
                     glBindBufferARB(GL_ARRAY_BUFFER_ARB, nName);
                     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                     glTexCoordPointer(nComponents, type_gl, 0, 0);
                  }
                  else
                  {
                     glClientActiveTextureARB(GL_TEXTURE3_ARB);
                     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                     glTexCoordPointer(nComponents, type_gl, 0, pGLVB->GetBuffer(i)->GetDataPtr());
                  }
                  break;
               case TexCoord4:
                  if (bHasVBO)
                  {
                     nName = pGLVB->GetBuffer(i)->GetGLName(TexCoord4);
                     glClientActiveTextureARB(GL_TEXTURE4_ARB);
                     glBindBufferARB(GL_ARRAY_BUFFER_ARB, nName);
                     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                     glTexCoordPointer(nComponents, type_gl, 0, 0);
                  }
                  else
                  {
                     glClientActiveTextureARB(GL_TEXTURE4_ARB);
                     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                     glTexCoordPointer(nComponents, type_gl, 0, pGLVB->GetBuffer(i)->GetDataPtr());
                  }
                  break;
               case TexCoord5:
                  if (bHasVBO)
                  {
                     nName = pGLVB->GetBuffer(i)->GetGLName(TexCoord5);
                     glClientActiveTextureARB(GL_TEXTURE5_ARB);
                     glBindBufferARB(GL_ARRAY_BUFFER_ARB, nName);
                     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                     glTexCoordPointer(nComponents, type_gl, 0, 0);
                  }
                  else
                  {
                     glClientActiveTextureARB(GL_TEXTURE5_ARB);
                     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                     glTexCoordPointer(nComponents, type_gl, 0, pGLVB->GetBuffer(i)->GetDataPtr());
                  }
                  break;
               case TexCoord6: 
                  if (bHasVBO)
                  {
                     nName = pGLVB->GetBuffer(i)->GetGLName(TexCoord6);
                     glClientActiveTextureARB(GL_TEXTURE6_ARB);
                     glBindBufferARB(GL_ARRAY_BUFFER_ARB, nName);
                     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                     glTexCoordPointer(nComponents, type_gl, 0, 0);
                  }
                  else
                  {
                     glClientActiveTextureARB(GL_TEXTURE6_ARB);
                     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                     glTexCoordPointer(nComponents, type_gl, 0, pGLVB->GetBuffer(i)->GetDataPtr());
                  }
                  break;
               case TexCoord7:
                  if (bHasVBO)
                  {
                     nName = pGLVB->GetBuffer(i)->GetGLName(TexCoord7);
                     glClientActiveTextureARB(GL_TEXTURE7_ARB);
                     glBindBufferARB(GL_ARRAY_BUFFER_ARB, nName);
                     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                     glTexCoordPointer(nComponents, type_gl, 0, 0);
                  }
                  else
                  {
                     glClientActiveTextureARB(GL_TEXTURE7_ARB);
                     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                     glTexCoordPointer(nComponents, type_gl, 0, pGLVB->GetBuffer(i)->GetDataPtr());
                  }
                  break;
               }
            }
         }
      }
   }
   
   void DrawIndexedPrimitive(unsigned int nPolyType, IIndexBuffer* pIB)
   {
      GLuint nStart;
      GLuint nEnd;
      GLuint buffername;
      GLenum GLpolytype;

      if (pIB)
      {
         if (pIB->GetSize()==0)
         {
            assert(false); // empty index buffers are currently considered invalid...
            return;
         }

         bool bVBO = false;
#ifdef DISABLE_VBO
         bVBO = false;
#else
         if (GLEW_ARB_vertex_buffer_object)
            bVBO = true;
#endif

         if (bVBO)
         {
               buffername = ((IndexBufferOGL*)pIB)->GetGLName();

               assert(glIsBuffer(buffername));

               // index
               glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, buffername);

               // draw
               GLpolytype = nPolyType;
               nStart = 0;
               nEnd = ((IndexBufferOGL*)pIB)->GetMaxIndexValue();

               glDrawRangeElements(GLpolytype, nStart, nEnd, pIB->GetSize(), GL_UNSIGNED_INT, 0);

         }
         else
         {
            GLpolytype = nPolyType;
            glDrawElements(GLpolytype, pIB->GetSize(), GL_UNSIGNED_INT, pIB->GetIndicesPtr()); 
         }

         switch (nPolyType)
         {
         case GL_TRIANGLES:
            _numTriangles += pIB->GetSize() / 3;
            break;
         case GL_QUADS:
            _numTriangles += pIB->GetSize() / 4 * 2;
            break;
         case GL_TRIANGLE_STRIP:
            _numTriangles += pIB->GetSize()-2;
            break;
         }
      }
   }

   void DrawPrimitive(unsigned int nPolyType, IVertexBuffer* pVB)
   {
      SetVertexBuffer(pVB);

      bool bVBO = false;
#ifdef DISABLE_VBO
      bVBO = false;
#else
      if (GLEW_ARB_vertex_buffer_object)
         bVBO = true;
#endif
      if (bVBO)
      {
         // draw
         GLenum GLpolytype = nPolyType;
         glDrawArrays(GLpolytype, 0, pVB->GetSize());
      }
      else
      {
         GLenum GLpolytype = nPolyType;
         glDrawArrays(GLpolytype, 0, pVB->GetSize());	
      }

      switch (nPolyType)
      {
      case GL_TRIANGLES:
         _numTriangles += pVB->GetSize() / 3;
         break;
      case GL_QUADS:
         _numTriangles += pVB->GetSize() / 4 * 2;
         break;
      case GL_TRIANGLE_STRIP:
         _numTriangles += pVB->GetSize()-2;
         break;
      }
   }
//}
#pragma endregion // Creating, Destroying Vertex/Index/Attribute Buffers

#pragma region VertexAttributeBufferOGL

//-----------------------------------------------------------------------------

VertexAttributeBufferOGL::VertexAttributeBufferOGL()
{
#ifdef DISABLE_VBO
   _bVBO = false;
#else
   if (GLEW_ARB_vertex_buffer_object) 
      _bVBO = true; 
   else 
      _bVBO = false;
#endif
}

//-----------------------------------------------------------------------------

int VertexAttributeBufferOGL::Create(unsigned int Size, EVertexAttribute nVertexAttribute, EVertexDataType edecltype)
{
   _declt = edecltype;
   _nVertexAttribute = nVertexAttribute;
   _size = Size;
   _mem = false;  // no engine specific memory reserved yet

   _pVertexData = (void*) malloc(_size * GetChannelSizeInBytes());
   if (_pVertexData == 0) return 1; // out of memory!

   return 0;
}

//-----------------------------------------------------------------------------

void VertexAttributeBufferOGL::Release(void)
{
   if (_pVertexData)
   {
      if (_mem) 
      { 
         if (_bVBO)
         {
            glDeleteBuffersARB(1, &_nStreamName);
            _mem = false;
         }
      }
      free(_pVertexData);
      _pVertexData = 0;
   }
}

//-----------------------------------------------------------------------------

void* VertexAttributeBufferOGL::GetDataPtr(void)
{
   return (void*) _pVertexData;
}

//-----------------------------------------------------------------------------

void  VertexAttributeBufferOGL::SetValue(unsigned int index, float a, float b, float c, float d)
{
   assert(index < _size);
   unsigned int nComponents = GetNumComponents();
   float* pData = (float*) _pVertexData;

   if (nComponents>0) pData[index*nComponents+0] = a;
   if (nComponents>1) pData[index*nComponents+1] = b;
   if (nComponents>2) pData[index*nComponents+2] = c;
   if (nComponents>3) pData[index*nComponents+3] = d;
}

//-----------------------------------------------------------------------------

void  VertexAttributeBufferOGL::SetPackedValue(unsigned int index, unsigned int argb)
{
   // Byte Order is ARGB but OpenGL requires ABGR
   assert(index < _size);
   unsigned char r,g,b,a;

   a = (unsigned char)((argb >> 24) & 0xff);
   r = (unsigned char)((argb >> 16) & 0xff);
   g = (unsigned char)((argb >> 8) & 0xff);
   b = (unsigned char)((argb & 0xff));
   unsigned int rgba = ((a & 0xff) << 24) | ((b & 0xff) << 16) | ((g & 0xff) << 8) | (r & 0xff);

   ((unsigned int*)_pVertexData)[index] = rgba;//rgba;
}

//-----------------------------------------------------------------------------

unsigned int VertexAttributeBufferOGL::GetPackedValue(unsigned int index)
{
   assert(index < _size);
   unsigned char r,g,b,a;

   unsigned int rgba = ((unsigned int*)_pVertexData)[index];
   a = (unsigned char)((rgba >> 24) & 0xff);
   b = (unsigned char)((rgba >> 16) & 0xff);
   g = (unsigned char)((rgba >> 8) & 0xff);
   r = (unsigned char)((rgba >> 0) & 0xff);

   rgba = ((a & 0xff) << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);

   return rgba;
}

//-----------------------------------------------------------------------------

unsigned int VertexAttributeBufferOGL::GetChannelSizeInBytes()
{
   switch(_declt)
   {
   case float1:
      return sizeof(float);
   case float2:
      return 2*sizeof(float);
   case float3:
      return 3*sizeof(float);
   case float4:
      return 4*sizeof(float);
   case pcolor:
   case ubyte4:
   case ubyte4_normalized:
      return sizeof(unsigned int);
   default:
      assert(false);
   }

   return 0;
}

//-----------------------------------------------------------------------------

unsigned int VertexAttributeBufferOGL::GetNumComponents()
{
   switch(_declt)
   {
   case float1:
      return 1;
   case float2:
      return 2;
   case float3:
      return 3;
   case float4:
      return 4;
   case pcolor:
   case ubyte4:
   case ubyte4_normalized:
      return 1;
   default:
      assert(false);
   }

   return 0;
}

//-----------------------------------------------------------------------------

int VertexAttributeBufferOGL::GetAttributeDataType()
{
   switch(_declt)
   {
   case float1:
   case float2:
   case float3:
   case float4:
      return 1;
   case pcolor:
   case ubyte4:
   case ubyte4_normalized:
      return 2;
   }

   return 0;
}

//-----------------------------------------------------------------------------

unsigned int VertexAttributeBufferOGL::GetSize(void)
{
   return _size;
}

//-----------------------------------------------------------------------------

unsigned int VertexAttributeBufferOGL::CreateVRAMObject(void)
{
   if (_mem) 
   { 
      //assert(false);
      if (_bVBO)
      {  
         glDeleteBuffersARB(1, &_nStreamName);
         _mem = false;
      }
   }

   if (_bVBO)
   {  
      glGenBuffersARB(1, &_nStreamName);
      _mem = true;
   }
   else
   {
      _mem = true;
   }

   return 0;
}

//-----------------------------------------------------------------------------

unsigned int VertexAttributeBufferOGL::CopyToVRAM(bool bDynamic)
{
   assert(_mem);
   if ((_pVertexData != 0) && _mem)
   {
      if (_bVBO)
      {
         GLenum usage;
         if (bDynamic)
            usage = GL_DYNAMIC_DRAW_ARB;
         else
            usage = GL_STATIC_DRAW_ARB;

         glBindBufferARB(GL_ARRAY_BUFFER_ARB, _nStreamName);
         glBufferDataARB(GL_ARRAY_BUFFER_ARB, _size*GetChannelSizeInBytes(),  (GLfloat*) _pVertexData, usage); 
         glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
      }
   }   

   return 0;
}

//-----------------------------------------------------------------------------

unsigned int VertexAttributeBufferOGL::StreamToVRAM()
{
   assert(_mem);
   if ((_pVertexData != 0) && _mem)
   {
      if (_bVBO)
      {
         glBindBufferARB(GL_ARRAY_BUFFER_ARB, _nStreamName);
         glBufferDataARB(GL_ARRAY_BUFFER_ARB, _size*GetChannelSizeInBytes(),  (GLfloat*) _pVertexData, GL_STREAM_DRAW_ARB); 
         glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
      }
   }   

   return 0;
}

//-----------------------------------------------------------------------------

#pragma endregion // Vertex Attribute Buffer implementation for OpenGL

#pragma region VertexBufferOGL

VertexBufferOGL::VertexBufferOGL()
{
#ifdef DISABLE_VBO
   _bVBO = false;
#else
   if (GLEW_ARB_vertex_buffer_object) 
      _bVBO = true; 
   else _bVBO = false; 
#endif
   _numBuffers++;
}

VertexBufferOGL::~VertexBufferOGL()
{
   _Destroy();
   _numBuffers--;
}

//-----------------------------------------------------------------------------

void VertexBufferOGL::_Destroy()
{
   if (_bVBO)
   {
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
   }
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   FreeResources();
}

//-----------------------------------------------------------------------------

void VertexBufferOGL::Initialize(unsigned int size)
{
   _size = size;

   /*for (unsigned int i=0;i<MAX_VERTEX_ATTRIB;i++)
   {
      _buffers[i].reset();
   }*/
}


//-----------------------------------------------------------------------------

void VertexBufferOGL::FreeResources()
{
   for (int i=0;i<MAX_VERTEX_ATTRIB;i++)
   {
      if (_buffers[i])
      {
         _buffers[i]->Release();
         _buffers[i].reset();
      }
   }
}

//-----------------------------------------------------------------------------

void VertexBufferOGL::AddVertexAttributeBuffer(boost::shared_ptr<IVertexAttributeBuffer> pVertexAttributeBuffer)
{
   if (!pVertexAttributeBuffer) return;

   // Add by type
   unsigned int nSlot = (unsigned int) pVertexAttributeBuffer->GetAttributeType();

   _buffers[nSlot] = pVertexAttributeBuffer;
}

//-----------------------------------------------------------------------------

IVertexAttributeBuffer* VertexBufferOGL::GetVertexAttributeBuffer(EVertexAttribute eAttribute)
{
   return _buffers[(unsigned int)eAttribute].get();
}


//-----------------------------------------------------------------------------

void VertexBufferOGL::Flush()
{

}

//-----------------------------------------------------------------------------

unsigned int VertexBufferOGL::CreateVRAMObject()
{
   for (int i=0;i<MAX_VERTEX_ATTRIB;i++)
   {
      if (_buffers[i]!=0)
      {
         _buffers[i]->CreateVRAMObject();
      }
   }
   return 0;
}

//-----------------------------------------------------------------------------

unsigned int VertexBufferOGL::CopyToVRAM(bool bDynamic)
{
   for (int i=0;i<MAX_VERTEX_ATTRIB;i++)
   {
      if (_buffers[i]!=0)
      {
         _buffers[i]->CopyToVRAM(bDynamic);
      }
   }
   return 0;
}

//-----------------------------------------------------------------------------

unsigned int VertexBufferOGL::StreamToVRAM()
{
   for (int i=0;i<MAX_VERTEX_ATTRIB;i++)
   {
      if (_buffers[i]!=0)
      {
         _buffers[i]->StreamToVRAM();
      }
   }
   return 0;
}

//-----------------------------------------------------------------------------

#pragma endregion // Vertex Buffer implementation for OpenGL

#pragma region IndexBufferOGL
//-----------------------------------------------------------------------------

IndexBufferOGL::IndexBufferOGL() 
{
   _numBuffers++;
   _size = 0;
   _pIndexData = 0;
   _pFaceIdData = 0;
#ifdef DISABLE_VBO
   _bVBO = false;
#else
   if (GLEW_ARB_vertex_buffer_object) 
      _bVBO = true; 
   else 
      _bVBO = false;
#endif
}

//-----------------------------------------------------------------------------

IndexBufferOGL::~IndexBufferOGL()
{  
   _Destroy();
   _numBuffers--;
}

void IndexBufferOGL::_Destroy()
{
   if (_bVBO)
   {
      glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
   }
   FreeResources();
}

//-----------------------------------------------------------------------------

unsigned int  IndexBufferOGL::CreateIndexBuffer(void)
{
   if (_size == 0) return 2; // error: buffer not initialized

   _pIndexData = (unsigned int*) malloc(_size * sizeof(unsigned int));
   if (_pIndexData == 0) return 1; // error: out of memory

   _pFaceIdData = (int*) malloc(_size * sizeof(int));
   if (_pFaceIdData == 0) return 1; // error: out of memory

   return 0;
}

//-----------------------------------------------------------------------------

unsigned int* IndexBufferOGL::GetIndicesPtr(void)
{
   return _pIndexData;
}

int* IndexBufferOGL::GetFaceIdPtr(void)
{
   return _pFaceIdData;
}

//-----------------------------------------------------------------------------

void IndexBufferOGL::SetValue(unsigned int index, unsigned int value)
{
   if (_size == 0) return;

   if (_pIndexData)  _pIndexData[index] = value;
   if (_pFaceIdData) _pFaceIdData[index] = -1;
}

//-----------------------------------------------------------------------------

void IndexBufferOGL::SetValue(unsigned int index, unsigned int value, int faceId)
{
   if (_size == 0) return;
   if (_pIndexData) _pIndexData[index] = value;
   if (_pFaceIdData) _pFaceIdData[index] = faceId;
}

//-----------------------------------------------------------------------------

void IndexBufferOGL::Initialize(unsigned int size)
{
   _size = size;
   _glMem = false;
   _nMaxIndexValue = 0;
   CreateIndexBuffer();
}

//-----------------------------------------------------------------------------
#define iMax(x,y) ((x)<(y)?(y):(x))

unsigned int IndexBufferOGL::CopyToVRAM(bool bDynamic)
{   
   if(_glMem)
   {
      if (_bVBO)
      {
         GLenum usage;
         if (bDynamic)
            usage = GL_DYNAMIC_DRAW_ARB;
         else
            usage = GL_STATIC_DRAW_ARB;

         glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, nIndexName);
         glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _size*sizeof(GLuint),  (GLuint*)_pIndexData, usage);
         glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

         if (_size)
         {
            _nMaxIndexValue = ((GLuint*)_pIndexData)[0];
            for (unsigned int i = 1; i < _size; i++)
            {
               _nMaxIndexValue = iMax(_nMaxIndexValue, ((GLuint*)_pIndexData)[i]);
            }
         }

      }
   }


   return 0;
}

//-----------------------------------------------------------------------------

unsigned int IndexBufferOGL::StreamToVRAM()
{   
   if(_glMem)
   {
      if (_bVBO)
      {
         glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, nIndexName);
         glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _size*sizeof(GLuint),  (GLuint*)_pIndexData, GL_STREAM_DRAW_ARB);
         glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

         if (_size)
         {
            _nMaxIndexValue = ((GLuint*)_pIndexData)[0];
            for (unsigned int i = 1; i < _size; i++)
            {
               _nMaxIndexValue = iMax(_nMaxIndexValue, ((GLuint*)_pIndexData)[i]);
            }
         }
      }
   }

   return 0;
}

//-----------------------------------------------------------------------------

unsigned int IndexBufferOGL::CreateVRAMObject(void)
{
   if (_bVBO)
   {
      if (_glMem) 
      { 
         glDeleteBuffersARB(1, &nIndexName); 
      }

      glGenBuffersARB(1, &nIndexName);  
      _glMem = true;
   }
   else
   {
      _glMem = false;
   }
   return 0;
}

//--------------------------------------------- --------------------------------

void IndexBufferOGL::FreeResources()
{ 
   if (_bVBO)
   {
      if (_glMem) 
      { 
         glDeleteBuffersARB(1, &nIndexName); 
         _glMem = false; 
      }
   }

   if (_pIndexData) 
   {
      free(_pIndexData);
      _pIndexData = 0;
   }

   if (_pFaceIdData)
   {
      free(_pFaceIdData);
      _pFaceIdData = 0;
   }
}

#pragma endregion // Index Buffer implementation for OpenGL



