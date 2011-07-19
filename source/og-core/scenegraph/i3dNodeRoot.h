/*******************************************************************************
Project       : i3D Studio  
Purpose       : Root Node
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _I3D_ROOTNODE_H 
#define _I3D_ROOTNODE_H

#include "og.h"
#include "Node.h"


//! \class i3dNodeRoot
//! \brief Scene Root.
//!
//! The Root node is a special node, it represents the whole scene, so it could be called
//! "Scene Settings". However this node is unique! It can only be one time in the scene and it
//! must be the root.
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup node
class OPENGLOBE_API i3dNodeRoot : public Node
{
public:
   i3dNodeRoot();
   virtual ~i3dNodeRoot();

   virtual void OnInit();
   virtual void OnExit();

   virtual bool IsGroup(){return true;}

   virtual void OnTraverse(TraversalState& oTrav);

   // Events (required for scripting)
   virtual void OnSlotConnection(EventManager* pEventManager);

   void OnMouseMove(int x, int y);
   void OnMouseClick(int button, int x, int y);
   void OnMouseRelease(int button, int x, int y);
   void OnTick(double dTick);
   void OnResize(int w, int h);
   void OnKeyDown(int key);
   void OnKeyUp(int key);
   void OnSpaceMouse(const vec3<double>& vTranslation, double fAngle, const vec3<double>& vAxis, int key);
   void OnPositionUpdateWGS(double lat, double lng, double elv);
   void OnOrientationUpdateQuaternion(double qx, double qy, double qz, double qw);
   void OnOrientationUpdateYPR(double fYaw, double fPitch, double fRoll);

protected:
   void        UpdateSettings();


};



#endif