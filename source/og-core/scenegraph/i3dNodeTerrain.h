/*******************************************************************************
Project       : i3D Studio  
Purpose       : Begin Render Node
Creation Date : 17.3.2011
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _I3D_NODETERRAIN_H 
#define _I3D_NODETERRAIN_H

#include "og.h"
#include "scenegraph/Node.h"
#include "math/vec3.h"
#include "geo/VirtualGlobeRenderer.h"

//! \class i3dNodeCamera
//! \brief Camera (setting projection matrix)
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup node
class OPENGLOBE_API i3dNodeTerrain : public Node
{
public:
   i3dNodeTerrain();
   virtual ~i3dNodeTerrain();

   virtual void OnInit();
   virtual void OnExit();

   virtual bool IsGroup(){return false;}
   virtual void OnRender(IRenderEngine* pEngine);

   virtual void OnTraverse(TraversalState& oTrav);
   virtual void OnChangeState(IRenderEngine* pEngine);

   // Events (required for scripting)
   virtual void OnSlotConnection(EventManager* pEventManager);

   void OnMouseClick(int button, int x, int y);
   void OnMouseRelease(int button, int x, int y);
   void OnResize(int w, int h);

   void SetTerrainRenderer(VirtualGlobeRenderer* pRenderer) {_pRenderer = pRenderer;}

protected:
   int _nWidth, _nHeight;
   mat4<double> _view;                  // current view matrix
   mat4<double> _proj;                  // current projection matrix
   mat4<double> _model;                 // current model matrix
   mat4<double> _modelview;             // current concatenated model + view matrix
   VirtualGlobeRenderOptions  _options;
   VirtualGlobeRenderer* _pRenderer;    // world render engine



};


#endif
