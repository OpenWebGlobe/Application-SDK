/*******************************************************************************
Project       : i3D Studio  
Purpose       : Base Node
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _I3D_NODE_H 
#define _I3D_NODE_H

#include "og.h"

#include "scenegraph/BaseNode.h"
#include "scenegraph/SceneGraph.h"
#include "renderer/TraversalStates.h"
#include "math/aabb3.h"
#include "scenegraph/EventReceiver.h"
#include "renderer/IRenderEngine.h"

//! \class Node
//! \brief Base class for all nodes.
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup node
class OPENGLOBE_API Node : public BaseNode, public EventReceiver
{
public:
   friend class SceneGraph;

   //! ctor
   Node();

   //! dtor
	virtual ~Node();

   //! This is called when the node is initialized. At this time the render context already exists!
   virtual void OnInit();

   //! This is called when the node is destroyed
   virtual void OnExit();

   //! This is called when the node is being drawn
   virtual void OnRender(IRenderEngine* pEngine){}

   //! Traversal. Updates Matrix states, Texture states etc.
   virtual void OnTraverse(TraversalState& oTrav){}

   //! Change Render State. This is done before calling OnRender. Use this method when doing GLOBAL state changes instead of putting them into "OnRender".
   virtual void OnChangeState(IRenderEngine* pEngine){}

   //! If this node requires to be drawn in alpha pass, return true. Otherwise return false.
   virtual bool AlphaPass(){return false;}

   //! If this node should do a frustum test before being drawn, it returns true.
   //! If true is returned, make sure "GetBoundingBox()" is implemented for that node.
   virtual bool FrustumTest(){return false;}

   //! Returns (untransformed) bounding Box. The method returns false if bounding box can't be calculated.
   //! If FrustumTest() returns true, this method MUST be implemented.
   virtual bool GetBoundingBox(aabb3<double>& BoundingBox){return false;}

   template<class T>
   static bool is_nodetype(Node* pNode);

   // Returns pointer to the scenegraph (container) of the current node.
   SceneGraph*   GetSceneGraph();

   // Returns Render Engine
   IRenderEngine* GetRenderEngine();
   
   // Do not overload! -> use OnInit()
   virtual void OnCreate();

   // Do not overload! -> use OnExit()
   virtual void OnDestroy();

};

//-----------------------------------------------------------------------------

template<class T>
bool Node::is_nodetype(Node* pNode)
{
   if (dynamic_cast<T*>(pNode))
      return true;
   return false;
}

//-----------------------------------------------------------------------------


#endif