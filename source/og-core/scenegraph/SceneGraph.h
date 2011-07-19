/*******************************************************************************
Project       : i3D Studio  
Purpose       : Scene Graph
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _I3D_GRAPH_H 
#define _I3D_GRAPH_H

#include "og.h"

//-----------------------------------------------------------------------------

class OPENGLOBE_API Node;
class OPENGLOBE_API FontImages;
class OPENGLOBE_API CollisionResult;

//-----------------------------------------------------------------------------

#include "scenegraph/BaseNode.h"
#include "scenegraph/Container.h"
#include "renderer/TraversalStates.h"
#include "EventReceiver.h"
#include "scenegraph/CollisionResult.h"
#include "scenegraph/PickResult.h"
#include "math/aabb3.h"
#include "renderer/IRenderEngine.h"


//#include <i3dTexture.h>

//-----------------------------------------------------------------------------

//! \struct i3dSceneFlags
//! \brief This class contains scene related properties. (currently unused)
//! \ingroup scenegraph
struct OPENGLOBE_API i3dSceneFlags
{
   bool bDrawCompass;
};

//-----------------------------------------------------------------------------

class OPENGLOBE_API i3dNodeRoot;
class OPENGLOBE_API i3dNodeNavigation;
class OPENGLOBE_API i3dNodeCamera;
class OPENGLOBE_API i3dNodeBeginRender;
class OPENGLOBE_API i3dNodeRender;
class OPENGLOBE_API i3dNodeEndRender;
class OPENGLOBE_API i3dNodeScene;
class OPENGLOBE_API i3dNodeArtwork;
class OPENGLOBE_API i3dNodeTerrain;

//! \class SceneGraph
//! \brief Class holding Scene Graph
//! \ingroup scenegraph
class OPENGLOBE_API SceneGraph : public Container, public EventManager
{
public:
   SceneGraph(IRenderEngine* pRenderEngine);             //<! Constructor
   virtual ~SceneGraph();    //<! Destructor

   void Setup();  // Setup Scenegraph
                                        
   //! Called when graph is initialized
   virtual void OnInit();  

   //! Called when graph is freed
   virtual void OnExit();  
      
   //! Traverse Graph: calc bouding boxes, matrices and render scene
   void Traverse();     

   IRenderEngine* GetRenderEngine(){return _pRenderEngine;}

   //! Returns pointer of scene root
   i3dNodeRoot* GetSceneRoot();

   //! Return Navigation Node
   i3dNodeNavigation* GetNavigationNode() {return _navigation;}

   //! Return Camera Node
   i3dNodeCamera* GetCameraNode() {return _camera;}

   //! Return Begin Render Node
   i3dNodeBeginRender* GetBeginRenderNode() {return _begin;}

   //! Return Render Node
   i3dNodeRender* GetRenderNode() {return _render;}

   //! Return End Render Node
   i3dNodeEndRender* GetEndRenderNode() {return _end;}

   //! Return Scene (Group) Node
   i3dNodeScene* GetSceneNode() { return _scene;}

   //! Return Artwork Node
   i3dNodeArtwork* GetArtworkNode() { return _art;}

   //! Return Terrain Node
   i3dNodeTerrain* GetTerrainNode() { return _terrain;}

   //! Return shared pointer of scene root.
   boost::shared_ptr<BaseNode> GetSharedSceneRoot();

   //! Set Viewport of scene
   void SetViewport(int x, int y, unsigned int w, unsigned int h);
   
   //! Get Viewport of scene
   Viewport GetViewport();

   // Set Render Callback 
   void SetCallbackRender(CallBack_Render cbRender);

   // Set Scene Id
   void SetSceneId(int sceneid);


protected:
   friend class Node;
   TraversalState          _oTraversalState;
   Viewport                _oBaseViewport;

   i3dNodeNavigation* _navigation;
   i3dNodeCamera*              _camera;
   i3dNodeBeginRender*         _begin;
   i3dNodeRender*              _render;
   i3dNodeEndRender*           _end;
   i3dNodeScene*               _scene;
   i3dNodeArtwork*             _art;
   i3dNodeTerrain*             _terrain;
   IRenderEngine*              _pRenderEngine;

private:
   SceneGraph(){}
   SceneGraph(const SceneGraph&){}

};


//-----------------------------------------------------------------------------

#endif

