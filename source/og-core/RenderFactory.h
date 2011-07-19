/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

// RenderWindowFactory (platform independent render window factory...)

#ifndef _RENDERFACTORY_H
#define _RENDERFACTORY_H

#include "og.h"
#include "shared_ptr.h"
#include "Object.h"
#include "renderer/IRenderWindow.h"
#include "renderer/IRenderEngine.h"
#include "ObjectFactory.h"

namespace OpenGlobe
{
   class OPENGLOBE_API RenderFactory
   {
   public:
      // Create RenderWindow using specified options
      // options are platform dependent, please consult documentation there
      static shared_ptr<IRenderWindow> RenderWindow(const OptionVector& options);
      
   };

};

#endif
