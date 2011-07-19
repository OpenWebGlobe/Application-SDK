/*******************************************************************************
Project       : i3D Studio  
Purpose       : Begin Render Node
Creation Date : 17.3.2011
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _I3D_NODEENDRENDER_H 
#define _I3D_NODEENDRENDER_H

#include "og.h"
#include "scenegraph/Node.h"
#include "math/vec3.h"

//! \class i3dNodeCamera
//! \brief Camera (setting projection matrix)
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup node
class OPENGLOBE_API i3dNodeEndRender : public Node
{
public:
   i3dNodeEndRender();
   virtual ~i3dNodeEndRender();

   virtual void OnInit();
   virtual void OnExit();

   virtual bool IsGroup(){return false;}
   virtual void OnRender(IRenderEngine* pEngine);

   virtual void OnTraverse(TraversalState& oTrav);
   virtual void OnChangeState(IRenderEngine* pEngine);

protected:


};


#endif
