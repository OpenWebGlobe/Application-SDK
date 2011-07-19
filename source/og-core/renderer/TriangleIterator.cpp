/*******************************************************************************
Project       : i3D Studio  
Purpose       : Triangle Iterator
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#include "renderer/TriangleIterator.h"
#include <GL/glew.h>


TriangleIterator::TriangleIterator()
{
   _nQueryType = -1; // NO ACTIVE QUERY
}

TriangleIterator::~TriangleIterator()
{
}

void TriangleIterator::QueryAll(int nPolyType, boost::shared_ptr<IVertexBuffer> pVB, boost::shared_ptr<IIndexBuffer> pIB)
{
   _nQueryType = 0;
   _nCurrent = 0;
   _nPolyType = nPolyType;
   _pVB = pVB;
   _pIB = pIB;
   _vOffset.x = _vOffset.y = _vOffset.z = 0;

   // currently, only indexed primitives are supported!
   if (pIB == 0)
   {
      assert(false);
      return;
   }
}

//-----------------------------------------------------------------------------

void TriangleIterator::QueryAllHighPrecision(int nPolyType, boost::shared_ptr<IVertexBuffer> pVB, boost::shared_ptr<IIndexBuffer> pIB, const vec3<double>& vOffset)
{
   _nQueryType = 0;
   _nCurrent = 0;
   _nPolyType = nPolyType;
   _pVB = pVB;
   _pIB = pIB;
   _vOffset = vOffset;

   // currently, only indexed primitives are supported!
   if (pIB == 0)
   {
      assert(false);
      return;
   }
}

//-----------------------------------------------------------------------------

bool TriangleIterator::GetNextTriangle(boost::shared_ptr<TriangleResult>& pResult)
{
   pResult = boost::shared_ptr<TriangleResult>(new TriangleResult());

   if (_pVB == 0)
      return false;

   int numTri = 0;
   int vertex1, vertex2, vertex3;
   unsigned int* pIdx;
   int*          pFaceId;
   int nFaceId = -1;

   // Currently, only Triangles and Triangle Strips are supported
   switch (_nPolyType)
   {
   case GL_TRIANGLES:
      pResult->bIsDegenerate = false;
      // number of triangles = index buffer size!
      numTri = _pIB->GetSize() / 3;
      assert(numTri >= 1 /*&& (numTri%3) == 0*/);
      if (_nCurrent>numTri-1)
         return false;   // no more triangles!!
      pIdx = _pIB->GetIndicesPtr();
      pFaceId  = _pIB->GetFaceIdPtr();
      //-----------------------------------------------------------------------
      vertex1 = pIdx[_nCurrent*3]; vertex2 = pIdx[_nCurrent*3+1]; vertex3 = pIdx[_nCurrent*3+2];
      nFaceId = pFaceId[_nCurrent*3];
      //-----------------------------------------------------------------------
      _nCurrent = _nCurrent + 1;
      break;

   case GL_TRIANGLE_STRIP:
      // number of triangles = index buffer - 2
      numTri = _pIB->GetSize()-2;
      assert(numTri >= 3);

      if (_nCurrent+2>_pIB->GetSize()-1)
         return false;   // no more triangles!!

      pIdx = _pIB->GetIndicesPtr();
      pFaceId  = _pIB->GetFaceIdPtr();

      vertex1 = vertex2 = vertex3 = -1;
      //-----------------------------------------------------------------------
      //while ((vertex1==vertex2 || vertex2==vertex3 || vertex1==vertex3) && _nCurrent<numTri-1)
      {
         vertex1 = pIdx[_nCurrent]; vertex2 = pIdx[_nCurrent+1]; vertex3 = pIdx[_nCurrent+2];
         nFaceId = pFaceId[_nCurrent];
         if (_nCurrent%2 == 1)
            std::swap(vertex2, vertex3);
         _nCurrent++;

         if (vertex1==vertex2 || vertex2==vertex3 || vertex1==vertex3)
         {
            //bool bDegenerate = true;
            //std::cout << "** DEGENERATE TRIANGLE **\n";
            pResult->bIsDegenerate = true;
         }
         else
         {
             pResult->bIsDegenerate = false;
         }
      }
      break;

   default:
      assert(false); // primitive is NOT supported!
      return false;
   }

   //
   // Get {vertex1,vertex2,vertex3} from vertex buffer
   // and store in result   

   IVertexAttributeBuffer* AttributePosition = _pVB->GetVertexAttributeBuffer(Vertex);

   // Set Face ID
   pResult->nFaceId = nFaceId;

   if (AttributePosition)
   {
      if (AttributePosition->GetNumComponents() == 3)
      {
         pResult->A._bHasPosition = true;
         pResult->B._bHasPosition = true;
         pResult->C._bHasPosition = true;

         // Store Original Index Number in vertex (for future reference)
         pResult->A._nIndex = vertex1;
         pResult->B._nIndex = vertex2;
         pResult->C._nIndex = vertex3;

         float* data = (float*)AttributePosition->GetDataPtr();
         
         size_t adr = 3 * vertex1;
         pResult->A._pos.x = double(data[adr]) + _vOffset.x;
         pResult->A._pos.y = double(data[adr+1]) + _vOffset.y;
         pResult->A._pos.z = double(data[adr+2]) + _vOffset.z;

         adr = 3* vertex2;
         pResult->B._pos.x = double(data[adr]) + _vOffset.x;
         pResult->B._pos.y = double(data[adr+1]) + _vOffset.y;
         pResult->B._pos.z = double(data[adr+2]) + _vOffset.z;

         adr = 3* vertex3;
         pResult->C._pos.x = double(data[adr]) + _vOffset.x;
         pResult->C._pos.y = double(data[adr+1]) + _vOffset.y;
         pResult->C._pos.z = double(data[adr+2]) + _vOffset.z;
      }  
   }

   IVertexAttributeBuffer* AttributeTexCoord = _pVB->GetVertexAttributeBuffer(TexCoord0);
   if (AttributeTexCoord)
   {
      if (AttributeTexCoord->GetNumComponents() == 2)
      {
         pResult->A._bHasTexCoord = true;
         pResult->B._bHasTexCoord = true;
         pResult->C._bHasTexCoord = true;

         float* data = (float*)AttributeTexCoord->GetDataPtr();

         size_t adr = 2 * vertex1;
         pResult->A._texcoord.x = double(data[adr]);
         pResult->A._texcoord.y = double(data[adr+1]);
        

         adr = 2* vertex2;
         pResult->B._texcoord.x = double(data[adr]);
         pResult->B._texcoord.y = double(data[adr+1]);
        

         adr = 2* vertex3;
         pResult->C._texcoord.x = double(data[adr]);
         pResult->C._texcoord.y = double(data[adr+1]);
      }  
   }

   bool bSurfaceNormals = false;

   IVertexAttributeBuffer* AttributeNormal = _pVB->GetVertexAttributeBuffer(Normal);
   if (AttributeNormal)
   {
      if (AttributeNormal->GetNumComponents() == 3)
      {
         pResult->A._bHasNormal = true;
         pResult->B._bHasNormal = true;
         pResult->C._bHasNormal = true;

         float* data = (float*)AttributeNormal->GetDataPtr();

         size_t adr = 3 * vertex1;
         pResult->A._norm.x = double(data[adr]);
         pResult->A._norm.y = double(data[adr+1]);
         pResult->A._norm.z = double(data[adr+2]);

         adr = 3* vertex2;
         pResult->B._norm.x = double(data[adr]);
         pResult->B._norm.y = double(data[adr+1]);
         pResult->B._norm.z = double(data[adr+2]);

         adr = 3* vertex3;
         pResult->C._norm.x = double(data[adr]);
         pResult->C._norm.y = double(data[adr+1]);
         pResult->C._norm.z = double(data[adr+2]);
      }
      else
      {
         bSurfaceNormals = true;
      }
   }
   else
   {
      bSurfaceNormals = true;
   }

   if (bSurfaceNormals) // there are no normals (or no valid normals) in this model, calculate surface normal!
   {
         vec3<double> u =  pResult->B._pos - pResult->A._pos;
         vec3<double> v =  pResult->C._pos - pResult->A._pos;

         vec3<double> vNormal = cross_dbl(u,v);
         vNormal = normalize_dbl(vNormal);

         pResult->A._bHasNormal = true;
         pResult->B._bHasNormal = true;
         pResult->C._bHasNormal = true;

         pResult->A._norm = vNormal;
         pResult->B._norm = vNormal;
         pResult->C._norm = vNormal;
   }

   return true;
}

//-----------------------------------------------------------------------------

i3dVertex::i3dVertex()
{
   _bHasPosition = false;
   _bHasNormal = false;
   _bHasTexCoord = false;
   _nIndex = 0;
}

i3dVertex::~i3dVertex()
{

}

bool i3dVertex::GetPosition(vec3<double>& pos)
{
   if (_bHasPosition)
   {
      pos = _pos;
   }

   return _bHasPosition;
}


bool i3dVertex::GetNormal(vec3<double>& normal)
{
   if (_bHasNormal)
   {
      normal = _norm;
   }

   return _bHasNormal;
}

bool i3dVertex::GetPosition_flt(vec3<float>& pos)
{
   if (_bHasPosition)
   {
      pos.x = (float)_pos.x;
      pos.y = (float)_pos.y;
      pos.z = (float)_pos.z;
   }

   return _bHasPosition;
}

bool i3dVertex::GetNormal_flt(vec3<float>& normal)
{
   if (_bHasNormal)
   {
      normal.x = (float)_norm.x;
      normal.y = (float)_norm.y;
      normal.z = (float)_norm.z;
   }

   return _bHasNormal;
}


bool  i3dVertex::GetTexCoord(vec2<double>& coord)
{
   if (_bHasTexCoord)
   {
      coord = _texcoord; 
   }

   return _bHasTexCoord;
}


bool  i3dVertex::GetTexCoord_flt(vec2<float>& coord)
{
   if (_bHasTexCoord)
   {
      coord.x = (float)_texcoord.x;
      coord.y = (float)_texcoord.y; 
   }

   return _bHasTexCoord;
}