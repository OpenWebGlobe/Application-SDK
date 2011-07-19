/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _IRENDERENGINE_H
#define _IRENDERENGINE_H

#include "og.h"
#include "ObjectFactory.h"
#include "renderer/ITexture.h"
#include "math/vec4.h"
#include "math/mat4.h"


   class OPENGLOBE_API IRenderEngine
   {
   public:
      IRenderEngine(){}
      virtual ~IRenderEngine(){}

      // Setup/Initialize Engine
      virtual void InitEngine(const OpenGlobe::OptionVector& settings) = 0;
      
      // Set Clear Color
      // color values are normalized [0,1]
      virtual void SetClearColor(const vec4<double>& color) = 0;

      // Retrieve current clear color
      virtual void GetClearColor(vec4<double>& color) = 0;

      // Set Draw Color
      // color values are normalized [0,1]
      virtual void SetDrawColor(const vec4<double>& color) = 0;

      // Clear specified buffers
      virtual void Clear(bool colorbuffer, bool depthbuffer) = 0; 

      // Set Viewport Dimension
      virtual void SetViewport(int x, int y, int width, int height) = 0;

      // Set Current Projection Matrix (double precision)
      virtual void SetProjectionMatrixd(const mat4<double>& proj) = 0;

      // Set Current View Matrix (double precision)
      virtual void SetViewMatrixd(const mat4<double>& view) = 0;

      // Set Current Model Matrix (double precision)
      virtual void SetModelMatrixd(const mat4<double>& model) = 0;

      // Blit a texture to screen coordinates [pixel], (0,0) is top left.
      // texture : the texture to be blitted
      // x,y : Screen position in pixel
      // z: z value in range -1,1 ("multilayer") default is 0.0.
      // angle : optional rotation (clockwise) in degree, default is 0.0.
      // scalex : scale factor in x direction, default is 1.0
      // scaley : scale factory in y direction, deftault is 1.0
      // blend: true if alpha channel should be respected
      virtual void BlitTexture(ITexture* pTexture, int x, int y, double z=0, double angle=0, double scalex=1.0, double scaley=1.0, bool blend=false) = 0;

      //----------------
      // OBJECT FACTORY:
      //----------------

      // Generate (Engine specific) Texture
      virtual ITexture* CreateTexture() = 0;

      // Destroy (free memory) of a previously created texture
      virtual void Destroy(ITexture* pTexture) = 0;


   };



#endif
