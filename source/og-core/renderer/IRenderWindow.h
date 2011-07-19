/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _IRENDERWINDOW_H
#define _IRENDERWINDOW_H

#include "og.h"
#include "renderer/IRenderEngine.h"
#include "Object.h"

namespace OpenGlobe
{
   class Context;

   class OPENGLOBE_API IRenderWindow  
   {
   public:
      IRenderWindow() {}
      virtual ~IRenderWindow(){}

      // Set parent context
      virtual void SetParentContext(Context* pContext) = 0;

      // Create Window using current settings
      virtual bool Create() = 0;

      // Destroy (Close) Window
      virtual void Destroy() = 0;

      virtual int GetWidth() = 0;
      virtual int GetHeight() = 0;
      
      // Create Context in an existing window handle (for example HWND under windows) 
      virtual bool CreateUsingExistingHandle(void* handle) = 0;

      // Set options. You can only do this BEFORE creating window
      // keys: "width", "height", "bpp", ...
      virtual void SetOptions(const OptionVector& options) = 0;

      // Set Callback functions:
      virtual void SetCallBack_MouseDown(CallBack_MouseDown cbf) = 0;
      virtual void SetCallBack_MouseUp(CallBack_MouseUp cbf) = 0;
      virtual void SetCallBack_MouseMove(CallBack_MouseMove cbf) = 0;
      virtual void SetCallBack_MouseWheel(CallBack_MouseWheel cbf) = 0;
      virtual void SetCallBack_KeyDown(CallBack_KeyDown cbf) = 0;
      virtual void SetCallBack_KeyUp(CallBack_KeyUp cbf) = 0;
      virtual void SetCallBack_Render(CallBack_Render cbf) = 0;
      virtual void SetCallBack_Resize(CallBack_Resize cbf) = 0;
      virtual void SetCallBack_Timer(CallBack_Timer cbf) = 0;
      virtual void SetCallBack_Init(CallBack_Init cbf) = 0;
      virtual void SetCallBack_Exit(CallBack_Exit cbf) = 0;
      virtual void SetCallBack_Object(CallBack_Object cbf) = 0;
      virtual void SetCallBack_RenderGeometry(CallBack_RenderGeometry cbf) = 0;
      virtual void SetCallBack_BeginRender(CallBack_BeginRender cbf) = 0;
      virtual void SetCallBack_EndRender(CallBack_EndRender cbf) = 0;

      // Get Render Engine
      virtual IRenderEngine* GetEngine() = 0;

      
    
   };
   // Enter Message Loop

   OPENGLOBE_API int Exec();
}



#endif
