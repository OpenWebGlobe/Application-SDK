/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _PROPERTY_H
#define _PROPERTY_H

#include "og.h"
#include <string>
#include <map>


namespace OpenGlobe
{
   //---------------------------------------------------------------------------
   class OPENGLOBE_API Property
   {
   public: 
      Property();
      virtual ~Property();

      enum Type
      {
         STRING,
         INTEGER,
         DOUBLE,
         BOOL,

         INVALID,
      };

      void SetReadonly(bool b);
      bool IsReadonly(){ return _readonly;}

      void SetProtected(bool b);
      bool IsProtected(){ return _protected;}

      virtual OpenGlobe::Property::Type GetType();
      void SetType(OpenGlobe::Property::Type pt) {_type = pt;} 

      virtual std::string GetAsString();
      virtual int GetAsInteger();
      virtual double GetAsDouble();
      virtual bool GetAsBool();

      virtual void SetFromString(const std::string& sValue);
      virtual void SetFromInteger(int value);
      virtual void SetFromDouble(double value);
      virtual void SetFromBool(bool value);

      protected:
         OpenGlobe::Property::Type  _type;      // native type
         std::string                _value;     // value (as string)
         bool _readonly;
         bool _protected;
   };

   //---------------------------------------------------------------------------

}

typedef  std::map<std::string, OpenGlobe::Property> ogKeyValue;

namespace OpenGlobe
{
   class OPENGLOBE_API PropertyMap
   {
   public:
      PropertyMap();
      virtual ~PropertyMap();

      void SetProperty(const std::string& key, OpenGlobe::Property& prop);
      bool GetProperty(const std::string& key, OpenGlobe::Property& result);
      void RemoveProperty(const std::string& key);

      ogKeyValue* GetPropertyMap() {return _map;}

   protected:
      ogKeyValue* _map;
   };

   //---------------------------------------------------------------------------
}

typedef OpenGlobe::PropertyMap   Properties;


#endif

