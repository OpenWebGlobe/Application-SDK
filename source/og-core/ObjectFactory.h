/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _OBJECTFACTORY_H
#define _OBJECTFACTORY_H

#include "Object.h"
#include "shared_ptr.h"
#include <string>


namespace OpenGlobe
{
   OPENGLOBE_API OptionVector OptionString(const std::string& sOptions);

   OPENGLOBE_API Object* CreateObject(Object::Type eType, Object* parent, const std::string& sOptions);
   OPENGLOBE_API void DestroyObject(Object* obj);
   //OPENGLOBE_API shared_ptr<Object> CreateObjectFromXML(const std::string sXML);
   //OPENGLOBE_API shared_ptr<Object> CreateObjectFromUrl(std::string sURL, shared_ptr<Object> parent, bool async, std::string sOption);
}


#endif