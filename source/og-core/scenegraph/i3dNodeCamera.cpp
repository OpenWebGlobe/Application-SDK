/*******************************************************************************
Project       : i3D Studio  
Purpose       : Simple Camera Node
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#include "i3dNodeCamera.h"
#include "scenegraph/PickResult.h"
#include <GL/glew.h>

//-----------------------------------------------------------------------------

i3dNodeCamera::i3dNodeCamera()
{
   _sNodeName = "Camera";
   _sNodeClassName = "i3dNodeCamera";

   _fAspect = 1.0;
   _fFovY = 45;
   _fFovX = 45;
   _fNear = 0.00001;
   _fFar = 10.0;
   _nScreenWidth = _nScreenHeight = 0;
   _pixelsizeX = _pixelsizeY = 0.001;
   _camtype = CT_PERSPECTIVE_PROJECTION;
   _oleft = _oright = _obottom = _otop = 0;

}

i3dNodeCamera::~i3dNodeCamera()
{

}

void i3dNodeCamera::OnInit()
{

}

void i3dNodeCamera::OnExit()
{
}

//------------------------------------------------------------------------------

void i3dNodeCamera::OnRender(IRenderEngine* pEngine)
{
}

//------------------------------------------------------------------------------

void i3dNodeCamera::CreatePerspectiveProjection(double fovY, double fNear, double fFar)
{
   _camtype = CT_PERSPECTIVE_PROJECTION;
   _fFovY = fovY;
   _fNear = fNear;
   _fFar = fFar;
   _SetupCamera(_oMatrixProjection);
}

//------------------------------------------------------------------------------

void i3dNodeCamera::CreateCustomProjection(const mat4<double>& P)
{
   _camtype = CT_CUSTOM_MATRIX;
   _oMatrixProjection = P;
   _SetupCamera(_oMatrixProjection);
}

//------------------------------------------------------------------------------

void i3dNodeCamera::CreateOrtho2d(double left, double right, double bottom, double top)
{
   _camtype = CT_ORTHO_2D;
   _fNear = -1;
   _fFar = 1;
   _oleft = left;
   _oright = right;
   _obottom = bottom;
   _otop = top;
   _SetupCamera(_oMatrixProjection);
}

//------------------------------------------------------------------------------

void i3dNodeCamera::CreateProjectionPixelsize(double pixelsizex, double pixelsizey, double fFar, double fNear)
{
   _camtype = CT_PIXELSIZE;
   _pixelsizeX = pixelsizex;
   _pixelsizeY = pixelsizey;
   _fNear = fNear;
   _fFar = fFar;
   _SetupCamera(_oMatrixProjection);
}

//------------------------------------------------------------------------------

void i3dNodeCamera::_SetupCamera(mat4<double>& M)
{
   //_nScreenWidth = _oViewport.GetWidth() - _oViewport.nX;
   //_nScreenHeight = _oViewport.GetHeight() - _oViewport.nY;

   int w = _nScreenWidth;
   int h = _nScreenHeight;

    _fAspect= 1.0 * (double)w / (double)h;

   //--------------------------------------------------------------------------

   if (_camtype == CT_PERSPECTIVE_PROJECTION) //-------------------------------
   {
      double f = 1.0 / tan(DEG2RAD(_fFovY)/2);
      
      M.Set(f/_fAspect, 0, 0,                               0,
            0,          f, 0,                               0,
            0,          0, (_fFar+_fNear) / (_fNear-_fFar), 2*_fFar*_fNear/(_fNear-_fFar),
            0,          0, -1,                              0);
   }
   else if (_camtype == CT_PIXELSIZE) //----------------------------------------
   {
      
      _fFovX = RAD2DEG(2.0 * atan((0.5*_nScreenWidth*_pixelsizeX)/_fNear));
      _fFovY = RAD2DEG(2.0 * atan((0.5*_nScreenHeight*_pixelsizeY)/_fNear));
      
      if (_fFovX<1) _fFovX = 45; // #!!!
      if (_fFovY<1) _fFovY = 45;
      if (_fFovX>179) _fFovX = 45; // #!!!
      if (_fFovY>179) _fFovY = 45;

      double w = 1.0 / tan(DEG2RAD(_fFovX)*0.5);
      double h = 1.0 / tan(DEG2RAD(_fFovY)*0.5);
 
      double Q = (_fFar) / (_fFar-_fNear);

      M.Set(w,  0,  0, 0,
            0,  h,  0, 0,
            0,  0,  (_fFar+_fNear) / (_fNear-_fFar), 2*_fFar*_fNear/(_fNear-_fFar),
            0,  0, -1, 0);

   }               
   else if (_camtype == CT_CUSTOM_MATRIX) //------------------------------------
   {
      // _oMatrixProjection already set
   }
   else if (_camtype == CT_ORTHO_2D) //-----------------------------------------
   {
      M.Set(2.0/(_oright-_oleft), 0,                    0,                   -(_oright+_oleft)/(_oright-_oleft),
            0,                    2.0/(_otop-_obottom), 0,                   -(_otop+_obottom)/(_otop-_obottom),
            0,                    0,                    -2.0/(_fFar-_fNear), -(_fFar+_fNear)/(_fFar-_fNear),
            0,                    0,                    0,                   1);

   }
   else //----------------------------------------------------------------------
   {
      assert(false); // camera type not supported
   }
}

//------------------------------------------------------------------------------

void i3dNodeCamera::OnTraverse(TraversalState& oTrav)
{
   mat4<double> M;

   oTrav.GetViewport(_oViewport);

   oTrav.AccumulateProjection(_oMatrixProjection);
   oTrav.GetModelViewMatrix(_oMatrixModelView);

   _viewdirection = oTrav.GetCameraViewVector();
   _right = oTrav.GetCameraRightVector();
   _up = oTrav.GetCameraUpVector(); 

   _camerapos = oTrav.GetCameraPosition();
  
   _hh = tan(DEG2RAD(_fFovY)/2)*_fNear;
   _hw = _hh * _fAspect;
   _pixelsize = _hw * 2.0f / (float)_nScreenWidth;
   _center = _viewdirection*(_fNear);
   _tl = _center - _right*_hw + _up*_hh;

   CameraState oCamState;

   oCamState.fFovY            = _fFovY;
   oCamState.fAspect          = _fAspect;
   oCamState.fFar             = _fFar;
   oCamState.fNear            = _fNear;
   oCamState.hh               = _hh;
   oCamState.hw               = _hw;
   oCamState.nScreenHeight    = _nScreenHeight;
   oCamState.nScreenWidth     = _nScreenWidth;
   oCamState.pixelsize        = _pixelsize;
   oCamState.vCameraPosition  = _camerapos;
   oCamState.vCenter          = _center;
   oCamState.vRight           = _right;
   oCamState.vTopLeft         = _tl;
   oCamState.vUp              = _up;
   oCamState.vViewDirection   = _viewdirection;
   oCamState.vUpPixel         = _up * _pixelsize;
   oCamState.vRightPixel      = _right * _pixelsize;

   oTrav.OverwriteCamera(oCamState);
}

//------------------------------------------------------------------------------

void i3dNodeCamera::OnChangeState(IRenderEngine* pEngine)
{
   glMatrixMode(GL_PROJECTION);
   glLoadMatrixd(_oMatrixProjection.GetGLMatrix());
   glMatrixMode(GL_MODELVIEW);
}

//------------------------------------------------------------------------------

void i3dNodeCamera::OnSlotConnection(EventManager* pEventManager)
{
   Connect_CameraUpdate(i3dNodeCamera::OnUpdateCamera);
   Connect_Resize(i3dNodeCamera::OnResize);
}

//------------------------------------------------------------------------------

void i3dNodeCamera::OnResize(int w, int h)
{
   _nScreenWidth = w;      // warning: viewport is not used, always full window!
   _nScreenHeight = h;
   _SetupCamera(_oMatrixProjection); // initial setup...
}


//------------------------------------------------------------------------------

void i3dNodeCamera::GetCameraPos(vec3<double>& cameraPos)
{
   cameraPos.x = _camerapos.x;
   cameraPos.y = _camerapos.y;
   cameraPos.z = _camerapos.z;
}

//------------------------------------------------------------------------------

void i3dNodeCamera::OnUpdateCamera(SCameraUpdate camupdate)
{
   _fNear = camupdate.fNear;
   _fFar = camupdate.fFar;

   _pixelsizeX = _fNear * camupdate.pixelsizex / camupdate.flen;
   _pixelsizeY = _fNear * camupdate.pixelsizey / camupdate.flen;

}

//------------------------------------------------------------------------------