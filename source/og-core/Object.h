/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _OPENGLOBE_OBJECTORIENTED_H
#define _OPENGLOBE_OBJECTORIENTED_H

#include "og.h"
#include "xml/xml.h"
#include "data/Property.h"
#include "data/DiskCache.h"
#include "shared_ptr.h"
#include <string>
#include <list>
#include <map>

//------------------------------------------------------------------------------
namespace OpenGlobe
{
   // the central cache retrieval
   OPENGLOBE_API Cache& _ogCache();

   //---------------------------------------------------------------------------
   typedef std::vector<std::pair<std::string, std::string> > OptionVector;

   //! Object Class. This is the base class for all OpenGlobe Objects
   class OPENGLOBE_API Object
   {
   public:
      // Object Type. All Object types are listed here
      enum Type
      {
         CONTEXT,
         SCENE,
         WORLD,
         IMAGELAYER,
         ELEVATIONLAYER,
         POILAYER,
         GEOMETRYLAYER,
         VOXELLAYER,
         IMAGE,
         TEXTURE,
         PIXELBUFFER,
         GEOMETRY,
         MESH,
         SURFACE,
         CAMERA,
         TEXT,
         BINARYDATA,
         LIGHT,
         NAVIGATIONCONTROLLER,

         OBJECT,
      };

      //------------------------------------------------------------------------
      // Object Status (for asynchronous objects)
      enum Status
      {
         READY    = 0,
         BUSY     = 1,
         FAILED   = 2,
         UNKNOWN  = 255,
      };

       //------------------------------------------------------------------------

   public:
      Object();
      virtual ~Object();

      int GetId();

      OpenGlobe::Object*  GetParent() {return _pParent;}
      void SetParent(OpenGlobe::Object* obj){ _pParent = obj;}

      // void AddChild(OpenGlobe::Object* child);
      // void RemoveChild(OpenGlobe::Object* child);

      void SetCallbackOnLoad(CallBack_Object cbf) {_cbfOnLoad = cbf;}
      void SetCallbackOnFailure(CallBack_Object cbf) {_cbfOnFailure = cbf;}

      Object::Type GetObjectType();
      Object::Status GetObjectStatus();

      // Set Object name
      void SetName(const std::string& sName){ _name = sName;}
      const std::string& GetName(){return _name;}

      std::string GetClassName();

      // "events"
      virtual void OnInit();
      virtual void OnExit();

      virtual void OnReady();          // for async objects: called on ready
      virtual void OnFailure();        // for async objects: called on failure

      virtual void RegisterObject();
      virtual void UnregisterObject();

      virtual void SetOptions(const OptionVector& options);

      // Object -> XML
      virtual std::string ToXML();

      // Set Property (internal + custom)
      void SetStringProperty(const std::string& key, const std::string& value);
      void SetIntegerProperty(const std::string& key, int value);
      void SetDoubleProperty(const std::string& key, double value);
      void SetBoolProperty(const std::string& key, bool value);

      // Get Property (internal + custom)
      std::string GetStringProperty(const std::string& key);
      int GetIntegerProperty(const std::string& key);
      double GetDoubleProperty(const std::string& key);
      bool GetBoolProperty(const std::string& key);

      // Property functions
      Property::Type GetPropertyType(const std::string& key);
      bool HasProperty(const std::string& key);
      bool PropertyIsReadonly(const std::string& key);
      bool PropertyIsProtected(const std::string& key);

      // Get total number of properties
      unsigned int GetNumProperties();
      // Get name of property key at specified index
      std::string GetPropertyKeyAt(unsigned int index);

      // Remove property
      void RemoveProperty(const std::string& key);

   protected:
      bool _SetInternalProperty(const std::string& key, const std::string& value);
      bool _GetInternalProperty(const std::string& key, std::string& value);
      bool _IsInternalProperty(const std::string& key);
      Property::Type _getInternalType(const std::string& key);
      Object::Status                   _eObjectStatus;
      Object::Type                     _eObjectType;
      int                              _id;
      std::string                      _name;
      Object*                          _pParent;      // Reference to parent Object
      std::list<Object*>               _vChildren;
      bool                             _bRegistered;
      Properties                       _props;        // Custom Properties

   private:
      CallBack_Object                  _cbfOnLoad;
      CallBack_Object                  _cbfOnFailure;
   };

   //---------------------------------------------------------------------------
   typedef std::list<OpenGlobe::Object*>        ObjectList;
   typedef ObjectList::iterator                 ObjectListIterator;
   typedef std::map<int, ObjectList >           ObjectMap;
   typedef std::map<int, ObjectList >::iterator ObjectMapIterator;
   OPENGLOBE_API ObjectMap& GetObjectMap();
   OPENGLOBE_API void _ogAddObject(OpenGlobe::Object* pObject);
   OPENGLOBE_API void _ogRemoveObject(OpenGlobe::Object* pObject);
   OPENGLOBE_API void _ogCleanUp();
   //---------------------------------------------------------------------------

}

#endif