/*******************************************************************************
Project       : i3D Studio  
Purpose       : Root Node
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#include "scenegraph/i3dNodeRoot.h"
#include "string/StringUtils.h"

//-----------------------------------------------------------------------------

i3dNodeRoot::i3dNodeRoot()
{
   _sNodeName = "Root";
   _sNodeClassName = "i3dNodeRoot";
}

//-----------------------------------------------------------------------------

i3dNodeRoot::~i3dNodeRoot()
{

}

//-----------------------------------------------------------------------------

void i3dNodeRoot::OnTraverse(TraversalState& oTrav)
{
}

//-----------------------------------------------------------------------------

void i3dNodeRoot::OnInit()
{
   if (!GetSceneGraph())
   {
      assert(false);
      return;
   }
}

//-----------------------------------------------------------------------------

void i3dNodeRoot::OnExit()
{
}

//-----------------------------------------------------------------------------

void i3dNodeRoot::OnSlotConnection(EventManager* pEventManager)
{
   Connect_Resize(i3dNodeRoot::OnResize);
   Connect_MouseMove(i3dNodeRoot::OnMouseMove);
   Connect_MouseClick(i3dNodeRoot::OnMouseClick);
   Connect_MouseRelease(i3dNodeRoot::OnMouseRelease);
   Connect_Tick(i3dNodeRoot::OnTick);
   Connect_KeyUp(i3dNodeRoot::OnKeyUp);
   Connect_KeyDown(i3dNodeRoot::OnKeyDown);
   Connect_SpaceMouse(i3dNodeRoot::OnSpaceMouse);
   Connect_PositionUpdateWGS(i3dNodeRoot::OnPositionUpdateWGS);
   Connect_OrientationUpdateQuaternion(i3dNodeRoot::OnOrientationUpdateQuaternion);
   Connect_OrientationUpdateYPR(i3dNodeRoot::OnOrientationUpdateYPR);
}

//-----------------------------------------------------------------------------

void i3dNodeRoot::OnMouseMove(int x, int y)
{
}

//-----------------------------------------------------------------------------

void i3dNodeRoot::OnMouseClick(int button, int x, int y)
{ 

}

//-----------------------------------------------------------------------------

void i3dNodeRoot::OnMouseRelease(int button, int x, int y)
{
}

//-----------------------------------------------------------------------------

void i3dNodeRoot::OnTick(double dTick)
{
}

//-----------------------------------------------------------------------------

void i3dNodeRoot::OnResize(int w, int h)
{
}

//-----------------------------------------------------------------------------

void i3dNodeRoot::OnKeyDown(int key)
{
}

//-----------------------------------------------------------------------------

void i3dNodeRoot::OnKeyUp(int key)
{
}

//-----------------------------------------------------------------------------

void i3dNodeRoot::OnSpaceMouse(const vec3<double>& vTranslation, double fAngle, const vec3<double>& vAxis, int key)
{
}

//-----------------------------------------------------------------------------

void i3dNodeRoot::OnPositionUpdateWGS(double lat, double lng, double elv)
{
}

//-----------------------------------------------------------------------------

void i3dNodeRoot::OnOrientationUpdateQuaternion(double qx, double qy, double qz, double qw)
{
}

//-----------------------------------------------------------------------------

void i3dNodeRoot::OnOrientationUpdateYPR(double fYaw, double fPitch, double fRoll)
{
}

//-----------------------------------------------------------------------------


