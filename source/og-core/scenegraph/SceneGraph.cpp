/*******************************************************************************
Project       : i3D Studio  
Purpose       : Scene Graph definition
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#include "scenegraph/SceneGraph.h"
#include "scenegraph/CollisionResult.h"
#include "scenegraph/Node.h"
#include "scenegraph/i3dNodeRoot.h"
#include "scenegraph/i3dNodeNavigation.h"
#include "scenegraph/i3dNodeCamera.h"
#include "scenegraph/i3dNodeBeginRender.h"
#include "scenegraph/i3dNodeEndRender.h"
#include "scenegraph/i3dNodeRender.h"
#include "scenegraph/i3dNodeArtwork.h"
#include "scenegraph/i3dNodeTerrain.h"

#include "io/FileSystem.h"
#include "io/CommonPath.h"
#include "string/StringUtils.h"
#include "string/FilenameUtils.h"
#include <utility>
#include <cassert>

//------------------------------------------------------------------------------

SceneGraph::SceneGraph(IRenderEngine* pRenderEngine)
   : _navigation(0), _camera(0), _begin(0), _end(0), _scene(0), _art(0), _terrain(0), _pRenderEngine(pRenderEngine)
{
   assert(pRenderEngine);
}

//------------------------------------------------------------------------------

SceneGraph::~SceneGraph()
{
	OnExit();
}

//------------------------------------------------------------------------------

void SceneGraph::Setup()
{
   // Setup Scenegraph
   boost::shared_ptr<BaseNode> qRoot =  boost::shared_ptr<BaseNode>(new i3dNodeRoot());
   SetRootNode(qRoot);

   // Insert Navigation Node:
   boost::shared_ptr<BaseNode> node = boost::shared_ptr<BaseNode>(new i3dNodeNavigation());
   qRoot->InsertNode(node);
   _navigation = (i3dNodeNavigation*)node.get();

   // Insert Camera Node:
   node = boost::shared_ptr<BaseNode>(new i3dNodeCamera());
   qRoot->InsertNode(node);
   _camera = (i3dNodeCamera*)node.get();

   // Insert Begin Render Node
   node = boost::shared_ptr<BaseNode>(new i3dNodeBeginRender());
   qRoot->InsertNode(node);
   _begin = (i3dNodeBeginRender*)node.get();

   // insert Terrain Node
   node = boost::shared_ptr<BaseNode>(new i3dNodeTerrain());
   qRoot->InsertNode(node);
   _terrain = (i3dNodeTerrain*)node.get();

   // Insert Render Node
   node = boost::shared_ptr<BaseNode>(new i3dNodeRender());
   qRoot->InsertNode(node);
   _render = (i3dNodeRender*)node.get();

   // Insert End Render Node
   node = boost::shared_ptr<BaseNode>(new i3dNodeEndRender());
   qRoot->InsertNode(node);
   _end = (i3dNodeEndRender*)node.get();

   // Insert Artwork Node
    node = boost::shared_ptr<BaseNode>(new i3dNodeArtwork());
   qRoot->InsertNode(node);
   _art = (i3dNodeArtwork*)node.get();
   
}

//------------------------------------------------------------------------------

void SceneGraph::OnInit()
{
}

//------------------------------------------------------------------------------

void SceneGraph::OnExit()
{
   int onexit = 0;
}

//------------------------------------------------------------------------------

void SceneGraph::Traverse()
{
	if (!_qRootNode)  // no root ? -> do nothing!
		return;

	_oTraversalState.PushViewport(_oBaseViewport);

	Node* pNode = 0;

	// Make sure root is a Node, otherwise it isn't a valid scene graph!
	pNode = dynamic_cast<Node*>(_qRootNode.get());

	if (!pNode)
	{
		// no root set!!
		return;
	}


	pNode->OnRender(_pRenderEngine);   // Render Root Node

	//--------------------------------------------
	int nMaxPass = 1;
	for (int iPass=0;iPass<nMaxPass;iPass++)
	{
		mat4<double> ViewMatrix;
		mat4<double> ModelMatrix;
		mat4<double> ProjectionMatrix;
		MaterialState MatState;
		CameraState   CamState;
		TextureState TexState[8];

		ViewMatrix.SetIdentity();
		ModelMatrix.SetIdentity();
		ProjectionMatrix.SetIdentity();

		_oTraversalState.PushView(ViewMatrix);
		_oTraversalState.PushModel(ModelMatrix);
		_oTraversalState.PushProjection(ProjectionMatrix);
		for (int i=0;i<8;i++)
		{
			_oTraversalState.PushTextureState(i, TexState[i]);
		}
		_oTraversalState.PushMaterial(MatState);
		_oTraversalState.PushCamera(CamState);

		//assert(_oTraversalState.GetSize() == 1);   

		if (pNode)
		{
			std::list<boost::shared_ptr<BaseNode> >::iterator i;
			
			// Render PASS 0: Regular Geometry
			i = pNode->GetNodeList().begin();
			while(i!=pNode->GetNodeList().end())
			{
				Node* pCurrentNode = dynamic_cast<Node*>((*i).get());
				if (pCurrentNode)
				{
					//pCurrentNode->SetPass(iPass);
					pCurrentNode->OnTraverse(_oTraversalState);
					pCurrentNode->OnChangeState(_pRenderEngine);
					pCurrentNode->OnRender(_pRenderEngine);
				}
				++i;
			}
		}
		_oTraversalState.PopView();
		_oTraversalState.PopModel();
		_oTraversalState.PopProjection();
		for (int i=0;i<8;i++)
			_oTraversalState.PopTextureState(i);

		_oTraversalState.PopMaterial();
		_oTraversalState.PopCamera();
	}

	_oTraversalState.PopViewport();
	
}

//------------------------------------------------------------------------------

i3dNodeRoot* SceneGraph::GetSceneRoot()
{
	return dynamic_cast<i3dNodeRoot*>(_qRootNode.get());
}

//------------------------------------------------------------------------------

boost::shared_ptr<BaseNode> SceneGraph::GetSharedSceneRoot()
{
	return _qRootNode;
}

//------------------------------------------------------------------------------

void SceneGraph::SetViewport(int x, int y, unsigned int w, unsigned int h)
{
	_oBaseViewport.Set(x,y,w,h);
}

//------------------------------------------------------------------------------

Viewport SceneGraph::GetViewport()
{
	return _oBaseViewport;
}

//------------------------------------------------------------------------------

void SceneGraph::SetCallbackRender(CallBack_Render cbRender)
{
   if (_render)
   {
      _render->SetCallbackRender(cbRender);
   }
}

//------------------------------------------------------------------------------

void SceneGraph::SetSceneId(int sceneid)
{
   if (_render)
   {
      _render->SetSceneId(sceneid);
   }
}

//------------------------------------------------------------------------------