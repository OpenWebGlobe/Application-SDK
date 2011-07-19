

#include "geo/layer/LayerFactory.h"
#include "geo/layer/ImageLayeri3d.h"
//#include "geo/layer/ImageLayerOSM.h"

//------------------------------------------------------------------------------

boost::shared_ptr<IImageLayer> LayerFactory::CreateImageLayeri3D(const std::string& server, const std::string& layername, boost::function<void(IImageLayer*)> cbNotify)
{
   boost::shared_ptr<IImageLayer> result;

   // creates layer and starts downloading information file from server / filesystem

   ImageLayeri3d* imglayer = new ImageLayeri3d();

   if (imglayer)
   {
      imglayer->Setup(server, layername, cbNotify);
      result = boost::shared_ptr<IImageLayer>(imglayer);
   }

   return result;
}

//------------------------------------------------------------------------------

boost::shared_ptr<IImageLayer> LayerFactory::CreateImagerLayerOpenStreetMap(const std::string& base, size_t maxdepth)
{
   boost::shared_ptr<IImageLayer> result;


   return result;
}

//------------------------------------------------------------------------------
