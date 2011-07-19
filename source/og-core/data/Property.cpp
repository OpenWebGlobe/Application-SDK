/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "data/Property.h"
#include "xml/xml.h"
#include <string>
#include <sstream>

//------------------------------------------------------------------------------
std::string PropertiesToXML(void* pAddress, std::string sTag)
{
   std::ostringstream os;
   Properties* data = (Properties*)(pAddress);

   int adv = access::Members::GetXMLadvance();
   bool prev = false;

   if (data->GetPropertyMap()->size() == 0)
   {
      return std::string(); // only serialize if there is actually data
   }

   os << "<" << sTag << ">\n";

   ogKeyValue::iterator it = data->GetPropertyMap()->begin();
   while (it != data->GetPropertyMap()->end())
   {
      for (int i=0;i<adv+3;i++) {os << " ";}

      os << "<" << it->first;
      
      if (it->second.GetType() == OpenGlobe::Property::BOOL)
      {
         os << " type=bool";
      }
      else if (it->second.GetType() == OpenGlobe::Property::INTEGER)
      {
         os << " type=integer";
      }
      else if (it->second.GetType() == OpenGlobe::Property::STRING)
      {
         //os << " type=string"; // string is default...
      }
      else if (it->second.GetType() == OpenGlobe::Property::DOUBLE)
      {
         os << " type=double";
      }
      os << ">";
      os << XMLUtils::Encode(it->second.GetAsString());
      os << "</" << it->first << ">\n";
      prev = true;
      it++;
   }

   for (int i=0;i<adv;i++) {os << " ";}
   os << "</" << sTag << ">";

   return os.str();
}
TYPE_TO_XML_CONVERSION(Properties, PropertiesToXML)

//------------------------------------------------------------------------------
// String to property map
// "key0=value0; key1=value1; key2=value2"
//      ---> (key0,value0),(key1,value1),(key2,value2)
//    
void XMLToProperties(void* pAddress, std::string sXML)
{
   // Set a char using first character of string...
   Properties* data = (Properties*)(pAddress);

    std::stringstream os;
    os << sXML;

     bool bValueTag;
     std::string attribs;
     std::vector<std::pair<std::string, std::string> > vAttrib;
     std::string sTag = ObjectFactory::GetTag(os, bValueTag);
     std::string sValue = ObjectFactory::GetValue(os);
     std::string sTagEnd = "/" + sTag;

     do
     {
         sTag = ObjectFactory::GetTag(os, bValueTag, false, attribs);
         vAttrib = ObjectFactory::ParseAttribs(attribs); 
         sValue = ObjectFactory::GetValue(os);
         if (sTag != sTagEnd)
         {
           
            std::string sKey = sTag;
            std::string sProperty = XMLUtils::Decode(sValue);

            OpenGlobe::Property prop;
            prop.SetFromString(sProperty);
            
            if (vAttrib.size() > 0)
            {
               for (size_t i=0;i<vAttrib.size();i++)
               {
                  if (vAttrib[i].first == "type")
                  {
                     // type found
                     if (vAttrib[i].second == "string") // string type
                     {
                        prop.SetType(OpenGlobe::Property::STRING);
                     }
                     else if (vAttrib[i].second == "integer") // integer type
                     {
                        prop.SetType(OpenGlobe::Property::INTEGER);
                     }
                     else if (vAttrib[i].second == "double") // double type
                     {
                        prop.SetType(OpenGlobe::Property::DOUBLE);
                     }
                     else if (vAttrib[i].second == "bool") // bool type
                     {
                        prop.SetType(OpenGlobe::Property::BOOL);
                     }
                  }
               }
            }
            data->SetProperty(sKey, prop);

            sTag = ObjectFactory::GetTag(os, bValueTag);
         }

     } while (sTag != sTagEnd && os.good());

   
   // parse sValue
}
XML_TO_TYPE_CONVERSION(Properties, XMLToProperties)
//------------------------------------------------------------------------------

namespace OpenGlobe
{
   //---------------------------------------------------------------------------
   Property::Property() 
   {  
      _readonly = false;
      _protected = false;
      _type = Property::STRING;
   }
   //---------------------------------------------------------------------------
   Property::~Property() 
   {
   }
   //---------------------------------------------------------------------------
   void Property::SetReadonly(bool b)
   {
      _readonly = b;
   }
   //---------------------------------------------------------------------------
   void Property::SetProtected(bool b)
   {
      _protected = b;
   }
   //---------------------------------------------------------------------------
   OpenGlobe::Property::Type Property::GetType()
   {
      return _type;
   }
   //---------------------------------------------------------------------------
   std::string Property::GetAsString()
   {
      return _value;
   }
   //---------------------------------------------------------------------------
   int Property::GetAsInteger()
   {
      if (_value == "false")
         return 0;
      if (_value == "true")
         return 1;
      return atoi(_value.c_str());
   }
   //---------------------------------------------------------------------------
   double Property::GetAsDouble()
   {
      if (_value == "false")
         return 0.0;
      if (_value == "true")
         return 1.0;
      return atof(_value.c_str());
   }
   //---------------------------------------------------------------------------
   bool Property::GetAsBool()
   {
      if (_value == "false" || _value == "0")
         return false;
      if (_value == "true" || _value == "1")
         return true;

      return false;
   }
   //---------------------------------------------------------------------------
   void Property::SetFromString(const std::string& sValue)
   {
      _value = sValue;
      _type = Property::STRING;
   }
   //---------------------------------------------------------------------------
   void Property::SetFromInteger(int value)
   {
      std::ostringstream os;
      os << value;
      _value = os.str();
      _type = Property::INTEGER;
   }
   //---------------------------------------------------------------------------
   void Property::SetFromDouble(double value)
   {
      std::ostringstream os;
      os.precision(17);
      os << value;
      _value = os.str();
      _type = Property::DOUBLE;
   }
   //---------------------------------------------------------------------------
   void Property::SetFromBool(bool value)
   {
      if (value)
         _value = "true";
      else
         _value = "false";

      _type = Property::BOOL;
   }
   //---------------------------------------------------------------------------
   //---------------------------------------------------------------------------

      PropertyMap::PropertyMap() 
      {
         _map = new ogKeyValue();
      }

      PropertyMap::~PropertyMap() 
      {
         if (_map)
         {
            delete _map;
         }
      }

      void PropertyMap::SetProperty(const std::string& key, OpenGlobe::Property& prop)
      {
         ogKeyValue::iterator it = _map->find(key);
         if (it != _map->end()) // already exists
         {
            if (!it->second.IsReadonly())
            {
               it->second = prop; // overwrite if not readonly
            }
         }
         else // add new entry!
         {
            _map->insert(std::pair<std::string, OpenGlobe::Property>(key, prop));
         }
      }

      bool PropertyMap::GetProperty(const std::string& key, OpenGlobe::Property& result)
      {
         ogKeyValue::iterator it = _map->find(key);
         if (it != _map->end()) // key exists
         {
            result = it->second;
            return true;
         }

         return false; // key doesn't exist!
      }

      void PropertyMap::RemoveProperty(const std::string& key)
      {
         ogKeyValue::iterator it = _map->find(key);
         if (it != _map->end()) // key exists
         {
            _map->erase(it);
         }

      }
}

