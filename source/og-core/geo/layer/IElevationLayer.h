/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _IELEVATIONLAYER_H
#define _IELEVATIONLAYER_H

#include "og.h"
#include "geo/ElevationTile.h"
#include <string>
#include "boost/function.hpp"

class OPENGLOBE_API IElevationLayer
{
public:
    IElevationLayer(){}
    virtual ~IElevationLayer(){}

    // Elevation Layer is ready. Now you can start download requests.
    virtual bool Ready() = 0;

    // Elevation Layer failed and can't be used. Don't try starting download requests, it is pointless!
    virtual bool Failed() = 0;

    // Start download request. The appropriate functions will be called when the tile is ready.
    virtual void RequestTile(
            const std::string& sQuadcode, 
            boost::function<void(const std::string&, ElevationTile*)> cbfReady, 
            boost::function<void(const std::string&)>             cbfFailed) = 0; 

    // Retrieve minimum level of detail for this layer
    virtual int GetMinLod() = 0; 

    // Retrieve maximum level of detail for this layer
    virtual int GetMaxLod() = 0;
    
  
};

#endif
