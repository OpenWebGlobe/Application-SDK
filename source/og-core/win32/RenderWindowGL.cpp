/*******************************************************************************
Project       : OpenGL Render Window for Windows
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "RenderWindowGL.h"
#include "renderer/gl/RenderEngineGL.h"
#include <map>
#include "GL/glew.h"
#include "string/StringUtils.h"
#include "system/Timer.h"
#include "objects/Context.h"
#include "objects/Scene.h"
#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>



namespace OpenGlobe
{
   LRESULT CALLBACK ogWndProc(HWND	hWnd,	UINT uMsg, WPARAM wParam, LPARAM lParam);		


   //--------------------------------------------------------------------------
   // To hold HWND in RenderWindow a static map is created.
   typedef std::map<HWND, RenderWindowGL*> WinMap;
   typedef std::pair<HWND, RenderWindowGL*> WinPair;
   WinMap& GetWinMap()
   {
      static WinMap map;
      return map;
   }
   //--------------------------------------------------------------------------
   RenderWindowGL* GetRenderClass(HWND hwnd)
   {
      WinMap::iterator it = GetWinMap().find(hwnd);
      if (it != GetWinMap().end())
      {
         return it->second;
      }

      return 0;
   }
   //--------------------------------------------------------------------------
   void AddRenderClass(HWND hwnd, RenderWindowGL* pWin)
   {
      GetWinMap().insert(WinPair(hwnd, pWin));
   }
   //--------------------------------------------------------------------------
   void RemoveRenderClass(HWND hwnd)
   {
      WinMap::iterator it = GetWinMap().find(hwnd);
      if (it != GetWinMap().end())
      {  
         GetWinMap().erase(it);
      }
   }
   //--------------------------------------------------------------------------
   RenderWindowGL::RenderWindowGL()
   :  _nWidth(640), _nHeight(480), _nBpp(32), _bStereo(false), _bFullscreen(false), 
      _handle(0), _bSelfCreatedHandle(false), _hdc(0), _hrc(0), _bActive(false),
      _cbMouseDown(0),_cbMouseUp(0),_cbMouseMove(0),_cbMouseWheel(0),_cbKeyDown(0),
      _cbKeyUp(0),_cbRender(0),_cbResize(0), _cbTimer(0),_cbInit(0), _cbExit(0),
      _cbObject(0),_cbRenderGeometry(0),_cbBeginRender(0),_cbEndRender(0),
      _pContext(0), _pEngine(0), _mx(0), _my(0)
   {
   }
   //--------------------------------------------------------------------------
   RenderWindowGL::~RenderWindowGL()
   {
      if (_handle)
      {
         RemoveRenderClass((HWND)_handle);
      }

      if (_pEngine)
      {
         delete _pEngine;
         _pEngine = 0;
      }
   }
   //--------------------------------------------------------------------------
   void RenderWindowGL::SetParentContext(Context* pContext)
   {
      _pContext = pContext;
   }
   //--------------------------------------------------------------------------
   void RenderWindowGL::SetOptions(const std::vector<std::pair<std::string, std::string> >& options)
   {
      _options = options;

      std::vector<std::pair<std::string, std::string> >::const_iterator it = options.begin();

      while (it!=options.end())
      {
         // key: it->first
         // value: it->second  (value os optional, presence of a key may be enough)

         if (it->first == "width")
         {
            int w = atoi(it->second.c_str());
            if (w>0 && w<=8192)
            {
               _nWidth = w;
            }
         }
         else if (it->first == "height")
         {
            int h = atoi(it->second.c_str());
            if (h>0 && h<=8192)
            {
               _nHeight = h;
            }
         }
         else if (it->first == "title")
         {
            _sTitle = it->second;
         }
         else if (it->first == "fullscreen")
         {
            _bFullscreen = true;
         }

         it++;
      }
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::SetCallBack_MouseDown(CallBack_MouseDown cbf)
   {
      _cbMouseDown = cbf;
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::SetCallBack_MouseUp(CallBack_MouseUp cbf)
   {
      _cbMouseUp = cbf;
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::SetCallBack_MouseMove(CallBack_MouseMove cbf)
   {
      _cbMouseMove = cbf;
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::SetCallBack_MouseWheel(CallBack_MouseWheel cbf)
   {
      _cbMouseWheel = cbf;
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::SetCallBack_KeyDown(CallBack_KeyDown cbf)
   {
      _cbKeyDown = cbf;
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::SetCallBack_KeyUp(CallBack_KeyUp cbf)
   {
      _cbKeyUp = cbf;
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::SetCallBack_Render(CallBack_Render cbf)
   {
      _cbRender = cbf;
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::SetCallBack_Resize(CallBack_Resize cbf)
   {
      _cbResize = cbf;
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::SetCallBack_Timer(CallBack_Timer cbf)
   {
      _cbTimer = cbf;
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::SetCallBack_Init(CallBack_Init cbf)
   {
      _cbInit = cbf;
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::SetCallBack_Exit(CallBack_Exit cbf)
   {
      _cbExit = cbf;
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::SetCallBack_Object(CallBack_Object cbf)
   {
      _cbObject = cbf;
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::SetCallBack_RenderGeometry(CallBack_RenderGeometry cbf)
   {
      _cbRenderGeometry = cbf;
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::SetCallBack_BeginRender(CallBack_BeginRender cbf)
   {
      _cbBeginRender = cbf;
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::SetCallBack_EndRender(CallBack_EndRender cbf)
   {
      _cbEndRender = cbf;
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::OnInit()
   {
      _pEngine->InitEngine(_options);
     
      if (_pContext && _pContext->GetId() != -1 )
      {
         if (_cbInit)
         {
            _cbInit(_pContext->GetId());
         }
      }

   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::OnExit()
   {
      if (_pContext && _pContext->GetId() != -1 )
      {
         SceneGraph* pGraph = _pContext->GetSceneGraph();
         if (pGraph)
         {
            pGraph->OnExit();
         }

         if (_cbExit)
         {
            _cbExit(_pContext->GetId());
         }
      }
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::OnActivate(bool active)
   {
      std::cout << "OnActivate(" << active << ")\n";
      _bActive = active;
   }
   //---------------------------------------------------------------------------
   bool RenderWindowGL::OnRender()
   {
      glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
      glPushAttrib(GL_ALL_ATTRIB_BITS);

      glShadeModel(GL_SMOOTH);
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
      glDepthRange(0.0, 1.0);

      if (GLEW_ARB_texture_rectangle)
         glDisable(GL_TEXTURE_RECTANGLE_ARB);
      glEnable(GL_TEXTURE_2D);

      glBindTexture(GL_TEXTURE_2D, 0);

      glMatrixMode(GL_TEXTURE);
      glLoadIdentity(); 

      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
    
      //glEnable(GL_CULL_FACE); 

      _pEngine->SetViewport(0,0, _nWidth, _nHeight);
      _pEngine->Clear(true, true);


      

      // Call custom render function
      if (_pContext )
      {
         SceneGraph* pSceneGraph = _pContext->GetSceneGraph();

         pSceneGraph->SetViewport(0,0,_nWidth,_nHeight);
         


         // Render Context (which renders scene etc.)
         //_pContext->Render();

         pSceneGraph->SetCallbackRender(_cbRender);

         

         int sceneid = -1;
         if (_pContext->GetScene())
         {
            sceneid = _pContext->GetScene()->GetId();
         }
         pSceneGraph->SetSceneId(sceneid);
         pSceneGraph->Traverse();
      }

     
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);

      glPopAttrib();
      glPopClientAttrib();

      return true;
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::OnMouseDown(int button, int xPos, int yPos)
   {
      _mx = xPos;
      _my = yPos;

      if (_pContext && _pContext->GetId() != -1)
      {
         SceneGraph* pGraph = _pContext->GetSceneGraph();
         if (pGraph)
         {
            pGraph->MouseClick(button, xPos, yPos);
         }

         if (_cbMouseDown)
         {
            _cbMouseDown(_pContext->GetId(), button, xPos, yPos);
         }
      }
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::OnMouseUp(int button, int xPos, int yPos)
   {
      _mx = xPos;
      _my = yPos;

      if (_pContext && _pContext->GetId() != -1)
      {
         SceneGraph* pGraph = _pContext->GetSceneGraph();
         if (pGraph)
         {
            pGraph->MouseRelease(button, xPos, yPos);
         }

         if (_cbMouseUp)
         {
            _cbMouseUp(_pContext->GetId(), button, xPos, yPos);
         }
      }
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::OnMouseMove(int xPos, int yPos)
   {
      _mx = xPos;
      _my = yPos;

      if (_pContext && _pContext->GetId() != -1)
      {
         SceneGraph* pGraph = _pContext->GetSceneGraph();
         if (pGraph)
         {
            pGraph->MouseMove(xPos, yPos);
         }

         if (_cbMouseMove)
         {
            _cbMouseMove(_pContext->GetId(), xPos, yPos);
         }
      }
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::OnMouseWheel(double delta)
   {
      if (_pContext && _pContext->GetId() != -1)
      {
         // in the scene graph, mouse wheel is interpreted as mouse button 4 and 5.
         int button = 4;
         if (delta>0)
            button = 5;

         SceneGraph* pGraph = _pContext->GetSceneGraph();
         if (pGraph)
         {
            pGraph->MouseClick(button, _mx, _my);
         }

         if (_cbMouseWheel)
         {
            _cbMouseWheel(_pContext->GetId(), (float)delta);
         }
      }
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::OnKeyDown(unsigned long key)
   {
      if (_pContext && _pContext->GetId() != -1)
      {
         SceneGraph* pGraph = _pContext->GetSceneGraph();
         if (pGraph)
         {
            pGraph->KeyDown(key);
         }

         if (_cbKeyDown)
         {
            _cbKeyDown(_pContext->GetId(), (int)key);
         }
      }
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::OnKeyUp(unsigned long key)
   {
      if (_pContext && _pContext->GetId() != -1)
      {
         SceneGraph* pGraph = _pContext->GetSceneGraph();
         if (pGraph)
         {
            pGraph->KeyUp(key);
         }

         if (_cbKeyUp)
         {
            _cbKeyUp(_pContext->GetId(), (int)key);
         }
      }
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::OnResize(int w, int h)
   {
      _nWidth = w; _nHeight = h;

      if (_pContext)
      {
         _pContext->SetDimension(w, h);
      }


      if (_pContext && _pContext->GetId() != -1)
      {
         SceneGraph* pGraph = _pContext->GetSceneGraph();
         if (pGraph)
         {
            pGraph->Resize(w,h);
         }

         if (_cbResize)
         {
            _cbResize(_pContext->GetId(), w, h);
         }
      }


   }
   //---------------------------------------------------------------------------
   void  RenderWindowGL::OnTimer(double dt)
   {
      if (_pContext && _pContext->GetId() != -1)
      {
         SceneGraph* pGraph = _pContext->GetSceneGraph();
         if (pGraph)
         {
            pGraph->Tick(dt);
         }

         if (_cbTimer)
         {
            _cbTimer(_pContext->GetId(), (float)dt);
         }
      }
   }
   //---------------------------------------------------------------------------
   bool RenderWindowGL::IsActive()
   {
      return _bActive;
   }
   //---------------------------------------------------------------------------
   inline std::wstring _GenClassName(RenderWindowGL* pWin)
   {
      std::wostringstream oss;
      oss << L"OpenGL_";
      oss << (int)pWin;
      return oss.str();
   }
   //---------------------------------------------------------------------------
   bool RenderWindowGL::Create()
   {
      if (_pContext)
      {
         _pContext->SetContextInfo(_nBpp, _bFullscreen, _bStereo, false);
         _pContext->SetDimension(_nWidth, _nHeight);
      }

      // Create Render Engine
      _pEngine = new OpenGlobe::RenderEngineGL();
     
      // Create Render Window
      HWND     hwnd;
      WNDCLASS	wc;						   // Windows Class Structure
      DWORD		dwExStyle;				   // Window Extended Style
      DWORD		dwStyle;				      // Window Style
      RECT		WindowRect;				   
      WindowRect.left=(long)0; WindowRect.right=(long)_nWidth;		
      WindowRect.top=(long)0; WindowRect.bottom=(long)_nHeight;		
      HINSTANCE hInstance = GetModuleHandle(NULL);
      _hinstance = (void*)hInstance;
      std::wstring clsname;	

      wc.style			   = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
      wc.lpfnWndProc		= (WNDPROC) ogWndProc;					   // WndProc Handles Messages
      wc.cbClsExtra		= 0;									         // No Extra Window Data
      wc.cbWndExtra		= 0;									         // No Extra Window Data
      wc.hInstance		= hInstance;							      // Set The Instance
      wc.hIcon			   = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
      wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
      wc.hbrBackground	= NULL;									
      wc.lpszMenuName		= NULL;									
      clsname = _GenClassName(this);
      wc.lpszClassName	= clsname.c_str();						

      if (!RegisterClass(&wc))									      // Register The Window Class
      {
         MessageBox(NULL,L"Failed To Register The Window Class.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
         return false;											
      }

      if (_bFullscreen)												// Attempt Fullscreen Mode?
      {
         DEVMODE dmScreenSettings;								// Device Mode
         memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
         dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
         dmScreenSettings.dmPelsWidth	= _nWidth;				// Selected Screen Width
         dmScreenSettings.dmPelsHeight	= _nHeight;				// Selected Screen Height
         dmScreenSettings.dmBitsPerPel	= _nBpp;					// Selected Bits Per Pixel
         dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

         if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
         {
            _bFullscreen=false;		// can't go to fullscreen, use windowed mode instead
         }
      }

      if (_bFullscreen)	// Still fullscreen mode?
      {
         dwExStyle=WS_EX_APPWINDOW; // Window Extended Style
         dwStyle=WS_POPUP;	 // Windows Style
         //ShowCursor(FALSE); // Hide Mouse Pointer -> do an option later...
      }
      else
      {
         dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;   // Window Extended Style
         dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
      }

      AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);	// Adjust Window To True Requested Size

      std::wstring sTitle = StringUtils::Utf8_To_wstring(_sTitle);

      if (!(hwnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
            clsname.c_str(),	// Class Name
            sTitle.c_str(),								// Window Title
            dwStyle |							// Defined Window Style
            WS_CLIPSIBLINGS |					// Required Window Style
            WS_CLIPCHILDREN,					// Required Window Style
            0, 0,								// Window Position
            WindowRect.right-WindowRect.left,	// Calculate Window Width
            WindowRect.bottom-WindowRect.top,	// Calculate Window Height
            0,								// No Parent Window
            NULL,								// No Menu
            hInstance,							// Instance
            NULL)))								// Dont Pass Anything To WM_CREATE
         {
            _FreeStuff();								// Reset The Display
            MessageBox(NULL,L"Window Creation Error.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
            return FALSE;								// Return FALSE
         }

      _bSelfCreatedHandle = true;

      return CreateUsingExistingHandle((void*)hwnd);
   }

   //---------------------------------------------------------------------------
   void RenderWindowGL::Destroy()
   {
      HWND hwnd = (HWND)_handle;
      SendMessage(hwnd, WM_CLOSE, 0, 0); 

   }

   //---------------------------------------------------------------------------     
   bool RenderWindowGL::CreateUsingExistingHandle(void* handle)
   {
      _handle = handle;
      HWND hwnd = (HWND)handle;
      AddRenderClass(hwnd, this);

      if (!_bSelfCreatedHandle)
      {
         // this is not our window -> overwrite window proc
         SetWindowLong(hwnd, GWL_WNDPROC, (LONG)ogWndProc);
      }

      DWORD dwFlags;
      
      dwFlags = PFD_DRAW_TO_WINDOW |						// Format Must Support Window
			       PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
			       PFD_DOUBLEBUFFER;							// Must Support Double Buffering

      if (_bStereo)
      {
          dwFlags |= PFD_STEREO;    // Enable Quad buffered Stereo Rendering (if possible...)
      }


      BYTE  iPixelType = PFD_TYPE_RGBA;	// Request An RGBA Format

      static	PIXELFORMATDESCRIPTOR pfd=		// pfd Tells Windows How We Want Things To Be
      {
         sizeof(PIXELFORMATDESCRIPTOR),   // Size Of This Pixel Format Descriptor
         1,                               // Version Number
		   dwFlags,
         iPixelType,								
         _nBpp,                           // Select Our Color Depth
         0, 0, 0, 0, 0, 0,                // Color Bits Ignored
         0,									      // No Alpha Buffer
         0,										   // Shift Bit Ignored
         0,	                              // No Accumulation Buffer
         0, 0, 0, 0,							   // Accumulation Bits Ignored
         32,									   // 32Bit Z-Buffer (Depth Buffer)  
         0,									      // No Stencil Buffer
         0,									      // No Auxiliary Buffer
         PFD_MAIN_PLANE,						// Main Drawing Layer
         0,									      // Reserved
         0, 0, 0								   // Layer Masks Ignored
      };

      HDC hDC;

      if (!(hDC=GetDC(hwnd)))	// Did We Get A Device Context?
      {
         _FreeStuff();								
         MessageBox(NULL,L"Can't Create A GL Device Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
         return false;								
      }

      _hdc = (void*) hDC;

      GLuint		PixelFormat;

      if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
      {
         _FreeStuff();
         MessageBox(NULL,L"Can't Find A Suitable PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
         return FALSE;
      }

      if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
      {
         _FreeStuff();	
         MessageBox(NULL,L"Can't Set The PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
         return false;	
      }

      HGLRC hRC;

      if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
      {
         _FreeStuff();								
         MessageBox(NULL,L"Can't Create A GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
         return false;								
      }
      _hrc = (void*)hRC;

      if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
      {
         _FreeStuff();								// Reset The Display
         MessageBox(NULL,L"Can't Activate The GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
         return false;							
      }

      ShowWindow(hwnd,SW_SHOW);	 // Show The Window
      SetForegroundWindow(hwnd);	 // Slightly Higher Priority
      SetFocus(hwnd);				 // Sets Keyboard Focus To The Window

      OnInit();
      OnResize(_nWidth, _nHeight);
					
      return true;
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::MakeCurrent()
   {
      HDC	      hDC   = (HDC) _hdc;	      
      HGLRC		   hRC  = (HGLRC) _hrc;
      wglMakeCurrent(hDC, hRC);
   }
   //---------------------------------------------------------------------------
   void RenderWindowGL::SwapBuffers()
   {
      HDC	      hDC   = (HDC) _hdc;	 
      ::SwapBuffers(hDC);
   }
   //---------------------------------------------------------------------------
   void  RenderWindowGL::_FreeStuff()
   {
      HDC	      hDC   = (HDC) _hdc;	      
      HGLRC		   hRC  = (HGLRC) _hrc;		     
      HWND		   hWnd = (HWND)_handle; 
      HINSTANCE   hInstance = (HINSTANCE)_hinstance;

      if (_bFullscreen)        // Are We In Fullscreen Mode?
      {
         ChangeDisplaySettings(NULL,0);         // If So Switch Back To The Desktop
         ShowCursor(TRUE);								// Show Mouse Pointer
      }

      if (hRC) // Do We Have A Rendering Context?
      {
         if (!wglMakeCurrent(NULL,NULL)) // Are We Able To Release The DC And RC Contexts?
         {
            MessageBox(NULL,L"Release Of DC And RC Failed.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
         }

         // call OnExit();

         if (!wglDeleteContext(hRC)) 
         {
            MessageBox(NULL,L"Release Rendering Context Failed.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
         }

         _hrc=0;										// Set RC To NULL
      }

      if (hDC && !ReleaseDC(hWnd,hDC))	// Are We Able To Release The DC
      {
         MessageBox(NULL,L"Release Device Context Failed.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
         _hdc=0;// Set DC To NULL
      }

      if (_bSelfCreatedHandle)
      {
         _bSelfCreatedHandle = false;

         if (hWnd && !DestroyWindow(hWnd))// Are We Able To Destroy The Window?
         {
            MessageBox(NULL,L"Could Not Release hWnd.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
            RemoveRenderClass(hWnd); // remove from map
            _handle=0; // Set hWnd To NULL
         }

         std::wstring clsname = _GenClassName(this);
         if (!UnregisterClass(clsname.c_str(),hInstance))// Are We Able To Unregister Class
         {
            MessageBox(NULL,L"Could Not Unregister Class.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
            _hinstance=0;	// Set hInstance To NULL
         }
      }
   }
   //---------------------------------------------------------------------------
   LRESULT CALLBACK ogWndProc(HWND	hWnd,	UINT uMsg, WPARAM wParam, LPARAM lParam)
   {
      if (uMsg == WM_CREATE || uMsg == WM_GETMINMAXINFO || uMsg == WM_NCCREATE || uMsg == WM_NCCALCSIZE)
      {
         return DefWindowProc(hWnd,uMsg,wParam,lParam);
      }
      // retrieve render window from map
      RenderWindowGL* pWin = GetRenderClass(hWnd);
      if (!pWin)
      {
         // window already deleted...
         return DefWindowProc(hWnd,uMsg,wParam,lParam);
      }

      switch (uMsg)									
      {
         case WM_CLOSE:
         {
            pWin->OnExit();
            PostQuitMessage(0);		     // Send A Quit Message
            return 0;	
         }
         //---------------------------------------------------------------------
         case WM_ACTIVATE:	// Window Activate Message
         {
            if (!HIWORD(wParam))					
            {
               pWin->OnActivate(true);
            }
            else
            {
               pWin->OnActivate(false);
            }
            return 0;								
         }
         //---------------------------------------------------------------------
         case WM_SYSCOMMAND:
         {
            switch (wParam)
            {
               case SC_SCREENSAVE:
               case SC_MONITORPOWER:
               return 0;
            }
            break;
         }

         //---------------------------------------------------------------------
         case WM_LBUTTONDOWN:
         case WM_RBUTTONDOWN:
         case WM_MBUTTONDOWN:
         {
            int button = -1;

            if (wParam & MK_LBUTTON)
            {
               button = 1;
            }
            else if (wParam & MK_RBUTTON)
            {
               button = 2;
            }
            else if (wParam & MK_MBUTTON)
            {
               button = 3;
            }

            int xPos, yPos;

            xPos = LOWORD(lParam); 
            yPos = HIWORD(lParam); 

            if (button != -1)
            {
               pWin->OnMouseDown(button, xPos, yPos);
            }
         
            return 0;
         }
         //---------------------------------------------------------------------
         case WM_LBUTTONUP:
         {
            int button = 1;
            int xPos, yPos;

            xPos = LOWORD(lParam); 
            yPos = HIWORD(lParam); 
            pWin->OnMouseUp(button, xPos, yPos);
            return 0; 
         }
         //---------------------------------------------------------------------
         case WM_RBUTTONUP:
         {
            int button = 2;
            int xPos, yPos;

            xPos = LOWORD(lParam); 
            yPos = HIWORD(lParam); 
            pWin->OnMouseUp(button, xPos, yPos);
            return 0; 
         }
         //---------------------------------------------------------------------
         case WM_MBUTTONUP:
         {
            int button = 3;
            int xPos, yPos;
            xPos = LOWORD(lParam); 
            yPos = HIWORD(lParam); 
            pWin->OnMouseUp(button, xPos, yPos);
            return 0;  
         }
         //---------------------------------------------------------------------
         case WM_MOUSEMOVE:
         {
            int xPos = LOWORD(lParam); 
            int yPos = HIWORD(lParam); 

            pWin->OnMouseMove(xPos, yPos);
            return 0;
         }
         //---------------------------------------------------------------------
         case WM_MOUSEWHEEL:
         {
            short hw = HIWORD(wParam);
            double delta = double(hw) / double(WHEEL_DELTA);
            pWin->OnMouseWheel(delta);
            return 0;
         }
         //---------------------------------------------------------------------
         case WM_KEYDOWN:							// Is A Key Being Held Down?
         {				
            pWin->OnKeyDown(wParam);  // todo: use lParam       
            return 0;								
         }
         //---------------------------------------------------------------------
         case WM_KEYUP:								// Has A Key Been Released?
         {
            pWin->OnKeyUp(wParam);					
            return 0;								
         }
         //---------------------------------------------------------------------
         case WM_SIZE:								// Resize The OpenGL Window
         {
            int Width = LOWORD(lParam);
            int Height = HIWORD(lParam); // Loword=Width, Hiword=Height

            pWin->SetWidth(Width);
            pWin->SetHeight(Height);
            pWin->OnResize(Width, Height);  
            return 0;								
         }
         //---------------------------------------------------------------------
      }   

      return DefWindowProc(hWnd,uMsg,wParam,lParam); // unhandled messages are passed...
   }

   // the effective Render Loop:
   OPENGLOBE_API int Exec()
   {
      MSG      msg;					// Windows Message Structure
      bool	   bDone=false;      // Bool Variable To Exit Loop
      double   t0,t1;            // Timer
    
      t0 = Timer::getRealTimeHighPrecision();

      while(!bDone)									
      {
         if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	
         {
            if (msg.message==WM_QUIT)				
            {
               bDone=TRUE;						
            }
            else									
            {
               TranslateMessage(&msg);	// Translate The Message
               DispatchMessage(&msg);  // Dispatch The Message
            }
         }
         else // If There Are No Messages
         {
            t1 = Timer::getRealTimeHighPrecision();
          
            // iterate through all windows in map:
            WinMap& map = GetWinMap();
            WinMap::const_iterator it = map.begin();
            RenderWindowGL* pWindow = 0;
            while (it != map.end())
            {
               pWindow = it->second;
               bool bActive = pWindow->IsActive();

               if (bActive)
               {
                   pWindow->MakeCurrent();
                   if (!pWindow->OnRender())
                   {
                     bDone = TRUE;
                   }
                   pWindow->SwapBuffers();
              
               }
               pWindow->OnTimer(t1-t0);         
               it++;
            }
          
          
            t0 = t1;
         }

         // Process cache requests...
         OpenGlobe::_ogCache().process();
      }
      
      // Cleanup:
      // todo: iterate through all render windows and call OnExit
      
      // finish all current downloads:
      OpenGlobe::_ogCache().wait();
      _ogCleanUp();

      return msg.wParam;							// Exit The Program
   }

}