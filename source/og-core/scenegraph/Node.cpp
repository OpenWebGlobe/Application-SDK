/*******************************************************************************
Project       : i3D Studio  
Purpose       : Base Node
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#include "Node.h"
#include "scenegraph/i3dNodeRoot.h"


//-----------------------------------------------------------------------------

Node::Node()
{

}

//-----------------------------------------------------------------------------

Node::~Node()
{

}

//-----------------------------------------------------------------------------

SceneGraph* Node::GetSceneGraph()
{
   return dynamic_cast<SceneGraph*>(_pContainer);
}

//-----------------------------------------------------------------------------

IRenderEngine* Node::GetRenderEngine()
{
   SceneGraph* pGraph = GetSceneGraph();
   IRenderEngine* pEngine = 0;
   if (pGraph)
   {
      pEngine = pGraph->GetRenderEngine();
   }

   return pEngine;
}

//-----------------------------------------------------------------------------
 
void Node::OnCreate()
{
   if (_nId == 0xFFFFFFFF && _pContainer)
   {
      SetId(_pContainer->GetCurrentId());
      _pContainer->IncrementId();
   }

   if (!_pContainer)
   {
      if (is_nodetype<i3dNodeRoot>(this))
         SetId(0);
   }

   OnSlotConnection(GetSceneGraph());
   OnInit();
}

//-----------------------------------------------------------------------------

void Node::OnDestroy()
{
   OnExit();
}

//-----------------------------------------------------------------------------

void Node::OnInit()
{

}

//-----------------------------------------------------------------------------

void Node::OnExit()
{

}

//-----------------------------------------------------------------------------


