/*******************************************************************************
Project       : i3D Studio  
Purpose       : Triangle Iterator
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef I3D_TRIANGLEITERATOR_H
#define I3D_TRIANGLEITERATOR_H

#include "og.h"
#include <math/vec3.h>
#include <math/vec2.h>
#include <math/mat4.h>
#include <math/ray.h>
#include <boost/shared_ptr.hpp>

#include "renderer/IVertexBuffer.h"
#include "renderer/IIndexBuffer.h"
#include "renderer/IVertexAttributeBuffer.h"

class TriangleIterator;
class i3dVertex;
class TriangleResult;

//-----------------------------------------------------------------------------

class OPENGLOBE_API i3dVertex
{
public:
   i3dVertex();
   virtual ~i3dVertex();

   // Easier Interface: (returns true if data is available)
   bool        GetPosition(vec3<double>& pos);
   bool        GetNormal(vec3<double>& normal);
   /*bool        GetPrimaryColor(unsigned int& col);
   bool        GetSecondaryColor(unsigned int& col);*/
   bool        GetTexCoord(vec2<double>& coord);


   bool        GetPosition_flt(vec3<float>& pos);
   bool        GetNormal_flt(vec3<float>& normal);
   bool        GetTexCoord_flt(vec2<float>& coord);

   // \brief Retrieve original index.
   int         GetIndex(){return _nIndex;}

protected:
   friend class TriangleIterator;
   friend class TriangleResult;

   bool         _bHasPosition;
   bool         _bHasNormal;
   bool         _bHasTexCoord;
   vec3<double> _pos;
   vec3<double> _norm;
   vec2<double> _texcoord;
   int          _nIndex;   //    Original Index
};

//-----------------------------------------------------------------------------

class OPENGLOBE_API TriangleResult
{
public:

   bool bBufferEnabled[MAX_VERTEX_ATTRIB];
   
   i3dVertex    A;
   i3dVertex    B;
   i3dVertex    C;

   bool         bIsDegenerate;
   int          nFaceId;   // user defined face id (-1 is unset)
};

//-----------------------------------------------------------------------------

/*
              b
         A -------- C       if the triangle is part of
           \      /         a - for example - quad, then the edge flag is not
          c \    / a        set at the shared edge.
             \  /           It is even possible that no edge flags are set!
              \/
              B
*/

//-----------------------------------------------------------------------------

class OPENGLOBE_API TriangleIterator
{
public:
   TriangleIterator();
   virtual ~TriangleIterator();

   //! Query all triangles of the specified Buffer.
   void QueryAll(int nPolyType, boost::shared_ptr<IVertexBuffer> pVB, boost::shared_ptr<IIndexBuffer> pIB);

   //! Query all triangles but use specified double precision offset!
   void QueryAllHighPrecision(int nPolyType, boost::shared_ptr<IVertexBuffer> pVB, boost::shared_ptr<IIndexBuffer> pIB, const vec3<double>& vOffset);



   // 
   bool GetNextTriangle(boost::shared_ptr<TriangleResult>& pResult);


private:
   int   _nQueryType;
   int   _nCurrent;
   int   _nPolyType;
   int   _nLastindex;
   boost::shared_ptr<IVertexBuffer> _pVB;
   boost::shared_ptr<IIndexBuffer>  _pIB;
   vec3<double> _vOffset;

};

//-----------------------------------------------------------------------------

#endif