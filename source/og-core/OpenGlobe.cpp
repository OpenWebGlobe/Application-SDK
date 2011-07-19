/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "og.h"
#include "Object.h"
#include "System.h"
#include "objects/Context.h"
#include "objects/Scene.h"
#include "objects/Camera.h"
#include "objects/Texture.h"
#include "objects/World.h"
#include "objects/ImageLayer.h"
#include "ObjectFactory.h"
#include "io/CommonPath.h"
#include "data/DiskCache.h"
#include "math/GeoCoord.h"
#include <iostream>
#include <cstdlib>



OpenGlobe::Object* _ogGetObjectFromId(int object);

//------------------------------------------------------------------------------
// OpenGlobe Error Handling
//------------------------------------------------------------------------------
inline void _ogPushError(const char* error)
{
   std::cout << "*ERROR*: " << error << "\n";
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogInfo()
{
    std::cout << "-------------------------------------------------------------------------------\n";
   std::cout << "    ____                    _____ _       _             _____ _____  _  __  \n";
   std::cout << "   / __ \\                  / ____| |     | |           / ____|  __ \\| |/ /  \n";
   std::cout << "  | |  | |_ __   ___ _ __ | |  __| | ___ | |__   ___  | (___ | |  | | ' /   \n";
   std::cout << "  | |  | | '_ \\ / _ \\ '_ \\| | |_ | |/ _ \\| '_ \\ / _ \\  \\___ \\| |  | |  <    \n";
   std::cout << "  | |__| | |_) |  __/ | | | |__| | | (_) | |_) |  __/  ____) | |__| | . \\   \n";
   std::cout << "   \\____/| .__/ \\___|_| |_|\\_____|_|\\___/|_.__/ \\___| |_____/|_____/|_|\\_\\  \n";
   std::cout << "         | |                                                                \n";
   std::cout << "         |_|                                                                \n";
   std::cout << "  created by Martin Christen, martin.christen@fhnw.ch\n";
   std::cout << "  Version " << ogGetString(OG_VERSION) << "\n";
   std::cout << "-------------------------------------------------------------------------------\n\n";

}

//------------------------------------------------------------------------------

OPENGLOBE_API const char* ogGetString(unsigned int type)
{
const char* cVendor = "FHNW/IVGI http://www.fhnw.ch/habg/ivgi";
const char* cVersion = "1.0 [Alpha 1.0]";

   if (type == OG_VENDOR)
   {
      return cVendor;
   }
   else if (type == OG_VERSION)
   {
      return cVersion;
   }
   else
   {
      _ogPushError("ogGetString: wrong type");
      return 0;
   }
}

//------------------------------------------------------------------------------
// MISC / APPLICATION WIDE FUNCTIONS
//------------------------------------------------------------------------------
namespace OpenGlobe
{
   Cache& _ogCache()
   {
      // Default: 500 MB Disk Cache & 250 MB Memory Cache & 2 download threads
      static Cache ogCache(std::string(ogGetSettingsDirectory()) + std::string("cache"), 500, 250, 2);

      return ogCache;
   }
}

//------------------------------------------------------------------------------

std::string& _appname()
{
   // construct on first use ideom
   static std::string appname("OpenWebGlobe");
   return appname;
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogSetApplicationName(const char* name)
{
   std::string& appname = _appname();
   appname.assign(name);
}

//------------------------------------------------------------------------------

OPENGLOBE_API const char* ogGetApplicationName()
{
   std::string& appname = _appname();
   return appname.c_str();
}

//------------------------------------------------------------------------------

OPENGLOBE_API const char* ogGetSettingsDirectory()
{
   static std::string sPath;
   sPath = CommonPath::GetApplicationDirectory(_appname());

   return sPath.c_str();
}

//------------------------------------------------------------------------------

OPENGLOBE_API float ogRand01()
{
   return fabs((float)rand()/float(RAND_MAX));
}

//------------------------------------------------------------------------------

OPENGLOBE_API unsigned int ogGetNumObjects()
{
   OpenGlobe::ObjectMap& mapobj = OpenGlobe::GetObjectMap();
   int threadid = System::GetThreadId();
   OpenGlobe::ObjectMapIterator it = mapobj.find(threadid);
   
   if (it != mapobj.end())
   {
      OpenGlobe::ObjectList& lst = it->second;
      return lst.size();
   }
   
   return 0;
}

//------------------------------------------------------------------------------

OPENGLOBE_API int ogGetObjectAt(unsigned int index)
{
   OpenGlobe::ObjectMap& mapobj = OpenGlobe::GetObjectMap();
   int threadid = System::GetThreadId();
   OpenGlobe::ObjectMapIterator it = mapobj.find(threadid);
   
   if (it != mapobj.end())
   {
      OpenGlobe::ObjectList& lst = it->second;
      OpenGlobe::ObjectListIterator jt = lst.begin();
      unsigned int curidx = 0;

      while (jt != lst.end())
      {
         if (curidx == index)
         {
            return (*jt)->GetId();
         }
         curidx++;
         jt++;
      }
   }

   return -1;
}

//------------------------------------------------------------------------------
// Get Objectptr from object id
OpenGlobe::Object* _ogGetObjectFromId(int object)
{
   OpenGlobe::ObjectMap& mapobj = OpenGlobe::GetObjectMap();
   int threadid = System::GetThreadId();
   OpenGlobe::ObjectMapIterator it = mapobj.find(threadid);
   
   if (it != mapobj.end())
   {
      OpenGlobe::ObjectList& lst = it->second;
      OpenGlobe::ObjectListIterator jt = lst.begin();

      while (jt != lst.end())
      {
         if ((*jt)->GetId() == object)
         {
            return *jt;
         }
         
         jt++;
      }
   }

   return 0;
}

//------------------------------------------------------------------------------

inline OpenGlobe::Object* _ogGetObjectByName(const char* name)
{
   std::string sName(name);
   OpenGlobe::ObjectMap& mapobj = OpenGlobe::GetObjectMap();
   int threadid = System::GetThreadId();
   OpenGlobe::ObjectMapIterator it = mapobj.find(threadid);
   
   if (it != mapobj.end())
   {
      OpenGlobe::ObjectList& lst = it->second;
      OpenGlobe::ObjectListIterator jt = lst.begin();

      while (jt != lst.end())
      {
         if ((*jt)->GetName() == sName)
         {
            return *jt;
         }
         
         jt++;
      }
   }

   return 0;
}

//------------------------------------------------------------------------------

OPENGLOBE_API unsigned int ogGetObjectType(int object)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      switch (obj->GetObjectType())
      {
         case OpenGlobe::Object::CONTEXT:
            return OG_OBJECT_CONTEXT;
            break;
         case OpenGlobe::Object::SCENE:
            return OG_OBJECT_SCENE;
            break;
         case OpenGlobe::Object::WORLD:
            return OG_OBJECT_WORLD;
            break;
         case OpenGlobe::Object::IMAGELAYER:
            return OG_OBJECT_IMAGELAYER;
            break;
         case OpenGlobe::Object::ELEVATIONLAYER:
            return OG_OBJECT_ELEVATIONLAYER;
            break;
         case OpenGlobe::Object::POILAYER:
            return OG_OBJECT_POILAYER;
            break;
         case OpenGlobe::Object::GEOMETRYLAYER:
            return OG_OBJECT_GEOMETRYLAYER;
            break;
         case OpenGlobe::Object::VOXELLAYER:
            return OG_OBJECT_VOXELLAYER;
            break;
         case OpenGlobe::Object::IMAGE:
            return OG_OBJECT_IMAGE;
            break;
         case OpenGlobe::Object::TEXTURE:
            return OG_OBJECT_TEXTURE;
            break;
         case OpenGlobe::Object::PIXELBUFFER:
            return OG_OBJECT_PIXELBUFFER;
            break;
         case OpenGlobe::Object::GEOMETRY:
            return OG_OBJECT_GEOMETRY;
            break;
         case OpenGlobe::Object::MESH:
            return OG_OBJECT_MESH;
            break;
         case OpenGlobe::Object::SURFACE:
            return OG_OBJECT_SURFACE;
            break;
         case OpenGlobe::Object::CAMERA:
            return OG_OBJECT_CAMERA;
            break;
         case OpenGlobe::Object::TEXT:
            return OG_OBJECT_TEXT;
            break;
         case OpenGlobe::Object::BINARYDATA:
            return OG_OBJECT_BINARYDATA;
            break;
         case OpenGlobe::Object::LIGHT:
            return OG_OBJECT_LIGHT;
            break;
      }
   }

   return OG_OBJECT_INVALID;
}

//------------------------------------------------------------------------------

OPENGLOBE_API int ogGetParentObject(int object)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);

   if (obj)
   {
      OpenGlobe::Object* parent = obj->GetParent();
      if (parent)
      {
         return parent->GetId();
      }
   }

   return -1;
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogSetObjectName(int object, const char* name)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      obj->SetName(std::string(name));
   }
}

//------------------------------------------------------------------------------

OPENGLOBE_API int ogGetObjectByName(const char* name)
{
   OpenGlobe::Object* obj = _ogGetObjectByName(name);
   if (obj)
   {
      return obj->GetId();
   }

   return -1;
}

//------------------------------------------------------------------------------

OPENGLOBE_API unsigned int ogGetObjectStatus(int object)
{
   return OG_OBJECT_FAILED;
}

//-----------------------------------------------------------------------------

OPENGLOBE_API void ogOnLoad(int object, CallBack_Object cbf)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      obj->SetCallbackOnLoad(cbf);
   }
}

//-----------------------------------------------------------------------------

OPENGLOBE_API void ogOnFailure(int object, CallBack_Object cbf)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      obj->SetCallbackOnFailure(cbf);
   }
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogSetStringProperty(int object, const char* key, const char* value)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      obj->SetStringProperty(key, value);
   }
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogSetIntegerProperty(int object, const char* key, int value)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      obj->SetIntegerProperty(key, value);
   }
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogSetDoubleProperty(int object, const char* key, double value)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      obj->SetDoubleProperty(key, value);
   }
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogSetBoolProperty(int object, const char* key, bool value)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      obj->SetBoolProperty(key, value);
   }
}

//------------------------------------------------------------------------------

OPENGLOBE_API const char* ogGetStringProperty(int object, const char* key)
{
   static std::string result;
   result.clear();

   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      result = obj->GetStringProperty(key);
   }

   return result.c_str();
}

//------------------------------------------------------------------------------

OPENGLOBE_API int ogGetIntegerProperty(int object, const char* key)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      return obj->GetIntegerProperty(key);
   }

   return 0;
}

//------------------------------------------------------------------------------

OPENGLOBE_API double ogGetDoubleProperty(int object, const char* key)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      return obj->GetDoubleProperty(key);
   }

   return 0;
}

//------------------------------------------------------------------------------

OPENGLOBE_API bool ogGetBoolProperty(int object, const char* key)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      return obj->GetBoolProperty(key);
   }

   return false;
}

//------------------------------------------------------------------------------

OPENGLOBE_API unsigned int ogGetPropertyType(int object, const char* key)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      OpenGlobe::Property::Type type = obj->GetPropertyType(key);
      if (type == OpenGlobe::Property::STRING)
      {
         return OG_PROPERTY_STRING;
      }
      else if (type == OpenGlobe::Property::INTEGER)
      {
         return OG_PROPERTY_INTEGER;
      }
      else if (type == OpenGlobe::Property::DOUBLE)
      {
         return OG_PROPERTY_DOUBLE;
      }
      else if (type == OpenGlobe::Property::BOOL)
      {
         return OG_PROPERTY_BOOL;
      }
   }

   return OG_INVALID;
}
//------------------------------------------------------------------------------
OPENGLOBE_API bool ogHasProperty(int object, const char* key)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      return obj->HasProperty(key);
   }

   return false;
}
//------------------------------------------------------------------------------
OPENGLOBE_API bool ogPropertyIsReadonly(int object, const char* key)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      return obj->PropertyIsReadonly(key);
   }

   return false;
}
//------------------------------------------------------------------------------
OPENGLOBE_API bool ogPropertyIsProtected(int object, const char* key)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      return obj->PropertyIsProtected(key);
   }

   return false;
}
//------------------------------------------------------------------------------
OPENGLOBE_API unsigned int ogGetNumProperties(int object)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      return obj->GetNumProperties();
   }

   return 0;
}
//------------------------------------------------------------------------------
OPENGLOBE_API const char* ogGetPropertyKey(int object, unsigned int index)
{
   static std::string result;
   result.clear();
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      result =  obj->GetPropertyKeyAt(index);
      return result.c_str();
   }

   return result.c_str();
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogRemoveProperty(int object, const char* key)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj)
   {
      obj->RemoveProperty(key);
   }
}
//------------------------------------------------------------------------------

OPENGLOBE_API const char* ogToXML(int object)
{
   static std::string sXML;
   sXML.clear();
   OpenGlobe::Object* obj = _ogGetObjectFromId(object);
   if (obj) 
   {
      sXML = obj->ToXML();
   }

   return sXML.c_str();
}

//------------------------------------------------------------------------------
// M I S C
//------------------------------------------------------------------------------

OPENGLOBE_API void ogMemoryDump(const char* file)
{
}

//------------------------------------------------------------------------------

OPENGLOBE_API const char* ogGetLastError()
{
   return 0;
}

//------------------------------------------------------------------------------
// C O N T E X T
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

OPENGLOBE_API int ogCreateContext(const char* options, CallBack_Init cbfInit, CallBack_Exit cbfExit, CallBack_Resize cbfResize)
{
   OpenGlobe::Object* pObject;
   OpenGlobe::Context* pContext;

   pObject = OpenGlobe::CreateObject(OpenGlobe::Object::CONTEXT, 0, options);
   pContext = dynamic_cast<OpenGlobe::Context*>(pObject);

   if (pContext)
   {
      pContext->SetCallBack_Init(cbfInit);
      pContext->SetCallBack_Resize(cbfResize);
      pContext->SetCallBack_Exit(cbfExit);
      pContext->CreateContext();
      return pContext->GetId();
   }
   else
   {
      return -1;
   }
}

//------------------------------------------------------------------------------

OPENGLOBE_API int ogCreateRenderWindow(const char* title, unsigned int width, unsigned int height)
{
   OpenGlobe::Object* pObject;
   OpenGlobe::Context* pContext;

   std::ostringstream oss;

   oss << "title='" << title << "'";
   oss << " width=" << width;
   oss << " height=" << height;
   
   pObject = OpenGlobe::CreateObject(OpenGlobe::Object::CONTEXT, 0, oss.str());
   pContext = dynamic_cast<OpenGlobe::Context*>(pObject);

   if (pContext)
   {
      pContext->CreateContext();
      return pContext->GetId();
   }
   else
   {
      return -1;
   }
}

//------------------------------------------------------------------------------

OPENGLOBE_API int ogExec()
{
   return OpenGlobe::Exec();
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogDestroyContext(int context)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      OpenGlobe::IRenderWindow* pRenderWindow = pContext->GetRenderWindow();
      if (pRenderWindow)
      {
         pRenderWindow->Destroy();
      }
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API int ogGetWidth(int context)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
     return pContext->GetWidth();
   }

   return 0;
}
//------------------------------------------------------------------------------
OPENGLOBE_API int ogGetHeight(int context)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
     return pContext->GetHeight();
   }

   return 0;
}
//------------------------------------------------------------------------------
OPENGLOBE_API int ogGetScene(int context)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      OpenGlobe::Scene* pScene = pContext->GetScene();
      if (pScene)
      {
         return pScene->GetId();
      }

   }
   return -1;
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetBackgroundColor(int context, float r, float g, float b, float a)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      IRenderEngine* pEngine = pContext->GetRenderEngine();
      if (pEngine)
      {
         pEngine->SetClearColor(vec4<double>(r,g,b,a));
      }
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogGetBackgroundColor(int context, float* r, float* g, float* b, float* a)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      IRenderEngine* pEngine = pContext->GetRenderEngine();
      if (pEngine)
      {
         vec4<double> color;
         pEngine->GetClearColor(color);
         *r = (float)color.r;
         *g = (float)color.g;
         *b = (float)color.b;
         *a = (float)color.a;
         return;
      }
   }

   *r = *g = *b = *a = 0;
}

//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetDrawColor(int context, float r, float g, float b,  float a)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      IRenderEngine* pEngine = pContext->GetRenderEngine();
      if (pEngine)
      {
         pEngine->SetDrawColor(vec4<double>(r,g,b,a));
      }
   }
}

//------------------------------------------------------------------------------
OPENGLOBE_API void ogDrawText(int context, const char* text, float x, float y)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      IRenderEngine* pEngine = pContext->GetRenderEngine();
      if (pEngine)
      {
        
      }
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetTextColor(int context, float r, float g, float b, float a)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      IRenderEngine* pEngine = pContext->GetRenderEngine();
      assert(false); // not yet implemented!
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogGetTextSize(int context, char* text, int* width, int* height)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      IRenderEngine* pEngine = pContext->GetRenderEngine();
      assert(false); // not yet implemented!
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetViewport(int context, int x, int y, int width, int height)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      IRenderEngine* pEngine = pContext->GetRenderEngine();
      if (pEngine)
      {
         pEngine->SetViewport(x,y,width,height);
      }
   }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// E V E N T S
//------------------------------------------------------------------------------

OPENGLOBE_API void ogSetMouseDownFunction(int context, CallBack_MouseDown callback)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      pContext->SetCallBack_MouseDown(callback);
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetMouseUpFunction(int context, CallBack_MouseUp callback)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      pContext->SetCallBack_MouseUp(callback);
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetMouseMoveFunction(int context, CallBack_MouseMove callback)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      pContext->SetCallBack_MouseMove(callback);
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetMouseWheelFunction(int context, CallBack_MouseWheel callback)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      pContext->SetCallBack_MouseWheel(callback);
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetKeyDownFunction(int context,  CallBack_KeyDown callback)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      pContext->SetCallBack_KeyDown(callback);
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetKeyUpFunction(int context, CallBack_KeyUp callback)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      pContext->SetCallBack_KeyUp(callback);
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetResizeFunction(int context,  CallBack_Resize callback)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      pContext->SetCallBack_Resize(callback);

      // call resize function after setting...
      callback(context, ogGetWidth(context), ogGetHeight(context));

   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetRenderFunction(int context,  CallBack_Render callback)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      pContext->SetCallBack_Render(callback);
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetTimerFunction(int context, CallBack_Timer callback)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      pContext->SetCallBack_Timer(callback);
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetRenderGeometryFunction(int context, CallBack_RenderGeometry callback)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      pContext->SetCallBack_RenderGeometry(callback);
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetBeginRenderFunction(int context, CallBack_BeginRender callback)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      pContext->SetCallBack_BeginRender(callback);
   }
      
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetEndRenderFunction(int context, CallBack_EndRender callback)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      pContext->SetCallBack_EndRender(callback);
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetNumRenderPasses(int context, int numPasses)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);

   if (pContext)
   {
      assert(false); // not yet implemented!
   }
}
//------------------------------------------------------------------------------
// SCENE
OPENGLOBE_API int ogCreateScene(int context, unsigned int sceneType)
{
   OpenGlobe::Object* pObject;
   OpenGlobe::Object* obj = _ogGetObjectFromId(context);
   OpenGlobe::Context* pContext = dynamic_cast<OpenGlobe::Context*>(obj);
   std::string options; // empty

   pObject = OpenGlobe::CreateObject(OpenGlobe::Object::SCENE, obj, options);
   OpenGlobe::Scene* pScene = dynamic_cast<OpenGlobe::Scene*>(pObject);
   OpenGlobe::Scene::Type eType;

   if (pScene && pContext)
   {
      switch (sceneType)
      {
         case OG_SCENE_3D_ELLIPSOID_WGS84:
            eType = OpenGlobe::Scene::ELLIPSOID_WGS84;
            break;
         case OG_SCENE_3D_FLAT_CARTESIAN:
            eType = OpenGlobe::Scene::FLAT_CARTESIAN;
            break;
         case OG_SCENE_2D_SCREEN:
            eType = OpenGlobe::Scene::SCREEN;
            break;
         case OG_SCENE_CUSTOM:
            eType = OpenGlobe::Scene::CUSTOM;
            break;
         default:
            assert(false);
            return -1;
      }

      pScene->CreateScene(eType);
      return pScene->GetId();
   }

   return -1;
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogDestroyScene(int scene)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(scene);
   OpenGlobe::Scene* pScene = dynamic_cast<OpenGlobe::Scene*>(obj);

   if (pScene)
   {
      pScene->DestroyScene();
      delete pScene;
   }
}

//------------------------------------------------------------------------------

OPENGLOBE_API int ogGetContext(int scene)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(scene);
   OpenGlobe::Scene* pScene = dynamic_cast<OpenGlobe::Scene*>(obj);

   if (pScene)
   {
   }

   return -1;
}

//------------------------------------------------------------------------------

OPENGLOBE_API int ogGetWorld(int scene)
{
   /*OpenGlobe::Object* obj = _ogGetObjectFromId(scene);
   OpenGlobe::Scene* pScene = dynamic_cast<OpenGlobe::Scene*>(obj);

   if (pScene)
   {
      pScene->GetWorld();
   }*/

   return -1;
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogSetActiveCamera(int scene, int camera)
{
   if (scene == -1 || camera == -1)
      return;

   OpenGlobe::Object* obj = _ogGetObjectFromId(scene);
   OpenGlobe::Scene* pScene = dynamic_cast<OpenGlobe::Scene*>(obj);

   if (pScene)
   {
      obj = _ogGetObjectFromId(camera);
      OpenGlobe::Camera* pCamera = dynamic_cast<OpenGlobe::Camera*>(obj);
      if (pCamera)
      {
         pScene->SetActiveCamera(pCamera);
      }
   }
}

//------------------------------------------------------------------------------

OPENGLOBE_API int ogGetActiveCamera(int scene)
{
   if (scene == -1)
      return -1;

   OpenGlobe::Object* obj = _ogGetObjectFromId(scene);
   OpenGlobe::Scene* pScene = dynamic_cast<OpenGlobe::Scene*>(obj);

   if (pScene)
   {
      OpenGlobe::Camera* pCamera = pScene->GetActiveCamera();
      if (pCamera)
      {
         return pCamera->GetId();
      }
   }

   return -1;
}

//------------------------------------------------------------------------------

OPENGLOBE_API unsigned int ogGetNumCameras(int scene)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(scene);
   OpenGlobe::Scene* pScene = dynamic_cast<OpenGlobe::Scene*>(obj);

   if (pScene)
   {
      return pScene->GetNumCameras();
   }

   return 0;
}

//------------------------------------------------------------------------------

OPENGLOBE_API int ogGetCameraAt(int scene, unsigned int index)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(scene);
   OpenGlobe::Scene* pScene = dynamic_cast<OpenGlobe::Scene*>(obj);

   if (pScene)
   {
      OpenGlobe::Camera* pCamera = pScene->GetCameraAt(index);
      if (pCamera)
      {
         return pCamera->GetId();
      }
   }

   return -1;
}

//------------------------------------------------------------------------------

// **************
// *** CAMERA ***
// **************

OPENGLOBE_API int ogCreateCamera(int scene)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(scene);
   OpenGlobe::Scene* pScene = dynamic_cast<OpenGlobe::Scene*>(obj);

   if (pScene)
   {
      OpenGlobe::Camera* pCamera = pScene->CreateCamera();
      if (pCamera)
      {
         return pCamera->GetId();
      }

   }

   return -1;
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogDestroyCamera(int camera)
{
   assert(false); // not yet implemented
}

//------------------------------------------------------------------------------

OPENGLOBE_API int ogGetNavigationController(int camera)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(camera);
   OpenGlobe::Camera* pCamera = dynamic_cast<OpenGlobe::Camera*>(obj);
   if (pCamera)
   {
      OpenGlobe::NavigationController* pNC = pCamera->GetNavigationController();
      if (pNC)
      {
         return pNC->GetId();
      }
   }

   return -1;
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogSetProjectionMatrixd(int camera, double* m)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(camera);
   OpenGlobe::Camera* pCamera = dynamic_cast<OpenGlobe::Camera*>(obj);
   if (pCamera)
   {
      mat4<double> P(m);
      pCamera->SetProjectionMatrix(P);
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetProjectionMatrixf(int camera, float* m)
{
   assert(false);
   OpenGlobe::Object* obj = _ogGetObjectFromId(camera);
   OpenGlobe::Camera* pCamera = dynamic_cast<OpenGlobe::Camera*>(obj);
   if (pCamera)
   {
      mat4<double> P;
      double* ptr = (double*)P._vals;
      for (short i=0; i<16; ++i)
      {
         ptr[i] = (double)m[i];
      }
      pCamera->SetProjectionMatrix(P);
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogCreatePerspectiveProjection(int camera, float fovy, float zNear, float zFar)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(camera);
   OpenGlobe::Camera* pCamera = dynamic_cast<OpenGlobe::Camera*>(obj);
   if (pCamera)
   {
      pCamera->CreatePerspectiveProjection((double)fovy, (double)zNear, (double)zFar);
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogCreateOrtho2D(int camera, float left, float right, float bottom, float top)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(camera);
   OpenGlobe::Camera* pCamera = dynamic_cast<OpenGlobe::Camera*>(obj);
   if (pCamera)
   {
      pCamera->CreateOrtho2D((double)left, (double)right, (double)bottom, (double)top);
   }
} 
//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetViewMatrixd(int camera, double* m)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(camera);
   OpenGlobe::Camera* pCamera = dynamic_cast<OpenGlobe::Camera*>(obj);
   if (pCamera)
   {
      // #TODO
   }
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogSetViewMatrixf(int camera, float* m)
{
   assert(false);
   // #TODO
}
//------------------------------------------------------------------------------
OPENGLOBE_API void ogLookAt(int camera, double eyeX, double eyeY, double eyeZ, double centerX, double centerY, double centerZ, double upX, double upY, double upZ)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(camera);
   OpenGlobe::Camera* pCamera = dynamic_cast<OpenGlobe::Camera*>(obj);
   if (pCamera)
   {
      pCamera->CreateLookAt(eyeX, eyeY, eyeZ,
                            centerX, centerY, centerZ,
                            upX, upY, upZ);
   }
}
//------------------------------------------------------------------------------

// ******************
// ** IMAGE OBJECT **
// ******************

OPENGLOBE_API int ogCreateImage(int scene, int width, int height, unsigned int pixelformat)
{
   return -1;
}

//------------------------------------------------------------------------------

OPENGLOBE_API int ogLoadImage(int scene, const char* url)
{
   return -1;
}

//------------------------------------------------------------------------------

OPENGLOBE_API int ogLoadImageAsync(int scene, const char* url)
{
   return -1;
}

//------------------------------------------------------------------------------

OPENGLOBE_API int ogDestroyImage(int image)
{
   return -1;
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogSaveImage(int image, const char* filename, const char* formatoptions)
{
}

//------------------------------------------------------------------------------

OPENGLOBE_API int ogGetImageWidth(int image)
{
   return 0;
}

//------------------------------------------------------------------------------

OPENGLOBE_API int ogGetImageHeight(int image)
{
   return 0;
}

//------------------------------------------------------------------------------

OPENGLOBE_API unsigned int ogGetImagePixelFormat(int image)
{
   return 0;
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogAddAlphaChannel(int image)
{
}

//------------------------------------------------------------------------------

OPENGLOBE_API int ogResizeImage(int image, int newwidth, int newheight)
{
   return -1;
}

//------------------------------------------------------------------------------

OPENGLOBE_API unsigned char* ogLockImage(int image)
{
   return 0;
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogUnlockImage(int image)
{
}

//------------------------------------------------------------------------------


// WGS84 Globe
OPENGLOBE_API int ogCreateGlobe(int context)
{
   int scene = ogCreateScene(context, OG_SCENE_3D_ELLIPSOID_WGS84);
   
   OpenGlobe::Object* pObject;
   OpenGlobe::Object* obj = _ogGetObjectFromId(scene);
   OpenGlobe::Scene* pScene = dynamic_cast<OpenGlobe::Scene*>(obj);
   std::string options; // empty

   pObject = OpenGlobe::CreateObject(OpenGlobe::Object::WORLD, obj, options);

   if (pObject)
   {
      return pObject->GetId();
   }

   return -1;
}

//------------------------------------------------------------------------------

OPENGLOBE_API int ogPickGlobe(int scene,float mx, float my, double* lng, double* lat, double* elv)
{
   // #TODO
   return -1;
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogToCartesian(int scene, double lng, double lat, double elv,double* x,double* y,double* z)
{
   GeoCoord gc(lng, lat, elv);
   gc.ToCartesian(x,y,z);
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogToWGS84(int scene, double x, double y, double z, double* lng,double* lat,double* elv)
{
   GeoCoord gc;
   gc.FromCartesian(x,y,z);
   *lng = gc.longitude();
   *lat = gc.latitude();
   *elv = gc.elliposid_height();
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogSetPositionWGS84(int camera, double longitude, double latitude, double elv)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(camera);
   OpenGlobe::Camera* pCamera = dynamic_cast<OpenGlobe::Camera*>(obj);
   if (pCamera)
   {
      pCamera->SetPositionWGS84(longitude, latitude, elv);
   }
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogSetOrientationWGS84(int camera, double yaw, double pitch, double roll)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(camera);
   OpenGlobe::Camera* pCamera = dynamic_cast<OpenGlobe::Camera*>(obj);
   if (pCamera)
   {
      pCamera->SetOrientationWGS84(yaw, pitch,  roll);
   }
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogLookAtWGS84(int camera, double longitude, double latitude, double elv)
{
   OpenGlobe::Object* obj = _ogGetObjectFromId(camera);
   OpenGlobe::Camera* pCamera = dynamic_cast<OpenGlobe::Camera*>(obj);
   if (pCamera)
   {
      
   }
}

//------------------------------------------------------------------------------
   
Texture::PixelFormat _convert_pixelformat(unsigned int pf)
{
   switch (pf)
   {
      //case OG_PIXELFORMAT_RED:
      //case OG_PIXELFORMAT_GREEN:
      //case OG_PIXELFORMAT_BLUE:
      //case OG_PIXELFORMAT_ALPHA:
      case OG_PIXELFORMAT_RGB:
         return Texture::GFX_RGB;
      break;
      case OG_PIXELFORMAT_BGR:
         return Texture::GFX_BGR;
      break;
      case OG_PIXELFORMAT_RGBA:
         return Texture::GFX_RGBA;
      break;
      case OG_PIXELFORMAT_BGRA:
         return Texture::GFX_BGRA;
      break;
      //case OG_PIXELFORMAT_LUMINANCE:
      //case OG_PIXELFORMAT_LUMINANCE_ALPHA:

      default:
      assert(false);
      return Texture::GFX_RGBA;
   }
}

//------------------------------------------------------------------------------

// Example: ogLoadTextureAsync(scene, "http://www.images.com/image.png", "png"
OPENGLOBE_API int ogLoadTextureAsync(int scene, const char* url, const char* imageformat, unsigned int pixelformat)
{
   OpenGlobe::Object* pObject;
   OpenGlobe::Object* obj = _ogGetObjectFromId(scene);
   OpenGlobe::Scene* pScene = dynamic_cast<OpenGlobe::Scene*>(obj);
   
   std::string imgfmt(imageformat);

   if (pScene)
   {
      std::string options; // empty
      pObject = OpenGlobe::CreateObject(OpenGlobe::Object::TEXTURE, obj, options);
      OpenGlobe::TextureObject* pTexture = dynamic_cast<OpenGlobe::TextureObject*>(pObject);
      if (pTexture)
      {
         Img::FileFormat format = Img::Format_PNG;

         if (imgfmt == "png" || imgfmt == "PNG")
         {
            format = Img::Format_PNG;
         }
         else if (imgfmt == "gif" || imgfmt == "GIF")
         {
            format = Img::Format_GIF;
         }
         else if (imgfmt == "jpg" || imgfmt == "JPG")
         {
            format = Img::Format_JPG;
         }
         else if (imgfmt == "tga" || imgfmt == "TGA")
         {
            format = Img::Format_TGA;
         }

         pTexture->LoadTextureAsync(url, format, _convert_pixelformat(pixelformat));

         return pTexture->GetId();
      }
   }

   return -1;
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogBlitTexture(int texture, int x, int y)
{
   OpenGlobe::Object* pObject;
   OpenGlobe::Object* obj = _ogGetObjectFromId(texture);
   OpenGlobe::TextureObject* pTexture = dynamic_cast<OpenGlobe::TextureObject*>(obj);

   if (pTexture)
   {
      pTexture->BlitTexture(x,y,0,0,1.0,1.0,true);
   }

}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogDestroyTexture(int texture)
{
   OpenGlobe::Object* pObject;
   OpenGlobe::Object* obj = _ogGetObjectFromId(texture);
   OpenGlobe::TextureObject* pTexture = dynamic_cast<OpenGlobe::TextureObject*>(obj);

   if (pTexture)
   {
      pTexture->Destroy();
      delete pTexture;
   }
}

//------------------------------------------------------------------------------
// WORLD OBJECT

OPENGLOBE_API int ogCreateWorld(int scene)
{
   OpenGlobe::Object* pObject;
   OpenGlobe::Object* obj = _ogGetObjectFromId(scene);
   OpenGlobe::Scene* pScene = dynamic_cast<OpenGlobe::Scene*>(obj);
   
   if (pScene)
   {
      std::string options; // empty
      pObject = OpenGlobe::CreateObject(OpenGlobe::Object::WORLD, obj, options);
      OpenGlobe::World* pWorld = dynamic_cast<OpenGlobe::World*>(pObject);
      if(pWorld)
      {
         return pWorld->GetId();
      }
   }

   return -1;
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogDestroyWorld(int world)
{
   OpenGlobe::Object* pObject;
   OpenGlobe::Object* obj = _ogGetObjectFromId(world);
   OpenGlobe::World* pWorld = dynamic_cast<OpenGlobe::World*>(obj);

   if (pWorld)
   {
      delete pWorld;
   }
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogShowWorld(int world)
{
   OpenGlobe::Object* pObject;
   OpenGlobe::Object* obj = _ogGetObjectFromId(world);
   OpenGlobe::World* pWorld = dynamic_cast<OpenGlobe::World*>(obj);

   if (pWorld)
   {
      pWorld->Show();
   }
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogHideWorld(int world)
{
   OpenGlobe::Object* pObject;
   OpenGlobe::Object* obj = _ogGetObjectFromId(world);
   OpenGlobe::World* pWorld = dynamic_cast<OpenGlobe::World*>(obj);

   if (pWorld)
   {
      pWorld->Hide();
   }
}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogRenderWorld(int world)
{
   // this function is not implemented yet...
}

//------------------------------------------------------------------------------

// OptionVector OptionString(const std::string& sOptions);

OPENGLOBE_API int ogAddImageLayer(int world, const char* layeroptions)
{
   OpenGlobe::Object* pObject;
   OpenGlobe::Object* obj = _ogGetObjectFromId(world);
   OpenGlobe::World* pWorld = dynamic_cast<OpenGlobe::World*>(obj);

   if (pWorld)
   {
      std::string options(layeroptions); // layer options
      pObject = OpenGlobe::CreateObject(OpenGlobe::Object::IMAGELAYER, obj, options);
      OpenGlobe::ImageLayer* pImageLayer = dynamic_cast<OpenGlobe::ImageLayer*>(pObject);
      if (pImageLayer)
      {
         return pImageLayer->GetId();
      }
   }

   return -1;

}

//------------------------------------------------------------------------------

OPENGLOBE_API void ogRemoveImageLayer(int layer)
{
}

//------------------------------------------------------------------------------