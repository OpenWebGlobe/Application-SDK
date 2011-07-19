/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "VirtualGlobeRenderer.h"
//#include "nodes/i3dNodeVirtualGlobe.h"
//#include "TileDownload.h"
#include <math/viewfrustum.h>
//#include "TriangleIterator.h"
#include <string/StringUtils.h>
#include <string/FilenameUtils.h>
#include <system/Timer.h>
#include <GL/glew.h>

//------------------------------------------------------------------------------

VirtualGlobeRenderer::VirtualGlobeRenderer()
{  
   _quadtree = boost::shared_ptr<MercatorQuadtree>(new MercatorQuadtree());
   _nMaxLod = 1;      
   _dQuality = 1.0;      
}

//------------------------------------------------------------------------------

VirtualGlobeRenderer::~VirtualGlobeRenderer()
{
}

//------------------------------------------------------------------------------

void VirtualGlobeRenderer::SetLayers(std::vector< boost::shared_ptr<IImageLayer> >& lstImgLayers)
{
   _lstImageLayers = lstImgLayers;

   // (re)create cache:
   _pCache = boost::shared_ptr<VirtualGlobeCache>(new VirtualGlobeCache(_lstImageLayers, _lstElevationLayers, _quadtree, 1000));
}

//------------------------------------------------------------------------------

bool VirtualGlobeRenderer::_TestLayers()
{
   _nMaxLod = 0;
   std::vector< boost::shared_ptr<IImageLayer> >::iterator it=_lstImageLayers.begin();

   size_t cnt = 0;
   
   while (it != _lstImageLayers.end())
   {
      if ((*it)->Failed())
      {
         //this layer failed, remove it!!
         it = _lstImageLayers.erase(it);
      }
      else
      {
         if ((*it)->Ready())
         {
            _nMaxLod = math::Max<unsigned int>(_nMaxLod, (*it)->GetMaxLod());
            ++cnt;
         }
         ++it;
      }
   }
   
   return (cnt == _lstImageLayers.size() && _lstImageLayers.size()>0);
}   
   

//------------------------------------------------------------------------------

void VirtualGlobeRenderer::Render(IRenderEngine* pEngine, const mat4<double>& mView, const mat4<double>& mModel, const mat4<double>& mModelView, const mat4<double>& mProjection, unsigned int nWidth, unsigned int nHeight, const VirtualGlobeRenderOptions& oRenderOptions)
{
   if (!_pCache)  // Test if cache is already created.
      return;

   _pCache->SetRenderEngine(pEngine);

   if (!_TestLayers()) // Test if layers are ready before starting downloads...
      return;

   /*int cnt=0;
   for (int x=0;x<16;x++)
   {
      for (int y=0;y<16;y++)
      {
         _block[cnt] = _pCache->RequestBlock(_quadtree->TileCoordToQuadkey(x,y,4));
         cnt++;
      }
   }
   
   for (int i=0;i<256;i++)
   {
      if (_block[i]->IsAvailable())
      {
         _block[i]->Render(pEngine, mModel, mView, _pCache);
      }
   }*/

    // Store Matrices
   _mView = mView;
   _mModel = mModel;
   _mModelView = mModelView;
   _mProjection = mProjection;
   _mModelViewProjection = mProjection * mModelView;
   _nWidth = nWidth;
   _nHeight = nHeight;

   // Get Camera Position:
   mat4<double> mViewInverse = mView.Inverse();
   _vCameraPosition = mViewInverse.vec3mul(vec3<double>(0,0,0));

   //--------------------------------------------------------------------------
   // Convert Camera Position to WGS84 and to Mercator Projection
   GeoCoord gcCam;
   gcCam.FromCartesian(_vCameraPosition.x, _vCameraPosition.y, _vCameraPosition.z);
   _vCameraPositionWGS84.x = gcCam.GetLongitude();
   _vCameraPositionWGS84.y = gcCam.GetLatitude();
   _vCameraPositionWGS84.z = gcCam.GetEllipsoidHeight();
   _vCameraPositionMercator.x = _vCameraPositionWGS84.x;
   _vCameraPositionMercator.y = _vCameraPositionWGS84.y;
   _quadtree->WGS84ToMercator(_vCameraPositionMercator.x, _vCameraPositionMercator.y);
   //--------------------------------------------------------------------------

   _oFrustum.Setup(mModelView, mProjection);
   _lstFrustum.clear();

   // Retrieve basic blocks
   _block[0] = _pCache->RequestBlock("0");
   _block[1] = _pCache->RequestBlock("1");
   _block[2] = _pCache->RequestBlock("2");
   _block[3] = _pCache->RequestBlock("3");

   if (!_block[0]->IsAvailable() || 
       !_block[1]->IsAvailable() ||
       !_block[2]->IsAvailable() ||
       !_block[3]->IsAvailable())
   {
      return;   // first 4 blocks must be available. Keep waiting!
   }


   _lstFrustum.push_back(_block[0]);
   _lstFrustum.push_back(_block[1]);
   _lstFrustum.push_back(_block[2]);
   _lstFrustum.push_back(_block[3]);

   _Divide();  // Subdivide Planet
   _Optimize(); // Optimize Planet: Remove Hidden Tiles!

   std::list<boost::shared_ptr<VirtualGlobeTerrainBlock> >::iterator it = _lstFrustum.begin();

   //glPolygonMode(GL_FRONT, GL_LINE);
   //glPolygonMode(GL_BACK, GL_LINE);


   while (it != _lstFrustum.end())
   {
      if ((*it)->GetLod())
      {  
         (*it)->Render(mModel, mView, _pCache);
      }
      ++it;
   }


   glPushAttrib(GL_ALL_ATTRIB_BITS);
   glDisable(GL_TEXTURE_2D);
   
   glPushMatrix();
   
   glViewport( 0, 0, _nWidth, _nHeight );
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, _nWidth, 0, _nHeight, 0, 1000); 
   
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   
   it = _lstFrustum.begin();
   while (it != _lstFrustum.end())
   {
      double x0,y0,x1,y1;
      _quadtree->QuadKeyToNormalizedCoord( (*it)->GetQuadcode(), x0, y0, x1, y1);
      
      glLineWidth(1.0);
   
      glColor4d(0, 0, 0, 1);
      glBegin(GL_LINE_LOOP);
         glVertex2d(x0*600.0f+1,y0*600.0f+1);
         glVertex2d(x1*600.0f+1,y0*600.0f+1);
         glVertex2d(x1*600.0f+1,y1*600.0f+1);
         glVertex2d(x0*600.0f+1,y1*600.0f+1);
      glEnd();
   
      glColor4d(255, 255, 255, 1);
      glBegin(GL_LINE_LOOP);
         glVertex2d(x0*600.0f,y0*600.0f);
         glVertex2d(x1*600.0f,y0*600.0f);
         glVertex2d(x1*600.0f,y1*600.0f);
         glVertex2d(x0*600.0f,y1*600.0f);
      glEnd();
   
   
      it++;
   }
   
   
   glPopMatrix();
   glPopAttrib();
   
   //pSceneGraph->PrintText(L"Hello World", 0, 0, 255,255,255);

   

}

//------------------------------------------------------------------------------

void VirtualGlobeRenderer::_Divide()
{
   std::list<boost::shared_ptr<VirtualGlobeTerrainBlock> >::iterator it;
   it = _lstFrustum.begin();

   while (it!=_lstFrustum.end())
   {
      _SubDivideElement(it);
   }
}

//------------------------------------------------------------------------------

void VirtualGlobeRenderer::_SubDivideElement(std::list<boost::shared_ptr<VirtualGlobeTerrainBlock> >::iterator& it)
{
   if ((*it)->IsAvailable())
   {
      if (_CalcErrorMetric(*it))
      {
         boost::shared_ptr<VirtualGlobeTerrainBlock> qChildBlock0 = _pCache->RequestBlock((*it)->GetQuadcode()+"0");
         boost::shared_ptr<VirtualGlobeTerrainBlock> qChildBlock1 = _pCache->RequestBlock((*it)->GetQuadcode()+"1");
         boost::shared_ptr<VirtualGlobeTerrainBlock> qChildBlock2 = _pCache->RequestBlock((*it)->GetQuadcode()+"2");
         boost::shared_ptr<VirtualGlobeTerrainBlock> qChildBlock3 = _pCache->RequestBlock((*it)->GetQuadcode()+"3");

         if (qChildBlock0->IsAvailable() && qChildBlock1->IsAvailable() && qChildBlock2->IsAvailable() && qChildBlock3->IsAvailable())
         {
            it = _lstFrustum.erase(it);
            _lstFrustum.push_back(qChildBlock0);
            _lstFrustum.push_back(qChildBlock1);
            _lstFrustum.push_back(qChildBlock2);
            _lstFrustum.push_back(qChildBlock3);
            return;
         }
      }
   }

   it++;
}

//------------------------------------------------------------------------------

bool VirtualGlobeRenderer::_CalcErrorMetric(const boost::shared_ptr<VirtualGlobeTerrainBlock>& qBlock)
{
   const int min_depth = 3;
   const int max_depth = 23;

   bool bVisible = true;

   int nDepth = qBlock->GetQuadcode().length();

   if (nDepth>max_depth)
   {
      return false;
   }

   if (nDepth<min_depth)
      return true;
        
   if (_nMaxLod <= nDepth)  // highest available LOD is reached!
   {
      return false;
   }

   vec3<double> vHitPoint;
   double dist = qBlock->CalcDistanceTo(_vCameraPosition, vHitPoint);
   double dDelta = qBlock->GetPixelSize(_mModelViewProjection, _nWidth, _nHeight);
   double dCell = qBlock->GetBlockSize();


   // test Visibility...
   if (nDepth>=min_depth)
   {
      bVisible = _FrustumTest(qBlock->GetBoundingBox(), vHitPoint);
      bVisible = bVisible && _SurfaceNormalTest(qBlock);
   }

   // error metric
   double error = _dQuality * dCell / dist;

   if (error>1.0 || nDepth<min_depth)
   {
      return bVisible;
   }
   
   return false;
}

//------------------------------------------------------------------------------

bool VirtualGlobeRenderer::_SurfaceNormalTest(const boost::shared_ptr<VirtualGlobeTerrainBlock>& qBlock)
{
      const vec3<double>& norm = qBlock->GetNormal();
      const vec3<double>& cent = qBlock->GetCenter();
      vec3<double> vDir = _vCameraPosition - cent;
      normalize<double>(vDir);
      
      double d = dot<double>(vDir, norm);

      return d>=0;
}

//------------------------------------------------------------------------------

void VirtualGlobeRenderer::_Optimize()
{
   std::list<boost::shared_ptr<VirtualGlobeTerrainBlock> >::iterator it;
   it = _lstFrustum.begin();

   while (it!=_lstFrustum.end())
   {
      const boost::shared_ptr<VirtualGlobeTerrainBlock>& b = (*it);
      
      const vec3<double>& norm = b->GetNormal();
      const vec3<double>& cent = b->GetCenter();
      vec3<double> vDir = _vCameraPosition - cent;
      normalize<double>(vDir);
      
      double d = dot<double>(vDir, norm);
     
      if (d < 0)
      {
         it = _lstFrustum.erase(it);
      }
      else
      {
         it++;
      }
   }
}

//------------------------------------------------------------------------------

bool VirtualGlobeRenderer::_FrustumTest(const aabb3<double>& oBoundingBox,  const vec3<double>& hitPoint)
{
   aabb3<double> oBBox = oBoundingBox;
   vec3<double> vCamPos = _vCameraPosition;

   return _oFrustum.TestBox(oBBox);
}

//------------------------------------------------------------------------------

#ifdef NEW_RENDERER

#define MAX_TIME_BUFFERCREATION 0

size_t VirtualGlobeRenderer::_numBlocks = 0;

//-----------------------------------------------------------------------------
// VIRTUAL GLOBE RENDERER
//-----------------------------------------------------------------------------

VirtualGlobeRenderer::VirtualGlobeRenderer()
{
   //_qCache = boost::shared_ptr<VirtualGlobeCache>(new VirtualGlobeCache());
   _dQuality = 1.0;
   _bOverlay = false;
   _lasttick = Timer::getRealTime();
   _bElevationInterpolation = false;
   _numBlocks = 0;
}

//-----------------------------------------------------------------------------

VirtualGlobeRenderer::~VirtualGlobeRenderer()
{
   _numBlocks = 0;
}

//-----------------------------------------------------------------------------

void VirtualGlobeRenderer::SetQuadtree(boost::shared_ptr<MercatorQuadtree>& qQuadtree)
{
   _qQuadtree = qQuadtree;
}

//-----------------------------------------------------------------------------

void VirtualGlobeRenderer::SetChannels( const std::list<boost::shared_ptr<TreeListNode> >& lstElevations, const std::list<boost::shared_ptr<TreeListNode> >& lstImages, unsigned int nCacheSize, int nForceThreads)
{
   // Recreate Cache!
   _qCache = boost::shared_ptr<VirtualGlobeCache>(new VirtualGlobeCache(nCacheSize, nForceThreads));

   _lstImages = lstImages;
   _lstElevations = lstElevations;
   _lstTileReader.clear();
   _lstElevationReader.clear();

   //--------------------------------------------------------------------------
   // Create Elevation Reader-List

   std::list<boost::shared_ptr<TreeListNode> >::iterator elvit = _lstElevations.begin();
   
   while (elvit != _lstElevations.end())
   {
      ElevationChannel* pElevationChannel = dynamic_cast<ElevationChannel*>((*elvit).get());
      if (pElevationChannel)
      {
         boost::shared_ptr<ElevationTileFileSystemReader> qElevationReader = boost::shared_ptr<ElevationTileFileSystemReader>(new ElevationTileFileSystemReader);

         std::wstring sElevationPath = pElevationChannel->GetDatasetPath();
         std::wstring sElevationName = pElevationChannel->GetDatasetName();
         qElevationReader->Open(sElevationPath, sElevationName);
         
         _lstElevationReader.push_back(qElevationReader);
      }
     
      elvit++;
   }
   //--------------------------------------------------------------------------

   std::list<boost::shared_ptr<TreeListNode> >::iterator it = _lstImages.begin();

   while (it != _lstImages.end())
   {
      ImageChannel* pImageChannel = dynamic_cast<ImageChannel*>((*it).get());
      if (pImageChannel)
      {
         std::wstring sName = pImageChannel->GetDatasetName();
         std::wstring sPath = pImageChannel->GetDatasetPath();

         if (sName.size()>7 && StringUtils::Left(sName,7)==L"service")
         {
            boost::shared_ptr<ImageTileFileSystemReader> qReader;
            qReader = boost::shared_ptr<ImageTileFileSystemReader>(new ImageTileFileSystemReader());
            if (qReader)
            {
               if (qReader->Open(sPath, sName, pImageChannel->GetTransparency(), pImageChannel->GetMaxLod()))
               {
                  _lstTileReader.push_back(qReader);
               }
            }
         }
         else if (sName.size()>0 && sPath.size()>0)
         {
            boost::shared_ptr<ImageTileFileSystemReader> qReader;
            qReader = boost::shared_ptr<ImageTileFileSystemReader>(new ImageTileFileSystemReader());
            if (qReader)
            {
               if (qReader->Open(sPath, sName, pImageChannel->GetTransparency(), pImageChannel->GetMaxLod()))
               {
                  DatasetInfo* pDatasetInfo = qReader->GetDatasetInfo();
                  if (pDatasetInfo)
                  {
                     pImageChannel->SetLayerName(pDatasetInfo->GetLayerName());
                     pImageChannel->SetLayerCopyright(pDatasetInfo->GetLayerCopyright());
                     pImageChannel->SetMaxLod(pDatasetInfo->GetLevelOfDetail());

                     double lng, lat;
                     pDatasetInfo->GetCenter(lng,lat);
                     pImageChannel->SetDatasetCenter(lng, lat);


                     double ulx, uly,  lrx,  lry;
                     unsigned int nTileSize;
                     unsigned int nLod;
                     unsigned int tilex0,  tiley0,  tilex1, tiley1;
                     std::string sTileFormatUTF8;
                     unsigned int nSRS;
                     unsigned int numTilesX, numTilesY;

                     nTileSize = pDatasetInfo->GetTileSize();
                     pDatasetInfo->GetBoundingBox(ulx, uly, lrx, lry);
                     pDatasetInfo->GetBounds(nLod, tilex0, tiley0, tilex1, tiley1);
                     sTileFormatUTF8 = pDatasetInfo->GetTileFormat();
                     nSRS = pDatasetInfo->GetSRS();
                     pDatasetInfo->GetTileLayout(numTilesX, numTilesY);

                     pImageChannel->SetTileSize(nTileSize);
                     pImageChannel->SetBoundingBox(ulx, uly, lrx, lry);
                     pImageChannel->SetBounds(tilex0, tiley0, tilex1, tiley1);
                     pImageChannel->SetTileFormat(sTileFormatUTF8);
                     pImageChannel->SetSRS(nSRS);
                     pImageChannel->SetTileLayout(numTilesX, numTilesY);

                     // Add Dataset:
                     _lstTileReader.push_back(qReader);
                  }
               }
            }
         }
         //pImageChannel->RefreshConnections();
      }
      it++;
   }

}

//-----------------------------------------------------------------------------

void VirtualGlobeRenderer::Render(i3dGraph* pSceneGraph, const mat4<double>& mView, const mat4<double>& mModel, const mat4<double>& mModelView, const mat4<double>& mProjection, unsigned int nWidth, unsigned int nHeight, const VirtualGlobeRenderOptions& oRenderOptions)
{
   if (!_qQuadtree)
   {
      assert(false); // Make sure quadtree is properly set!
      return;
   }

   // Store Matrices
   _mView = mView;
   _mModel = mModel;
   _mModelView = mModelView;
   _mProjection = mProjection;
   _mModelViewProjection = mProjection * mModelView;
   _nWidth = nWidth;
   _nHeight = nHeight;

   mat4<double> mViewInverse = mView.Inverse();
   _vCameraPosition = mViewInverse.vec3mul(vec3<double>(0,0,0));

   //--------------------------------------------------------------------------
   // Convert Camera Position to WGS84 and to Mercator Projection
   GeoCoord gcCam;
   gcCam.FromCartesian(_vCameraPosition.x, _vCameraPosition.y, _vCameraPosition.z);
   _vCameraPositionWGS84.x = gcCam.GetLongitude();
   _vCameraPositionWGS84.y = gcCam.GetLatitude();
   _vCameraPositionWGS84.z = gcCam.GetEllipsoidHeight();
   _vCameraPositionMercator.x = _vCameraPositionWGS84.x;
   _vCameraPositionMercator.y = _vCameraPositionWGS84.y;
   _qQuadtree->WGS84ToMercator(_vCameraPositionMercator.x, _vCameraPositionMercator.y);
   //--------------------------------------------------------------------------

   _oFrustum.Setup(mModelView, mProjection);

   _qCache->UpdateCameraPosition(_vCameraPositionMercator);


   unsigned int now = Timer::getRealTime();
   double dTimeDelta = double(now - _lasttick);
  
   // Process incoming tiles (one tile per X milliseconds)
   if (dTimeDelta>MAX_TIME_BUFFERCREATION)
   {
      _lasttick=now;
      _qCache->Process();
   }

   //if (_lstTileReader.size()>0)
   {
      // Create View Frustum for culling
      ViewFrustum<double> oFrustum(mModelView, mProjection);
      _lstFrustum.clear();

      if (_qCache->RequiresUpdate())
      {
         if (pSceneGraph)
            pSceneGraph->RequestFrameUpdate();
      }

      // Always request top block:
      boost::shared_ptr<VirtualGlobeTerrainBlock> qBlock0 = _qCache->RequestBlock("0", _qQuadtree, _lstTileReader, _lstElevationReader);
      boost::shared_ptr<VirtualGlobeTerrainBlock> qBlock1 = _qCache->RequestBlock("1", _qQuadtree, _lstTileReader, _lstElevationReader);
      boost::shared_ptr<VirtualGlobeTerrainBlock> qBlock2 = _qCache->RequestBlock("2", _qQuadtree, _lstTileReader, _lstElevationReader);
      boost::shared_ptr<VirtualGlobeTerrainBlock> qBlock3 = _qCache->RequestBlock("3", _qQuadtree, _lstTileReader, _lstElevationReader);

      // the first 4 blocks are required, otherwise nothing is displayed!
      if (  !qBlock0->IsAvailable() || !qBlock1->IsAvailable() || !qBlock2->IsAvailable() || !qBlock3->IsAvailable())
      {
         return;
      }

      _lstFrustum.push_back(qBlock0);
      _lstFrustum.push_back(qBlock1);
      _lstFrustum.push_back(qBlock2);
      _lstFrustum.push_back(qBlock3);

      _Divide();  // Subdivide Planet
      
      _Optimize(); // Optimize Planet: Remove Hidden Tiles!
      
      // Render Frustum Array

      glPushAttrib(GL_ALL_ATTRIB_BITS);

      if (oRenderOptions.bWireframe)
      {
         glPolygonMode(GL_FRONT, GL_LINE);
         glPolygonMode(GL_BACK, GL_LINE);
      }
      else
      {
         glPolygonMode(GL_FRONT, GL_FILL);
         glPolygonMode(GL_BACK, GL_FILL);
      }
       
      if (!_bOverlay)    
      {  
         std::list<boost::shared_ptr<VirtualGlobeTerrainBlock> >::iterator it;
         it = _lstFrustum.begin();
         while (it != _lstFrustum.end())
         {
            (*it)->Render(_mModel, _mView, _qCache, oRenderOptions);
            it++;
         }
      }

      glPopAttrib();
   }


   // Draw Quadtree Overlay:
   if (_bOverlay)
   {
      glPushAttrib(GL_ALL_ATTRIB_BITS);
      glDisable(GL_TEXTURE_2D);

      glPushMatrix();
      GLsizei nWidth, nHeight;
     
      glViewport( 0, 0, _nWidth, _nHeight );
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0, _nWidth, 0, _nHeight, 0, 1000); 

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();


      std::list<boost::shared_ptr<VirtualGlobeTerrainBlock> >::iterator it;
      it = _lstFrustum.begin();
      while (it != _lstFrustum.end())
      {
         double x0,y0,x1,y1;
         _qQuadtree->QuadKeyToNormalizedCoord( (*it)->GetQuadcode(), x0, y0, x1, y1);
         
         glLineWidth(1.0);

         glColor4d(0, 0, 0, 1);
         glBegin(GL_LINE_LOOP);
            glVertex2d(x0*600.0f+1,y0*600.0f+1);
            glVertex2d(x1*600.0f+1,y0*600.0f+1);
            glVertex2d(x1*600.0f+1,y1*600.0f+1);
            glVertex2d(x0*600.0f+1,y1*600.0f+1);
         glEnd();

         /*glColor4d(255, 255, 255, 1);
         glBegin(GL_LINE_LOOP);
            glVertex2d(x0*600.0f,y0*600.0f);
            glVertex2d(x1*600.0f,y0*600.0f);
            glVertex2d(x1*600.0f,y1*600.0f);
            glVertex2d(x0*600.0f,y1*600.0f);
         glEnd();*/


         it++;
      }


      glPopMatrix();
      glPopAttrib();

      //pSceneGraph->PrintText(L"Hello World", 0, 0, 255,255,255);

   }
   
   _numBlocks = _lstFrustum.size();

}



//-----------------------------------------------------------------------------

void VirtualGlobeRenderer::_SubDivideElement(std::list<boost::shared_ptr<VirtualGlobeTerrainBlock> >::iterator& it)
{
   if ((*it)->IsAvailable())
   {
      if (_CalcErrorMetric(*it))
      {
         boost::shared_ptr<VirtualGlobeTerrainBlock> qChildBlock0 = _qCache->RequestBlock((*it)->GetQuadcode()+"0", _qQuadtree, _lstTileReader, _lstElevationReader);
         boost::shared_ptr<VirtualGlobeTerrainBlock> qChildBlock1 = _qCache->RequestBlock((*it)->GetQuadcode()+"1", _qQuadtree, _lstTileReader, _lstElevationReader);
         boost::shared_ptr<VirtualGlobeTerrainBlock> qChildBlock2 = _qCache->RequestBlock((*it)->GetQuadcode()+"2", _qQuadtree, _lstTileReader, _lstElevationReader);
         boost::shared_ptr<VirtualGlobeTerrainBlock> qChildBlock3 = _qCache->RequestBlock((*it)->GetQuadcode()+"3", _qQuadtree, _lstTileReader, _lstElevationReader);

         if (qChildBlock0->IsAvailable() && qChildBlock1->IsAvailable() && qChildBlock2->IsAvailable() && qChildBlock3->IsAvailable())
         {
            it = _lstFrustum.erase(it);
            _lstFrustum.push_back(qChildBlock0);
            _lstFrustum.push_back(qChildBlock1);
            _lstFrustum.push_back(qChildBlock2);
            _lstFrustum.push_back(qChildBlock3);
            return;
         }
      }
   }

   it++;
}

//-----------------------------------------------------------------------------

void VirtualGlobeRenderer::_Divide()
{
   std::list<boost::shared_ptr<VirtualGlobeTerrainBlock> >::iterator it;
      it = _lstFrustum.begin();

   while (it!=_lstFrustum.end())
   {
      _SubDivideElement(it);
   }
}

//-----------------------------------------------------------------------------

void VirtualGlobeRenderer::_Optimize()
{
   std::list<boost::shared_ptr<VirtualGlobeTerrainBlock> >::iterator it;
   it = _lstFrustum.begin();

   while (it!=_lstFrustum.end())
   {
      const boost::shared_ptr<VirtualGlobeTerrainBlock>& b = (*it);
      
      const vec3<double>& norm = b->GetNormal();
      const vec3<double>& cent = b->GetCenter();
      vec3<double> vDir = _vCameraPosition - cent;
      normalize<double>(vDir);
      
      double d = dot<double>(vDir, norm);
     
      if (d < 0)
      {
         it = _lstFrustum.erase(it);
      }
      else
      {
         it++;
      }
   }
}

//-----------------------------------------------------------------------------

// precondition: {qBlock is available when CalcErrorMetric is called}
bool VirtualGlobeRenderer::_CalcErrorMetric(const boost::shared_ptr<VirtualGlobeTerrainBlock>& qBlock)
{
   const int min_depth = 4;
   const int max_depth = 23;

   bool bVisible = true;

   int nDepth = qBlock->GetQuadcode().length();

   if (nDepth>max_depth)
      return false;

   if (nDepth<min_depth)
      return true;

   if (!_bElevationInterpolation)
   {
      std::list<boost::shared_ptr<ElevationTileFileSystemReader> >::iterator it = _lstElevationReader.begin();
      int maxlod = 4;
      while (it != _lstElevationReader.end())
      {
           int lod = (*it)->GetLevelOfDetail(); 
           maxlod = math::Max<int>(maxlod, lod);
           it++;
      }
        
      if (maxlod <= nDepth)
      {
         return false;
      }
   }


   vec3<double> vHitPoint;
   double dist = qBlock->CalcDistanceTo(_vCameraPosition, vHitPoint);
   double dDelta = qBlock->GetPixelSize(_mModelViewProjection, _nWidth, _nHeight);
   double dCell = qBlock->GetBlockSize();

  
   double quality = _dQuality;
  


  
   
   
   // test Visibility...
   if (nDepth>=min_depth)
   {
      bVisible = _TestVisibility(qBlock->GetBoundingBox(), vHitPoint);
   }

   double error = quality * dCell / dist;

   if (error>1.0 || nDepth<min_depth)
   {
      return bVisible;
   }
   
   

   
   

   return false;
}

//-----------------------------------------------------------------------------

bool VirtualGlobeRenderer::_TestVisibility(const aabb3<double>& oBoundingBox,  const vec3<double>& hitPoint)
{
   // Test if "side" of earth is visible

   aabb3<double> oBBox = oBoundingBox;
   vec3<double> vCamPos = _vCameraPosition;

   bool bVisibleInFrustum = _oFrustum.TestBox(oBBox);

   if (!bVisibleInFrustum)
      return false;

   vec3<double> vEarthSurface = hitPoint;

   GeoCoord gcSurface;
   gcSurface.FromCartesian(vEarthSurface.x, vEarthSurface.y, vEarthSurface.z);

   vec3<double> vSurfaceNormal;
   gcSurface.CalcNormal(vSurfaceNormal);
   vSurfaceNormal.Normalize();
   vec3<double> vCamNormal = normalize_dbl(vCamPos);
   double angle = dot_dbl(vCamNormal, vSurfaceNormal);
   angle = fabs(acos(angle));

   if (angle < 0.5*AGEPI)
      return true;

   return false;
}

//-----------------------------------------------------------------------------

void VirtualGlobeRenderer::OnTerrainGetLod(double lng, double lat, int& lod)
{
   lod = 0;

   std::list<boost::shared_ptr<VirtualGlobeTerrainBlock> >::iterator it;
   it = _lstFrustum.begin();
   while (it != _lstFrustum.end())
   {
      if ((*it)->IsInside(lng, lat))
      {
         lod = (*it)->GetLod();
         return;
      }

      it++;
   }
}

//-----------------------------------------------------------------------------

void VirtualGlobeRenderer::OnTerrainPickTest(vec3<double>& vPos, vec3<double>& vDir, TerrainPickResult& oResult)
{
   oResult.SetRayPos(vPos);
   oResult.SetRayDir(vDir);

   // For Triangle Intersection:
   double orig[3];
   double dir[3];
   double vert0[3];
   double vert1[3];
   double vert2[3];
   double t,u,v;
   double tmin;
   vec3<double> hitpoint;

   orig[0] = vPos.x; orig[1] = vPos.y; orig[2] = vPos.z;
   dir[0] = vDir.x;  dir[1] = vDir.y;  dir[2] = vDir.z;


   // Triangle Iterator and Result:
   boost::shared_ptr<TriangleResult> triRes;

   int lod = -1;
   std::list<boost::shared_ptr<VirtualGlobeTerrainBlock> >::iterator it;
   it = _lstFrustum.begin();
   while (it != _lstFrustum.end())
   {
      lod = (*it)->GetLod();
      aabb3<double>&  bbox = (*it)->GetBoundingBox(); // Bounding Box of Tile (World Coord)
      vec3<double>&   vOffset = (*it)->GetOffset();   // virtual camera offset

      if ((*it)->GetVertexBuffer() && (*it)->GetIndexBuffer())
      {
         if (bbox.IntersectWithRay(vPos, vDir, tmin, hitpoint))
         {
            TriangleIterator triIt;
            if ((*it)->IsTin())
            {
               triIt.QueryAll(GL_TRIANGLES, (*it)->GetVertexBuffer(), (*it)->GetIndexBuffer());
            }
            else
            {
               triIt.QueryAll(GL_TRIANGLE_STRIP, (*it)->GetVertexBuffer(), (*it)->GetIndexBuffer());
            }

            while (triIt.GetNextTriangle(triRes))
            {
               if (!triRes->bIsDegenerate)
               {
                  vec3<double> a,b,c;
                  if (triRes->A.GetPosition(a) &&
                     triRes->B.GetPosition(b) && 
                     triRes->C.GetPosition(c))
                  {
                     a = a + vOffset;
                     b = b + vOffset;
                     c = c + vOffset;

                     vert0[0] = a.x; vert0[1] = a.y; vert0[2] = a.z;
                     vert1[0] = b.x; vert1[1] = b.y; vert1[2] = b.z;
                     vert2[0] = c.x; vert2[1] = c.y; vert2[2] = c.z;

                     if (math::IntersectTriangle( orig, dir, vert0, vert1, vert2, &t, &u, &v))
                     {
                        oResult.AddPickResult(t, lod);
                     }
                  }
               }
               else
               {
                  volatile int i = 0;
               }
            }
         }
      }

      it++;
   }

}

//------------------------------------------------------------------------------

#endif
