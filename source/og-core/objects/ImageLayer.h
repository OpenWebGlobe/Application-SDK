
#ifndef _IMAGELAYER_H
#define _IMAGELAYER_H

#include "Object.h"
#include "Scene.h"
#include "objects/World.h"
#include "geo/layer/IImageLayer.h"
#include "boost/shared_ptr.hpp"
#include <list>



namespace OpenGlobe
{
   class OPENGLOBE_API ImageLayer : public OpenGlobe::Object
   {
   public:
      ImageLayer();
      virtual ~ImageLayer();

      virtual void SetOptions(const OptionVector& options);

      bool Failed();
      bool Ready();

      boost::shared_ptr<IImageLayer>   GetImageLayer(){return _pImageLayer;}
   protected:
      World*                           _parentWorld;
      boost::shared_ptr<IImageLayer>   _pImageLayer;
   };

}

#endif
