/*******************************************************************************
Project       : i3D Studio  
Purpose       : Navigation
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _I3D_NODENAVIGATIONSPHERICAL_H 
#define _I3D_NODENAVIGATIONSPHERICAL_H

#include "og.h"
#include "scenegraph/Node.h"
#include "math/quaternion.h"
#include "math/yawpitchroll.h"
#include "math/angleaxis.h"
//#include "i3dTexture.h"

#include <fstream>

//! \class i3dNodeNavigation
//! \brief Spherical Navigation (for the virtual globe).
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup node globerender
class OPENGLOBE_API i3dNodeNavigation : public Node
{
public:

   i3dNodeNavigation();
   virtual ~i3dNodeNavigation();

   void Reset();

   virtual bool IsGroup(){return false;}

   virtual void OnRender(IRenderEngine* pEngine);

   virtual void OnInit();
   virtual void OnExit();

   virtual void OnTraverse(TraversalState& oTrav);
   virtual void OnChangeState(IRenderEngine* pEngine);

   //! Event Handling:
   virtual void OnSlotConnection(EventManager* pEventManager);
   void OnMouseMove(int x, int y);
   void OnMouseClick(int button, int x, int y);
   void OnMouseRelease(int button, int x, int y);
   void OnTick(double dTick);
   void OnKeyDown(int key);
   void OnKeyUp(int key);
   void OnSpaceMouse(const vec3<double>& vTranslation, double fAngle, const vec3<double>& vAxis, int key);
   void OnPositionUpdateWGS(double lat, double lng, double elv);
   void OnOrientationUpdateQuaternion(double qx, double qy, double qz, double qw);
   void OnOrientationUpdateYPR(double fYaw, double fPitch, double fRoll);

   // Set Position
   void SetPosition(double lng, double lat, double elv);
   
   // Get Position
   void GetPosition(double& lng, double& lat, double& elv);
   
   // Set Orientation
   void SetOrientation(double yaw, double pitch, double roll);
   
   // Get Orientation
   void GetOrientation(double& yaw, double& pitch, double& roll);

   // Set Navigation Speed
   void SetNavigationSpeed(double fspeed){_fSpeed = fspeed;}

protected:
   bool                 _bShowCompass;
   bool                 _bShowLogo;
   Viewport             _oViewport;
   bool                 _CorrectElevation(vec3<double>& vDir, vec3<double>& vEye, bool bUnderworld=false);
   bool                 _TestElevation(vec3<double>& vDir, vec3<double>& vEye, bool bUnderworld=false);
   void                 ChangeSpeedDescription();
   double               _fSpeed;
   void                 SetCameraPosition0();
   void                 SetCameraPosition1();
   void                 SetCameraPosition2();
   void                 SetCameraPosition3();
   void                 SetCameraPosition4();
   void                 SetCameraPosition5();
   double               _angle;
   double               _dist;
   int                  _nPass;
   int                  _nWidth;
   int                  _nHeight;
   bool                 _btn[3];
   int                  _ptDragOrigin[2];
   int                  _nMouseX, _nMouseY;
   vec3<double>         _vSize;
   vec2<double>         _vR;
   double               _dSpeed;
   bool                 _keyPageUp;
   bool                 _keyPageDown;

   bool                 _bDragging;

   mat4<double>         _matOrientation;
   mat4<double>         _oMatrix;
   mat4<double>         _oMatrixInv;
   vec3<double>         _vEye;               // POSITION 

   double               _fYawSpeed;
   double               _fSurfacePitchSpeed;
   double               _fRollSpeed;
   double               _fPitchSpeed;
   double               _fVelocityY;
   bool                 _bMatRotChanged;
   double               _fSurfacePitch;

   double               _dRadius;
   double               _dHeight;

   double               _fLastRoll;

   vec3<double>         _vFlightDirection;
   vec3<double>         _vNormalDirection;

   double               _longitude, _latitude, _ellipsoidHeight;
   double               _yaw, _pitch, _roll;
   double               _pitch_increase;
   double               _pitch_decrease;
   double               _roll_increase;
   double               _roll_decrease;
   bool                 _bRollAnim;
   double               _MinElevation;
   quaternion<double>    _externalQuaternion;
   bool                 _bExternalFeed;
   bool                 _bDoCollisionCheck;
   bool                 _bGlue; // "glue" on ground

   unsigned int         viewportX, viewportY;

   double               _dFlightVelocity;
   double               _dYawVelocity;
   double               _dPitchVelocity;
   double               _dRollVelocity;
   double               _dElevationVelocity;

   vec3<double>         _vSpaceMouseTranslation;
   double              _fSpaceMouseAngle;
   vec3<double>         _vSpaceMouseAxis;
   bool                _bSpaceMouse;
   bool                _bExternalUpdates;
   
   /*boost::shared_ptr<TextureBase>   pCompassRose;
   boost::shared_ptr<TextureBase>   pCompassBackground;
   boost::shared_ptr<TextureBase>   pi3dArtwork;*/

   double _dAccumulatedTick;
   bool _bPositionChanged;

   mat4<double>  _matProjection;
   mat4<double>  _matView;
   mat4<double>  _matModel;

};


#endif