/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "ObjectFactory.h"
#include "xml/xml.h"

namespace OpenGlobe
{
   //------------------------------------------------------------------------------

   std::vector<std::pair<std::string, std::string> > OptionString(const std::string& attribs)
   {
      std::vector<std::pair<std::string, std::string> > vRet;

      if (attribs.length()>0)
      {
         std::string key;
         std::string value;
         int parse = 0; // 0: key, 1:value
         bool bstartval = false;
         bool bhyph = false;
         for (size_t i=0;i<attribs.length();i++)
         {
            char cur = attribs[i];
            if (parse == 0) // parsing key
            {
               if (cur == '=')
               {
                  bstartval = true;
                  parse = 1;
               }
               else if (cur == ' ')
               {
                  vRet.push_back(std::pair<std::string, std::string>(key, value));
                  key = "";
                  value = "";
               }
               else
               {
                  key = key+cur;
               }
            }
            else if (parse == 1)
            {
               if (bstartval && (cur == '\"' || cur == '\''))
               {
                  bhyph = true;  // ignore opening " or ' (this could be done stricter, as now it is possible to mix ' and ")
               }
               else
               {
                  if (bhyph && (cur == '\"' || cur == '\''))
                  {
                     // ignore closing " or '
                     bhyph = false;
                  }
                  else
                  {  
                     if (cur == ' ' && !bhyph) // finish!
                     {
                        vRet.push_back(std::pair<std::string, std::string>(key, value));
                        key = "";
                        value = "";
                        parse = 0;
                     }
                     else
                     {   
                        value = value + cur;
                     }
                  }
               }
               bstartval = false;
            }
         }

         if (key.length()>0)
         {
            vRet.push_back(std::pair<std::string, std::string>(key, value));
            key = "";
            value = "";
         }
      }

      // remove empty entries

      std::vector<std::pair<std::string, std::string> >::iterator it = vRet.begin();

      while (it != vRet.end())
      {
         if (it->first.length() == 0 && it->first.length() == 0)
         {
            it = vRet.erase(it);
         }
         else
            it++;
      }


      return vRet;
   }

   //------------------------------------------------------------------------------

   OPENGLOBE_API Object* CreateObject(Object::Type eType, Object* parent, const std::string& sOptions)
   {
         Object* qNewObject;
         Object* pObject = 0;

         switch (eType)
         {
         case Object::CONTEXT:
            pObject = (Object*) types::CreateInstance("Context");
            break;
         case Object::SCENE:
            pObject = (Object*) types::CreateInstance("Scene");
            break;
         case Object::WORLD:
            pObject = (Object*) types::CreateInstance("World");
            break;
         case Object::IMAGELAYER:
            pObject = (Object*) types::CreateInstance("ImageLayer");
            break;
         case Object::ELEVATIONLAYER:
            pObject = (Object*) types::CreateInstance("ElevationLayer");
            break;
         case Object::POILAYER:
            pObject = (Object*) types::CreateInstance("POILayer");
            break;
         case Object::GEOMETRYLAYER:
            pObject = (Object*) types::CreateInstance("GeometryLayer");
            break;
         case Object::VOXELLAYER:
            pObject = (Object*) types::CreateInstance("VoxelLayer");
            break;
         case Object::IMAGE:
            pObject = (Object*) types::CreateInstance("ImageLayer");
            break;
         case Object::TEXTURE:
            pObject = (Object*) types::CreateInstance("TextureObject");
            break;
         case Object::PIXELBUFFER:
            pObject = (Object*) types::CreateInstance("PixelBuffer");
            break;
         case Object::GEOMETRY:
            pObject = (Object*) types::CreateInstance("Geometry");
            break;
         case Object::MESH:
            pObject = (Object*) types::CreateInstance("Mesh");
            break;
         case Object::SURFACE:
            pObject = (Object*) types::CreateInstance("Surface");
            break;
         case Object::CAMERA:
            pObject = (Object*) types::CreateInstance("Camera");
            break;
         case Object::TEXT:
            pObject = (Object*) types::CreateInstance("Text");
            break;
         case Object::BINARYDATA:
            pObject = (Object*) types::CreateInstance("BinaryData");
            break;
         case Object::LIGHT:
            pObject = (Object*) types::CreateInstance("Light");
            break;
         case Object::OBJECT:
            pObject = (Object*) types::CreateInstance("Object");
            break;
         case Object::NAVIGATIONCONTROLLER:
            pObject = (Object*) types::CreateInstance("NavigationController");
            break;
         default:
            assert(false);
            return 0;
         }

         if (pObject)
         {
            qNewObject = pObject;
         }

         // Register Object (using current process id)
         if (qNewObject)
         {
            qNewObject->SetParent(parent);
            qNewObject->RegisterObject();
            std::vector<std::pair<std::string, std::string> > vecOptions = OptionString(sOptions);
            qNewObject->SetOptions(vecOptions);
         }

         return qNewObject;
   }

   OPENGLOBE_API void DestroyObject(Object* obj)
   {
      delete obj;
   }


}