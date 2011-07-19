
#ifndef _IMAGE_H
#define _IMAGE_H

#include "Object.h"
#include "Scene.h"
#include <list>


namespace OpenGlobe
{
   class OPENGLOBE_API _Template : public OpenGlobe::Object
   {
   public:
      _Template();
      virtual ~_Template();

      virtual void SetOptions(const OptionVector& options);


   protected:
      Scene*                  _parentScene;
   };

}

#endif
