/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "geo/VirtualGlobeTerrainBlock.h"
#include "renderer/BufferFactory.h"
#include <math/mathutils.h>
#include "geo/MercatorTransformation.h"
#include "math/TextureTransformation.h"
#include "geo/VirtualGlobeCache.h"
//#include "VirtualGlobeRenderer.h"
#include "renderer/gl/BuffersOGL.h"
#include <iostream>

//-----------------------------------------------------------------------------

VirtualGlobeTerrainBlock::VirtualGlobeTerrainBlock(IRenderEngine* pEngine, const std::string& sQuadcode,  const boost::shared_ptr<MercatorQuadtree>& qQuadtree)
{
   _pEngine = pEngine;
   _qTexture = 0;
   _sQuadcode = sQuadcode;
   _qQuadtree = qQuadtree;

   _nLod = (int)_sQuadcode.length();

   _bReady = false;
   
   _nElevationBlockSize = 17;
   _nTextureSize        = 256;
   _nImageDataChannels  = 4;
   _bIsBlock = false;
   _bFinishMerge = false;
   _dCurtainElevation = 0.0;
   _curLayer = 0;
   _bBuffersCreated = false;
   CalcLocation();
}

//-----------------------------------------------------------------------------

VirtualGlobeTerrainBlock::~VirtualGlobeTerrainBlock()
{
   if (_qTexture)
   {
      _qTexture->Destroy();
   }
}

//-----------------------------------------------------------------------------
void VirtualGlobeTerrainBlock::_OnTileReady(const std::string& sQuadcode, ImageTile* pTile)
{
   if (!pTile || pTile->GetHeight() != pTile->GetWidth() || pTile->GetWidth() != 256)
   {
      assert(false);
      return;
   }

   _MergeImageData(pTile->GetRawData(), pTile->GetNumChannels(), pTile->GetWidth());
   delete pTile;

   if (IsMergeFinished())
   {
      SetReady();
      return;
   }

   _ContinueDownload();
}

//-----------------------------------------------------------------------------

void VirtualGlobeTerrainBlock::_ContinueDownload()
{
   size_t maximg = _lstImg.size();

   if (_curLayer>=(int)_lstImg.size()-1)
   {
      SetReady();
      return; //no more layers!
   }

   _curLayer++;
   for (size_t i=_curLayer;i<(int)_lstImg.size();i++)
   {
      _curLayer = i;
      IImageLayer* pLayer = _lstImg[i].get();

      // always interpolate last layer. We want to see data!
      if (_curLayer == maximg-1 || _nLod <= pLayer->GetMaxLod() /*&& _nLod>3*/)
      {
         pLayer->RequestTile(_sQuadcode, 
            boost::bind(&VirtualGlobeTerrainBlock::_OnTileReady, this, _1, _2), 
            boost::bind(&VirtualGlobeTerrainBlock::_OnTileFailed, this, _1));
         return;
      }
   }

   SetReady();
}

//-----------------------------------------------------------------------------

void VirtualGlobeTerrainBlock::_OnTileFailed(const std::string& sQuadcode)
{
   _ContinueDownload();
}

//-----------------------------------------------------------------------------

void VirtualGlobeTerrainBlock::Request(std::vector< boost::shared_ptr<IImageLayer> >&  lstImg, std::vector< boost::shared_ptr<IElevationLayer> >& lstElv)
{
   _lstImg = lstImg;
   _lstElv = lstElv; 

   // Request Elevation Tile
   const int tilesize = 9;
   boost::shared_array<float> data = boost::shared_array<float>(new float[tilesize*tilesize]);
   for (int i=0;i<tilesize*tilesize;i++) data[i] = 0;
   SetElevationDataBlock(data, tilesize);

   // Request Image Tile
   size_t maximg = lstImg.size();

   if (lstImg.size()>0)
   {
     
     for (size_t i=0;i<maximg;i++)
     {
         _curLayer = i;
         IImageLayer* pLayer = lstImg[i].get();

         // always interpolate last layer. We want to see data!
         if (_curLayer == maximg-1 || _nLod <= pLayer->GetMaxLod())
         {
            pLayer->RequestTile(_sQuadcode, 
            boost::bind(&VirtualGlobeTerrainBlock::_OnTileReady, this, _1, _2), 
            boost::bind(&VirtualGlobeTerrainBlock::_OnTileFailed, this, _1));
            return;
         }
     }
   }
}

//-----------------------------------------------------------------------------

bool  VirtualGlobeTerrainBlock::IsAvailable()
{  
   return _bReady;
}

//-----------------------------------------------------------------------------

void  VirtualGlobeTerrainBlock::SetReady()
{
   _bReady = true;
   _CreateBuffers();
}

//-----------------------------------------------------------------------------

void VirtualGlobeTerrainBlock::SetElevationData(ElevationTile& oElevationTile)
{
   _vOffset = oElevationTile.GetOffset();

   _lstElevationPointWGS84 = oElevationTile.GetPointsWGS84();
   _lstTexCoord = oElevationTile.GetTexCoord();      
   _lstPosIdx = oElevationTile.GetIndices();              

   _vCornerPoints = oElevationTile.GetCornerPoints();
   _vNorthPoints = oElevationTile.GetNorthPoints();
   _vEastPoints = oElevationTile.GetEastPoints();
   _vSouthPoints = oElevationTile.GetSouthPoints(); 
   _vWestPoints = oElevationTile.GetWestPoints();
   //_vMiddlePoints = oElevationTile.GetMiddlePoints(); // middle points are currently not of interest... at a later time (real time recreation) it will be required...

      _bIsBlock = false; // this tile is a TIN!
   //}
}

//-----------------------------------------------------------------------------


void VirtualGlobeTerrainBlock::SetElevationDataBlock(boost::shared_array<float>& data, unsigned int nBlocksize)
{
   _nElevationBlockSize = nBlocksize;
   _qElevationData = data;
   _bIsBlock = true;
}

//-----------------------------------------------------------------------------

void VirtualGlobeTerrainBlock::_MergeImageData(boost::shared_array<unsigned char>& data, unsigned int nChannels, unsigned int nTextureSize)
{
   _nImageDataChannels = 4;

   // Mixing different Texture sizes is currently unsupported and could be added here...
   _nTextureSize = nTextureSize;

   if (!_qImageData) // no image data yet!
   {
      _qImageData = boost::shared_array<unsigned char>(new unsigned char[_nTextureSize*_nTextureSize*4]);
      memset(_qImageData.get(), 0, _nTextureSize*_nTextureSize*4);
   }

   // if a RGB image is loaded, convert to RGBA and overwrite everything that is transparent
   if (nChannels == 3)
   {
      // convert to RGBA
      unsigned int adr_src, adr_dst;
      for (unsigned int y=0;y<_nTextureSize;y++)
      {
         for (unsigned int x=0;x<_nTextureSize;x++)
         {
            adr_dst = 4*y*_nTextureSize+4*x; // RGBA
            adr_src = 3*y*_nTextureSize+3*x; // RGB

            unsigned char curAlpha = _qImageData.get()[adr_dst+3];

            //if (curAlpha < 255)
            {
               unsigned char red, green, blue;
               red = data.get()[adr_src+0];
               green = data.get()[adr_src+1];
               blue = data.get()[adr_src+2];

               if (red == 0 && green == 0 && blue == 0)
               {

               }
               else
               {
                  double alpha = double(curAlpha) / 255.0; // 0...1
                  _qImageData.get()[adr_dst+0] = unsigned char(alpha*(double)_qImageData.get()[adr_dst+0] + (1.0-alpha)*(double)data.get()[adr_src+0]);
                  _qImageData.get()[adr_dst+1] = unsigned char(alpha*(double)_qImageData.get()[adr_dst+1] + (1.0-alpha)*(double)data.get()[adr_src+1]);
                  _qImageData.get()[adr_dst+2] = unsigned char(alpha*(double)_qImageData.get()[adr_dst+2] + (1.0-alpha)*(double)data.get()[adr_src+2]);
                  _qImageData.get()[adr_dst+3] = 255;
               }
            }
         }
      }

      _bFinishMerge = true;
   }
   else // channels = 4, _qImageData is set...
   {
      // Blend new data with old data using alpha value of new data...
      unsigned int adr_src, adr_dst;
      //unsigned char alpha;
      size_t alphacnt = _nTextureSize*_nTextureSize;
      for (unsigned int y=0;y<_nTextureSize;y++)
      {
         for (unsigned int x=0;x<_nTextureSize;x++)
         {
            adr_dst = 4*y*_nTextureSize+4*x; // RGBA
            adr_src = 4*y*_nTextureSize+4*x; // RGBA

            unsigned char curAlpha = _qImageData.get()[adr_dst+3];

            unsigned char red, green, blue, alpha;
            red = data.get()[adr_src+0];
            green = data.get()[adr_src+1];
            blue = data.get()[adr_src+2];
            alpha = data.get()[adr_src+3];

            /*if (curAlpha <5)
            {
               _qImageData.get()[adr_dst+0] = data.get()[adr_src+0];
               _qImageData.get()[adr_dst+1] = data.get()[adr_src+1];
               _qImageData.get()[adr_dst+2] = data.get()[adr_src+2];
               _qImageData.get()[adr_dst+3] = data.get()[adr_src+3]; 
            }*/
            //else //if (curAlpha < 255)
            {  
               if (red == 0 && green == 0 && blue == 0)
               {

               }
               else
               {
                  double talpha = _qImageData.get()[adr_src+3] / 255.0; // 0...1
                  _qImageData.get()[adr_dst+0] = unsigned char(talpha*(double)_qImageData.get()[adr_dst+0] + (1.0-talpha)*(double)data.get()[adr_src+0]);
                  _qImageData.get()[adr_dst+1] = unsigned char(talpha*(double)_qImageData.get()[adr_dst+1] + (1.0-talpha)*(double)data.get()[adr_src+1]);
                  _qImageData.get()[adr_dst+2] = unsigned char(talpha*(double)_qImageData.get()[adr_dst+2] + (1.0-talpha)*(double)data.get()[adr_src+2]);
                  _qImageData.get()[adr_dst+3] = unsigned char(talpha*(double)_qImageData.get()[adr_dst+3] + (1.0-talpha)*(double)data.get()[adr_src+3]);;
               }
            }
            
            if (curAlpha == 255)
            {
               alphacnt--;
            }
         }
      }

      if (alphacnt == 0)
      {
         _bFinishMerge = true;
      }

   }
}

//-----------------------------------------------------------------------------

inline void _ttScale(TextureTransformation<double>& oTexTransSet, double x, double y, double z)
{
   oTexTransSet._vScale.x *= x;
   oTexTransSet._vScale.y *= y;
   oTexTransSet._vScale.z *= z;
}

//-----------------------------------------------------------------------------

inline void _ttTranslate(TextureTransformation<double>& oTexTransSet, double x, double y, double z)
{
   oTexTransSet._vTranslation.x += oTexTransSet._vScale.x * x;
   oTexTransSet._vTranslation.y += oTexTransSet._vScale.y * y;
   oTexTransSet._vTranslation.z += oTexTransSet._vScale.z * z;
}

//-----------------------------------------------------------------------------

void VirtualGlobeTerrainBlock::_QuadKeyMatrixTraversal(std::string& sLowQuadcode, std::string& sHighQuadcode, TextureTransformation<double>& outTexTrans)
{
   outTexTrans.SetIdentity();

   int nHighDepth = sLowQuadcode.size();
   int nLowDepth = sHighQuadcode.size();
   std::string sCurQuadcode = sLowQuadcode;

   for (int i=nLowDepth; i < nHighDepth; i++)
   {
      int nLocation = _qQuadtree->GetQuad(sLowQuadcode,i);
        
      switch(nLocation)
      {
      case 0:
         _ttTranslate(outTexTrans, 0, 0.5, 0);
         _ttScale(outTexTrans, 0.5, 0.5, 0);
         break;
      case 1:
         _ttTranslate(outTexTrans, 0.5, 0.5f, 0);
         _ttScale(outTexTrans, 0.5, 0.5, 0);
         break;
      case 2:
         //_ttTranslate(oTexTransSet, 0, 0, 0);
         _ttScale(outTexTrans, 0.5, 0.5, 0);
         break;
      case 3:
         _ttTranslate(outTexTrans, 0.5, 0, 0);
         _ttScale(outTexTrans, 0.5, 0.5, 0);
         break;
      }
   }

}


void VirtualGlobeTerrainBlock::_SetTexture(ITexture* qTexture, bool bLinear)
{
   qTexture->BindTexture(0);

   if (bLinear)
   {
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtering
     
   }
   else
   {
         glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);	// Linear Filtering
         glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// Linear Filtering
   }
   
   //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.0f);

   
   if (GLEW_EXT_texture_edge_clamp)
   {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   }
   else
   {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   }
}

//-----------------------------------------------------------------------------

inline void _DrawBoundingBox(const aabb3<double>& bbox)
{
   //bool bAxisAligned = false;

   vec3<double> t(0,0,0);
   double r,g,b;
   vec3<double> minv;
   vec3<double> maxv;
   r = 1; g = 0; b = 0;

   minv = bbox.MinEdge;
   maxv = bbox.MaxEdge; 

   double dx, dy, dz;

   dx = fabs(maxv.x-minv.x);
   dy = fabs(maxv.y-minv.y);
   dz = fabs(maxv.z-minv.z);

   vec3<double> A( minv.x,     minv.y,     minv.z);
   vec3<double> B( minv.x+dx,  minv.y,     minv.z);
   vec3<double> C( minv.x+dx,  minv.y+dy,  minv.z);
   vec3<double> D( minv.x,     minv.y+dy,  minv.z);

   vec3<double> E( minv.x,     minv.y,     minv.z+dz);
   vec3<double> F( minv.x+dx,  minv.y,     minv.z+dz);
   vec3<double> G( minv.x+dx,  minv.y+dy,  minv.z+dz);
   vec3<double> H( minv.x,     minv.y+dy,  minv.z+dz);

   glPushAttrib(GL_CURRENT_BIT);
   glPushMatrix();

   // load view matrix, we don't want to use current model matrix because
   // aabb has absolute values!
   //glLoadMatrixd(_matView.GetGLMatrix());
   glLineWidth(2.5);

   // "Boden"
   glBegin(GL_LINES);

   // Boden: ABCD
   glColor3d(r,g,b); glVertex3d(A.x+t.x,A.y+t.y,A.z+t.z);
   glColor3d(r,g,b); glVertex3d(B.x+t.x,B.y+t.y,B.z+t.z);
   glColor3d(r,g,b); glVertex3d(B.x+t.x,B.y+t.y,B.z+t.z);
   glColor3d(r,g,b); glVertex3d(C.x+t.x,C.y+t.y,C.z+t.z);
   glColor3d(r,g,b); glVertex3d(C.x+t.x,C.y+t.y,C.z+t.z);
   glColor3d(r,g,b); glVertex3d(D.x+t.x,D.y+t.y,D.z+t.z);
   glColor3d(r,g,b); glVertex3d(D.x+t.x,D.y+t.y,D.z+t.z);
   glColor3d(r,g,b); glVertex3d(A.x+t.x,A.y+t.y,A.z+t.z);

   // Deckel
   glColor3d(r,g,b); glVertex3d(E.x+t.x,E.y+t.y,E.z+t.z);
   glColor3d(r,g,b); glVertex3d(F.x+t.x,F.y+t.y,F.z+t.z);
   glColor3d(r,g,b); glVertex3d(F.x+t.x,F.y+t.y,F.z+t.z);
   glColor3d(r,g,b); glVertex3d(G.x+t.x,G.y+t.y,G.z+t.z);
   glColor3d(r,g,b); glVertex3d(G.x+t.x,G.y+t.y,G.z+t.z);
   glColor3d(r,g,b); glVertex3d(H.x+t.x,H.y+t.y,H.z+t.z);
   glColor3d(r,g,b); glVertex3d(H.x+t.x,H.y+t.y,H.z+t.z);
   glColor3d(r,g,b); glVertex3d(E.x+t.x,E.y+t.y,E.z+t.z);

   //AE,BF,CG,DH
   glColor3d(r,g,b); glVertex3d(A.x+t.x,A.y+t.y,A.z+t.z);
   glColor3d(r,g,b); glVertex3d(E.x+t.x,E.y+t.y,E.z+t.z);
   glColor3d(r,g,b); glVertex3d(B.x+t.x,B.y+t.y,B.z+t.z);
   glColor3d(r,g,b); glVertex3d(F.x+t.x,F.y+t.y,F.z+t.z);
   glColor3d(r,g,b); glVertex3d(C.x+t.x,C.y+t.y,C.z+t.z);
   glColor3d(r,g,b); glVertex3d(G.x+t.x,G.y+t.y,G.z+t.z);
   glColor3d(r,g,b); glVertex3d(D.x+t.x,D.y+t.y,D.z+t.z);
   glColor3d(r,g,b); glVertex3d(H.x+t.x,H.y+t.y,H.z+t.z);

   glEnd();

   glPopMatrix();
   glPopAttrib();

}

//-----------------------------------------------------------------------------

void VirtualGlobeTerrainBlock::_CreateBuffers()
{
   if (_bBuffersCreated)
   {
      assert(false); // already created!
      return;
   }

   _CreateTexture();
   _CreateVertexBuffer();
   _CreateIndexBuffer();
   _bBuffersCreated = true;
}

//-----------------------------------------------------------------------------

void VirtualGlobeTerrainBlock::Render(const mat4<double>& oModelMatrix, const mat4<double>& oViewMatrix, boost::shared_ptr<VirtualGlobeCache>& qCache/*, const VirtualGlobeRenderOptions& oRenderOptions*/)
{  
   //-------------------------
   // some options:
   bool bShowCurtain = true;
   bool bShowBB = false;
   //-------------------------

   if (_qVB && _qIB)
   {
      if (_qTexture)
      {
         _SetTexture(_qTexture, true);
      }
      else
      {
         // Search through qCache and find texture of this area...
         std::string sQuadcode = _sQuadcode;
         std::string sQuadcodeWithTexture;
         ITexture* qParentTexture;

         do 
         {
            sQuadcode = _qQuadtree->GetParent(sQuadcode);
            boost::shared_ptr<VirtualGlobeTerrainBlock> qBlock = qCache->GetCachedBlock(sQuadcode);
            if (qBlock)
            {
               if (qBlock->IsAvailable())
               {
                  qParentTexture = qBlock->GetTexture();
                  if (qParentTexture)
                  {  
                     sQuadcodeWithTexture = sQuadcode;
                     break;
                  }
               }
            }

         } while(sQuadcode.size()>2);
         

         if (qParentTexture)
         {
            TextureTransformation<double> outTexTrans;
            mat4<double>   oTextureMatrix;
            _QuadKeyMatrixTraversal(_sQuadcode, sQuadcodeWithTexture, outTexTrans);
            outTexTrans.ToMatrix4x4(oTextureMatrix);

            glMatrixMode(GL_TEXTURE);
            glLoadMatrixd(oTextureMatrix.GetGLMatrix());
            _SetTexture(qParentTexture, true);
            glMatrixMode(GL_MODELVIEW);
         
         }
         else
         {
            glBindTexture(GL_TEXTURE_2D, 0);
         }
      }

      if (_bIsBlock)
      {
         glPushMatrix();
         {
            mat4<double> model = oModelMatrix;

            model._vals[3][0] += _vOffset.x;
            model._vals[3][1] += _vOffset.y;
            model._vals[3][2] += _vOffset.z;

            mat4<double> modelview = oViewMatrix * model;
            glLoadMatrixd(modelview.GetGLMatrix());

            SetVertexBuffer(_qVB.get());
            DrawIndexedPrimitive(GL_TRIANGLE_STRIP, _qIB.get());

            //_RenderPointCloud(oRenderOptions.fPointSize);
         }
         glPopMatrix();
      }
      else
      {
         glPushMatrix();

         mat4<double> model = oModelMatrix;

         model._vals[3][0] += _vOffset.x;
         model._vals[3][1] += _vOffset.y;
         model._vals[3][2] += _vOffset.z;

         mat4<double> modelview = oViewMatrix * model;
         glLoadMatrixd(modelview.GetGLMatrix());

         //Draw Regular Terrain BUffer:
         SetVertexBuffer(_qVB.get());
         DrawIndexedPrimitive(GL_TRIANGLES, _qIB.get());

         //if (!oRenderOptions.bWireframe)
         {
            if (bShowCurtain)
            {
               //Draw Stiching Buffer:
               if (_qVBStichN && _qIBStichN)
               {
                  SetVertexBuffer(_qVBStichN.get());
                  DrawIndexedPrimitive(GL_TRIANGLES, _qIBStichN.get());
               }

               if (_qVBStichE && _qIBStichE)
               {
                  SetVertexBuffer(_qVBStichE.get());
                  DrawIndexedPrimitive(GL_TRIANGLES, _qIBStichE.get());
               }

               if (_qVBStichS && _qIBStichS)
               {
                  SetVertexBuffer(_qVBStichS.get());
                  DrawIndexedPrimitive(GL_TRIANGLES, _qIBStichS.get());
               }

               if (_qVBStichW && _qIBStichW)
               {
                  SetVertexBuffer(_qVBStichW.get());
                  DrawIndexedPrimitive(GL_TRIANGLES, _qIBStichW.get());
               }
            }
         }

         //_RenderPointCloud(oRenderOptions.fPointSize);

         glPopMatrix();
         
         if (bShowBB)
         {
         
            _DrawBoundingBox(_BoundingBox);
         }
      }

      if (!_qTexture)
      {
         glMatrixMode(GL_TEXTURE);
         glLoadIdentity();   
         glMatrixMode(GL_MODELVIEW);
      }
   }
}



//-----------------------------------------------------------------------------
// Create Vertex Buffer as Block (if no TIN data is available!)

void VirtualGlobeTerrainBlock::_CreateVertexBufferBlock()
{
   if (!_qElevationData || !_qQuadtree)
   {
      assert(false);
      return;
   }

   boost::shared_ptr<IVertexAttributeBuffer> qAttributePosition;
   boost::shared_ptr<IVertexAttributeBuffer> qAttributeTextureCoord;


   boost::shared_array<float> qTempVertexBuffer = boost::shared_array<float>(new float[3*_nElevationBlockSize*_nElevationBlockSize]);
   boost::shared_array<float> qTempTexcoordBuffer = boost::shared_array<float>(new float[2*_nElevationBlockSize*_nElevationBlockSize]);

   _qVB = BufferFactory::CreateVertexBuffer(_nElevationBlockSize*_nElevationBlockSize, true);
   qAttributePosition = BufferFactory::CreateVertexAttributeBuffer(_nElevationBlockSize*_nElevationBlockSize,     Vertex,         float3);
   qAttributeTextureCoord = BufferFactory::CreateVertexAttributeBuffer(_nElevationBlockSize*_nElevationBlockSize, TexCoord0,      float2);

   //-----------------------------------------------------------------------------
   // 1. Create Texture Coordinates
   //-----------------------------------------------------------------------------

   float fdX;
   fdX = 1.0f / float(_nElevationBlockSize-1);

   if (qTempTexcoordBuffer)
   { 
      for (unsigned int nV = 0; nV<_nElevationBlockSize; nV++)
      {
         for (unsigned int nU = 0; nU<_nElevationBlockSize; nU++)
         {
            qTempTexcoordBuffer[2*(nV*_nElevationBlockSize+nU)+0] =  nU*fdX;
            qTempTexcoordBuffer[2*(nV*_nElevationBlockSize+nU)+1] =  /*1.0f-*/nV*fdX;
         }
      }  
   }

   //-----------------------------------------------------------------------------
   // 2. Create Vertex Positions
   //-----------------------------------------------------------------------------

   double x0,y0,x1,y1;
   _qQuadtree->QuadKeyToMercatorCoord(_sQuadcode, x0, y0, x1, y1);

   double dH = (y1-y0)/double(_nElevationBlockSize-1);
   double dW = (x1-x0)/double(_nElevationBlockSize-1);
   double x_coord, y_coord;
   double x_cart, y_cart, z_cart;
   GeoCoord g;

   for (unsigned int y=0;y<_nElevationBlockSize;y++)
   {
      for (unsigned int x=0;x<_nElevationBlockSize;x++)
      {
         x_coord = x0 + (double)x*dW;
         y_coord = y0 + (double)y*dH;

         // Calculate 3D Position:

         _qQuadtree->MercatorToWGS84(x_coord, y_coord);
         g.SetLongitude(x_coord);
         g.SetLatitude(y_coord);
         g.SetEllipsoidHeight((double)_qElevationData.get()[y*_nElevationBlockSize+x]);
         g.ToCartesian(&x_cart, &y_cart, &z_cart);

         if (x==0 && y==0)
         {
            _vOffset = vec3<double>(x_cart, y_cart, z_cart);
         }

         if (x==0 && y==0)
         {
            _vTilePoints[0].x = x_cart; _vTilePoints[0].y = y_cart; _vTilePoints[0].z = z_cart;
         }
         else if (x==_nElevationBlockSize-1 && y==0)
         {
            _vTilePoints[1].x = x_cart; _vTilePoints[1].y = y_cart; _vTilePoints[1].z = z_cart; 
         }
         else if (x==_nElevationBlockSize-1 && y==_nElevationBlockSize-1)
         {
            _vTilePoints[2].x = x_cart; _vTilePoints[2].y = y_cart; _vTilePoints[2].z = z_cart;
         }
         else if (x==0 && y==_nElevationBlockSize-1)
         {
            _vTilePoints[3].x = x_cart; _vTilePoints[3].y = y_cart; _vTilePoints[3].z = z_cart;
         }
         else if (x==_nElevationBlockSize/2 && y==_nElevationBlockSize/2)
         {
            _vTilePoints[4].x = x_cart; _vTilePoints[4].y = y_cart; _vTilePoints[4].z = z_cart;
         }

         qTempVertexBuffer.get()[3*y*_nElevationBlockSize+3*x+0] = (float) (x_cart -_vOffset.x);
         qTempVertexBuffer.get()[3*y*_nElevationBlockSize+3*x+1] = (float) (y_cart -_vOffset.y);
         qTempVertexBuffer.get()[3*y*_nElevationBlockSize+3*x+2] = (float) (z_cart -_vOffset.z);

      }

   }
   // Upload to GPU
   memcpy(qAttributePosition->GetDataPtr(), qTempVertexBuffer.get(), 3*_nElevationBlockSize*_nElevationBlockSize*sizeof(float));
   memcpy(qAttributeTextureCoord->GetDataPtr(), qTempTexcoordBuffer.get(), 2*_nElevationBlockSize*_nElevationBlockSize*sizeof(float));

   _qVB->AddVertexAttributeBuffer(qAttributePosition);
   _qVB->AddVertexAttributeBuffer(qAttributeTextureCoord);

   _qVB->CreateVRAMObject();
   _qVB->CopyToVRAM(true);

   // Update Bounding box for this block!
   _UpdateBoundingBox();
}

//-----------------------------------------------------------------------------

void VirtualGlobeTerrainBlock::_CreateVertexBufferTIN()
{
   if (_lstTexCoord.size() != _lstElevationPointWGS84.size())
   {
      assert(false);
      return;
   }

   if (_lstElevationPointWGS84.size()<3)
      return;

   unsigned int nBufferSize = (unsigned int)_lstElevationPointWGS84.size();

   _qVB = BufferFactory::CreateVertexBuffer(nBufferSize, true);

   if (!_qVB)
   {
      assert(false);
      return;
   }

   boost::shared_ptr<IVertexAttributeBuffer> qAttributeTextureCoord = BufferFactory::CreateVertexAttributeBuffer(nBufferSize, TexCoord0,      float2);
   boost::shared_ptr<IVertexAttributeBuffer> qAttrPosition = BufferFactory::CreateVertexAttributeBuffer(nBufferSize, Vertex, float3);
  

   for (size_t i=0; i< _lstElevationPointWGS84.size();i++)
   {
      qAttributeTextureCoord->SetValue(unsigned int(i), _lstTexCoord[i].x, _lstTexCoord[i].y);
      qAttrPosition->SetValue(unsigned int(i), _lstElevationPointWGS84[i].x, _lstElevationPointWGS84[i].y, _lstElevationPointWGS84[i].z);
   }

   // Add Buffers 
   _qVB->AddVertexAttributeBuffer(qAttrPosition);
   _qVB->AddVertexAttributeBuffer(qAttributeTextureCoord);
   //_qVB->AddVertexAttributeBuffer(qAttrNormals);

   _qVB->CreateVRAMObject();
   _qVB->CopyToVRAM(true);

   // Update Bounding box for this block!
   _UpdateBoundingBox();
}

//-----------------------------------------------------------------------------

void VirtualGlobeTerrainBlock::_CreateVertexBufferTINCurtainNorth()
{
   // Data is TIN
   std::vector<ElevationPoint>::iterator it;

   if (_lstElevationPointWGS84.size()<3)
      return;


   unsigned int nBufferSize = 2*(2 + // corner points count double...
      + (unsigned int) _vNorthPoints.size());
   
   _qVBStichN = BufferFactory::CreateVertexBuffer(nBufferSize, true);

  
   if (!_qVBStichN)
   {
      assert(false);
      return;
   }

   boost::shared_ptr<IVertexAttributeBuffer> qAttributeTextureCoord = BufferFactory::CreateVertexAttributeBuffer(nBufferSize, TexCoord0,      float2);
   boost::shared_ptr<IVertexAttributeBuffer> qAttrPosition = BufferFactory::CreateVertexAttributeBuffer(nBufferSize, Vertex, float3);

   std::vector<ElevationPoint> vWall;
   ElevationPoint ptTmp;

   /*             N
               ------> 

          3 +-----------+ 2
     ^      |           |     ^
     |      |           |     |
  W  |      |           |     | E
     |      |           |     |
            |           |
          0 +-----------+ 1

               ------->
                  S

   */

   //If one edge point is below zero, draw curtain 200 meters below the value of this edge point.
   _dCurtainElevation = 0;
   double minElevation = 0.0;
   for (size_t i = 0; i < _vNorthPoints.size(); i++)
   {
      minElevation = MIN(minElevation, _vNorthPoints[i].elevation);   
   }
   for (size_t i = 0; i < _vEastPoints.size(); i++)
   {
      minElevation = MIN(minElevation, _vEastPoints[i].elevation);   
   }
   for (size_t i = 0; i < _vSouthPoints.size(); i++)
   {
      minElevation = MIN(minElevation, _vSouthPoints[i].elevation);   
   }
   for (size_t i = 0; i < _vWestPoints.size(); i++)
   {
      minElevation = MIN(minElevation, _vWestPoints[i].elevation); 
   }
   if (minElevation < 0.0)
   {
      _dCurtainElevation = minElevation - 1000;
   }
   
   
   //----------------------
   // POINTS FOR NORTH WALL
   //----------------------
   ptTmp = _vCornerPoints[3];
   vWall.push_back(ptTmp);
   ptTmp.elevation = _dCurtainElevation;
   vWall.push_back(ptTmp);

   for (size_t p=0;p<_vNorthPoints.size();p++)
   {
      ptTmp = _vNorthPoints[p];
      vWall.push_back(ptTmp);
      ptTmp.elevation = _dCurtainElevation;
      vWall.push_back(ptTmp);
   }

   ptTmp = _vCornerPoints[2];
   vWall.push_back(ptTmp);
   ptTmp.elevation = _dCurtainElevation;
   vWall.push_back(ptTmp);


   // Make sure everything is calculated correctly!!
   assert(nBufferSize == vWall.size());


   // Create Buffer

   int i=0;

   GeoCoord oGeoCoord;
   GeoCoord oGeoCoordNorm;
   double lng, lat;
   double x,y,z;



   double TexCoordOffsetX = _vCornerPoints[0].x;
   double TexCoordOffsetY = _vCornerPoints[0].y;

   double TexCoordDX = fabs(_vCornerPoints[1].x - _vCornerPoints[0].x);
   double TexCoordDY = fabs(_vCornerPoints[3].y - _vCornerPoints[0].y);


   it = vWall.begin();

   while (it!=vWall.end())
   {
      GeoRef::Mercator::ReverseCustom((*it).x, (*it).y, lng, lat, 0.0);
      oGeoCoord.SetLatitude(lat);
      oGeoCoord.SetLongitude(lng);
      oGeoCoord.SetEllipsoidHeight((*it).elevation);
      oGeoCoord.ToCartesian(&x,&y,&z);

      double u = ((*it).x - TexCoordOffsetX) / TexCoordDX;
      double v = 1.0; // 0.0
      qAttributeTextureCoord->SetValue(unsigned int(i), (float)u, (float)v);

      // Set Attribute Position with virtual camera offset:
      qAttrPosition->SetValue(unsigned int(i), 
      (float) (x-_vOffset.x), 
      (float) (y-_vOffset.y), 
      (float) (z-_vOffset.z));
      it++;
      //normit++;
      i++;
   }

   // Texture Coordinate: #todo
 
   // Add Buffers 
   _qVBStichN->AddVertexAttributeBuffer(qAttrPosition);
   _qVBStichN->AddVertexAttributeBuffer(qAttributeTextureCoord);

   _qVBStichN->CreateVRAMObject();
   _qVBStichN->CopyToVRAM(true);
}

//-----------------------------------------------------------------------------

void VirtualGlobeTerrainBlock::_CreateVertexBufferTINCurtainEast()
{
   // Data is TIN
   std::vector<ElevationPoint>::iterator it;

   if (_lstElevationPointWGS84.size()<3)
      return;

   unsigned int nBufferSize = 2*(2 + // corner points count double...
      + (unsigned int) _vEastPoints.size());

   _qVBStichE = BufferFactory::CreateVertexBuffer(nBufferSize, true);


   if (!_qVBStichE)
   {
      assert(false);
      return;
   }

   boost::shared_ptr<IVertexAttributeBuffer> qAttributeTextureCoord = BufferFactory::CreateVertexAttributeBuffer(nBufferSize, TexCoord0,      float2);
   boost::shared_ptr<IVertexAttributeBuffer> qAttrPosition = BufferFactory::CreateVertexAttributeBuffer(nBufferSize, Vertex, float3);

   std::vector<ElevationPoint> vWall;
   ElevationPoint ptTmp;

   //----------------------
   // POINTS FOR EAST WALL
   //----------------------
   ptTmp = _vCornerPoints[1];
   vWall.push_back(ptTmp);
   ptTmp.elevation = _dCurtainElevation;
   vWall.push_back(ptTmp);

   for (size_t p=0;p<_vEastPoints.size();p++)
   {
   ptTmp = _vEastPoints[p];
   vWall.push_back(ptTmp);
   ptTmp.elevation = _dCurtainElevation;
   vWall.push_back(ptTmp);
   }

   ptTmp = _vCornerPoints[2];
   vWall.push_back(ptTmp);
   ptTmp.elevation = _dCurtainElevation;
   vWall.push_back(ptTmp);

   // Make sure everything is calculated correctly!!
   assert(nBufferSize == vWall.size());

   // Create Buffer

   int i=0;

   GeoCoord oGeoCoord;
   GeoCoord oGeoCoordNorm;
   double lng, lat;
   double x,y,z;


   double TexCoordOffsetX = _vCornerPoints[0].x;
   double TexCoordOffsetY = _vCornerPoints[0].y;

   double TexCoordDX = fabs(_vCornerPoints[1].x - _vCornerPoints[0].x);
   double TexCoordDY = fabs(_vCornerPoints[3].y - _vCornerPoints[0].y);


   it = vWall.begin();

   while (it!=vWall.end())
   {
      GeoRef::Mercator::ReverseCustom((*it).x, (*it).y, lng, lat, 0.0);
      oGeoCoord.SetLatitude(lat);
      oGeoCoord.SetLongitude(lng);
      oGeoCoord.SetEllipsoidHeight((*it).elevation);
      oGeoCoord.ToCartesian(&x,&y,&z);

      double u = 1.0;
      double v = ((*it).y - TexCoordOffsetY) / TexCoordDY;
      qAttributeTextureCoord->SetValue(unsigned int(i), (float)u, (float)v);

      // Set Attribute Position with virtual camera offset:
      qAttrPosition->SetValue(unsigned int(i), 
         (float) (x-_vOffset.x), 
         (float) (y-_vOffset.y), 
         (float) (z-_vOffset.z));
      it++;
      //normit++;
      i++;
   }

   // Texture Coordinate: #todo


   // Add Buffers 
   _qVBStichE->AddVertexAttributeBuffer(qAttrPosition);
   _qVBStichE->AddVertexAttributeBuffer(qAttributeTextureCoord);
   //_qVB->AddVertexAttributeBuffer(qAttrNormals);

   _qVBStichE->CreateVRAMObject();
   _qVBStichE->CopyToVRAM(true);
}

//-----------------------------------------------------------------------------
void VirtualGlobeTerrainBlock::_CreateVertexBufferTINCurtainSouth()
{
   // Data is TIN
   std::vector<ElevationPoint>::iterator it;

   if (_lstElevationPointWGS84.size()<3)
      return;

   unsigned int nBufferSize = 2*(2 + // corner points count double...
      + (unsigned int) _vSouthPoints.size());

   _qVBStichS = BufferFactory::CreateVertexBuffer(nBufferSize, true);


   if (!_qVBStichS)
   {
      assert(false);
      return;
   }

   boost::shared_ptr<IVertexAttributeBuffer> qAttributeTextureCoord = BufferFactory::CreateVertexAttributeBuffer(nBufferSize, TexCoord0,      float2);
   boost::shared_ptr<IVertexAttributeBuffer> qAttrPosition = BufferFactory::CreateVertexAttributeBuffer(nBufferSize, Vertex, float3);

   std::vector<ElevationPoint> vWall;
   ElevationPoint ptTmp;

  
   //----------------------
   // POINTS FOR SOUTH WALL
   //----------------------
   ptTmp = _vCornerPoints[0];
   vWall.push_back(ptTmp);
   ptTmp.elevation = _dCurtainElevation;
   vWall.push_back(ptTmp);

   for (size_t p=0;p<_vSouthPoints.size();p++)
   {
   ptTmp = _vSouthPoints[p];
   vWall.push_back(ptTmp);
   ptTmp.elevation = _dCurtainElevation;
   vWall.push_back(ptTmp);
   }

   ptTmp = _vCornerPoints[1];
   vWall.push_back(ptTmp);
   ptTmp.elevation = _dCurtainElevation;
   vWall.push_back(ptTmp);

 

   // Make sure everything is calculated correctly!!
   assert(nBufferSize == vWall.size());


   // Create Buffer

   int i=0;

   GeoCoord oGeoCoord;
   GeoCoord oGeoCoordNorm;
   double lng, lat;
   double x,y,z;


   double TexCoordOffsetX = _vCornerPoints[0].x;
   double TexCoordOffsetY = _vCornerPoints[0].y;

   double TexCoordDX = fabs(_vCornerPoints[1].x - _vCornerPoints[0].x);
   double TexCoordDY = fabs(_vCornerPoints[3].y - _vCornerPoints[0].y);


   it = vWall.begin();

   while (it!=vWall.end())
   {
      GeoRef::Mercator::ReverseCustom((*it).x, (*it).y, lng, lat, 0.0);
      oGeoCoord.SetLatitude(lat);
      oGeoCoord.SetLongitude(lng);
      oGeoCoord.SetEllipsoidHeight((*it).elevation);
      oGeoCoord.ToCartesian(&x,&y,&z);

      double u = ((*it).x - TexCoordOffsetX) / TexCoordDX;
      double v = 0.0; // 1.0
      qAttributeTextureCoord->SetValue(unsigned int(i), (float)u, (float)v);

      // Set Attribute Position with virtual camera offset:
      qAttrPosition->SetValue(unsigned int(i), 
         (float) (x-_vOffset.x), 
         (float) (y-_vOffset.y), 
         (float) (z-_vOffset.z));
      it++;
      //normit++;
      i++;
   }

   // Texture Coordinate: #todo


   // Add Buffers 
   _qVBStichS->AddVertexAttributeBuffer(qAttrPosition);
   _qVBStichS->AddVertexAttributeBuffer(qAttributeTextureCoord);

   _qVBStichS->CreateVRAMObject();
   _qVBStichS->CopyToVRAM(true);
}

//-----------------------------------------------------------------------------

void VirtualGlobeTerrainBlock::_CreateVertexBufferTINCurtainWest()
{
   // Data is TIN
   std::vector<ElevationPoint>::iterator it;

   if (_lstElevationPointWGS84.size()<3)
      return;


   unsigned int nBufferSize = 2*(2 + // corner points count double...
      + (unsigned int) _vWestPoints.size());

   _qVBStichW = BufferFactory::CreateVertexBuffer(nBufferSize, true);


   if (!_qVBStichW)
   {
      assert(false);
      return;
   }

   boost::shared_ptr<IVertexAttributeBuffer> qAttributeTextureCoord = BufferFactory::CreateVertexAttributeBuffer(nBufferSize, TexCoord0,      float2);
   boost::shared_ptr<IVertexAttributeBuffer> qAttrPosition = BufferFactory::CreateVertexAttributeBuffer(nBufferSize, Vertex, float3);

   std::vector<ElevationPoint> vWall;
   ElevationPoint ptTmp;

   //----------------------
   // POINTS FOR WEST WALL
   //----------------------
   ptTmp = _vCornerPoints[0];
   vWall.push_back(ptTmp);
   ptTmp.elevation = _dCurtainElevation;
   vWall.push_back(ptTmp);

   for (size_t p=0;p<_vWestPoints.size();p++)
   {
   ptTmp = _vWestPoints[p];
   vWall.push_back(ptTmp);
   ptTmp.elevation = _dCurtainElevation;
   vWall.push_back(ptTmp);
   }

   ptTmp = _vCornerPoints[3];
   vWall.push_back(ptTmp);
   ptTmp.elevation = _dCurtainElevation;
   vWall.push_back(ptTmp);


   // Make sure everything is calculated correctly!!
   assert(nBufferSize == vWall.size());


   // Create Buffer

   int i=0;

   GeoCoord oGeoCoord;
   GeoCoord oGeoCoordNorm;
   double lng, lat;
   double x,y,z;


   double TexCoordOffsetX = _vCornerPoints[0].x;
   double TexCoordOffsetY = _vCornerPoints[0].y;

   double TexCoordDX = fabs(_vCornerPoints[1].x - _vCornerPoints[0].x);
   double TexCoordDY = fabs(_vCornerPoints[3].y - _vCornerPoints[0].y);


   it = vWall.begin();

   while (it!=vWall.end())
   {
      GeoRef::Mercator::ReverseCustom((*it).x, (*it).y, lng, lat, 0.0);
      oGeoCoord.SetLatitude(lat);
      oGeoCoord.SetLongitude(lng);
      oGeoCoord.SetEllipsoidHeight((*it).elevation);
      oGeoCoord.ToCartesian(&x,&y,&z);

      double u = 0.0;
      double v = ((*it).y - TexCoordOffsetY) / TexCoordDY;
      qAttributeTextureCoord->SetValue(unsigned int(i), (float)u, (float)v);

      // Set Attribute Position with virtual camera offset:
      qAttrPosition->SetValue(unsigned int(i), 
         (float) (x-_vOffset.x), 
         (float) (y-_vOffset.y), 
         (float) (z-_vOffset.z));
      it++;
      //normit++;
      i++;
   }

   // Texture Coordinate: #todo


   // Add Buffers 
   _qVBStichW->AddVertexAttributeBuffer(qAttrPosition);
   _qVBStichW->AddVertexAttributeBuffer(qAttributeTextureCoord);

   _qVBStichW->CreateVRAMObject();
   _qVBStichW->CopyToVRAM(true);
}

//-----------------------------------------------------------------------------

void VirtualGlobeTerrainBlock::_CreateVertexBuffer()
{
   if (_bIsBlock)
   {
      _CreateVertexBufferBlock();
   }
   else
   {
      _CreateVertexBufferTIN();
      _CreateVertexBufferTINCurtainNorth(); // TIN with curtain!
      _CreateVertexBufferTINCurtainEast(); // TIN with curtain!
      _CreateVertexBufferTINCurtainSouth(); // TIN with curtain!
      _CreateVertexBufferTINCurtainWest(); // TIN with curtain!
   }

}

//-----------------------------------------------------------------------------

void VirtualGlobeTerrainBlock::_CreateIndexBufferTIN()
{
   // is TIN (or Geometry)
   std::vector<int>::iterator         idxit;

   if (_lstPosIdx.size() < 1)
      return;

   _qIB = BufferFactory::CreateIndexBuffer((unsigned int)_lstPosIdx.size());
   if (!_qIB)
      return;

   // --- Indices
   idxit = _lstPosIdx.begin();
   int i = 0;

   while (idxit != _lstPosIdx.end())
   {
      _qIB->SetValue((unsigned int)i, *idxit, 0);
      idxit++;
      i++;
   }

   _qIB->CreateVRAMObject();
   _qIB->CopyToVRAM(true);	
}

//-----------------------------------------------------------------------------

void VirtualGlobeTerrainBlock::_CreateIndexBufferTINCurtain()
{
   // NORTH 

   if (!_qVBStichN)
   {
      assert(false);
      return;
   }

   unsigned int nBufferSize;
   
   nBufferSize = 3*(_qVBStichN->GetSize() - 2);

   _qIBStichN = BufferFactory::CreateIndexBuffer(nBufferSize);
   if (!_qIBStichN)
      return;

   // *(s, s+1, s+2) and (s+1, s+3, s+2) 
   // (s+2, s+1, s+0) and (s+2, s+3, s+1)
   unsigned int i=0;
   for (unsigned int s=0;s<nBufferSize/6;s++)
   {
       _qIBStichN->SetValue(i, 2*s+2, 0); i++;
       _qIBStichN->SetValue(i, 2*s+1, 0); i++;
       _qIBStichN->SetValue(i, 2*s+0, 0); i++;

       _qIBStichN->SetValue(i, 2*s+2, 0); i++;
       _qIBStichN->SetValue(i, 2*s+3, 0); i++;
       _qIBStichN->SetValue(i, 2*s+1, 0); i++;
   }

   _qIBStichN->CreateVRAMObject();
   _qIBStichN->CopyToVRAM(true);


   // EAST

   if (!_qVBStichE)
   {
      assert(false);
      return;
   }

   nBufferSize = 3*(_qVBStichE->GetSize() - 2);

   _qIBStichE = BufferFactory::CreateIndexBuffer(nBufferSize);
   if (!_qIBStichE)
      return;

   // (s, s+1, s+2) and (s+1, s+3, s+2) 
   // *(s+2, s+1, s+0) and (s+2, s+3, s+1)
   i=0;
   for (unsigned int s=0;s<nBufferSize/6;s++)
   {
      _qIBStichE->SetValue(i, 2*s+0, 0); i++;
      _qIBStichE->SetValue(i, 2*s+1, 0); i++;
      _qIBStichE->SetValue(i, 2*s+2, 0); i++;

      _qIBStichE->SetValue(i, 2*s+1, 0); i++;
      _qIBStichE->SetValue(i, 2*s+3, 0); i++;
      _qIBStichE->SetValue(i, 2*s+2, 0); i++;
   }

   _qIBStichE->CreateVRAMObject();
   _qIBStichE->CopyToVRAM(true);

   // SOUTH
   if (!_qVBStichS)
   {
      assert(false);
      return;
   }

   nBufferSize = 3*(_qVBStichS->GetSize() - 2);

   _qIBStichS = BufferFactory::CreateIndexBuffer(nBufferSize);
   if (!_qIBStichS)
      return;

   // (s, s+1, s+2) and (s+1, s+3, s+2) 
   // *(s+2, s+1, s+0) and (s+2, s+3, s+1)
   i=0;
   for (unsigned int s=0;s<nBufferSize/6;s++)
   {
      _qIBStichS->SetValue(i, 2*s+0, 0); i++;
      _qIBStichS->SetValue(i, 2*s+1, 0); i++;
      _qIBStichS->SetValue(i, 2*s+2, 0); i++;

      _qIBStichS->SetValue(i, 2*s+1, 0); i++;
      _qIBStichS->SetValue(i, 2*s+3, 0); i++;
      _qIBStichS->SetValue(i, 2*s+2, 0); i++;
   }

   _qIBStichS->CreateVRAMObject();
   _qIBStichS->CopyToVRAM(true);

   //WEST
   if (!_qVBStichW)
   {
      assert(false);
      return;
   }


   nBufferSize = 3*(_qVBStichW->GetSize() - 2);

   _qIBStichW = BufferFactory::CreateIndexBuffer(nBufferSize);
   if (!_qIBStichW)
      return;

   // *(s, s+1, s+2) and (s+1, s+3, s+2) 
   // (s+2, s+1, s+0) and (s+2, s+3, s+1)
   i=0;
   for (unsigned int s=0;s<nBufferSize/6;s++)
   {
      _qIBStichW->SetValue(i, 2*s+2, 0); i++;
      _qIBStichW->SetValue(i, 2*s+1, 0); i++;
      _qIBStichW->SetValue(i, 2*s+0, 0); i++;

      _qIBStichW->SetValue(i, 2*s+2, 0); i++;
      _qIBStichW->SetValue(i, 2*s+3, 0); i++;
      _qIBStichW->SetValue(i, 2*s+1, 0); i++;
   }

   _qIBStichW->CreateVRAMObject();
   _qIBStichW->CopyToVRAM(true);


}

//-----------------------------------------------------------------------------


void VirtualGlobeTerrainBlock::_CreateIndexBuffer()
{
   if (_bIsBlock)
   {
      boost::shared_array<unsigned short> indices;

      int nIndexCount;
      indices = _CreateIndices(nIndexCount, 0,0,0,0, 0,0, _nElevationBlockSize-1, _nElevationBlockSize-1, 0);


      if (nIndexCount>0)
      {
         _qIB = BufferFactory::CreateIndexBuffer(nIndexCount);
         for (int i=0;i<nIndexCount;i++)
         {
            _qIB->SetValue(i, (int)indices[i]);
            assert(indices[i] < _nElevationBlockSize*_nElevationBlockSize);
         }

         _qIB->CreateVRAMObject();
         _qIB->CopyToVRAM(true);
      }
   }
   else
   {
      _CreateIndexBufferTIN();
      _CreateIndexBufferTINCurtain();
   }
}

//-----------------------------------------------------------------------------

void VirtualGlobeTerrainBlock::_CreateTexture()
{
   if (!_qImageData)
      return;

   // Convert Texture to RGB
   boost::shared_array<unsigned char> qData = 
      boost::shared_array<unsigned char>(new unsigned char[3*_nTextureSize*_nTextureSize]);

   for (unsigned int y = 0; y<_nTextureSize; y++)
   {
      for (unsigned int x=0; x<_nTextureSize; x++)
      {
         qData.get()[3*y*_nTextureSize+3*x+0] = _qImageData.get()[_nImageDataChannels*y*_nTextureSize+_nImageDataChannels*x+2];
         qData.get()[3*y*_nTextureSize+3*x+1] = _qImageData.get()[_nImageDataChannels*y*_nTextureSize+_nImageDataChannels*x+1];
         qData.get()[3*y*_nTextureSize+3*x+2] = _qImageData.get()[_nImageDataChannels*y*_nTextureSize+_nImageDataChannels*x+0];
      }
   }
 
   _qTexture = _pEngine->CreateTexture();
   _qTexture->CreateTexture(Texture::GFX_TEXTURE_2D, _nTextureSize, _nTextureSize, Texture::GFX_BGR, Texture::GFX_UNSIGNED_BYTE, (void*)qData.get());


  
   //#EXPERIMENT
   //if (_sQuadcode.length()<5)
   //_qTexture = TextureFactory::CreateTextureFromMemory((const char*)qData.get(), _nTextureSize, _nTextureSize, 3, true, i3dNodeRoot::GetMipmapSetting()); 
 
   

}

//-----------------------------------------------------------------------------
#define MACRO_getIndex(x,y,n) ((y)*(n)+(x))

boost::shared_array<unsigned short> VirtualGlobeTerrainBlock::_CreateIndices(int& nIndexCount, int a, int b, int c, int d, int x_start, int y_start, int x_end, int y_end, int LOD)
{	
   int    v3;
   boost::shared_array<unsigned short> _pIndexBuffer = boost::shared_array<unsigned short>(new unsigned short[4*_nElevationBlockSize*_nElevationBlockSize]);

   if (a<=LOD) a=0;
   if (b<=LOD) b=0;
   if (c<=LOD) c=0;
   if (d<=LOD) d=0;

   a = math::Pow2(a);
   b = math::Pow2(b);
   c = math::Pow2(c);
   d = math::Pow2(d);

   int lod = math::Pow2(LOD);

   x_start = (x_start / lod) * lod;
   y_start = (y_start / lod) * lod;

   int cnt = 0; int xcnt = 0;
   int xx1, xx2, yy1, yy2;
   for (int x=x_start;x<=x_end-1;x+=lod)
   {
      for (int y=y_start;y<=y_end;y+=lod)
      {
         xx1 = x; 
         xx2 = x+lod;

         if (xcnt%2 == 0) yy1 = yy2 = y_end+y_start-y;
         else yy1 = yy2 = y;

         // Neighbour based selection
         //      a
         //    +-----+
         //  b |     | d
         //    |     | 
         //    +-----+
         //      c

         //--------------------------------------------------------------------
         //c
         if (c>1)
         {
            if (yy1==y_start)
            {
               xx1 = (xx1/int(c))*int(c); // integer division is intentional!!
               xx2 = (xx2/int(c))*int(c); // integer division is intentional!!
            } 
         }
         //--------------------------------------------------------------------
         //b
         if (b>1)
         {
            if (xx2==x_end) 
            {
               yy2=yy2-yy2%b;
            }
         }
         //--------------------------------------------------------------------
         //d
         if  (d>1)
         {
            if (xx1==x_start) 
            {
               yy1=yy1-yy1%d;
            }
         }    
         //--------------------------------------------------------------------
         //a
         if (a>1)
         {
            if (yy2==y_end)
            {
               if (x<((x_end-x_start+1)/2))
               {
                  xx1 = (xx1/int(a))*int(a); // integer division is intentional!!
                  xx2 = (xx2/int(a))*int(a); // integer division is intentional!!
               }
               else
               {
                  xx1 = ((xx1+1*lod)/int(a))*int(a); // integer division is intentional!!
                  xx2 = ((xx2+2*lod)/int(a))*int(a); // integer division is intentional!!

                  if (xx2 > x_end)
                     xx2 = x_end;  
               }
            }
         }
         //--------------------------------------------------------------------
         v3 = MACRO_getIndex(xx2,yy2,_nElevationBlockSize);
         _pIndexBuffer[cnt++] = MACRO_getIndex(xx1,yy1,_nElevationBlockSize); 
         _pIndexBuffer[cnt++] = v3; 
      }
      _pIndexBuffer[cnt++] = v3; 
      xcnt += 1;
   }

   nIndexCount = cnt;
   boost::shared_array<unsigned short> index_buffer_real = boost::shared_array<unsigned short>(new unsigned short[cnt]);

   for (int c = 0;c<cnt;c++)
   {
      assert(_pIndexBuffer[c] < _nElevationBlockSize*_nElevationBlockSize);
      index_buffer_real[c] = _pIndexBuffer[c];
   }

   return index_buffer_real;
}

//-----------------------------------------------------------------------------

double VirtualGlobeTerrainBlock::GetPixelSize(const mat4<double>& mModelViewProjection, const int& nWidth, const int& nHeight)
{
   vec3<double> v[5];
   double dx = -1e20;
   double dy = -1e20;


   for (int i=0;i<5;i++)
   {
      v[i] = mModelViewProjection.vec3mul(_vTilePoints[i]);
   }

   dx= math::Max(dx, fabs(v[0].x / 2.0 - v[1].x / 2.0) * nWidth);
   dy= math::Max(dy, fabs(v[0].y / 2.0 - v[1].y / 2.0) * nHeight);
   
   dx= math::Max(dx, fabs(v[1].x / 2.0 - v[2].x / 2.0) * nWidth);
   dy= math::Max(dy, fabs(v[1].y / 2.0 - v[2].y / 2.0) * nHeight);

   dx= math::Max(dx, fabs(v[2].x / 2.0 - v[3].x / 2.0) * nWidth);
   dy= math::Max(dy, fabs(v[2].y / 2.0 - v[3].y / 2.0) * nHeight);

   dx= math::Max(dx, fabs(v[3].x / 2.0 - v[0].x / 2.0) * nWidth);
   dy= math::Max(dy, fabs(v[3].y / 2.0 - v[0].y / 2.0) * nHeight);
  
   return math::Max<double>(dx,dy) / double(_nTextureSize);
   
}

//------------------------------------------------------------------------------

double VirtualGlobeTerrainBlock::GetBlockSize()
{
   return _dBlockSize;
}

//-----------------------------------------------------------------------------

double VirtualGlobeTerrainBlock::CalcDistanceTo(vec3<double>& vWhere, vec3<double>& outHitpoint)
{
   double len = 1e20;
   vec3<double> vPos;
   vec3<double> vStart = vWhere;

   if (_qVB)
   {
      IVertexAttributeBuffer* pVertexAttribute;
      float* pData;
      int numVertices;

      numVertices = _qVB->GetSize();
      pVertexAttribute = _qVB->GetVertexAttributeBuffer(Vertex);

      if (pVertexAttribute)
      {
         if (pVertexAttribute->GetAttributeDataType() == 1) // FLOAT list
         {
            pData = (float*)pVertexAttribute->GetDataPtr();
            if (!pData)
            {
               assert(false); // no data ???
               return len;
            }
            int components = pVertexAttribute->GetNumComponents();
            if (components>=3) // bounding box requires 3D geometry! (x,y,z)
            {
               for (int j=0;j<numVertices;j++)
               {
                  vPos.Set(pData[components*j] + _vOffset.x,
                     pData[components*j+1] + _vOffset.y,
                     pData[components*j+2] + _vOffset.z);

                  vec3<double> lenv = (vStart-vPos);
                  double dist = lenv.SquaredLength();

                  if (dist<len)
                  {
                     outHitpoint = vPos;
                     len = dist;
                  }
               }
            }
            else
            {
               assert(false); 
            }
         }
         else
         {
            assert(false); 
         }
      }
   }

   return sqrt(len);
}

//-----------------------------------------------------------------------------

void VirtualGlobeTerrainBlock::_UpdateBoundingBox()
{
   // Caluclate Bounding Box
   if (_qVB)
   {
      IVertexAttributeBuffer* pVertexAttribute;
      double x,y,z;
      float* pData;
      int numVertices;
      _BoundingBox = aabb3<double>(1e20,1e20,1e20,-1e20,-1e20,-1e20);

      numVertices = _qVB->GetSize();
      pVertexAttribute = _qVB->GetVertexAttributeBuffer(Vertex);

      if (pVertexAttribute)
      {
         if (pVertexAttribute->GetAttributeDataType() == 1) // FLOAT list
         {
            pData = (float*)pVertexAttribute->GetDataPtr();
            if (!pData)
            {
               assert(false); // no data ???
               return;
            }
            int components = pVertexAttribute->GetNumComponents();
            if (components>=3) // bounding box requires 3D geometry! (x,y,z)
            {
               for (int j=0;j<numVertices;j++)
               {
                  x = double(pData[components*j] + _vOffset.x);
                  y = double(pData[components*j+1] + _vOffset.y);
                  z = double(pData[components*j+2] + _vOffset.z);

                  if (_BoundingBox.MaxEdge.x < x) _BoundingBox.MaxEdge.x = x;
                  if (_BoundingBox.MaxEdge.y < y) _BoundingBox.MaxEdge.y = y;
                  if (_BoundingBox.MaxEdge.z < z) _BoundingBox.MaxEdge.z = z;

                  if (_BoundingBox.MinEdge.x > x) _BoundingBox.MinEdge.x = x;
                  if (_BoundingBox.MinEdge.y > y) _BoundingBox.MinEdge.y = y;
                  if (_BoundingBox.MinEdge.z > z) _BoundingBox.MinEdge.z = z;
               }
            }
            else
            {
               assert(false); // BOUNDING BOX REQUIRES 3D GEOMETRY
            }
         }
         else
         {
            assert(false); // type is not supported!!
         }
      }
   }
}

//-----------------------------------------------------------------------------

void VirtualGlobeTerrainBlock::CalcLocation()
{
   double x0,y0,x1,y1;
   double x_cart, y_cart, z_cart;
   double xg, yg;
   GeoCoord g;
   _qQuadtree->QuadKeyToMercatorCoord(_sQuadcode, x0, y0, x1, y1);

   //---- Calc First Point -----
   xg = x0; yg= y0;
   _qQuadtree->MercatorToWGS84(xg, yg);
   _lng0 = xg;
   _lat1 = yg;
   g.SetLongitude(xg);
   g.SetLatitude(yg);
   g.SetEllipsoidHeight(0);
   g.ToCartesian(&x_cart, &y_cart, &z_cart);
   vec3<double> p0(x_cart, y_cart, z_cart);

   //---- Calc Second Point -----
   xg = x1; yg= y0;
   _qQuadtree->MercatorToWGS84(xg, yg);

   g.SetLongitude(xg);
   g.SetLatitude(yg);
   g.SetEllipsoidHeight(0);
   g.ToCartesian(&x_cart, &y_cart, &z_cart);
   vec3<double> p1(x_cart, y_cart, z_cart);
   xg = x1; yg= y1;
   _qQuadtree->MercatorToWGS84(xg, yg);
   _lng1 = xg;
   _lat0 = yg;
   
   //---- Calc Third Point -----
   xg = x1; yg= y1;
   _qQuadtree->MercatorToWGS84(xg, yg);

   g.SetLongitude(xg);
   g.SetLatitude(yg);
   g.SetEllipsoidHeight(0);
   g.ToCartesian(&x_cart, &y_cart, &z_cart);
   vec3<double> p2(x_cart, y_cart, z_cart);
   

   _dBlockSize = length<double>(p0-p1);
   
   // Calculate Ellipsoid-Normal of this cell: 
   vec3<double> u =  p1 - p0;
   vec3<double> v =  p2 - p0;

   _vNormal = -cross_dbl(u,v);
   _vNormal = normalize_dbl(_vNormal);
}

//-----------------------------------------------------------------------------

bool VirtualGlobeTerrainBlock::IsInside(double lng, double lat)
{
   return (lng>=_lng0 && lng<=_lng1 && lat>=_lat0 && lat<=_lat1);
}



