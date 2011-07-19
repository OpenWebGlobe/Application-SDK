/*******************************************************************************
Project       : i3D Studio  
Purpose       : Begin Render Node
Creation Date : 17.3.2011
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _I3D_NODERENDER_H 
#define _I3D_NODEDRENDER_H

#include "og.h"
#include "scenegraph/Node.h"
#include "math/vec3.h"

//! \class i3dNodeCamera
//! \brief Camera (setting projection matrix)
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup node
class OPENGLOBE_API i3dNodeRender : public Node
{
public:
   i3dNodeRender();
   virtual ~i3dNodeRender();

   virtual void OnInit();
   virtual void OnExit();

   virtual bool IsGroup(){return false;}
   virtual void OnRender(IRenderEngine* pEngine);

   virtual void OnTraverse(TraversalState& oTrav);
   virtual void OnChangeState(IRenderEngine* pEngine);

      // Set Render Callback
   void SetCallbackRender(CallBack_Render cbRender) {_cbRender = cbRender;};
   void SetSceneId(int sceneid) { _sceneid = sceneid;}


protected:
   CallBack_Render _cbRender;
   int               _sceneid;

};


#endif
