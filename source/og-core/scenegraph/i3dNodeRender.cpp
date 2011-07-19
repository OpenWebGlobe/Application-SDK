/*******************************************************************************
Project       : i3D Studio  
Purpose       : Begin Render Node
Creation Date : 17.3.2011
Author        : Martin Christen
Copyright     : Copyright (c) 2011 by FHNW. All rights reserved.
*******************************************************************************/

#include "i3dNodeRender.h"
#include "scenegraph/PickResult.h"
#include <GL/glew.h>

//-----------------------------------------------------------------------------

i3dNodeRender::i3dNodeRender()
   : _cbRender(0), _sceneid(-1)
{
   _sNodeName = "BeginRender";
   _sNodeClassName = "i3dNodeRender";
   
}

i3dNodeRender::~i3dNodeRender()
{

}

void i3dNodeRender::OnInit()
{

}

void i3dNodeRender::OnExit()
{
}

//------------------------------------------------------------------------------

void i3dNodeRender::OnRender(IRenderEngine* pEngine)
{
   // Call Used defined Render Callback...
   if (_cbRender && _sceneid != -1)
   {
      _cbRender(_sceneid);
   }
}

//------------------------------------------------------------------------------

void i3dNodeRender::OnTraverse(TraversalState& oTrav)
{
   
}

//------------------------------------------------------------------------------

void i3dNodeRender::OnChangeState(IRenderEngine* pEngine)
{
  
}

//------------------------------------------------------------------------------

