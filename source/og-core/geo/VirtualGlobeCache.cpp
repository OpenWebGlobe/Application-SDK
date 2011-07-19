/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "VirtualGlobeCache.h"
#include <cassert>

//------------------------------------------------------------------------------

VirtualGlobeCache::VirtualGlobeCache(std::vector< boost::shared_ptr<IImageLayer> >& lstImg, std::vector< boost::shared_ptr<IElevationLayer> >& lstElv, boost::shared_ptr<MercatorQuadtree> quadtree, size_t cachesize)
{
   _pEngine = 0;
   _lstImg = lstImg;
   _lstElv = lstElv;

   boost::function<boost::shared_ptr<VirtualGlobeTerrainBlock>(const std::string&)> fnc;
   fnc = boost::bind( &VirtualGlobeCache::_CreateBlock, this, _1);
   _cache = new DataCache<std::string, boost::shared_ptr<VirtualGlobeTerrainBlock> >(fnc, cachesize);

   _quadtree = quadtree;
}

//------------------------------------------------------------------------------

VirtualGlobeCache::~VirtualGlobeCache()
{
   if (_cache)
   {
      delete _cache;
      _cache = 0;
   }
}

//------------------------------------------------------------------------------


boost::shared_ptr<VirtualGlobeTerrainBlock> VirtualGlobeCache::_CreateBlock(const std::string& quadcode)
{
   // Create Block. Please note that a block request is async. The block is stored but not available.
   boost::shared_ptr<VirtualGlobeTerrainBlock> result;

   result = boost::shared_ptr<VirtualGlobeTerrainBlock>(new VirtualGlobeTerrainBlock(_pEngine, quadcode, _quadtree));

   if (result)
   {
      result->Request(_lstImg, _lstElv);
   }

   return result;
}

//------------------------------------------------------------------------------

boost::shared_ptr<VirtualGlobeTerrainBlock> VirtualGlobeCache::RequestBlock(const std::string& quadcode)
{
   boost::shared_ptr<VirtualGlobeTerrainBlock> result;

   result = _cache->Request(quadcode);
  
   return result;
}

//------------------------------------------------------------------------------

boost::shared_ptr<VirtualGlobeTerrainBlock> VirtualGlobeCache::GetCachedBlock(const std::string& quadcode)
{
   boost::shared_ptr<VirtualGlobeTerrainBlock> result;
   _cache->GetCachedEntry(quadcode, result);
   return result;
}

//------------------------------------------------------------------------------

