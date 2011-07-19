/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _CONTEXT_H
#define _CONTEXT_H

#include "Object.h"
#include "renderer/IRenderEngine.h"
#include "renderer/IRenderWindow.h"
#include "scenegraph/SceneGraph.h"

namespace OpenGlobe
{
   class OPENGLOBE_API Scene;

   class OPENGLOBE_API Context : public OpenGlobe::Object
   {
   public:
      Context();
      virtual ~Context();

      // Create Context
      void CreateContext();

      // Create Context by using an existing handle (under windows this would be a hwnd)
      void CreateContextExistingHandle(void* handle);

      // Get Render Engine (only possible after callign CreateContext or CreateContextExistingHandle)
      IRenderEngine* GetRenderEngine();

      // Get Render Window (only possible after callign CreateContext or CreateContextExistingHandle)
      IRenderWindow* GetRenderWindow();

      void SetCallBack_MouseDown(CallBack_MouseDown cbf);
      void SetCallBack_MouseUp(CallBack_MouseUp cbf);
      void SetCallBack_MouseMove(CallBack_MouseMove cbf);
      void SetCallBack_MouseWheel(CallBack_MouseWheel cbf);
      void SetCallBack_KeyDown(CallBack_KeyDown cbf);
      void SetCallBack_KeyUp(CallBack_KeyUp cbf);
      void SetCallBack_Render(CallBack_Render cbf);
      void SetCallBack_Resize(CallBack_Resize cbf);
      void SetCallBack_Timer(CallBack_Timer cbf);
      void SetCallBack_Init(CallBack_Init cbf);
      void SetCallBack_Exit(CallBack_Exit cbf);
      void SetCallBack_Object(CallBack_Object cbf);
      void SetCallBack_RenderGeometry(CallBack_RenderGeometry cbf);
      void SetCallBack_BeginRender(CallBack_BeginRender cbf);
      void SetCallBack_EndRender(CallBack_EndRender cbf);

      virtual void SetOptions(const OptionVector& options);

      int GetWidth();
      int GetHeight();

      void SetContextInfo(int nBpp, bool bFullscreen, bool bStereo, bool bVirtual) { _bpp = nBpp; _fullscreen = bFullscreen, _stereo = bStereo, _virtual = bVirtual;}
      void SetDimension(int w, int h) { _width = w; _height = h;}

      void SetScene(Scene* pScene) {_AttachedScene = pScene;}
      Scene* GetScene() { return _AttachedScene;}

      // Retrieve SceneGraph attached to context
      SceneGraph* GetSceneGraph(){return _scenegraph;}

      void Render();

   protected:
      int _width;
      int _height;
      int  _bpp;
      bool _fullscreen;
      bool _stereo;
      bool _virtual;

      shared_ptr<IRenderWindow> _qRenderWindow;
      OptionVector              _options;
   private:
      void _assignCallbacks();
      SceneGraph*                _scenegraph;
      Scene*                     _AttachedScene;
      CallBack_MouseDown         _cbMouseDown;
      CallBack_MouseUp           _cbMouseUp;
      CallBack_MouseMove         _cbMouseMove;
      CallBack_MouseWheel        _cbMouseWheel;
      CallBack_KeyDown           _cbKeyDown;
      CallBack_KeyUp             _cbKeyUp;
      CallBack_Render            _cbRender;
      CallBack_Resize            _cbResize;
      CallBack_Timer             _cbTimer;
      CallBack_Init              _cbInit;
      CallBack_Exit              _cbExit;
      CallBack_Object            _cbObject;
      CallBack_RenderGeometry    _cbRenderGeometry;
      CallBack_BeginRender       _cbBeginRender;
      CallBack_EndRender         _cbEndRender;
      
   };

}

#endif

