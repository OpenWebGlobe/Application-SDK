/*******************************************************************************
Project       : i3D Studio  
Purpose       : Event Manager
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2006 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _I3D_EVENTMANAGER_H 
#define _I3D_EVENTMANAGER_H

#include "og.h"

class OPENGLOBE_API CollisionResult;
class OPENGLOBE_API NodePickResult;
class OPENGLOBE_API TerrainPickResult;
class OPENGLOBE_API ObjectPickResult;

#include <string>
#include <list>
#include <boost/signal.hpp>
#include <boost/bind.hpp>
#include <math/vec3.h>
#include <math/vec4.h>
#include <math/sphere3.h>
#include <math/ray.h>
//#include "keycodes.h"

enum TerrainRenderFlags
{
   VG_ENABLE_CURTAIN=0,
};


//! \struct SCameraUpdate
//! \brief Parameters for Camera Update Events
//! \ingroup scenegraph event
struct OPENGLOBE_API SCameraUpdate
{
   double pixelsizex; 
   double pixelsizey; 
   int width; 
   int height; 
   double vfov; 
   double hfov; 
   double fNear; 
   double fFar;
   double flen;
};

//! \struct SHitRecord
//! \brief Parameters related to hit based events
//! \ingroup scenegraph event
struct OPENGLOBE_API SHitRecord
{
   double r,g,b,a;           //!< color
   vec3<double> vNormal;      //!< normal at hitpoint
   double t;                  //!< hitpoint
   double u,v;                //!< barycentric coordinates
   bool  bHit;
   int   nElementId;         //!< Element Id

   void Reset()
   {
      bHit = false;
      vNormal.x = 0.0;
      vNormal.y = 0.0;
      vNormal.z = 0.0;
      u = 0.0;
      v = 0.0;
      t = 0.0;
      r = 0.0;
      g = 0.0;
      b = 0.0;
      a = 0.0;
      nElementId = -1;
   }
};


struct OPENGLOBE_API SSimpleHitTest
{
   double t;                  //!< hitpoint
   std::string sObjectId;     //!< Object Id
};


//! \class EventManager
//! \brief Event Manager - contains signals.
//! i3d Library users should not call those events directly. They are called internally (automatically) when the appropriate event occurs.
//! \ingroup scenegraph event
class OPENGLOBE_API EventManager
{
public:
   EventManager(){}
   virtual ~EventManager(){}

   //! \brief Signal emitted when mouse is moved. Parameters are (x,y) where x and y are mouse coordinates.
   boost::signal<void (int, int)>       MouseMove;   // x,y
   
   //! \brief Signal emitted when mouse button is clicked. Parameters are (buttom, x,y) where button i is the mouse button and x,y are mouse coordinates.
   boost::signal<void (int, int, int)>  MouseClick; // button, x, y
   
   //! \brief Signal emitted when mouse button is released. Parameters are (buttom, x,y) where button i is the mouse button and x,y are mouse coordinates.
   boost::signal<void (int, int, int)>  MouseRelease; // button, x, y

   //! \brief Signal emitted when window is resized. Parameters (w,h) are width and height.
   boost::signal<void (int, int)>       Resize;       // width, height 

   //! \brief Signal emitted before frame is drawn. Parameter (Tick) contains time in milliseconds since last tick (frame).
   boost::signal<void (double)>         Tick;         // delta-time: time passed since last tick (milliseconds)
   
   //! \brief Signal emitted when key is pressed. Paramter (keycode) contains key code.
   boost::signal<void (int)>            KeyDown;      // keycode

   //! \brief Signal emitted when key is released. Paramter (keycode) contains key code.
   boost::signal<void (int)>            KeyUp;        // keycode

   //! \brief Signal emitted when space mouse is used. Parameters are (translation, angle, axis, key). 
   boost::signal<void (const vec3<double>&, double, const vec3<double>&, int)>            SpaceMouse;        // translation, angle, axis, key
   
   // Position Update Event (external sensor)

   //! \brief Signal emitted when position on virtual globe changes. Parameters are (lng, lat, elv) where lng, lat are in [degree] elv in [m].
   boost::signal<void (double, double, double)> PositionUpdateCartesian; // x, y, z


   //! \brief Signal emitted when position on virtual globe changes. Parameters are (lng, lat, elv) where lng, lat are in [degree] elv in [m].
   boost::signal<void (double, double, double)> PositionUpdateWGS; // lng, lat, elv [DEG]
   
   //! \brief Signal emitted when orientation on virtual globe changes. Paramters are (x, y, z, w) and represent quaternion.
   boost::signal<void (double, double, double, double)> OrientationUpdateQuaternion; // x, y, z, w
   
   //! \brief Signal emitted when orientation on virtual globe changes. Paramters are (y,p,r) yaw pich roll.
   boost::signal<void (double, double, double)> OrientationUpdateYPR;

   // Collision Tests: Currently only Sphere Test. Later there should be a test
   //                  using vertex buffer, too.

   //! \brief Signal emitted when pick test is used.
   boost::signal<void (vec3<double>&, vec3<double>&, NodePickResult&)> NodePickTest;  // pos, dir, pickresult

   //! \brief Signal emitted when terrain pick test used
   boost::signal<void (vec3<double>&, vec3<double>&, TerrainPickResult&)> TerrainPickTest;  // pos, dir, pickresult
   
   //! \brief Signal emitted when object pick test used
   boost::signal<void (vec3<double>&, vec3<double>&, ObjectPickResult&)> ObjectPickTest;  // pos, dir, pickresult
   
   boost::signal<void (std::string&, double, double, double, double)> ChangeObjectColor; // id, r,g,b,a 

   //! \brief get terrain level of detail at specified position
   boost::signal<void (double, double, int& )> TerrainGetLod;  // lng, lat, lod

   //! \brief Signal emitted when Collision test sphere is used.
   boost::signal<void (sphere3<double>&, CollisionResult&)>  CollisionTestSphere;  // sphere, collisionresult

   //! \brief Signal emitted when camera is updated.
   boost::signal<void (SCameraUpdate)> CameraUpdate; //

   //! \brief Signal emitted when ray tracing event is emitted.
   boost::signal<void (const ray<double>&, SHitRecord&)>   RayTrace; // [in] ray, [out] HitRecord

   //! \brief Signal emitted when inverse ray tracing event is emitted. (Inverse Ray Tracing is used to preprocess a color value, usually implemented by cameras and lights)
   boost::signal<void (const vec3<double>&, SHitRecord&)>   InverseRayTrace; // [in] position, [out] HitRecord

   boost::signal<void (const ray<double>&, SSimpleHitTest&)> AcceleratedHitTest;  // [in] ray, [out] HitRecord

   //! \brief Signal emitted when the Platform Configuration in the Node i3dNodePlatform changes.
   boost::signal<void (unsigned int)> PlatformChanged;

   boost::signal<void (double, double, double)> YawPitchRoll;

   boost::signal<void (std::wstring)> Play;
   
   //! \brief Signal emitted when setting Terrain Render Flags
   boost::signal<void (TerrainRenderFlags, bool)> VirtualGlobeMode;
      
};
 
//! \def Connect_MouseMove(Function)
//! \brief Connect a function to mousemove event. 
//! \ingroup event

//! \def Connect_MouseClick(Function)
//! \brief Connect a function to mouse click event. 
//! \ingroup event

//! \def Connect_MouseRelease(Function)
//! \brief Connect a function to mouse release event. 
//! \ingroup event

//! \def Connect_Resize(Function)
//! \brief Connect a function to resize event. 
//! \ingroup event

//! \def Connect_Tick(Function)
//! \brief Connect a function to tick event. 
//! \ingroup event

//! \def Connect_KeyDown(Function)
//! \brief Connect a function to key down event. 
//! \ingroup event

//! \def Connect_KeyUp(Function)
//! \brief Connect a function to key up event. 
//! \ingroup event

//! \def Connect_SpaceMouse(Function)
//! \brief Connect a function to space mouse event. 
//! \ingroup event

//! \def Connect_PositionUpdateWGS(Function)
//! \brief Connect a function to position update (WGS84) event. 
//! \ingroup event

//! \def Connect_OrientationUpdateQuaternion(Function)
//! \brief Connect a function to orientation update (quaternion based) event. 
//! \ingroup event

//! \def Connect_OrientationUpdateYPR(Function)
//! \brief Connect a function to orientation update event. 
//! \ingroup event

//! \def Connect_CameraUpdate(Function)
//! \brief Connect a function to camera update event. 
//! \ingroup event

//! \def Connect_NodePickTest(Function)
//! \brief Connect a function to pick test event. 
//! \ingroup event

//! \def Connect_CollisionTestSphere(Function)
//! \brief Connect a function to collision test sphere event. 
//! \ingroup event

//! \def Connect_RayTrace(Function)
//! \brief Connect a function to ray trace event. 
//! \ingroup event

#define Connect_MouseMove(Function)       _lstConnections.push_back(pEventManager->MouseMove.connect(boost::bind(&Function, this, _1, _2)));           
#define Connect_MouseClick(Function)      _lstConnections.push_back(pEventManager->MouseClick.connect(boost::bind(&Function, this, _1, _2, _3)));
#define Connect_MouseRelease(Function)    _lstConnections.push_back(pEventManager->MouseRelease.connect(boost::bind(&Function, this, _1, _2, _3)));
#define Connect_Resize(Function)          _lstConnections.push_back(pEventManager->Resize.connect(boost::bind(&Function, this, _1, _2)));
#define Connect_Tick(Function)            _lstConnections.push_back(pEventManager->Tick.connect(boost::bind(&Function, this, _1)));
#define Connect_KeyDown(Function)         _lstConnections.push_back(pEventManager->KeyDown.connect(boost::bind(&Function, this, _1)));
#define Connect_KeyUp(Function)           _lstConnections.push_back(pEventManager->KeyUp.connect(boost::bind(&Function, this, _1)));
#define Connect_SpaceMouse(Function)      _lstConnections.push_back(pEventManager->SpaceMouse.connect(boost::bind(&Function, this, _1, _2, _3, _4)));
#define Connect_PositionUpdateCartesian(Function)  _lstConnections.push_back(pEventManager->PositionUpdateCartesian.connect(boost::bind(&Function, this, _1, _2, _3)));
#define Connect_PositionUpdateWGS(Function)  _lstConnections.push_back(pEventManager->PositionUpdateWGS.connect(boost::bind(&Function, this, _1, _2, _3)));
#define Connect_OrientationUpdateQuaternion(Function) _lstConnections.push_back(pEventManager->OrientationUpdateQuaternion.connect(boost::bind(&Function, this, _1, _2, _3, _4)));
#define Connect_OrientationUpdateYPR(Function) _lstConnections.push_back(pEventManager->OrientationUpdateYPR.connect(boost::bind(&Function, this, _1, _2, _3)));
#define Connect_CameraUpdate(Function)            _lstConnections.push_back(pEventManager->CameraUpdate.connect(boost::bind(&Function, this, _1)));
#define Connect_NodePickTest(Function)        _lstConnections.push_back(pEventManager->NodePickTest.connect(boost::bind(&Function, this, _1, _2, _3)));
#define Connect_TerrainPickTest(Function)        _lstConnections.push_back(pEventManager->TerrainPickTest.connect(boost::bind(&Function, this, _1, _2, _3)));
#define Connect_TerrainGetLod(Function)   _lstConnections.push_back(pEventManager->TerrainGetLod.connect(boost::bind(&Function, this, _1, _2, _3)));
#define Connect_ObjectPickTest(Function)        _lstConnections.push_back(pEventManager->ObjectPickTest.connect(boost::bind(&Function, this, _1, _2, _3)));
#define Connect_ChangeObjectColor(Function)     _lstConnections.push_back(pEventManager->ChangeObjectColor.connect(boost::bind(&Function, this, _1, _2, _3, _4, _5)));
#define Connect_CollisionTestSphere(Function) _lstConnections.push_back(pEventManager->CollisionTestSphere.connect(boost::bind(&Function, this, _1, _2)));
#define Connect_RayTrace(Function)        _lstConnections.push_back(pEventManager->RayTrace.connect(boost::bind(&Function, this, _1, _2)));
#define Connect_InverseRayTrace(Function)    _lstConnections.push_back(pEventManager->InverseRayTrace.connect(boost::bind(&Function, this, _1, _2)));
#define Connect_AcceleratedHitTest(Function)    _lstConnections.push_back(pEventManager->AcceleratedHitTest.connect(boost::bind(&Function, this, _1, _2)));

#define Connect_PlatformChanged(Function)    _lstConnections.push_back(pEventManager->PlatformChanged.connect(boost::bind(&Function, this, _1)));


#define Connect_YawPitchRoll(Function)    _lstConnections.push_back(pEventManager->YawPitchRoll.connect(boost::bind(&Function, this, _1, _2, _3)));
#define Connect_Play(Function)    _lstConnections.push_back(pEventManager->Play.connect(boost::bind(&Function, this, _1)));

#define Connect_VirtualGlobeMode(Function) _lstConnections.push_back(pEventManager->VirtualGlobeMode.connect(boost::bind(&Function, this, _1, _2)));

/* 
  Example: 
  
  virtual void OnSlotConnection(EventManager* pEventManager)
  {
      Connect_MouseMoveEvent(myNode::OnMouseMove);
      Connect_Tick(myNode::OnTick)
  }
*/

//! \class EventReceiver
//! \brief Event Receiver class. Nodes must inherit this class if events must be processed.
//! Macros like Connect_MouseMove must be used to define the function that is called when the event is recieved.
//!
/*! \code
// Example: Connecting events

// Lets assume there is a class "myClass" which inherits EventReceiver
class myClass : public EventReceiver
{
public:
   virtual void OnSlotConnection(EventManager* pEventManager);

   void OnMouseClick(int button, int x, int y){...}
   void OnResize(int w, int h){...}
   void OnKeyDown(int key){...}
}

// the virtual method OnSlotConnection must be implemented this way:
void myClass::OnSlotConnection(EventManager* pEventManager)
{
   // Connect the events you want to use:
   Connect_Resize(myClass::OnResize);
   Connect_MouseClick(myClass::OnMouseClick);
   Connect_KeyDown(myClass::OnKeyDown);
}
\endcode
*/
//! \ingroup scenegraph event
class OPENGLOBE_API EventReceiver
{
public:
   //! \brief Constructor.
   EventReceiver();

   //! \brief Destructor.
   virtual ~EventReceiver();

   //! \brief Slot connection, must be implemented if you want to support events
   //! \param pEventmanager Pointer to Event Manager. To use the connect macros you should use the parameter name "pEventManager", otherwise you can't use the macros.
   virtual void OnSlotConnection(EventManager* pEventManager){}

protected:
   //! \brief list of all connections.
   std::list<boost::signals::connection> _lstConnections;
};




#endif