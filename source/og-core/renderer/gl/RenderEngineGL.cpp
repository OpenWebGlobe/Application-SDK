/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "renderer/gl/RenderEngineGL.h"
#include "renderer/gl/TextureGL.h"

#include "GL/glew.h"

#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")	
#endif

namespace OpenGlobe
{
   //---------------------------------------------------------------------------

   RenderEngineGL::RenderEngineGL()
   {
      _bgcolor = vec4<double>(0,0,0,0);
      _vpx = 0; _vpy = 0; _vpw = 0; _vph = 0;
   }

   //---------------------------------------------------------------------------

   RenderEngineGL::~RenderEngineGL()
   {
   }

   //---------------------------------------------------------------------------

   void RenderEngineGL::InitEngine(const OptionVector& settings)
   {
      //----------------------------------------------------------
      // currently there are no settings available for this engine
      //----------------------------------------------------------

      std::vector<std::pair<std::string, std::string> >::const_iterator it = settings.begin();

      while (it!=settings.end())
      {
         // key: it->first
         // value: it->second  (value os optional, presence of a key may be enough)

         //if (it->first == "bla")
         //{
         //}
       
         it++;
      }

      // initialize GLEW:
      glewInit();
   }

   //---------------------------------------------------------------------------

   void RenderEngineGL::SetClearColor(const vec4<double>& color)
   {
      _bgcolor = color;
   }

   //---------------------------------------------------------------------------

   void RenderEngineGL::GetClearColor(vec4<double>& color)
   {
      color = _bgcolor;
   }

   //---------------------------------------------------------------------------

   void RenderEngineGL::Clear(bool colorbuffer, bool depthbuffer)
   {
      GLbitfield flag = 0;

      if (colorbuffer)
      {
         flag |= GL_COLOR_BUFFER_BIT;
         glClearColor((float)_bgcolor.r, (float)_bgcolor.g, (float)_bgcolor.b, (float)_bgcolor.a);
      }

      if (depthbuffer)
      {
         flag |= GL_DEPTH_BUFFER_BIT;
      }

      if (flag != 0)
      {
         glClear(flag);
      }
   }

   //---------------------------------------------------------------------------
   void RenderEngineGL::SetViewport(int x, int y, int width, int height)
   {
      _vpx = x;
      _vpy = y;
      _vpw = width;
      _vph = height;

      glViewport(x,y,width,height);
   }
   //---------------------------------------------------------------------------
   void RenderEngineGL::SetProjectionMatrixd(const mat4<double>& projection)
   {
      _projection = projection;
      glMatrixMode(GL_PROJECTION);
      glLoadMatrixd(_projection.GetGLMatrix());
      glMatrixMode(GL_MODELVIEW);
   }
   //---------------------------------------------------------------------------
   void RenderEngineGL::SetViewMatrixd(const mat4<double>& view)
   {
      _view = view;
      _updatematrix();
      glLoadMatrixd(_mv.GetGLMatrix());
   }
   //---------------------------------------------------------------------------
   void RenderEngineGL::SetModelMatrixd(const mat4<double>& model)
   {
      _model = model;
      _updatematrix();
      glLoadMatrixd(_mv.GetGLMatrix());
   }
   //---------------------------------------------------------------------------
   void RenderEngineGL::_updatematrix()
   {
      _mv = _model * _view;
   }
   //---------------------------------------------------------------------------
   void RenderEngineGL::SetDrawColor(const vec4<double>& color)
   {
      glColor4d(color.r, color.g, color.b, color.a);
   }

   //---------------------------------------------------------------------------

   ITexture* RenderEngineGL::CreateTexture()
   {
      ITexture* pNewTexture = new GL::TextureGL();
      return pNewTexture;
   }

   //---------------------------------------------------------------------------

   void RenderEngineGL::Destroy(ITexture* pTexture)
   {  
      if (pTexture)
      {
         pTexture->Destroy();
         delete pTexture;
      }
   }
 
   //---------------------------------------------------------------------------

   void RenderEngineGL::BlitTexture(ITexture* pTexture, int x, int y, double z, double angle, double scalex, double scaley, bool blend)
   {
      if (pTexture)
      {
         glPushAttrib(GL_TEXTURE_BIT | GL_CURRENT_BIT |GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);
         glPolygonMode(GL_FRONT, GL_FILL);
         glPolygonMode(GL_BACK, GL_FILL);
         glDisable(GL_CULL_FACE);
         glLineWidth(1.0);

         glMatrixMode(GL_MODELVIEW);
         glPushMatrix();
         glMatrixMode(GL_PROJECTION);
         glPushMatrix();

         glDisable(GL_TEXTURE_2D);

         if (GLEW_ARB_texture_rectangle)
            glDisable(GL_TEXTURE_RECTANGLE_ARB);

         if (pTexture->GetTextureTarget() == Texture::GFX_TEXTURE_RECTANGLE)
         {
            if (GLEW_ARB_texture_rectangle)
               glEnable(GL_TEXTURE_RECTANGLE_ARB);
         }
         else
         {
            glEnable(GL_TEXTURE_2D);
         }

         glEnable(GL_TEXTURE_2D);
         
         if (blend)
         {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_DEPTH_TEST);
         }

         glDisable(GL_LIGHTING);

         glColor4f(1.0,1.0,1.0,1.0);

         glMatrixMode(GL_PROJECTION);
         glLoadIdentity();
         glOrtho(0, _vpw, _vph, 0, -1, 1); 
 
         glMatrixMode(GL_MODELVIEW);
         glLoadIdentity();

         // Draw...
         float tx0 = 0.0;
         float tx1 = 1.0;
         float ty0 = 0.0;
         float ty1 = 1.0;
         
         if (pTexture->GetTextureTarget() == Texture::GFX_TEXTURE_RECTANGLE)
         {
            // use non normalized coordinates!
            tx0 = 0.0;  tx1 = (float)pTexture->GetWidth();
            ty0 = 0.0;  ty1 = (float)pTexture->GetHeight();
         }

         float hw = (float)scalex*(float)pTexture->GetWidth() / 2.0f;
         float hh = (float)scaley*(float)pTexture->GetHeight() / 2.0f;

         
         glTranslatef(x+hw,y+hh,0);
         glRotatef((float)angle,0,0,1);
         


         pTexture->BindTexture(0);
         glBegin(GL_QUADS);
            glTexCoord2f(tx0, ty0);
            glVertex3f((float)-hw,(float)-hh,(float)z);
            glTexCoord2f(tx1, ty0);
            glVertex3f((float)hw,(float)-hh,(float)z);
            glTexCoord2f(tx1, ty1);
            glVertex3f((float)hw,(float)hh,(float)z);
            glTexCoord2f(tx0, ty1);
            glVertex3f((float)-hw,(float)hh,(float)z);
         glEnd();
         
         glMatrixMode(GL_PROJECTION);
         glPopMatrix();
         glMatrixMode(GL_MODELVIEW);
         glPopMatrix();

         glPopAttrib();

         glMatrixMode(GL_MODELVIEW);
      }
   }
   //--------------------------------------------------------------------------
}


// Implementation of Standard Primitives
// it is basically a copy of freeglut...

extern "C" {

void DrawWireRhombicDodecahedron( void );
void DrawSolidRhombicDodecahedron(void);
void DrawSolidIcosahedron( void );
void DrawWireIcosahedron( void );
void DrawSolidTetrahedron( void );
void DrawWireTetrahedron( void );
void DrawSolidOctahedron( void );
void DrawWireOctahedron( void );
void DrawSolidDodecahedron( void );
void DrawWireDodecahedron( void );
void DrawSolidTorus( GLdouble dInnerRadius, GLdouble dOuterRadius, GLint nSides, GLint nRings );
void DrawWireTorus( GLdouble dInnerRadius, GLdouble dOuterRadius, GLint nSides, GLint nRings );
void DrawWireCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks);
void DrawSolidCylinder(GLdouble radius, GLdouble height, GLint slices, GLint stacks);
void DrawWireCone( GLdouble base, GLdouble height, GLint slices, GLint stacks);
void DrawSolidCone( GLdouble base, GLdouble height, GLint slices, GLint stacks );
void DrawWireSphere(GLdouble radius, GLint slices, GLint stacks);
void DrawSolidSphere(GLdouble radius, GLint slices, GLint stacks);
void DrawSolidCube( GLdouble dSize );
void DrawWireCube( GLdouble dSize );

};

OPENGLOBE_API void ogDrawSolidTorus(double dInnerRadius, double dOuterRadius, int nSides, int nRings)
{
   DrawSolidTorus( dInnerRadius,  dOuterRadius,  nSides,  nRings);
}
OPENGLOBE_API void ogDrawWireTorus(double dInnerRadius, double dOuterRadius, int nSides, int nRings)
{
   DrawWireTorus( dInnerRadius,  dOuterRadius,  nSides,  nRings);
}
OPENGLOBE_API void ogDrawWireCylinder(double radius, double height, int slices, int stacks)
{
   DrawWireCylinder( radius,  height,  slices,  stacks);
}
OPENGLOBE_API void ogDrawSolidCylinder(double radius, double height, int slices, int stacks)
{
   DrawSolidCylinder( radius,  height,  slices,  stacks);
}
OPENGLOBE_API void ogDrawWireCone(double base, double height, int slices, int stacks)
{
}
OPENGLOBE_API void ogDrawSolidCone(double base, double height, int slices, int stacks)
{
   DrawSolidCone( base,  height,  slices,  stacks);
}
OPENGLOBE_API void ogDrawWireSphere(double radius, int slices, int stacks)
{
   DrawWireSphere( radius,  slices,  stacks);
}
OPENGLOBE_API void ogDrawSolidSphere(double radius, int slices, int stacks)
{
   DrawSolidSphere( radius,  slices,  stacks);
}
OPENGLOBE_API void ogDrawSolidCube(double dSize)
{
   DrawSolidCube( dSize);
}
OPENGLOBE_API void ogDrawWireCube(double dSize)
{
   DrawWireCube( dSize);
}
