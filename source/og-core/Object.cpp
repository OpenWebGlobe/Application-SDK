/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "Object.h"
#include "System.h"
#include <list>
#include <map>
#include <vector>
#include <cassert>
#include <iostream>

//---------------------------------------------------------------------------
using namespace OpenGlobe;
BeginPropertyMap(Object);
   XMLProperty(Object, "name", _name);
   XMLProperty(Object, "Properties", _props);
EndPropertyMap(Object);
//---------------------------------------------------------------------------

namespace OpenGlobe
{
   //---------------------------------------------------------------------------
   // OpenGlobe Object Map
   //---------------------------------------------------------------------------

   OPENGLOBE_API ObjectMap& GetObjectMap()
   {
      // Construct On First Use Idiom 
      // also see: http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.14
      static ObjectMap _mapobj;
      return _mapobj;
   }

   //---------------------------------------------------------------------------
   OPENGLOBE_API void _ogAddObject(OpenGlobe::Object* pObject)
   {
      ObjectMap& mapobj = GetObjectMap();
      int threadid = System::GetThreadId();
      ObjectMapIterator it = mapobj.find(threadid);

      if (it == mapobj.end())
      {
         // no list available yet for this process!! Create a new one
         ObjectList lst;
         mapobj.insert(std::pair<int, ObjectList>(threadid, lst));
         it = mapobj.find(threadid);
      }

      if (it != mapobj.end())
      {
         ObjectList& lst = it->second;
         lst.push_back(pObject);
      }
      else
      {
         assert(false); // there is some bug!
      }
   }
   //---------------------------------------------------------------------------
   OPENGLOBE_API void _ogRemoveObject(OpenGlobe::Object* pObject)
   {
      // remove object (iterate through all processes)
      ObjectMap& mapobj = GetObjectMap();
      int threadid = System::GetThreadId();
      ObjectMapIterator it = mapobj.find(threadid);

      while (it != mapobj.end())
      {
         ObjectList& lst = it->second;
         ObjectListIterator jt = lst.begin();
         while (jt != lst.end())
         {
            Object* pSearchObject = *jt;
            if (pSearchObject == pObject)
            {
               // found object -> remove from list.
               lst.erase(jt);

               if (lst.size() == 0) // list empty now ?
               {
                  mapobj.erase(it);
               }

               return;
            }
            
            jt++;
         }

         it++;
      }
   }

   OPENGLOBE_API void _ogCleanUp()
   {
      std::vector<Object*> vecDelete;
      // remove object (iterate through all processes)
      ObjectMap& mapobj = GetObjectMap();
      int threadid = System::GetThreadId();
      ObjectMapIterator it = mapobj.find(threadid);

      if (it != mapobj.end())
      {
         ObjectList& lst = it->second;
         ObjectListIterator jt = lst.begin();
         while (jt != lst.end())
         {
            Object* pObject = *jt;
            jt++;
            vecDelete.push_back(pObject);
         }
      }

      for (size_t i=0;i<vecDelete.size();i++)
      {
          delete vecDelete[i];
      }
      vecDelete.clear();
   }

   //---------------------------------------------------------------------------
   //------------------------------------------------------------------------------

   //---------------------------------------------------------------------------
   // generate unqiue id
   //---------------------------------------------------------------------------
   int _ogGenerateId()
   {
      static int lastid = 0;
      return lastid++;   
   };

   //---------------------------------------------------------------------------
   Object::Object()
      : _pParent(0), _cbfOnLoad(0), _cbfOnFailure(0), 
         _eObjectType(Object::OBJECT), _eObjectStatus(Object::UNKNOWN), _name("ogObject"),
        _id(-1), _bRegistered(false)
   {
   }
   //---------------------------------------------------------------------------
   Object::~Object()
   {
      // destructor
      int a;
      a = 0;

      if (_bRegistered)
      {
         this->UnregisterObject();
      }
   }
   //---------------------------------------------------------------------------
   int Object::GetId()
   {
      return _id;
   }
   //---------------------------------------------------------------------------
   Object::Type Object::GetObjectType()
   {
      return _eObjectType;
   }
   //---------------------------------------------------------------------------
   Object::Status Object::GetObjectStatus()
   {
      return _eObjectStatus;
   }
   //---------------------------------------------------------------------------
   std::string Object::GetClassName()
   {
      return types::GetClassNameFromTypeIdName(typeid(*this).name());
   }
   //---------------------------------------------------------------------------
   void Object::RegisterObject()
   {
      _id = _ogGenerateId();
      _ogAddObject(this);
      _bRegistered = true;
   }
   //---------------------------------------------------------------------------
   void Object::UnregisterObject()
   {
      if (!_bRegistered)
      {
         assert(false); // something is wrong!
         return;
      }

      // -> UNREGISTER, also make sure all children are unregistered
      _ogRemoveObject(this);
      _bRegistered = false;
   }
   //---------------------------------------------------------------------------
   void Object::OnInit()
   {
   }
   //---------------------------------------------------------------------------
   void Object::OnExit()
   {
   }
   //---------------------------------------------------------------------------
   void Object::OnReady()
   {
   }
   //---------------------------------------------------------------------------
   void Object::OnFailure()
   {
   }
   //---------------------------------------------------------------------------
   void Object::SetOptions(const std::vector<std::pair<std::string, std::string> >& options)
   {

   
   }
   //---------------------------------------------------------------------------
   OPENGLOBE_API unsigned int GetNumObjects()
   {
      return 0;
   }
   //---------------------------------------------------------------------------
   bool Object::_SetInternalProperty(const std::string& key, const std::string& value)
   {
      std::string sClass = this->GetClassName();

      access::Members members;
      access::Class::GetMembers(sClass, members);

      std::list< std::pair<std::string, access::Variable> >& varlist = members.GetVariableMap();
      std::list< std::pair<std::string, access::Variable> >::iterator jt = varlist.begin();

       while (jt != varlist.end())
       {
          std::string sName = jt->first;
          access::Variable& var = jt->second;

          if (key == sName)
          {
             // trying to set internal variable!  Make sure it is not readonly
             if (!var.readonly)
             {
                  var.FromString(value, this);
             }
             
             return true;
          }

          jt++;
       }

       return false; // no internal name found
   }
   //---------------------------------------------------------------------------
   void Object::SetStringProperty(const std::string& key, const std::string& value)
   {
      Property prop;
      prop.SetFromString(value);

      if (!_SetInternalProperty(key, prop.GetAsString()))
      {
         _props.SetProperty(key, prop);
      }
   }
   //---------------------------------------------------------------------------
   void Object::SetIntegerProperty(const std::string& key, int value)
   {
      Property prop;
      prop.SetFromInteger(value);

      if (!_SetInternalProperty(key, prop.GetAsString()))
      {
         _props.SetProperty(key, prop);
      }
   }
   //---------------------------------------------------------------------------
   void Object::SetDoubleProperty(const std::string& key, double value)
   {
      Property prop;
      prop.SetFromDouble(value);

      if (!_SetInternalProperty(key, prop.GetAsString()))
      {
         _props.SetProperty(key, prop);
      }
   }
   //---------------------------------------------------------------------------
   void Object::SetBoolProperty(const std::string& key, bool value)
   {
      Property prop;
      prop.SetFromBool(value);

      if (!_SetInternalProperty(key, prop.GetAsString()))
      {
         _props.SetProperty(key, prop);
      }
   }
   //---------------------------------------------------------------------------
   // Get Internal Property as string
   bool Object::_GetInternalProperty(const std::string& key, std::string& value)
   {
      std::string sClass = this->GetClassName();

      access::Members members;
      access::Class::GetMembers(sClass, members);

      std::list< std::pair<std::string, access::Variable> >& varlist = members.GetVariableMap();
      std::list< std::pair<std::string, access::Variable> >::iterator jt = varlist.begin();

       while (jt != varlist.end())
       {
          std::string sName = jt->first;
          access::Variable& var = jt->second;

          if (key == sName)
          {
            value = var.ToString(this);
            return true;
          }

          jt++;
       }

       return false; // no internal name found
   }
   //---------------------------------------------------------------------------
   std::string Object::GetStringProperty(const std::string& key)
   {
      std::string sValue;
      if (Object::_GetInternalProperty(key, sValue))
      {
         return sValue;
      }

      Property prop;
      if (_props.GetProperty(key, prop))
      {
         return prop.GetAsString();
      }

      return std::string();
   }
   //---------------------------------------------------------------------------
   int Object::GetIntegerProperty(const std::string& key)
   {
      std::string sValue;
      if (Object::_GetInternalProperty(key, sValue))
      {
         return atoi(sValue.c_str());
      }

      Property prop;
      if (_props.GetProperty(key, prop))
      {
         return prop.GetAsInteger();
      }

      return 0;
   }
   //---------------------------------------------------------------------------
   double Object::GetDoubleProperty(const std::string& key)
   {
      std::string sValue;
      if (Object::_GetInternalProperty(key, sValue))
      {
         return atof(sValue.c_str());
      }

      Property prop;
      if (_props.GetProperty(key, prop))
      {
         return prop.GetAsDouble();
      }
      return 0.0;
   }
   //---------------------------------------------------------------------------
   bool Object::GetBoolProperty(const std::string& key)
   {
      std::string sValue;
      if (Object::_GetInternalProperty(key, sValue))
      {
         if (sValue == "true")
            return true;
         else if (sValue == "false")
            return false;
      }

      Property prop;
      if (_props.GetProperty(key, prop))
      {
         return prop.GetAsBool();
      }

      return false;
   }
   //---------------------------------------------------------------------------
   bool Object::_IsInternalProperty(const std::string& key)
   {
      std::string sClass = this->GetClassName();

      access::Members members;
      access::Class::GetMembers(sClass, members);

      std::list< std::pair<std::string, access::Variable> >& varlist = members.GetVariableMap();
      std::list< std::pair<std::string, access::Variable> >::iterator jt = varlist.begin();

       while (jt != varlist.end())
       {
          std::string sName = jt->first;
          access::Variable& var = jt->second;

          if (key == sName)
          {
            return true;
          }

          jt++;
       }

       return false; // no internal name found
   }
   //---------------------------------------------------------------------------
   Property::Type Object::_getInternalType(const std::string& key)
   {
      std::string sClass = this->GetClassName();

      access::Members members;
      access::Class::GetMembers(sClass, members);

      std::list< std::pair<std::string, access::Variable> >& varlist = members.GetVariableMap();
      std::list< std::pair<std::string, access::Variable> >::iterator jt = varlist.begin();

       while (jt != varlist.end())
       {
          std::string sName = jt->first;
          access::Variable& var = jt->second;

          if (key == sName)
          {
             if (var.sTypename == typeid(unsigned int).name())
             {
               return Property::STRING;
             }
             else if (var.sTypename == typeid(unsigned int).name() || var.sTypename == typeid(int).name())
             {
               return Property::INTEGER;
             }
             else if (var.sTypename == typeid(double).name())
             {
               return Property::DOUBLE;
             }
             else if (var.sTypename == typeid(bool).name())
             {
               return Property::BOOL;
             }
             else
             {
               return Property::INVALID;
             }
          }

          jt++;
       }

       return Property::INVALID;
   }
   //---------------------------------------------------------------------------
   Property::Type Object::GetPropertyType(const std::string& key)
   {
      Property prop;

      if (_props.GetProperty(key, prop))
      {
            return prop.GetType();
      }
      else // invalid key or internal property
      {
         return _getInternalType(key);
      }
   }
   //---------------------------------------------------------------------------
   bool Object::HasProperty(const std::string& key)
   {
      if (_IsInternalProperty(key))
      {
         // found internal property -> so property exists...
         return true;
      }

      // try getting standard property
      Property prop;
      if (_props.GetProperty(key, prop))
      {
         // property exists!
         return true;
      }

      // not internal not standard property... so property doesn't exist
      return false;
   }
   //---------------------------------------------------------------------------
   bool Object::PropertyIsReadonly(const std::string& key)
   {
      // only internal properties can be read only
      std::string sClass = this->GetClassName();

      access::Members members;
      access::Class::GetMembers(sClass, members);

      std::list< std::pair<std::string, access::Variable> >& varlist = members.GetVariableMap();
      std::list< std::pair<std::string, access::Variable> >::iterator jt = varlist.begin();

       while (jt != varlist.end())
       {
          std::string sName = jt->first;
          access::Variable& var = jt->second;

          if (key == sName)
          {
               return var.readonly;
          }

          jt++;
       }

      return false;
   }
   //---------------------------------------------------------------------------
   bool Object::PropertyIsProtected(const std::string& key)
   {
      // all internal properties are protected. standard (user) properties can't be protected.
      return _IsInternalProperty(key);
   }
   //---------------------------------------------------------------------------
   unsigned int Object::GetNumProperties()
   {
      unsigned int numInternal = 0;

      std::string sClass = this->GetClassName();
      access::Members members;
      access::Class::GetMembers(sClass, members);
      numInternal = members.GetVariableMap().size();
      unsigned int numCustom = _props.GetPropertyMap()->size();

      return numInternal + numCustom;
   }
   //---------------------------------------------------------------------------
   std::string Object::GetPropertyKeyAt(unsigned int index)
   {
      int curidx = 0;

      //--------------------------------------------
      // Step 1: iterate through internal properties
      //--------------------------------------------
      std::string sClass = this->GetClassName();

      access::Members members;
      access::Class::GetMembers(sClass, members);

      std::list< std::pair<std::string, access::Variable> >& varlist = members.GetVariableMap();
      std::list< std::pair<std::string, access::Variable> >::iterator jt = varlist.begin();

       while (jt != varlist.end())
       {
          std::string sName = jt->first;
          access::Variable& var = jt->second;

          if (curidx == index)
          {
            return sName;
          }

          curidx++;
          jt++;
       }

       //--------------------------------------------
      // Step 2: iterate through standard properties
      //--------------------------------------------

      ogKeyValue* kv = _props.GetPropertyMap();
      ogKeyValue::iterator it = kv->begin();
      while (it != kv->end())
      {
         if (curidx == index)
         {
            return it->first;
         }
      
         curidx++;
         it++;
      }

      return std::string("");

   }
   //---------------------------------------------------------------------------
   void Object::RemoveProperty(const std::string& key)
   {
      _props.RemoveProperty(key);
   }
   //---------------------------------------------------------------------------
   std::string Object::ToXML()
   {
      std::ostringstream oss;
      access::Class::ToXML(oss, this->GetClassName(), this);
      return oss.str();
   }
   //---------------------------------------------------------------------------
}

