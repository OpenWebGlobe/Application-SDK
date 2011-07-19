/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/


#ifndef _VIRTUALGLOBECACHE_H
#define _VIRTUALGLOBECACHE_H

#include "og.h"
#include "data/DataCache.h"
#include "renderer/IRenderEngine.h"
#include "geo/VirtualGlobeTerrainBlock.h"
#include "geo/layer/IImageLayer.h"
#include "geo/layer/IElevationLayer.h"
#include "geo/MercatorQuadtree.h"
#include <boost/shared_ptr.hpp>
#include <vector>

class OPENGLOBE_API VirtualGlobeCache
{
public:
   VirtualGlobeCache(std::vector< boost::shared_ptr<IImageLayer> >& lstImg, std::vector< boost::shared_ptr<IElevationLayer> >& lstElv, boost::shared_ptr<MercatorQuadtree> quadtree, size_t cachesize=1000);
   virtual ~VirtualGlobeCache();

   void SetRenderEngine(IRenderEngine* pEngine) {_pEngine = pEngine;}

   // Request Block (async)
   boost::shared_ptr<VirtualGlobeTerrainBlock> RequestBlock(const std::string& quadcode);

   // Get cached block (or 0 if not available!)
   boost::shared_ptr<VirtualGlobeTerrainBlock> GetCachedBlock(const std::string& quadcode);

protected:
   boost::shared_ptr<VirtualGlobeTerrainBlock> _CreateBlock(const std::string& quadcode);

   std::vector< boost::shared_ptr<IImageLayer> >       _lstImg;
   std::vector< boost::shared_ptr<IElevationLayer> >   _lstElv;

private:
   DataCache< std::string, boost::shared_ptr<VirtualGlobeTerrainBlock> >* _cache;
   boost::shared_ptr<MercatorQuadtree>                                    _quadtree;
   IRenderEngine* _pEngine;
};


#endif
