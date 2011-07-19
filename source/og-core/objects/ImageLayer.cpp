   
#include "Camera.h"
#include "ImageLayer.h"
#include "geo/layer/LayerFactory.h"

//---------------------------------------------------------------------------
using namespace OpenGlobe;
BeginPropertyMap(ImageLayer);
   Inherits(Object);
   //XMLPropertyReadOnly(Scene, "width", _width);
EndPropertyMap(ImageLayer);
//---------------------------------------------------------------------------

namespace OpenGlobe
{
   //---------------------------------------------------------------------------
   ImageLayer::ImageLayer()
      : _parentWorld(0)
   {
      _name = "ogImageLayer";
      _eObjectType = Object::IMAGELAYER;

   }
   //---------------------------------------------------------------------------
   ImageLayer::~ImageLayer()
   {

   }
   //---------------------------------------------------------------------------

   void ImageLayer::SetOptions(const OptionVector& options)
   {
      _parentWorld = dynamic_cast<World*>(_pParent);
      
       std::vector<std::pair<std::string, std::string> >::const_iterator it = options.begin();
      
      bool bTileSystemi3d = true;
      bool bTileSystemOSM = false;
      
      std::vector<std::string> servers;
      std::string sLayername;
      
      while (it!=options.end())
      {
         // key: it->first
         // value: it->second  (value os optional, presence of a key may be enough)
      
         if (it->first == "i3d") // i3d server(s)
         {
            // i3d Tile System (this parameter is not necessary)
            bTileSystemi3d = true;
         }
         else if (it->first == "osm")  // Open Street Map Server(s)
         {
            bTileSystemi3d = false;
            bTileSystemOSM = true;
         }
         else if (it->first == "url")
         {
            if (it->second.length()>0)
               servers.push_back(it->second);
         }
         else if (it->first == "layer")
         {
            sLayername = it->second;
         }

         ++it;
      }

      if (bTileSystemi3d)
      {
         if (servers.size()>0 && sLayername.length()>0)
         {
            _pImageLayer = LayerFactory::CreateImageLayeri3D(servers[0], sLayername, 0);
         }
      }
      else if (bTileSystemOSM)
      {
      }
      else
      {
         // not possible!!
      }

      if (_parentWorld && _pImageLayer)
      {
         _parentWorld->AddImageLayer(this);
      }
   }

   //---------------------------------------------------------------------------

   bool ImageLayer::Ready()
   {
      if (_pImageLayer)
      {
         return _pImageLayer->Ready();
      }

      return false;
   }

   //---------------------------------------------------------------------------
   
   bool ImageLayer::Failed()
   {
      if (_pImageLayer)
      {
         return _pImageLayer->Failed();
      }

      return true; // failed because there is no image layer!!
   }
}