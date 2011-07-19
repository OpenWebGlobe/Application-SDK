/*******************************************************************************
Project       : i3D Studio  
Purpose       : Begin Render Node
Creation Date : 17.3.2011
Author        : Martin Christen
Copyright     : Copyright (c) 2011 by FHNW. All rights reserved.
*******************************************************************************/

#include "i3dNodeTerrain.h"
#include "scenegraph/PickResult.h"
#include <cassert>
#include "scenegraph/art/art.h"
#include "math/vec2.h"

//------------------------------------------------------------------------------

i3dNodeTerrain::i3dNodeTerrain()
   : _pRenderer(0)
{
   _sNodeName = "Terrain";
   _sNodeClassName = "i3dNodeTerrain";
   _nWidth = _nHeight = 0;

   _options.bWireframe = false;
   _options.bLinearTextureFilter = false;
   _options.ShowBB = false;
   _options.ShowCurtain = true;
   _options.bWireframe = false;
}

//------------------------------------------------------------------------------

i3dNodeTerrain::~i3dNodeTerrain()
{

}

//------------------------------------------------------------------------------

void i3dNodeTerrain::OnInit()
{
   

}

//------------------------------------------------------------------------------

void i3dNodeTerrain::OnExit()
{  
   
}

//------------------------------------------------------------------------------

void i3dNodeTerrain::OnRender(IRenderEngine* pEngine)
{
   if (_pRenderer)
   {
      _pRenderer->Render(pEngine, _view, _model, _modelview, _proj, _nWidth, _nHeight, _options);
   }
}

//------------------------------------------------------------------------------

void i3dNodeTerrain::OnTraverse(TraversalState& oTrav)
{
   oTrav.GetViewMatrix(_view);
   oTrav.GetProjectionMatrix(_proj);
   oTrav.GetModelViewMatrix(_modelview);
   oTrav.GetModelMatrix(_model);
}

//------------------------------------------------------------------------------

void i3dNodeTerrain::OnChangeState(IRenderEngine* pEngine)
{
   // no state change!
}

//------------------------------------------------------------------------------


void i3dNodeTerrain::OnSlotConnection(EventManager* pEventManager)
{
   Connect_Resize(i3dNodeTerrain::OnResize);
   Connect_MouseClick(i3dNodeTerrain::OnMouseClick);
   Connect_MouseRelease(i3dNodeTerrain::OnMouseRelease);
}

//------------------------------------------------------------------------------

void i3dNodeTerrain::OnMouseClick(int button, int x, int y)
{
   
}

//------------------------------------------------------------------------------

void i3dNodeTerrain::OnMouseRelease(int button, int x, int y)
{

}

//------------------------------------------------------------------------------

void i3dNodeTerrain::OnResize(int w, int h)
{
   _nWidth = w;
   _nHeight = h;
}
