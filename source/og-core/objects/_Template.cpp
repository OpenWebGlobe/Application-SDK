   
#include "Camera.h"


//---------------------------------------------------------------------------
using namespace OpenGlobe;
BeginPropertyMap(_Template);
   Inherits(Object);
   //XMLPropertyReadOnly(Scene, "width", _width);
EndPropertyMap(_Template);
//---------------------------------------------------------------------------

namespace OpenGlobe
{
   //---------------------------------------------------------------------------
   _Template::_Template()
      : _parentScene(0)
   {
      _name = "ogImage";
      _eObjectType = Object::IMAGE;

   }
   //---------------------------------------------------------------------------
   _Template::~_Template()
   {

   }
   //---------------------------------------------------------------------------
   void _Template::SetOptions(const OptionVector& options)
   {
      _parentScene = dynamic_cast<Scene*>(_pParent);


   }
   //---------------------------------------------------------------------------
}