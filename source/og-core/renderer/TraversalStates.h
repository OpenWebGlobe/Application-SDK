/*******************************************************************************
Project       : i3D Studio  
Purpose       : Traversal States
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _I3D_TRAVERSALSTATES_H 
#define _I3D_TRAVERSALSTATES_H

#include "og.h"
#include <math/vec3.h>
#include <math/vec4.h>
#include <math/mat4.h>
#include <stack>

//----------------------------------------------------------------------------
//! \class MaterialState
class OPENGLOBE_API MaterialState
{
public:
   MaterialState();
   virtual ~MaterialState();


   vec4<double>   DiffuseColor;    
   vec4<double>   AmbientColor;    
   vec4<double>   EmissiveColor;
   vec4<double>   SpecularColor;
   vec4<double>   TransparentColor;
   vec4<double>   ReflectiveColor; 
   
   double         fShininess;       // 10
   double         fRefraction;      // 1
   double         fReflectivity;    // 0
   double         fTransparency;    // 1

   bool operator == (const MaterialState& other) const;
   bool operator != (const MaterialState& other) const;
  
};

//-----------------------------------------------------------------------------

//! \class TextureState
class OPENGLOBE_API TextureState
{
public:
   TextureState();
   virtual ~TextureState();

   mat4<double>   MatrixTexture;    // [[*WARNING*: Texture matrix is NOT yet supported!]]
   int            Type;             //!< GL_TEXURE_2D, GL_TEXTURE_RECANGLE_ARB etc.
   unsigned int   Width;            //!< texture width in pixels
   unsigned int   Height;           //!< texture height in pixels
   bool           bNormalizedCoordinates; //!< true if coordinates are normlized (e.g. [0...1])
   int            MagFilter;        //!< e.g. GL_NEAREST or GL_LINEAR
   int            MinFilter;
   unsigned int   TextureName;      //!< OpenGL Texture Name
   std::string    sTextureFile;     //!< utf8 encoded texture filename (just filename, no path) 
};


//-----------------------------------------------------------------------------

//! \class Viewport
class OPENGLOBE_API Viewport
{
public:
   Viewport();
   virtual ~Viewport();

   void Set(int x, int y, unsigned int width, unsigned int height);

   int GetX(){return nX;}
   int GetY(){return nY;}
   unsigned int GetHeight(){return nHeight;}
   unsigned int GetWidth(){return nWidth;}

   // (nX, nY) Specify the lower left corner of	the viewport
   // rectangle, in pixels. The initial value is (0,0).
   int nX; 
   int nY;

   // (nWidth, nHeight) Specify the width and height of the viewport.
   unsigned int nWidth;
   unsigned int nHeight;
   
};

//-----------------------------------------------------------------------------
//! \struct CameraState
class OPENGLOBE_API CameraState
{
public:
   CameraState();
   virtual ~CameraState();

   double         fFovY;            //!< Vertical Field of View in [DEG]
   double         fNear;            //!< Near Plane
   double         fFar;             //!< Far Plane
   unsigned int   nScreenWidth;     //!< Width of graphics screen (or window)
   unsigned int   nScreenHeight;    //!< Height of graphics screen (or window)

   double         hh;               //!< Half height of image plane (in world coord)
   double         hw;               //!< Half width of image plane (in world coord)
   double         pixelsize;        //!< pixelsize (in world coord)
   double         fAspect;          //!<Aspect Ratio

   vec3<double>   vCenter;          //!< Center Vector
   vec3<double>   vTopLeft;         //!< Vector pointing to Top Left of image plane
   vec3<double>   vCameraPosition;  //!< Camera Position
   vec3<double>   vViewDirection;   //!< View Direction
   vec3<double>   vUp;              //!< Up-Direction
   vec3<double>   vRight;           //!< Right-Direction
   vec3<double>   vUpPixel;         //!< up * pixelsize precalc
   vec3<double>   vRightPixel;      //!< right * pixelsize precalc
};

//-----------------------------------------------------------------------------

//! \struct GeoState
struct OPENGLOBE_API GeoState
{
   GeoState()
   {
      bGeoRef = false;
      bRelativeGround = false;
      lng = lat = elv = 0.0;
      metricscale = 1.0;
      nBillboardType = 0;
   }

   bool bGeoRef; // true if Georeferencing is activated
   double lng; // longitude
   double lat; // latitude
   double elv; // elevation (ellipsoid)
   double metricscale;  // metric scale (1 for none)
   bool bRelativeGround;   // true if elevation is relative to ground
   int  nBillboardType;    // Billboard-Type (0: none, 1: screen aligned, 2: ellipsoid aligned)
};

//-----------------------------------------------------------------------------

//! \class TraversalState
class OPENGLOBE_API TraversalState
{
public:
   TraversalState();                         //!< Constructor
   virtual ~TraversalState();                //!< Destructor.

   void  PushView(mat4<double>& M);          //!< Push view matrix to "view matrix stack".
   void  PopView();                          //!< Pop view matrix from stack

   void  PushModel(mat4<double>& M);         //!< Push model matrix to "model matrix stack".
   void  PopModel();                         //!< Pop model matrix from stack        

   void  PushProjection(mat4<double>& M);    //!< Push projection matrix to "projection matrix stack".
   void  PopProjection();                    //!< Pop projection matrix from stack        

   void  PushTextureState(int TextureUnit, TextureState& oTextureState);   //!< Push texture state matrix to "texture state stack".
   void  PopTextureState(int TextureUnit);                                 //!< Pop texture state from stack        

   void  PushMaterial(const MaterialState& oMaterialState);                //!< Push material state matrix to "material state stack".
   void  PopMaterial();                                                    //!< Pop material state from stack        
                                                                           
   void  PushCamera(const CameraState& oCameraState);                      //!< Push camera state matrix to "camera state stack".
   void  PopCamera();       //!< Pop camera state from stack        

   void PushViewport(const Viewport& oViewport);
   void PopViewport();

   void PushGeo(const GeoState& oGeoState);
   void PopGeo();


   void  AccumulateView(const mat4<double>& M);          //!< Multiply current stack value with this matrix.
   void  AccumulateModel(const mat4<double>& M);         //!< Multiply current stack value with this matrix.
   void  AccumulateProjection(const mat4<double>& M);    //!< Multiply current stack value with this matrix.
   void  AccumulateTexture(int TextureUnit, const TextureState& oTextureState);  //!< Multiply current stack value of texture matrix with this matrix
   
   void  OverwriteView(const mat4<double>& M);        //!< Overwrite current view matrix          
   void  OverwriteModel(const mat4<double>& M);       //!< Overwrite current model matrix         
   void  OverwriteProjection(const mat4<double>& M);  //!< Overwrite current projection matrix
   void  OverwriteMaterial(const MaterialState& oMaterialState); //!< Overwrite current material
   void  OverwriteCamera(const CameraState& oCameraState);  //!< Overwrite current camera
   void  OverwriteGeo(const GeoState& oGeoState); //!< Overwrite current Geo State

   void  GetModelViewMatrix(mat4<double>& M);   //!< Retrieve modelview matrix of current stack
   void  GetViewMatrix(mat4<double>& M);        //!< Retrieve view matrix of current stack
   void  GetModelMatrix(mat4<double>& M);       //!< Retrieve model matrix of current stack
   void  GetProjectionMatrix(mat4<double>& M);  //!< Retrieve projection matrix of current stack
   void  GetTextureState(int TextureUnit, TextureState& oTextureState); //!< Retrieve texture state
   void  GetMaterial(MaterialState& oMaterialState); //!< Retrieve current material State.
   void  GetCamera(CameraState& oCameraState);  //!< Retrieve current camera state, this doesn't include transformations after the camera node.
   void  GetViewport(Viewport& oViewport);      //!< Retrieve current viewport state
   void  GetGeo(GeoState& oGeoState);           //!< Retrieve current geo state

   int   GetMaxSupportedTextureUnits();    //!< Returns 3d engine specific value (This is not a hardware specific value)

   vec3<double> GetCameraPosition();      //!< Retrieve Camera Position, includes all transformations after camera node!
   vec3<double> GetCameraUpVector();      //!< Retrieve Camera Up Vector, includes all transformations after camera node!
   vec3<double> GetCameraRightVector();   //!< Retrieve Camera Right Vector, includes all transformations after camera node!
   vec3<double> GetCameraViewVector();    //!< Retrieve Camera View Vector, includes all transformations after camera node!

   void SetCompassDirection(double yaw) {_compassYaw = yaw;}
   double GetCompassDirection() {return _compassYaw;}

protected:
   std::stack<mat4<double> > MatrixStackView;
   std::stack<mat4<double> > MatrixStackModel;
   std::stack<mat4<double> > MatrixStackProjection;
   std::stack<TextureState > StackTextureState[8];
   std::stack<MaterialState> StackMaterial;
   std::stack<CameraState>   StackCamera;
   std::stack<Viewport>      StackViewport;
   std::stack<GeoState>      StackGeo;
   double                    _compassYaw;
};


#endif