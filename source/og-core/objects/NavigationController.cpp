   
#include "NavigationController.h"
#include "Camera.h"


//---------------------------------------------------------------------------
using namespace OpenGlobe;
BeginPropertyMap(NavigationController);
   Inherits(Object);
   //XMLPropertyReadOnly(Scene, "width", _width);
EndPropertyMap(NavigationController);
//---------------------------------------------------------------------------

namespace OpenGlobe
{
   //---------------------------------------------------------------------------
   NavigationController::NavigationController()
      : _parentCamera(0),
      _cbInit(0),_cbExit(0),_cbMouseDown(0),_cbMouseUp(0),
      _cbMouseMove(0),_cbMouseWheel(0),_cbKeyDown(0),_cbKeyUp(0),
      _cbPreRender(0),_cbPostRender(0),_cbResize(0),_cbTimer(0),
      _cbCommand(0)
   {
   }
   //---------------------------------------------------------------------------
   NavigationController::~NavigationController()
   {
      _name = "ogNavigationController";
      _eObjectType = Object::NAVIGATIONCONTROLLER;
   }
   //---------------------------------------------------------------------------
   void NavigationController::CreateNavigationController(Camera* parentCamera)
   {
      _parentCamera = parentCamera;
   }
   //---------------------------------------------------------------------------
   void NavigationController::SetOptions(const OptionVector& options)
   {
      _parentCamera = dynamic_cast<Camera*>(_pParent);

   }
   //---------------------------------------------------------------------------
   void NavigationController::SetNavigationEventInit(CallBack_NavigationInit cbf)
   {
      _cbInit = cbf;
   }
   //---------------------------------------------------------------------------
   void NavigationController::SetNavigationEventExit(CallBack_NavigationExit cbf)
   {
      _cbExit = cbf;
   }
   //---------------------------------------------------------------------------
   void NavigationController::SetNavigationEventMouseDown(CallBack_NavigationMouseDown cbf)
   {
      _cbMouseDown = cbf;
   }
   //---------------------------------------------------------------------------
   void NavigationController::SetNavigationEventMouseUp(CallBack_NavigationMouseUp cbf)
   {
      _cbMouseUp = cbf;
   }
   //---------------------------------------------------------------------------
   void NavigationController::SetNavigationEventMouseMove (CallBack_NavigationMouseMove cbf)
   {
      _cbMouseMove = cbf;
   }
   //---------------------------------------------------------------------------
   void NavigationController::SetNavigationEventMouseWheel (CallBack_NavigationMouseWheel cbf)
   {
      _cbMouseWheel = cbf;
   }
   //---------------------------------------------------------------------------
   void NavigationController::SetNavigationEventKeyDown (CallBack_NavigationKeyDown cbf)
   {
      _cbKeyDown = cbf;
   }
   //---------------------------------------------------------------------------
   void NavigationController::SetNavigationEventKeyUp(CallBack_NavigationKeyUp cbf)
   {
      _cbKeyUp = cbf;
   }
   //---------------------------------------------------------------------------
   void NavigationController::SetNavigationEventPreRender (CallBack_NavigationPreRender cbf)
   {
      _cbPreRender = cbf;
   }
   //---------------------------------------------------------------------------
   void NavigationController::SetNavigationEventPostRender(CallBack_NavigationPostRender cbf)
   {
      _cbPostRender = cbf;
   }
   //---------------------------------------------------------------------------
   void NavigationController::SetNavigationEventResize(CallBack_NavigationResize cbf)
   {
      _cbResize = cbf;
   }
   //---------------------------------------------------------------------------
   void NavigationController::SetNavigationEventTimer(CallBack_NavigationTimer cbf)
   {
      _cbTimer = cbf;
   }
   //---------------------------------------------------------------------------
   void NavigationController::SetNavigationEventCommand (CallBack_NavigationCommand cbf)
   {
      _cbCommand = cbf;
   }
   //---------------------------------------------------------------------------
}

