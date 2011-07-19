/*******************************************************************************
Project       : i3D Studio  
Purpose       : Begin Render Node
Creation Date : 17.3.2011
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _I3D_NODEARTWORK_H 
#define _I3D_NODEARTWORK_H

#include "og.h"
#include "scenegraph/Node.h"
#include "math/vec3.h"

//! \class i3dNodeCamera
//! \brief Camera (setting projection matrix)
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup node
class OPENGLOBE_API i3dNodeArtwork : public Node
{
public:
   i3dNodeArtwork();
   virtual ~i3dNodeArtwork();

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

protected:
   ITexture* _pi3dArtwork;
   ITexture* _pCompassRose;
   ITexture* _pCompassBackground;
   Viewport  _oViewport;

   int _nWidth, _nHeight;
   double    _yaw; // compass yaw
   bool     _mousebtn;
   int      _mx, _my;

   bool  _bShowLogo;
   bool  _bShowCompass;


};


#endif
