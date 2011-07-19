/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _LAYERFACTORY_H
#define _LAYERFACTORY_H

#include "og.h"
#include "IImageLayer.h"
#include "IElevationLayer.h"
#include <boost/shared_ptr.hpp>

/*static*/ class OPENGLOBE_API LayerFactory 
{
public:
   static boost::shared_ptr<IImageLayer> CreateImageLayeri3D(const std::string& server, const std::string& layername, boost::function<void(IImageLayer*)> cbNotify); 
   static boost::shared_ptr<IImageLayer> CreateImagerLayerOpenStreetMap(const std::string& base, size_t maxdepth);
};


#endif
