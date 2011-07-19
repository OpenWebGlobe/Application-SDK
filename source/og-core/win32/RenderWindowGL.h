/*******************************************************************************
Project       : OpenGL Render Window for Windows
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _RenderWindowGL_H
#define _RenderWindowGL_H

#include "og.h"
#include <vector>
#include "Object.h"
#include "renderer/IRenderWindow.h"
#include "renderer/IRenderEngine.h"

namespace OpenGlobe
{
   

   class OPENGLOBE_API RenderWindowGL  : public IRenderWindow 
   {
   public:
      RenderWindowGL();
      virtual ~RenderWindowGL();

      // Set context id this render window belongs to
      virtual void SetParentContext(Context* pContext);

      // Create Window using current settings
      virtual bool Create();

      // Destroy Window (send close event)
      virtual void Destroy();

      virtual int GetWidth() { return _nWidth;}
      virtual int GetHeight() { return _nHeight;}
      
      // Create Context in an existing window handle (for example HWND under windows) 
      virtual bool CreateUsingExistingHandle(void* handle);

      // Set options. You can only do this BEFORE creating window
      // keys: "width", "height", "bpp", ...
      virtual void SetOptions(const OptionVector& options);

      // Set Callback functions:
      virtual void SetCallBack_MouseDown(CallBack_MouseDown cbf);
      virtual void SetCallBack_MouseUp(CallBack_MouseUp cbf);
      virtual void SetCallBack_MouseMove(CallBack_MouseMove cbf);
      virtual void SetCallBack_MouseWheel(CallBack_MouseWheel cbf);
      virtual void SetCallBack_KeyDown(CallBack_KeyDown cbf);
      virtual void SetCallBack_KeyUp(CallBack_KeyUp cbf);
      virtual void SetCallBack_Render(CallBack_Render cbf);
      virtual void SetCallBack_Resize(CallBack_Resize cbf);
      virtual void SetCallBack_Timer(CallBack_Timer cbf);
      virtual void SetCallBack_Init(CallBack_Init cbf);
      virtual void SetCallBack_Exit(CallBack_Exit cbf);
      virtual void SetCallBack_Object(CallBack_Object cbf);
      virtual void SetCallBack_RenderGeometry(CallBack_RenderGeometry cbf);
      virtual void SetCallBack_BeginRender(CallBack_BeginRender cbf);
      virtual void SetCallBack_EndRender(CallBack_EndRender cbf);


      // Get Render Engine
      virtual IRenderEngine* GetEngine() {return _pEngine;}

      // Setters

      virtual void SetWidth(int nWidth) { _nWidth = nWidth;} 
      virtual void SetHeight(int nHeight) { _nHeight = nHeight;}

      // Events
      void OnInit();
      void OnExit();
      void OnActivate(bool active);
      bool OnRender();
      void OnMouseDown(int button, int xPos, int yPos);
      void OnMouseUp(int button, int xPos, int yPos);
      void OnMouseMove(int xPos, int yPos);
      void OnMouseWheel(double delta);
      void OnKeyDown(unsigned long key);
      void OnKeyUp(unsigned long key);
      void OnResize(int w, int h);
      void OnTimer(double dt);

      bool IsActive();

      // call wglMakeCurrent or equivalent for platform
      void MakeCurrent();
      void SwapBuffers();

   protected:
      int            _nWidth;
      int            _nHeight;
      int            _nBpp;
      bool           _bStereo;
      bool           _bFullscreen;
      std::string    _sTitle;

    private:
      int _mx, _my; // current mouse pos
      IRenderEngine* _pEngine;
      Context*       _pContext; // context object
      void           _FreeStuff();
      bool           _bSelfCreatedHandle;
      void*          _handle;
      void*          _hdc;
      void*          _hrc;
      void*          _hinstance;
      bool           _bActive;
      OptionVector               _options;
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

