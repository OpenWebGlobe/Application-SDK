
#ifndef _NAVIGATIONCONTROLLER_H
#define _NAVIGATIONCONTROLLER_H

#include "Object.h"
#include <list>


namespace OpenGlobe
{
   class Camera;
   class OPENGLOBE_API NavigationController : public OpenGlobe::Object
   {
   public:
      NavigationController();
      virtual ~NavigationController();

      void CreateNavigationController(Camera* parentCamera);

      virtual void SetOptions(const OptionVector& options);

      void SetNavigationEventInit(CallBack_NavigationInit cbf);
      void SetNavigationEventExit(CallBack_NavigationExit cbf);
      void SetNavigationEventMouseDown(CallBack_NavigationMouseDown cbf);
      void SetNavigationEventMouseUp(CallBack_NavigationMouseUp cbf);
      void SetNavigationEventMouseMove (CallBack_NavigationMouseMove cbf);
      void SetNavigationEventMouseWheel (CallBack_NavigationMouseWheel cbf);
      void SetNavigationEventKeyDown (CallBack_NavigationKeyDown cbf);
      void SetNavigationEventKeyUp(CallBack_NavigationKeyUp cbf);
      void SetNavigationEventPreRender (CallBack_NavigationPreRender cbf);
      void SetNavigationEventPostRender(CallBack_NavigationPostRender cbf);
      void SetNavigationEventResize(CallBack_NavigationResize cbf);
      void SetNavigationEventTimer(CallBack_NavigationTimer cbf);
      void SetNavigationEventCommand(CallBack_NavigationCommand cbf);

   protected:
      Camera*                 _parentCamera;

   private:
      CallBack_NavigationInit       _cbInit;
      CallBack_NavigationExit       _cbExit;
      CallBack_NavigationMouseDown  _cbMouseDown;
      CallBack_NavigationMouseUp    _cbMouseUp;
      CallBack_NavigationMouseMove  _cbMouseMove;
      CallBack_NavigationMouseWheel _cbMouseWheel;
      CallBack_NavigationKeyDown    _cbKeyDown;
      CallBack_NavigationKeyUp      _cbKeyUp;
      CallBack_NavigationPreRender  _cbPreRender;
      CallBack_NavigationPostRender _cbPostRender;
      CallBack_NavigationResize     _cbResize;
      CallBack_NavigationTimer      _cbTimer;
      CallBack_NavigationCommand    _cbCommand;

   };

}

#endif


