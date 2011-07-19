/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _VIRTUALGLOBE_RENDERER_H
#define _VIRTUALGLOBE_RENDERER_H

//-----------------------------------------------------------------------------


#include "og.h"
#include <math/vec2.h>
#include <math/vec3.h>
#include <math/mat4.h>
#include <math/viewfrustum.h>

#include "renderer/IRenderEngine.h"

#include "geo/VirtualGlobeRenderOptions.h"
#include "geo/VirtualGlobeCache.h"
#include "geo/MercatorQuadtree.h"
#include "scenegraph/PickResult.h"
#include "geo/layer/IImageLayer.h"

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <vector>
#include <ctime>

class OPENGLOBE_API VirtualGlobeRenderer
{
public:
                     VirtualGlobeRenderer();
   virtual           ~VirtualGlobeRenderer();

   void              SetLayers(std::vector< boost::shared_ptr<IImageLayer> >& lstImageLayers);

   void              Render(IRenderEngine* pEngine, const mat4<double>& mView, const mat4<double>& mModel, const mat4<double>& mModelView, const mat4<double>& mProjection, unsigned int nWidth, unsigned int nHeight, const VirtualGlobeRenderOptions& oRenderOptions);

protected:
   void              _Divide();
   void              _Optimize();
   void              _SubDivideElement(std::list<boost::shared_ptr<VirtualGlobeTerrainBlock> >::iterator& it);
   bool              _CalcErrorMetric(const boost::shared_ptr<VirtualGlobeTerrainBlock>& qBlock);
   bool              _FrustumTest(const aabb3<double>& oBoundingBox,  const vec3<double>& hitPoint);
   bool              _SurfaceNormalTest(const boost::shared_ptr<VirtualGlobeTerrainBlock>& qBlock);


   std::list<boost::shared_ptr<VirtualGlobeTerrainBlock> >     _lstFrustum;  // Terrain Blocks in frustum!
   //
   bool              _TestLayers();
   boost::shared_ptr<VirtualGlobeCache>                  _pCache;
   boost::shared_ptr<MercatorQuadtree>                   _quadtree;
   std::vector< boost::shared_ptr<IImageLayer> >         _lstImageLayers;
   std::vector< boost::shared_ptr<IElevationLayer> >     _lstElevationLayers;
   boost::shared_ptr<VirtualGlobeTerrainBlock>           _block[256];

   unsigned int                                          _nMaxLod;


private:
   mat4<double>                                          _mView;
   mat4<double>                                          _mModel;
   mat4<double>                                          _mModelView;
   mat4<double>                                          _mProjection;
   mat4<double>                                          _mModelViewProjection;
   vec3<double>                                          _vCameraPosition;
   vec3<double>                                          _vCameraPositionWGS84;
   vec2<double>                                          _vCameraPositionMercator;
   unsigned int                                          _nWidth; // Screen/Window Width
   unsigned int                                          _nHeight; // Screen/Window Height
   double                                                _dQuality;
   ViewFrustum<double>                                   _oFrustum;
   bool                                                  _bOverlay;

};

//-----------------------------------------------------------------------------

/*class OPENGLOBE_API VirtualGlobeRenderer
{
public:
                     VirtualGlobeRenderer();
   virtual           ~VirtualGlobeRenderer();

   void              SetQuadtree(boost::shared_ptr<MercatorQuadtree>& qQuadtree);

   //void              SetChannels(const std::list<boost::shared_ptr<TreeListNode> >& lstElevations, const std::list<boost::shared_ptr<TreeListNode> >& lstImages, unsigned int nCacheSize, int nForceThreads);

   void              Render(const mat4<double>& mView, const mat4<double>& mModel, const mat4<double>& mModelView, const mat4<double>& mProjection, unsigned int nWidth, unsigned int nHeight, const VirtualGlobeRenderOptions& oRenderOptions);

   void              SetQualityParameter(double dQuality){_dQuality = dQuality;}

   void              EnableOverlay(bool bEnable){_bOverlay = bEnable;}

   void              OnTerrainPickTest(vec3<double>& vPos, vec3<double>& vDir, TerrainPickResult& oResult);
   void              OnTerrainGetLod(double lng, double lat, int& lod);

   void              SetElevationInterpolation(bool bInterpolation){_bElevationInterpolation = bInterpolation;}
   
   //static size_t     GetNumBlocks() {return _numBlocks;}

protected:
   void              _Divide();
   void              _Optimize();
   void              _SubDivideElement(std::list<boost::shared_ptr<VirtualGlobeTerrainBlock> >::iterator& it);
   bool              _CalcErrorMetric(const boost::shared_ptr<VirtualGlobeTerrainBlock>& qBlock);
   bool              _TestVisibility(const aabb3<double>& oBoundingBox,  const vec3<double>& hitPoint);


   //std::list<boost::shared_ptr<TreeListNode> >                 _lstImages;
   //std::list<boost::shared_ptr<TreeListNode> >                 _lstElevations;
   //std::list<boost::shared_ptr<ImageTileFileSystemReader> >    _lstTileReader;
   //std::list<boost::shared_ptr<ElevationTileFileSystemReader> >    _lstElevationReader;
   std::list<boost::shared_ptr<VirtualGlobeTerrainBlock> >     _lstFrustum;

   mat4<double>                                                _mView;
   mat4<double>                                                _mModel;
   mat4<double>                                                _mModelView;
   mat4<double>                                                _mProjection;
   mat4<double>                                                _mModelViewProjection;
   vec3<double>                                                _vCameraPosition;
   vec3<double>                                                _vCameraPositionWGS84;
   vec2<double>                                                _vCameraPositionMercator;
   unsigned int                                                _nWidth; // Screen/Window Width
   unsigned int                                                _nHeight; // Screen/Window Height
   double                                                      _dQuality;
   ViewFrustum<double>                                         _oFrustum;
   bool                                                        _bOverlay;
   bool                                                        _bElevationInterpolation;
   //static size_t _numBlocks;

private:
   boost::shared_ptr<VirtualGlobeCache>                        _qCache;
   boost::shared_ptr<MercatorQuadtree>                         _qQuadtree;
   unsigned int _lasttick;
};
*/

//-----------------------------------------------------------------------------

#endif


