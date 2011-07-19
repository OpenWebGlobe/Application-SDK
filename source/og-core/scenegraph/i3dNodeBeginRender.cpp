/*******************************************************************************
Project       : i3D Studio  
Purpose       : Begin Render Node
Creation Date : 17.3.2011
Author        : Martin Christen
Copyright     : Copyright (c) 2011 by FHNW. All rights reserved.
*******************************************************************************/

#include "i3dNodeBeginRender.h"
#include "scenegraph/PickResult.h"
#include <GL/glew.h>

//-----------------------------------------------------------------------------

i3dNodeBeginRender::i3dNodeBeginRender()
{
   _sNodeName = "BeginRender";
   _sNodeClassName = "i3dNodeBeginRender";
}

i3dNodeBeginRender::~i3dNodeBeginRender()
{

}

void i3dNodeBeginRender::OnInit()
{

}

void i3dNodeBeginRender::OnExit()
{
}

//------------------------------------------------------------------------------

void i3dNodeBeginRender::OnRender(IRenderEngine* pEngine)
{
}

//------------------------------------------------------------------------------

void i3dNodeBeginRender::OnTraverse(TraversalState& oTrav)
{
   
}

//------------------------------------------------------------------------------

void i3dNodeBeginRender::OnChangeState(IRenderEngine* pEngine)
{
  
}

//------------------------------------------------------------------------------

