/*******************************************************************************
Project       : i3D Studio  
Purpose       : Begin Render Node
Creation Date : 17.3.2011
Author        : Martin Christen
Copyright     : Copyright (c) 2011 by FHNW. All rights reserved.
*******************************************************************************/

#include "i3dNodeArtwork.h"
#include "scenegraph/PickResult.h"
#include <cassert>
#include "scenegraph/art/art.h"
#include "math/vec2.h"

//------------------------------------------------------------------------------

i3dNodeArtwork::i3dNodeArtwork()
{
   _sNodeName = "BeginRender";
   _sNodeClassName = "i3dNodeArtwork";
   _pi3dArtwork = 0;
   _pCompassRose = 0;
   _pCompassBackground = 0;
   _yaw = 0;
   _nWidth = _nHeight = 0;
   _mousebtn = false;
   _mx = _my = 0;
   _bShowLogo = true;
   _bShowCompass = true;
}

//------------------------------------------------------------------------------

i3dNodeArtwork::~i3dNodeArtwork()
{

}

//------------------------------------------------------------------------------

void i3dNodeArtwork::OnInit()
{
   IRenderEngine* pEngine = GetRenderEngine();
   if (pEngine)
   {
      _pi3dArtwork = pEngine->CreateTexture();
      _pCompassRose = pEngine->CreateTexture();
      _pCompassBackground = pEngine->CreateTexture();

      _pi3dArtwork->CreateTexture(
         Texture::GFX_TEXTURE_2D, 
         getWidth_i3d(), 
         getHeight_i3d(),
         Texture::GFX_RGBA,
         Texture::GFX_UNSIGNED_BYTE,
         getImage_i3d());

      _pCompassRose->CreateTexture(
         Texture::GFX_TEXTURE_2D, 
         getWidth_cmp128r(), 
         getHeight_cmp128r(),
         Texture::GFX_RGBA,
         Texture::GFX_UNSIGNED_BYTE,
         getImage_cmp128r());

      _pCompassBackground->CreateTexture(
         Texture::GFX_TEXTURE_2D, 
         getWidth_cmp128bg(), 
         getHeight_cmp128bg(),
         Texture::GFX_RGBA,
         Texture::GFX_UNSIGNED_BYTE,
         getImage_cmp128bg());
   }

}

//------------------------------------------------------------------------------

void i3dNodeArtwork::OnExit()
{  
   IRenderEngine* pEngine = GetRenderEngine();
   if (pEngine)
   {
      if (_pi3dArtwork) pEngine->Destroy(_pi3dArtwork);
      if (_pCompassRose) pEngine->Destroy(_pCompassRose);
      if (_pCompassBackground) pEngine->Destroy(_pCompassBackground);
   }
}

//------------------------------------------------------------------------------

void i3dNodeArtwork::OnRender(IRenderEngine* pEngine)
{

   if ( _mousebtn && _bShowCompass)
   {
      pEngine->BlitTexture(_pCompassRose, _mx-64, _my-64, 0, rad2deg(_yaw), 1,1, true);
      pEngine->BlitTexture(_pCompassBackground, _mx-64, _my-64,0, 0, 1, 1,true);
      
   }
}

//------------------------------------------------------------------------------

void i3dNodeArtwork::OnTraverse(TraversalState& oTrav)
{
   _yaw = oTrav.GetCompassDirection();
   oTrav.GetViewport(_oViewport);
}

//------------------------------------------------------------------------------

void i3dNodeArtwork::OnChangeState(IRenderEngine* pEngine)
{
   // no state change!
}

//------------------------------------------------------------------------------


void i3dNodeArtwork::OnSlotConnection(EventManager* pEventManager)
{
   Connect_Resize(i3dNodeArtwork::OnResize);
   Connect_MouseClick(i3dNodeArtwork::OnMouseClick);
   Connect_MouseRelease(i3dNodeArtwork::OnMouseRelease);
}

//------------------------------------------------------------------------------

void i3dNodeArtwork::OnMouseClick(int button, int x, int y)
{
   if (button == 1)
   {
      _mousebtn = true;
      _mx = x;
      _my = y;
   }
}

//------------------------------------------------------------------------------

void i3dNodeArtwork::OnMouseRelease(int button, int x, int y)
{
   if (button == 1)
   {
      _mousebtn = false;
   }
}

//------------------------------------------------------------------------------

void i3dNodeArtwork::OnResize(int w, int h)
{
   _nWidth = w;
   _nHeight = h;
}
