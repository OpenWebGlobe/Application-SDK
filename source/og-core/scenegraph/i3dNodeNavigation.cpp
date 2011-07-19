/*******************************************************************************
Project       : i3D Studio  
Purpose       : Spherical Navigation
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#include "i3dNodeNavigation.h"
#include <GL/glew.h>
#include "math/GeoCoord.h"
#include "scenegraph/i3dNodeRoot.h"
#include "math/mathutils.h"


i3dNodeNavigation::i3dNodeNavigation()
{
   _sNodeName = "NavigationSpherical";
   _sNodeClassName = "i3dNodeNavigation";

   _btn[0] = _btn[1] = _btn[2] = false;
   _nMouseX = _nMouseY = 0;

   Reset();

   _bShowCompass = true;
   _bShowLogo = true;

   _dSpeed = 0.0;

   _keyPageUp = false;
   _keyPageDown = false;

   _ptDragOrigin[0] = 0;
   _ptDragOrigin[1] = 0;

   _vEye = vec3<double>(1,0,0);

   _longitude = 7.7744205094639103; _latitude = 47.472720418012834;
   _ellipsoidHeight = 3000000;
   _yaw = _roll = 0;
   _pitch = -AGEHALFPI;

   _fYawSpeed = 0;
   _fSurfacePitchSpeed = 0;
   _fRollSpeed = 0;
   _fPitchSpeed = 0;
   _fVelocityY = 0;
   _bMatRotChanged = false;
   _fSurfacePitch = 0;
   _fLastRoll = 0;
   _fSpeed = 1.0;
   _dFlightVelocity = 1.0;//0.4;
   _dYawVelocity = 1.0;//0.1;
   _dPitchVelocity = 1.0;//0.4;
   _dRollVelocity = 1.0;//0.4;
   _dElevationVelocity = 1.0;
   _bSpaceMouse = false;

   _pitch_increase = 0;
   _pitch_decrease = 0;
   _roll_increase = 0;
   _roll_decrease = 0;
   _bRollAnim = false;
   
   _MinElevation = 150.0;

   _angle = 0.001;
   _dist = -0.4;

   _bPositionChanged = false;
   _dAccumulatedTick = 0;

   _bExternalFeed = false;
   _bDoCollisionCheck = true;
   _bGlue = false;
   _bExternalUpdates = true;


}

//------------------------------------------------------------------------------

void i3dNodeNavigation::Reset()
{
   _bDragging = false;
}

//------------------------------------------------------------------------------

i3dNodeNavigation::~i3dNodeNavigation()
{
 
}

//------------------------------------------------------------------------------

void i3dNodeNavigation::OnRender(IRenderEngine* pEngine)
{
}

//-----------------------------------------------------------------------------

void i3dNodeNavigation::OnChangeState(IRenderEngine* pEngine)
{
   glMatrixMode(GL_MODELVIEW);
   glLoadMatrixd(_oMatrix.GetGLMatrix());
}

//-----------------------------------------------------------------------------

void i3dNodeNavigation::OnTraverse(TraversalState& oTrav)
{
   mat4<double> matResult;
   mat4<double> matBody;
   mat4<double> matTrans;
   mat4<double> matNavigation;
   mat4<double> matCami3d;
   mat4<double> matView;

   GeoCoord pos(_longitude, _latitude, _ellipsoidHeight);
   pos.GetCartesian(_vEye);

   matTrans.SetTranslation(_vEye);

   if (!_bExternalFeed)
   {
      matNavigation.CalcNavigationFrame(DEG2RAD(_longitude), DEG2RAD(_latitude));
      matBody.CalcBodyFrame(_yaw, _pitch, _roll);
      matCami3d.Set(0,0,-1,0,
                 1,0,0,0,
                 0,-1,0,0,
                 0,0,0,1);

      matView = matTrans;
      matView *= matNavigation;
      matView *= matBody;
      matView *= matCami3d;

      mat4<double> matInv = matView.Inverse();
      matResult = matInv;
   }
   else
   {
      _externalQuaternion.ToRotationMatrix(matBody);

      matBody = matBody.Inverse();
      matView = matTrans;
      matView *= matBody;

      mat4<double> matInv = matView.Inverse();
      matResult = matInv;
   }

   oTrav.AccumulateView(matResult);
   oTrav.GetModelViewMatrix(_oMatrix);

   _oMatrixInv = _oMatrix.Inverse();

   _vFlightDirection = _oMatrixInv * vec3<double>(0,1,0);
   _vFlightDirection.Normalize();

   oTrav.GetProjectionMatrix(_matProjection);
   oTrav.GetViewMatrix(_matView);
   oTrav.GetModelMatrix(_matModel);

   oTrav.GetViewport(_oViewport);


   oTrav.SetCompassDirection(_yaw);

   _nWidth = _oViewport.GetWidth() - _oViewport.nX;
   _nHeight = _oViewport.GetHeight() - _oViewport.nY;
  
}

//-----------------------------------------------------------------------------

void i3dNodeNavigation::OnSlotConnection(EventManager* pEventManager)
{
   Connect_MouseMove(i3dNodeNavigation::OnMouseMove);
   Connect_MouseClick(i3dNodeNavigation::OnMouseClick);
   Connect_MouseRelease(i3dNodeNavigation::OnMouseRelease);
   Connect_Tick(i3dNodeNavigation::OnTick);
   Connect_KeyUp(i3dNodeNavigation::OnKeyUp);
   Connect_KeyDown(i3dNodeNavigation::OnKeyDown);
   Connect_SpaceMouse(i3dNodeNavigation::OnSpaceMouse);
   Connect_PositionUpdateWGS(i3dNodeNavigation::OnPositionUpdateWGS);
   Connect_OrientationUpdateQuaternion(i3dNodeNavigation::OnOrientationUpdateQuaternion);
   Connect_OrientationUpdateYPR(i3dNodeNavigation::OnOrientationUpdateYPR);
}

//------------------------------------------------------------------------------

void i3dNodeNavigation::OnSpaceMouse(const vec3<double>& vTranslation, double fAngle, const vec3<double>& vAxis, int key)
{
   _vSpaceMouseTranslation = vTranslation;
   _fSpaceMouseAngle = fAngle;
   _vSpaceMouseAxis = vAxis;
   _bSpaceMouse = true;
   _bPositionChanged = true;
}

//-----------------------------------------------------------------------------

void i3dNodeNavigation::OnPositionUpdateWGS(double lat, double lng, double elv)
{
   if (_bExternalUpdates)
   {
      _longitude = lat;
      _latitude = lng;
      _ellipsoidHeight = elv;
      _bPositionChanged = true;
   }
}

//-----------------------------------------------------------------------------

void i3dNodeNavigation::OnOrientationUpdateQuaternion(double qx, double qy, double qz, double qw)
{
   if (_bExternalUpdates)
   {
      quaternion<double> q;
      yawpitchroll<double> ypr;

      //_bExternalFeed = true;
      _externalQuaternion.Set(qw, qx, qy, qz);
      _bPositionChanged = true;
   }
}

//-----------------------------------------------------------------------------

void i3dNodeNavigation::OnOrientationUpdateYPR(double fYaw, double fPitch, double fRoll)
{
   if (_bExternalUpdates)
   {
      _yaw = fYaw;
      _pitch = fPitch;
      _roll = fRoll;
      _bPositionChanged = true;
   }
}

//------------------------------------------------------------------------------

void i3dNodeNavigation::OnKeyDown(int key)
{
   GeoCoord pos;

   switch (key)
   {
      //case KEY_PAGEUP:
      case '+':
      case 'Q':
         _fVelocityY = _fSpeed*_dElevationVelocity;
         break;
      //case KEY_PAGEDOWN:
      case '-':
      case 'A':
         _fVelocityY = -_fSpeed*_dElevationVelocity;
         break;
      /*case KEY_UP:
         _vFlightDirection = _oMatrixInv * vec3<double>(0,1,0);
         _vFlightDirection.Normalize();
         _fSurfacePitchSpeed = 0.5*_dFlightVelocity*_fSpeed;
         //_bDesiredPitch = false;
         break;
      case KEY_DOWN:
         _vFlightDirection = _oMatrixInv * vec3<double>(0,1,0);
         _vFlightDirection.Normalize();
         _fSurfacePitchSpeed = -0.5*_dFlightVelocity*_fSpeed;
         //_bDesiredPitch = false;
         break;
      case KEY_LEFT: 
         //_fRollSpeed = 0.4f;
         _fYawSpeed = -0.5*_dYawVelocity;
         break;
      case KEY_RIGHT:
         //_fRollSpeed = -0.4f;
         _fYawSpeed = 0.5*_dYawVelocity;
         break;*/
      case 'S': 
         _fPitchSpeed = 0.5*_dPitchVelocity;
         break;
      case 'X': 
         _fPitchSpeed = -0.5*_dPitchVelocity;
         break;
      /*case 'E': 
         _fRollSpeed = 0.5*_dRollVelocity;
         break;
      case 'W':
         _fRollSpeed = -0.5*_dRollVelocity;
         break;
      */
   };
  _bPositionChanged = true;

}

//-----------------------------------------------------------------------------

void i3dNodeNavigation::OnKeyUp(int key)
{
   vec3<double> v;

   switch (key)
   {
      //case KEY_PAGEUP: 
      //case KEY_PAGEDOWN:
      case '+':
      case 'Q':
      case '-':
      case 'A':
         _fVelocityY = 0;
         break;
      /*case KEY_UP: 
      case KEY_DOWN:
         _fSurfacePitchSpeed = 0;
         //_bDesiredPitch = false;
         //_fDesiredPitch = _fSurfacePitch;
         break;
      case KEY_LEFT:
      case KEY_RIGHT:
         //_fRollSpeed = 0;
         _fYawSpeed = 0;
         break;*/
      case 'S': 
      case 'X': 
         _fPitchSpeed = 0;
         break;
      /*case 'W': 
      case 'E':
         _fRollSpeed = 0;*/
         break;

      /*case 'C':
         double Rn = WGS84_a / sqrt(1.0-WGS84_E_SQUARED*sin(DEG2RAD(_latitude))*sin(DEG2RAD(_latitude)));

         double c = Rn * cos (DEG2RAD(_latitude)) * sin(_yaw);
         std::cout << c << "\n";*/


      case 'V':
         vec3<double> FlightDirection = _oMatrixInv * vec3<double>(0,0,1);
         FlightDirection.Normalize();
         std::cout << "Flight Direction: x=" << FlightDirection.x << ", y=" << FlightDirection.y << ", z=" << FlightDirection.z << "\n";
         
         break;
   };
   _bPositionChanged = true;
}

//-----------------------------------------------------------------------------

void i3dNodeNavigation::OnMouseMove(int x, int y)
{
   if (!_bExternalFeed)
   {
      
         _nMouseX = x;
         _nMouseY = y;

         // Calculate Movement Speed:
         double dX = double(_ptDragOrigin[0]-x)/double(_nWidth);
         double dY = double(_ptDragOrigin[1]-y)/double(_nHeight);
         dX *= dX;
         dY *= dY;
         _dSpeed = sqrt(dX + dY);

         int mx = _ptDragOrigin[0];
         int my = _nHeight-1 - _ptDragOrigin[1];

         int cx = x;
         int cy = _nHeight-1 - y;

         _vR.x = double(cx - mx);
         _vR.y = double(cy - my);

         if (_bDragging)
         {
            _fSurfacePitchSpeed = _fSpeed*_dFlightVelocity*dY*math::Sign<double>(_vR.y);
            _fYawSpeed = _dYawVelocity*dX*math::Sign<double>(_vR.x);
         }
         _vR.Normalize();
   }
}

//-----------------------------------------------------------------------------

void i3dNodeNavigation::OnMouseClick(int button, int x, int y)
{
   if (!_bExternalFeed)
   {
      if (button == 1)
      {
         _dSpeed = 0.0;
         _vR.x = 0.0;
         _vR.y = 0.0;
         _ptDragOrigin[0] = x;
         _ptDragOrigin[1] = y;
         _bDragging = true;
         _btn[0] = true;
      }
      else if (button == 2)
      {
         _btn[1] = true;
      }
      else if (button == 3)
      {
         _btn[2] = true;
      }
      else if (button == 4)
      {   
         if (_pitch_decrease > 0)
         {
            _pitch_decrease = 0;
            _pitch_increase = 0;
         }
         else
         {
            _pitch_increase += 0.05;
         }

         return;
      }
      else if (button == 5)
      {
         if (_pitch_increase > 0)
         {
            _pitch_increase = 0;
            _pitch_decrease = 0;
         }
         else
         {
            _pitch_decrease += 0.05;
         }
         return;
      }

      _nMouseX = x;
      _nMouseY = y;
      _bPositionChanged = true;
   }   
}

//-----------------------------------------------------------------------------

void i3dNodeNavigation::OnMouseRelease(int button, int x, int y)
{
   if (button == 1)
   {
      _bDragging = false;
      _btn[0] = false;
   }
   else if (button == 2)
      _btn[1] = false;
   else if (button == 3)
      _btn[2] = false;
   
   _nMouseX = x;
   _nMouseY = y;
   _dSpeed = 0.0;
   _fSurfacePitchSpeed = 0;
   _fYawSpeed = 0;
}

//-----------------------------------------------------------------------------

bool i3dNodeNavigation::_CorrectElevation(vec3<double>& vDir, vec3<double>& vEye, bool bUnderworld)
{
      bool bFoundHitpoint = false;
   
      vec3<double> vEyeC = vEye - vDir;
   
      SceneGraph* pGraph = GetSceneGraph();
      if (!pGraph)
      {
         assert(false);
         return false;
      }
   
      TerrainPickResult oPickResult;
      oPickResult.SetRayDir(vDir);
      oPickResult.SetRayPos(vEyeC);
   
      pGraph->TerrainPickTest(vEyeC, vDir, oPickResult);
      oPickResult.Sort();
   
      vec3<double> vHitPos;
      std::list<TerrainHitRecord>::iterator i = oPickResult.GetHitPoints().begin();
   
      while (i!=oPickResult.GetHitPoints().end())
      {
         
         double lng = (*i).lng;
         double lat = (*i).lat;
         double elv = (*i).elv;
   
         // Epsilon to compare longitude/latitude of camera with longitude/latitude of hitpoint
         // (to retrieve correct hitpoint)
         const double fEps = 10.0;
   
         if (     (math::Abs<double>(_longitude - lng) < fEps)
            && (math::Abs<double>(_latitude -  lat ) < fEps)
            )
         {
   
            double d = _ellipsoidHeight - elv;
   
            if (d>_MinElevation)
            {
               // elevation is ok!
               
               // if we glue to ground, update elevation to be near ground!!
               if (_bGlue)
               {
                  _ellipsoidHeight = elv + (_MinElevation+1.0);
               }
            }
            else
            {      
               _ellipsoidHeight = elv + (_MinElevation+1.0);
            }
   
            if (bUnderworld)
                _ellipsoidHeight = elv + (_MinElevation+1.0);
   
            bFoundHitpoint = true;
         }
  
   
         i++;
      } 
      return bFoundHitpoint;   
}


bool i3dNodeNavigation::_TestElevation(vec3<double>& vDir, vec3<double>& vEye, bool bUnderworld)
{
   vec3<double> vEyeC = vEye - vDir;

   SceneGraph* pGraph = GetSceneGraph();
   if (!pGraph)
   {
      assert(false);
      return false;
   }

   TerrainPickResult oPickResult;
   oPickResult.SetRayDir(vDir);
   oPickResult.SetRayPos(vEyeC);

   pGraph->TerrainPickTest(vEyeC, vDir, oPickResult);
   oPickResult.Sort();

   vec3<double> vHitPos;
   if (oPickResult.GetHitPoints().size() == 2)
      return true;
      
   return false;   
  
}

//-----------------------------------------------------------------------------

void i3dNodeNavigation::OnTick(double dTick)
{
   bool bChanged = false;
   //dTick = math::Max<double>(dTick, 50);

   SceneGraph* pGraph = GetSceneGraph();

   double old_longitude = _longitude;
   double old_latitude = _latitude;
   double old_yaw = _yaw;
   double old_ellipsoidHeight = _ellipsoidHeight;

   double deltaPitch = _fPitchSpeed*dTick/500.0;
   double deltaRoll = _fRollSpeed*dTick/500.0;
   double deltaYaw = _fYawSpeed*dTick/500;
   double deltaH = _fVelocityY*dTick;
   
   //double p = pow(_ellipsoidHeight,0.3087791506979535);
   double p = (_ellipsoidHeight / 500000.0 ) * (_ellipsoidHeight / 500000.0 );
   if (p>10) p=10;
   if (p<0.001) p=0.001;

   double deltaSurface = (p*_fSurfacePitchSpeed*dTick)/250;

   if (_pitch_increase>0)
   {
      double dp = 0.5*_dPitchVelocity*dTick/1000.0;

      _pitch = _pitch + dp;
      _pitch_increase -= dp;
      if (_pitch_increase<0)
      {
         _pitch = _pitch + _pitch_increase;
         _pitch_increase = 0;
      }

      bChanged = true;
   }


   if (_pitch_decrease>0)
   {
      double dp = 0.5*_dPitchVelocity*dTick/1000.0;

      _pitch = _pitch - dp;
      _pitch_decrease -= dp;
      if (_pitch_decrease<0)
      {
         _pitch = _pitch - _pitch_decrease;
         _pitch_decrease = 0;
      }

      bChanged = true;
   }


   if (_bRollAnim)
   {
      if (_roll_increase>0)
      {
         double dp = 0.5*_dRollVelocity*dTick/1000.0;

         _roll = _roll + dp;
         _roll_increase -= dp;
         if (_roll_increase<0)
         {
            _roll = _roll + _roll_increase;
            _roll_increase = 0;
         }

         bChanged = true;
      }


      if (_roll_decrease>0)
      {
         double dp = 0.5*_dRollVelocity*dTick/1000.0;

         _roll = _roll - dp;
         _roll_decrease -= dp;
         if (_roll_decrease<0)
         {
            _roll = _roll - _roll_decrease;
            _roll_decrease = 0;
         }

         bChanged = true;
      }


      if (_roll_decrease == 0 && _roll_increase == 0 && _roll != 0)
      {
         if (_roll<0)
         {
            _roll_increase = -_roll;
         }
         else if (_roll>0)
         {
            _roll_decrease = _roll;
         }
      }


   }



   if (_bSpaceMouse)
   {
      vec3<double> sm = _vSpaceMouseTranslation;
      sm.Normalize();
      deltaH = sm.y*0.1*_fSpeed*dTick;
      deltaSurface = -sm.z*0.00005*_dFlightVelocity*_fSpeed*dTick/1500;

      vec3<double> smaxis = _vSpaceMouseAxis;
      smaxis.Normalize();
      
      if (smaxis.y)
         deltaYaw = -_dYawVelocity*0.1*smaxis.y*dTick/1500.0;
      if (smaxis.x)
         deltaPitch = _dPitchVelocity*0.1*0.5*smaxis.x*dTick/500.0;
      // smaxis.z would be roll

      _bSpaceMouse = false;
      
   }



   if (deltaPitch)
   {
      _pitch += deltaPitch;

      bChanged = true;
   }

   if (deltaYaw)
   {
      _yaw += deltaYaw;

      if (_yaw>2.0*AGEPI)
      {
         _yaw = _yaw-2.0*AGEPI;
      }
      if (_yaw<0)
      {
         _yaw = 2.0*AGEPI - _yaw;
      }

      bChanged = true;

      double maxangle = fabs(_vR.x * _vR.x * AGEPI/4.5);

      if (deltaYaw>0)
      {
         _roll_increase += deltaYaw;
         if ((_roll + _roll_increase) > maxangle)
         {
            _roll_increase = maxangle - _roll;
            if (_roll_increase < 0)
            {
               _roll_decrease = -_roll_increase; 
               _roll_increase = 0;
            }
         }
         else if (_roll_decrease>0)
            _roll_decrease = 0;
      }
      else
      {
         _roll_decrease += -deltaYaw;
         if (-_roll + _roll_decrease > maxangle)
         {
            _roll_decrease = maxangle + _roll;
            if (_roll_decrease < 0)
            {
               _roll_increase = -_roll_decrease;
               _roll_decrease = 0;
            }
         }
         else if (_roll_increase>0)
            _roll_increase = 0;
      }

   }

   if (deltaRoll)
   {
      _roll += deltaRoll;

      bChanged = true;
   }

   if (deltaH)
   {
      _ellipsoidHeight += 1000*deltaH*p;

      // limit ellipsoid height
      /*if (_ellipsoidHeight<=10)
         _ellipsoidHeight = 10;*/
   }

   if (_ellipsoidHeight>7000000)
      _ellipsoidHeight = 7000000;

   if (_ellipsoidHeight<0)
      _ellipsoidHeight = 0;

   if (deltaSurface != 0)
   {
         //--------------------------------------------------------------------
         // Navigation: (Geodaetische Linie [entpricht Grosskreis auf Kugel])
         //--------------------------------------------------------------------

         double A1 = _yaw;
         double B1 = DEG2RAD(_latitude);
         double L1 = DEG2RAD(_longitude);
         //double s  = deltaSurface * WGS84_a;
         // Normalkruemungsradius Rn
         double Rn = WGS84_a / sqrt(1.0-WGS84_E_SQUARED*sin(DEG2RAD(_latitude))*sin(DEG2RAD(_latitude)));
         // Meridiankruemungsradius Rm
         double coslat2 = cos(DEG2RAD(_latitude));
         double Rm = Rn / (1+WGS84_E_SQUARED2*coslat2*coslat2);

         //double deltaA = (s * sin(A1) * tan(B1)) / Rn;
         //double deltaB = (s * cos(A1) / Rm);
         //double deltaL = (s * sin(A1) / (Rn*cos(B1)));


         double deltaA = (WGS84_a / Rn) * deltaSurface * sin(A1) * tan(B1);
         double deltaB = (WGS84_a / Rm) * deltaSurface * cos(A1);
         double deltaL = (WGS84_a / Rn) * deltaSurface * sin(A1) / cos(B1);


         double A2, B2, L2;
         A2 = deltaA + A1;
         B2 = deltaB + B1;
         L2 = deltaL + L1;
        
         _longitude = RAD2DEG(L2);
         _latitude = RAD2DEG(B2);
         _yaw = A2;

         bChanged = true;
   }

   // turn on curtaining (default!)
   if (pGraph)
   {
      pGraph->VirtualGlobeMode(VG_ENABLE_CURTAIN, true);
   }
   // Collision Test: Ray

   if (_bExternalFeed)
   {
      //_bDoCollisionCheck = false;
      vec3<double> vEye;
      GeoCoord pos(_longitude, _latitude, _ellipsoidHeight);
      pos.GetCartesian(vEye);
      vec3<double> vDir = -vEye; vDir.Normalize();
      bool bOverworld = _TestElevation(vDir, vEye);
      
      if (!bOverworld)
      {
         // turn off curtain
         if (pGraph)
         {
            pGraph->VirtualGlobeMode(VG_ENABLE_CURTAIN, false);
         }
      }
   }
   else if (pGraph && _bDoCollisionCheck)
   {
      vec3<double> vEye;
      GeoCoord pos(_longitude, _latitude, _ellipsoidHeight);
      pos.GetCartesian(vEye);
      vec3<double> vDir = -vEye; vDir.Normalize();
      bool bFoundHitpoint = _CorrectElevation(vDir, vEye);

      // if no hitpoint is found, camera is somehow under the earth surface!!
      // in that case move back to surface!
      if (!bFoundHitpoint)
      {
         //std::cout << "Welcome to Underworld! [No hitpoint found, doing test opposite side]\n";

         GeoCoord newpos(_longitude, _latitude, 10000);
        
         vec3<double> vVirtualEye; // virtual eye is 10000 meters over ground, now do collision test again!
         newpos.ToCartesian(&vVirtualEye.x,&vVirtualEye.y,&vVirtualEye.z);
         vec3<double> vVirtualDir = -vVirtualEye; vVirtualDir.Normalize();

         bFoundHitpoint = _CorrectElevation(vVirtualDir, vVirtualEye, true);

         if (!bFoundHitpoint)
         {
            // ?
         }

      }
   }

   if (bChanged)
   {
      GetSceneGraph()->YawPitchRoll(_yaw, _pitch, _roll);
   }
}

//-----------------------------------------------------------------------------

void i3dNodeNavigation::OnInit()
{

}

//-----------------------------------------------------------------------------

void i3dNodeNavigation::OnExit()
{
   // Destroy Compass Texture
}

//-----------------------------------------------------------------------------

void i3dNodeNavigation::SetPosition(double lng, double lat, double elv)
{
   _longitude = lng;
   _latitude = lat;
   _ellipsoidHeight = elv;
}

//-----------------------------------------------------------------------------

void i3dNodeNavigation::GetPosition(double& lng, double& lat, double& elv)
{
   lng = _longitude;
   lat = _latitude;
   elv = _ellipsoidHeight;
}

//-----------------------------------------------------------------------------

void i3dNodeNavigation::SetOrientation(double yaw, double pitch, double roll)
{
   _yaw = yaw;
   _pitch = pitch;
   _roll = roll;
}

//-----------------------------------------------------------------------------

void i3dNodeNavigation::GetOrientation(double& yaw, double& pitch, double& roll)
{
   yaw = _yaw;
   pitch = _pitch;
   roll = _roll;  
}

//-----------------------------------------------------------------------------
