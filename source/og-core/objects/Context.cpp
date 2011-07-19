/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "Context.h"
#include "RenderFactory.h"
#include "Scene.h"

//---------------------------------------------------------------------------
using namespace OpenGlobe;
BeginPropertyMap(Context);
   Inherits(Object);
   XMLPropertyReadOnly(Context, "width", _width);
   XMLPropertyReadOnly(Context, "height", _height);
   XMLPropertyReadOnly(Context, "bpp", _bpp);
   XMLPropertyReadOnly(Context, "fullscreen", _fullscreen);
   XMLPropertyReadOnly(Context, "stereo", _stereo);
   XMLPropertyReadOnly(Context, "virtual", _virtual);
EndPropertyMap(Context);
//--------------------------------------------------------------------------

namespace OpenGlobe
{
   //---------------------------------------------------------------------------

   Context::Context()
      : _cbMouseDown(0),_cbMouseUp(0),_cbMouseMove(0),_cbMouseWheel(0),_cbKeyDown(0),
      _cbKeyUp(0),_cbRender(0),_cbResize(0), _cbTimer(0),_cbInit(0), _cbExit(0),
      _cbObject(0),_cbRenderGeometry(0),_cbBeginRender(0),_cbEndRender(0), _AttachedScene(0), _scenegraph(0)
   {
      _name = "ogContext";
      _eObjectType = Object::CONTEXT;
   }
   
   //---------------------------------------------------------------------------
   
   Context::~Context()
   {
      if (_scenegraph)
      {
         delete _scenegraph;
         _scenegraph = 0;
      }
   }

   //---------------------------------------------------------------------------
   
   void Context::_assignCallbacks()
   {
      IRenderWindow* pRenderWindow = _qRenderWindow.get();
      if (pRenderWindow)
      {
         pRenderWindow->SetCallBack_MouseDown(_cbMouseDown);
         pRenderWindow->SetCallBack_MouseUp(_cbMouseUp);
         pRenderWindow->SetCallBack_MouseMove(_cbMouseMove);
         pRenderWindow->SetCallBack_MouseWheel(_cbMouseWheel);
         pRenderWindow->SetCallBack_KeyDown(_cbKeyDown);
         pRenderWindow->SetCallBack_KeyUp(_cbKeyUp);
         pRenderWindow->SetCallBack_Render(_cbRender);
         pRenderWindow->SetCallBack_Resize(_cbResize);
         pRenderWindow->SetCallBack_Timer(_cbTimer);
         pRenderWindow->SetCallBack_Init(_cbInit);
         pRenderWindow->SetCallBack_Exit(_cbExit);
         pRenderWindow->SetCallBack_Object(_cbObject);
         pRenderWindow->SetCallBack_RenderGeometry(_cbRenderGeometry);
         pRenderWindow->SetCallBack_BeginRender(_cbBeginRender);
         pRenderWindow->SetCallBack_EndRender(_cbEndRender);
      }
   }

   //---------------------------------------------------------------------------
   void Context::CreateContext()
   {
      _qRenderWindow = RenderFactory::RenderWindow(_options);
      
      _assignCallbacks();
      _qRenderWindow->SetParentContext(this);
      _qRenderWindow->Create();
      _scenegraph = new SceneGraph(GetRenderEngine());
      _scenegraph->Setup();      // this also calls OnInit on every node...

      // Because the render window didn't have the scenegraph yet, 
      // the resize event must be called manually.
      int w = _qRenderWindow->GetWidth();
      int h = _qRenderWindow->GetHeight();
      _scenegraph->Resize(w,h); // force resize event!
   }

   //---------------------------------------------------------------------------

   void Context::CreateContextExistingHandle(void* handle)
   {
      _qRenderWindow = RenderFactory::RenderWindow(_options);
      _assignCallbacks();
      _qRenderWindow->SetParentContext(this);
      _qRenderWindow->CreateUsingExistingHandle(handle);
   }

   //---------------------------------------------------------------------------

   void Context::SetOptions(const OptionVector& options)
   {
      std::vector<std::pair<std::string, std::string> >::const_iterator it = options.begin();

      while (it!=options.end())
      {
         // key: it->first
         // value: it->second  (value os optional, presence of a key may be enough)

         if (it->first == "engine")
         {
            /*int w = atoi(it->second.c_str());
            if (w>0 && w<=8192)
            {
               _width = w;
            }*/
         }
         
         it++;
      }


      _options = options;
   }

   //---------------------------------------------------------------------------
    IRenderEngine* Context::GetRenderEngine()
    {
      return _qRenderWindow->GetEngine();
    }
   //---------------------------------------------------------------------------
    IRenderWindow* Context::GetRenderWindow()
    {
      return _qRenderWindow.get();
    }
   //---------------------------------------------------------------------------
    void Context::SetCallBack_MouseDown(CallBack_MouseDown cbf)
    {
      _cbMouseDown = cbf;
      if (Context::GetRenderWindow())
      {
         Context::GetRenderWindow()->SetCallBack_MouseDown(cbf);
      }
    }
   //---------------------------------------------------------------------------
    void Context::SetCallBack_MouseUp(CallBack_MouseUp cbf)
    {
      _cbMouseUp = cbf;
      if (Context::GetRenderWindow())
      {
         Context::GetRenderWindow()->SetCallBack_MouseUp(cbf);
      }
    }
   //---------------------------------------------------------------------------
    void Context::SetCallBack_MouseMove(CallBack_MouseMove cbf)
    {
      _cbMouseMove = cbf;
      if (Context::GetRenderWindow())
      {
         Context::GetRenderWindow()->SetCallBack_MouseMove(cbf);
      }
    }
   //---------------------------------------------------------------------------
    void Context::SetCallBack_MouseWheel(CallBack_MouseWheel cbf)
    {
      _cbMouseWheel = cbf;
      if (Context::GetRenderWindow())
      {
         Context::GetRenderWindow()->SetCallBack_MouseWheel(cbf);
      }
    }
   //---------------------------------------------------------------------------
    void Context::SetCallBack_KeyDown(CallBack_KeyDown cbf)
    {
      _cbKeyDown = cbf;
      if (Context::GetRenderWindow())
      {
         Context::GetRenderWindow()->SetCallBack_KeyDown(cbf);
      }
    }
   //---------------------------------------------------------------------------
    void Context::SetCallBack_KeyUp(CallBack_KeyUp cbf)
    {
      _cbKeyUp = cbf;
      if (Context::GetRenderWindow())
      {
         Context::GetRenderWindow()->SetCallBack_KeyUp(cbf);
      }
    }
   //---------------------------------------------------------------------------
    void Context::SetCallBack_Render(CallBack_Render cbf)
    {
      _cbRender = cbf;
      if (Context::GetRenderWindow())
      {
         Context::GetRenderWindow()->SetCallBack_Render(cbf);
      }
    }
   //---------------------------------------------------------------------------
    void Context::SetCallBack_Resize(CallBack_Resize cbf)
    {
      _cbResize = cbf;
      if (Context::GetRenderWindow())
      {
         Context::GetRenderWindow()->SetCallBack_Resize(cbf);
      }
    }
   //---------------------------------------------------------------------------
    void Context::SetCallBack_Timer(CallBack_Timer cbf)
    {
      _cbTimer = cbf;
      if (Context::GetRenderWindow())
      {
         Context::GetRenderWindow()->SetCallBack_Timer(cbf);
      }
    }
   //---------------------------------------------------------------------------
    void Context::SetCallBack_Init(CallBack_Init cbf)
    {
      _cbInit = cbf;
      if (Context::GetRenderWindow())
      {
         Context::GetRenderWindow()->SetCallBack_Init(cbf);
      }
    }
   //---------------------------------------------------------------------------
    void Context::SetCallBack_Exit(CallBack_Exit cbf)
    {
      _cbExit = cbf;
      if (Context::GetRenderWindow())
      {
         Context::GetRenderWindow()->SetCallBack_Exit(cbf);
      }
    }
   //---------------------------------------------------------------------------
    void Context::SetCallBack_Object(CallBack_Object cbf)
    {
      _cbObject = cbf;
      if (Context::GetRenderWindow())
      {
         Context::GetRenderWindow()->SetCallBack_Object(cbf);
      }
    }
   //---------------------------------------------------------------------------
    void Context::SetCallBack_RenderGeometry(CallBack_RenderGeometry cbf)
    {
      _cbRenderGeometry = cbf;
      if (Context::GetRenderWindow())
      {
         Context::GetRenderWindow()->SetCallBack_RenderGeometry(cbf);
      }
    }
   //---------------------------------------------------------------------------
    void Context::SetCallBack_BeginRender(CallBack_BeginRender cbf)
    {
      _cbBeginRender = cbf;
      if (Context::GetRenderWindow())
      {
         Context::GetRenderWindow()->SetCallBack_BeginRender(cbf);
      } 
    }
   //---------------------------------------------------------------------------
    void Context::SetCallBack_EndRender(CallBack_EndRender cbf)
    {
      _cbEndRender = cbf;
      if (Context::GetRenderWindow())
      {
         Context::GetRenderWindow()->SetCallBack_EndRender(cbf);
      }
    }
   //---------------------------------------------------------------------------
   int Context::GetWidth()
   {
      if (Context::GetRenderWindow())
      {
         return Context::GetRenderWindow()->GetWidth();
      }
      else
      {
         return 0;
      }
   }
   //--------------------------------------------------------------------------
   int Context::GetHeight()
   {
     if (Context::GetRenderWindow())
     {
        return Context::GetRenderWindow()->GetHeight();
     }
     else
     {
        return 0;
     }
   }

   //---------------------------------------------------------------------------

   void Context::Render()
   {
      if (_AttachedScene)
      {
         _AttachedScene->Render(GetRenderEngine());
      }
   }

   //---------------------------------------------------------------------------

}