/*******************************************************************************
Project       : i3D Studio  
Purpose       : Traversal States
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#include "TraversalStates.h"
#include <GL/glew.h>
#include <limits>

//-----------------------------------------------------------------------------

inline bool IsEqual(double x, double y)
{
   const double eps = std::numeric_limits<double>::epsilon();
   if (std::fabs(x) < eps && std::fabs(y) < eps)
      return true;
   else
      return (std::fabs(x-y) < eps*(std::fabs(x) + std::fabs(y)));
}

//-----------------------------------------------------------------------------

TraversalState::TraversalState()
{
   _compassYaw = 0;
}

TraversalState::~TraversalState()
{

}

int   TraversalState::GetMaxSupportedTextureUnits() 
{
   return 8;
}

void  TraversalState::PushView(mat4<double>& M)
{
   MatrixStackView.push(M);
}

void  TraversalState::PopView()
{
   MatrixStackView.pop();
}

void  TraversalState::PushModel(mat4<double>& M)
{
   MatrixStackModel.push(M);
}

void  TraversalState::PushTextureState(int TextureUnit, TextureState& oTextureState)
{
   StackTextureState[TextureUnit].push(oTextureState);
}

void  TraversalState::PopModel()
{
   MatrixStackModel.pop();
}

void  TraversalState::PushProjection(mat4<double>& M)
{
   MatrixStackProjection.push(M);
}

void  TraversalState::PopProjection()
{
   MatrixStackProjection.pop();
}

void  TraversalState::PopTextureState(int TextureUnit)
{
   StackTextureState[TextureUnit].pop();
}

void  TraversalState::PushMaterial(const MaterialState& oMaterialState)
{
   StackMaterial.push(oMaterialState);
}

void  TraversalState::PopMaterial()
{
   StackMaterial.pop();
}

void  TraversalState::PushCamera(const CameraState& oCameraState)
{
   StackCamera.push(oCameraState);
}

void  TraversalState::PopCamera()
{
   StackCamera.pop();
}

void  TraversalState::PushViewport(const Viewport& oViewportState)
{
   StackViewport.push(oViewportState);
}

void  TraversalState::PopViewport()
{
   StackViewport.pop();
}

void TraversalState::GetCamera(CameraState& oCameraState)
{
   oCameraState = StackCamera.top();
}

void TraversalState::GetViewport(Viewport& oViewport)
{
   oViewport = StackViewport.top();
}

void TraversalState::GetMaterial(MaterialState& oMaterialState)
{
   oMaterialState = StackMaterial.top();
}

void  TraversalState::GetViewMatrix(mat4<double>& M)
{
   M = MatrixStackView.top();
}

void  TraversalState::GetModelMatrix(mat4<double>& M)
{
   M = MatrixStackModel.top();
}

void  TraversalState::GetProjectionMatrix(mat4<double>& M)
{
   M = MatrixStackProjection.top();
}

void  TraversalState::GetModelViewMatrix(mat4<double>& M)
{
   M = MatrixStackView.top() * MatrixStackModel.top();
}


void  TraversalState::GetTextureState(int TextureUnit, TextureState& oTextureState)
{
   oTextureState = StackTextureState[TextureUnit].top();
}



void  TraversalState::AccumulateView(const mat4<double>& M)
{
   MatrixStackView.top() *= M;
}

void  TraversalState::AccumulateModel(const mat4<double>& M)
{
   MatrixStackModel.top() *= M;
}

void  TraversalState::AccumulateProjection(const mat4<double>& M)
{
   MatrixStackProjection.top() *= M;
}

void TraversalState::AccumulateTexture(int TextureUnit, const TextureState& oTextureState)
{
   // #TODO: calculate Texture Matrix (which is currently not yet supported)

   StackTextureState[TextureUnit].top() = oTextureState;
}

void  TraversalState::OverwriteCamera(const CameraState& oCameraState)
{
   StackCamera.top() = oCameraState;
}

void  TraversalState::OverwriteMaterial(const MaterialState& oMaterialState)
{
   StackMaterial.top() = oMaterialState;
}

void  TraversalState::OverwriteView(const mat4<double>& M)
{
   MatrixStackView.top() = M;
}

void  TraversalState::OverwriteModel(const mat4<double>& M)
{
   MatrixStackModel.top() = M;
}

void  TraversalState::OverwriteProjection(const mat4<double>& M)
{
   MatrixStackProjection.top() = M;
}

vec3<double> TraversalState::GetCameraPosition()
{
   mat4<double> oMatrixViewInverse;
   mat4<double> oMatrixView;
   GetViewMatrix(oMatrixView);
   oMatrixViewInverse =  oMatrixView.Inverse();
   vec3<double> vNull(0,0,0);
   return oMatrixViewInverse.vec3mul(vNull);
}

/*vec3<double> TraversalState::GetCameraPosition()
{
   mat4<double> oMatrixView;
   GetViewMatrix(oMatrixView);

   const double* mat = oMatrixView.GetGLMatrix();

   return vec3<double>(mat[3], mat[7], mat[11]);
}*/

vec3<double> TraversalState::GetCameraUpVector()
{
   mat4<double> oMatrixView;
   GetViewMatrix(oMatrixView);

   const double* mat = oMatrixView.GetGLMatrix();

   return vec3<double>(mat[1], mat[5], mat[9]);
}

vec3<double> TraversalState::GetCameraRightVector()
{
   mat4<double> oMatrixView;
   GetViewMatrix(oMatrixView);

   const double* mat = oMatrixView.GetGLMatrix();

   return vec3<double>(mat[0], mat[4], mat[8]);
}

vec3<double> TraversalState::GetCameraViewVector()
{
   mat4<double> oMatrixView;
   GetViewMatrix(oMatrixView);

   const double* mat = oMatrixView.GetGLMatrix();

   return vec3<double>(-mat[2], -mat[6], -mat[10]);
}


void TraversalState::PushGeo(const GeoState& oGeoState)
{
   StackGeo.push(oGeoState);
}

void TraversalState::PopGeo()
{
   StackGeo.pop();
}

void TraversalState::OverwriteGeo(const GeoState& oGeoState)
{
   StackGeo.top() = oGeoState;
}

void TraversalState::GetGeo(GeoState& oGeoState)
{
   oGeoState = StackGeo.top();
}

//-----------------------------------------------------------------------------

TextureState::TextureState()
{
   Type = GL_TEXTURE_2D;      // 0X0DE1
   Width = 0;                 // texture width in pixels
   Height = 0;                // texture height in pixels
   bNormalizedCoordinates = true;
   MagFilter = GL_NEAREST;    // 0x2600
   MinFilter = GL_NEAREST;    // 0x2600
   TextureName = 0;
}

TextureState::~TextureState()
{

}

//-----------------------------------------------------------------------------


MaterialState::MaterialState()
{
   DiffuseColor      = vec4<double>(1,1,1,1);    
   AmbientColor      = vec4<double>(0,0,0,1);    
   EmissiveColor     = vec4<double>(0,0,0,1);   
   TransparentColor  = vec4<double>(1,1,1,1);
   ReflectiveColor   = vec4<double>(0,0,0,1); 

   fShininess     = 10.0;     
   fRefraction    = 1.0; 
   fReflectivity  = 0.0;
   fTransparency  = 1.0;

}

MaterialState::~MaterialState()
{

}


bool MaterialState::operator == (const MaterialState& other) const
{
   return ( IsEqual(fShininess, other.fShininess) &&
            IsEqual(fRefraction, other.fRefraction) && 
            IsEqual(fReflectivity, other.fReflectivity) &&
            IsEqual(fTransparency, other.fTransparency) &&
            IsEqual(DiffuseColor.r, other.DiffuseColor.r) &&  
            IsEqual(AmbientColor.r, other.AmbientColor.r) &&
            IsEqual(EmissiveColor.r, other.EmissiveColor.r) &&
            IsEqual(SpecularColor.r, other.SpecularColor.r) &&
            IsEqual(TransparentColor.r, other.TransparentColor.r) &&
            IsEqual(ReflectiveColor.r, other.ReflectiveColor.r) &&
            IsEqual(DiffuseColor.g, other.DiffuseColor.g) &&  
            IsEqual(AmbientColor.g, other.AmbientColor.g) &&
            IsEqual(EmissiveColor.g, other.EmissiveColor.g) &&
            IsEqual(SpecularColor.g, other.SpecularColor.g) &&
            IsEqual(TransparentColor.g, other.TransparentColor.g) &&
            IsEqual(ReflectiveColor.g, other.ReflectiveColor.g) &&
            IsEqual(DiffuseColor.b, other.DiffuseColor.b) &&  
            IsEqual(AmbientColor.b, other.AmbientColor.b) &&
            IsEqual(EmissiveColor.b, other.EmissiveColor.b) &&
            IsEqual(SpecularColor.b, other.SpecularColor.b) &&
            IsEqual(TransparentColor.b, other.TransparentColor.b) &&
            IsEqual(ReflectiveColor.b, other.ReflectiveColor.b) &&
            IsEqual(DiffuseColor.a, other.DiffuseColor.a) &&  
            IsEqual(AmbientColor.a, other.AmbientColor.a) &&
            IsEqual(EmissiveColor.a, other.EmissiveColor.a) &&
            IsEqual(SpecularColor.a, other.SpecularColor.a) &&
            IsEqual(TransparentColor.a, other.TransparentColor.a) &&
            IsEqual(ReflectiveColor.a, other.ReflectiveColor.a));

}

bool MaterialState::operator != (const MaterialState& other) const
{
   return !( IsEqual(fShininess, other.fShininess) &&
      IsEqual(fRefraction, other.fRefraction) && 
      IsEqual(fReflectivity, other.fReflectivity) &&
      IsEqual(fTransparency, other.fTransparency) &&
      IsEqual(DiffuseColor.r, other.DiffuseColor.r) &&  
      IsEqual(AmbientColor.r, other.AmbientColor.r) &&
      IsEqual(EmissiveColor.r, other.EmissiveColor.r) &&
      IsEqual(SpecularColor.r, other.SpecularColor.r) &&
      IsEqual(TransparentColor.r, other.TransparentColor.r) &&
      IsEqual(ReflectiveColor.r, other.ReflectiveColor.r) &&
      IsEqual(DiffuseColor.g, other.DiffuseColor.g) &&  
      IsEqual(AmbientColor.g, other.AmbientColor.g) &&
      IsEqual(EmissiveColor.g, other.EmissiveColor.g) &&
      IsEqual(SpecularColor.g, other.SpecularColor.g) &&
      IsEqual(TransparentColor.g, other.TransparentColor.g) &&
      IsEqual(ReflectiveColor.g, other.ReflectiveColor.g) &&
      IsEqual(DiffuseColor.b, other.DiffuseColor.b) &&  
      IsEqual(AmbientColor.b, other.AmbientColor.b) &&
      IsEqual(EmissiveColor.b, other.EmissiveColor.b) &&
      IsEqual(SpecularColor.b, other.SpecularColor.b) &&
      IsEqual(TransparentColor.b, other.TransparentColor.b) &&
      IsEqual(ReflectiveColor.b, other.ReflectiveColor.b) &&
      IsEqual(DiffuseColor.a, other.DiffuseColor.a) &&  
      IsEqual(AmbientColor.a, other.AmbientColor.a) &&
      IsEqual(EmissiveColor.a, other.EmissiveColor.a) &&
      IsEqual(SpecularColor.a, other.SpecularColor.a) &&
      IsEqual(TransparentColor.a, other.TransparentColor.a) &&
      IsEqual(ReflectiveColor.a, other.ReflectiveColor.a));
}

//-----------------------------------------------------------------------------

CameraState::CameraState()
{
   fFovY = 0;
   fNear = 0;               
   fFar =  0;                 
   nScreenWidth = -1;     
   nScreenHeight = -1;    

   hh = 0;                
   hw = 0;                
   pixelsize = 0;         
   fAspect = 1.0;         
   //vec3<double> vCenter;            
   //vec3<double> vTopLeft;          
   //vec3<double> vCameraPosition;    
   //vec3<double> vViewDirection;     
   //vec3<double> vUp;                
   //vec3<double> vRight;             
}

CameraState::~CameraState()
{
}

//-----------------------------------------------------------------------------


Viewport::Viewport()
{
   nX = 0;
   nY = 0;
   nWidth = 0;
   nHeight = 0;
}


Viewport::~Viewport()
{

}


void Viewport::Set(int x, int y, unsigned int width, unsigned int height)
{
   nX = x;
   nY = y;
   nWidth = width;
   nHeight = height;
}