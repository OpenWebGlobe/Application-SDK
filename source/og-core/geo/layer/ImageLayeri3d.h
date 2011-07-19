/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _IMAGELAYERI3D_H
#define _IMAGELAYERI3D_H

#include "geo/DatasetInfo.h"
#include "geo/layer/IImageLayer.h"
#include "Object.h"

class OPENGLOBE_API ImageLayeri3d : public IImageLayer
{
public:
    ImageLayeri3d();
    virtual ~ImageLayeri3d();

    // Setup from i3d tile server. Once finished the cbfNotify is called. There you have to check if failed or ready.
    void Setup(const std::string& server, const std::string& layer, boost::function<void(IImageLayer*)> cbNotify);

    // Image Layer is ready. Now you can start download requests.
    virtual bool Ready();

    // Image Layer failed and can't be used. Don't try starting download requests, it is pointless!
    virtual bool Failed();

    // Start download request. The appropriate functions will be called when the tile is ready.
    virtual void RequestTile(
            const std::string& sQuadcode, 
            boost::function<void(const std::string&, ImageTile*)> cbfReady, 
            boost::function<void(const std::string&)> cbfFailed); 


    // Retrieve minimum level of detail for this layer
    virtual int GetMinLod(); 

    // Retrieve maximum level of detail for this layer
    virtual int GetMaxLod();

protected:
   void _OnLoadDatasetinfo(const std::string& sFilename, MemoryEntry* pEntry, void* userdata);

   bool _bIsReady;
   bool _bIsFailed;
   DatasetInfo _oDatasetInfo;
   std::string _servername;
   std::string _layername;

   boost::function<void(IImageLayer*)> _cbNotify;

};


#endif
