/*******************************************************************************
Project       : i3D Studio  
Purpose       : Simple Camera Node
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/


#ifndef _I3D_NODECAMERA_H 
#define _I3D_NODECAMERA_H

#include "og.h"
#include "scenegraph/Node.h"
#include "math/vec3.h"

//------------------------------------------------------------------------------

enum ECameraType
{
   CT_CUSTOM_MATRIX,             // custom projection matrix (M)
   CT_PERSPECTIVE_PROJECTION,    // perspective projection (fovy, aspect, zNear, zFar)  DEFAULT
   CT_ORTHO_2D,                  // ortho 2D (left, right, bottom, top)
   CT_PIXELSIZE,                 // projection matrix with pixel size (pixelsizex, pixelsizey, far, near)
};

//------------------------------------------------------------------------------

//! \class i3dNodeCamera
//! \brief Camera (setting projection matrix)
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup node
class OPENGLOBE_API i3dNodeCamera : public Node
{
public:
   i3dNodeCamera();
   virtual ~i3dNodeCamera();

   virtual void OnInit();
   virtual void OnExit();

   virtual bool IsGroup(){return false;}
   virtual void OnRender(IRenderEngine* pEngine);

   virtual void OnTraverse(TraversalState& oTrav);
   virtual void OnChangeState(IRenderEngine* pEngine);

   void CreatePerspectiveProjection(double fovY, double fNear, double fFar);
   void CreateCustomProjection(const mat4<double>& P);
   void CreateOrtho2d(double left, double right, double bottom, double top);
   void CreateProjectionPixelsize(double pixelsizex, double pixelsizey, double fFar, double fNear);

   // Events:
   virtual void OnSlotConnection(EventManager* pEventManager);
   void OnResize(int w, int h);
   void OnUpdateCamera(SCameraUpdate camupdate);

   // Retrive current position
   void GetCameraPos(vec3<double>& cameraPos);

   // calculate camera pick direction direction. (Note: Start point is always _cameraposition)
   inline void  CameraPickDirection(int x, int y, vec3<double>& dir)
   {
      dir = _tl + _right*(_pixelsize/2) - _up*(_pixelsize/2);
      dir = dir + _right*((double)x*_pixelsize) - _up*((double)y*_pixelsize);
   }
   
protected:
   void           _SetupCamera(mat4<double>& M);
   ECameraType    _camtype;

   Viewport       _oViewport;
   mat4<double>   _oMatrix;
   double         _pixelsizeY, _pixelsizeX;
   
   double         _fFovY;
   double         _fFovX;
   double         _fAspect;
   double         _fNear;
   double         _fFar;
   double         _oleft, _oright, _obottom, _otop;
   mat4<double>   _oMatrixProjection;
   int            _nScreenWidth, _nScreenHeight;

   mat4<double>   _oMatrixModelView;
   vec3<double>   _viewdirection;
   vec3<double>   _right;
   vec3<double>   _up;
   vec3<double>   _camerapos;
   vec3<double>   _center; // screen center
   vec3<double>   _tl;
   double         _hh, _hw, _pixelsize;
};


#endif
