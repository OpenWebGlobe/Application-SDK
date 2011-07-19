/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _RENDERENGINE_GL
#define _RENDERENGINE_GL

#include "og.h"
#include "renderer/IRenderEngine.h"


namespace OpenGlobe
{

   class OPENGLOBE_API RenderEngineGL : public IRenderEngine
   {
   public:
      RenderEngineGL();
      virtual ~RenderEngineGL();

      // init engine
      virtual void InitEngine(const OptionVector& settings);

      // Set Clear Color
      // color values are normalized [0,1]
      virtual void SetClearColor(const vec4<double>& color);

      // retrieve current clear color
      virtual void GetClearColor(vec4<double>& color);

      // Set Draw Color
      // color values are normalized [0,1]
      virtual void SetDrawColor(const vec4<double>& color);

      // Clear specified buffers 
      virtual void Clear(bool colorbuffer, bool depthbuffer); 

      // Set Viewport Dimension
      virtual void SetViewport(int x, int y, int width, int height);

      virtual void SetProjectionMatrixd(const mat4<double>& projection);

      // Set Current View Matrix (double precision)
      virtual void SetViewMatrixd(const mat4<double>& view);

      // Set Current Model Matrix (double precision)
      virtual void SetModelMatrixd(const mat4<double>& model);

      // Blit a texture to screen coordinates [pixel], (0,0) is top left.
      // texture : the texture to be blitted
      // x,y : Screen position in pixel
      // z: z value in range -1,1 ("multilayer") default is 0.0.
      // angle : optional rotation (clockwise) in degree, default is 0.0.
      // scalex : scale factor in x direction, default is 1.0
      // scaley : scale factory in y direction, deftault is 1.0
      // blend: true if alpha channel should be respected
      virtual void BlitTexture(ITexture* pTexture, int x, int y, double z=0, double angle=0, double scalex=1.0, double scaley=1.0, bool blend=false);


      //---------
      // FACTORY
      //---------

      // Generate (Engine specific) Texture
      virtual ITexture* CreateTexture();

      // Destroy (free memory) of a previously created texture
      virtual void Destroy(ITexture* pTexture);
   
   private:
      void  _updatematrix();
      vec4<double> _bgcolor;
      mat4<double> _model;
      mat4<double> _view;
      mat4<double> _projection;
      mat4<double> _mv;
      int _vpx, _vpy, _vpw, _vph; // viewport
   };

}



#endif