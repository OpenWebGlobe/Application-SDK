
#ifndef _WORLD_H
#define _WORLD_H

#include "Object.h"
#include "Scene.h"
#include "geo/VirtualGlobeRenderer.h"
#include <list>


namespace OpenGlobe
{
   class OPENGLOBE_API ImageLayer;
   class OPENGLOBE_API ElevationLayer;

   class OPENGLOBE_API World : public OpenGlobe::Object
   {
   public:
      World();
      virtual ~World();

      virtual void SetOptions(const OptionVector& options);

      void AddImageLayer(ImageLayer* pImgLayer);
      void RemoveImageLayer(ImageLayer* pImgLayer);

      // Show World
      void Show();

      // Hide World (stop rendering)
      void Hide();
    

   protected:
      void                       _RecreateWorld();
      Scene*                     _parentScene;
      bool                       _bShow;
      std::list<ImageLayer*>     _lstImgLayers;
      //std::list<ElevationLayer*> _lstElvLayers;
      VirtualGlobeRenderer*      _pRenderer;
   };

}

#endif
