/*******************************************************************************
Project       : i3D Studio  
Purpose       : Begin Render Node
Creation Date : 17.3.2011
Author        : Martin Christen
Copyright     : Copyright (c) 2011 by FHNW. All rights reserved.
*******************************************************************************/

#include "i3dNodeEndRender.h"
#include "scenegraph/PickResult.h"
#include <GL/glew.h>

//-----------------------------------------------------------------------------

i3dNodeEndRender::i3dNodeEndRender()
{
   _sNodeName = "BeginRender";
   _sNodeClassName = "i3dNodeEndRender";
}

i3dNodeEndRender::~i3dNodeEndRender()
{

}

void i3dNodeEndRender::OnInit()
{

}

void i3dNodeEndRender::OnExit()
{
}

//------------------------------------------------------------------------------

void i3dNodeEndRender::OnRender(IRenderEngine* pEngine)
{
}

//------------------------------------------------------------------------------

void i3dNodeEndRender::OnTraverse(TraversalState& oTrav)
{
   
}

//------------------------------------------------------------------------------

void i3dNodeEndRender::OnChangeState(IRenderEngine* pEngine)
{
  
}

//------------------------------------------------------------------------------

