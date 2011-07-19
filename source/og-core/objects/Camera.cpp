   
#include "Camera.h"
#include "Context.h"
#include "scenegraph/i3dNodeCamera.h"
#include "scenegraph/i3dNodeNavigation.h"

//---------------------------------------------------------------------------
using namespace OpenGlobe;
BeginPropertyMap(Camera);
   Inherits(Object);
   //XMLPropertyReadOnly(Scene, "width", _width);
EndPropertyMap(Camera);
//---------------------------------------------------------------------------

namespace OpenGlobe
{
   //---------------------------------------------------------------------------
   Camera::Camera()
      : _parentScene(0), _pController(0), _pContext(0)
   {
      _name = "ogCamera";
      _eObjectType = Object::CAMERA;
   }
   //---------------------------------------------------------------------------
   Camera::~Camera()
   {

   }
   //---------------------------------------------------------------------------
   void Camera::SetOptions(const OptionVector& options)
   {
      _parentScene = dynamic_cast<Scene*>(_pParent);
      if (_parentScene)
         _pContext = _parentScene->GetContext();

      // create navigation controller:
      std::string nav_opt;
      Object* pObject = OpenGlobe::CreateObject(OpenGlobe::Object::NAVIGATIONCONTROLLER, this, nav_opt);
      _pController = dynamic_cast<NavigationController*>(pObject);
   }
   //---------------------------------------------------------------------------
   void Camera::CreateOrtho2D(double left, double right, double bottom, double top)
   {
      if (!_pContext)
         return;

      i3dNodeCamera* pCam = 0;
      SceneGraph* pSceneGraph = _pContext->GetSceneGraph();
      if (pSceneGraph)
      {
         pCam = pSceneGraph->GetCameraNode();
      }

      if (pCam)
      {
         pCam->CreateOrtho2d(left, right, bottom, top);
      }
      
   }
   //---------------------------------------------------------------------------
   void Camera::CreatePerspectiveProjection(double fovY, double zNear, double zFar)
   {
       if (!_pContext)
         return;

      i3dNodeCamera* pCam = 0;
      SceneGraph* pSceneGraph = _pContext->GetSceneGraph();
      if (pSceneGraph)
      {
         pCam = pSceneGraph->GetCameraNode();
      }

      if (pCam)
      {
         pCam->CreatePerspectiveProjection(fovY, zNear, zFar);
      }
   }
   //---------------------------------------------------------------------------
   void Camera::CreateLookAt(double eyeX,    double eyeY,    double eyeZ, 
                     double centerX, double centerY, double centerZ,
                     double upX,     double upY,     double upZ)
   {
      
   }
   //---------------------------------------------------------------------------
      
   void Camera::SetProjectionMatrix(const mat4<double>& matProjection)
   {
      if (!_pContext)
         return;

      i3dNodeCamera* pCam = 0;
      SceneGraph* pSceneGraph = _pContext->GetSceneGraph();
      if (pSceneGraph)
      {
         pCam = pSceneGraph->GetCameraNode();
      }

      if (pCam)
      {
         pCam->CreateCustomProjection(matProjection);
      }
   }

   //---------------------------------------------------------------------------
   // VIEW MATRIX
   void Camera::SetViewMatrix(const mat4<double>& matView)
   {
   }

   //---------------------------------------------------------------------------

   void Camera::SetPositionWGS84(double longitude, double latitude, double elv)
   {
       if (!_pContext)
         return;

      i3dNodeNavigation* pNav = 0;
      SceneGraph* pSceneGraph = _pContext->GetSceneGraph();
      if (pSceneGraph)
      {
         pNav = pSceneGraph->GetNavigationNode();
      }

      if (pNav)
      {
         pNav->SetPosition(longitude, latitude, elv);
      }
   }

   //---------------------------------------------------------------------------

   void Camera::SetOrientationWGS84(double yaw, double pitch, double roll)
   {
      if (!_pContext)
         return;

      i3dNodeNavigation* pNav = 0;
      SceneGraph* pSceneGraph = _pContext->GetSceneGraph();
      if (pSceneGraph)
      {
         pNav = pSceneGraph->GetNavigationNode();
      }

      if (pNav)
      {
         pNav->SetOrientation(yaw, pitch, roll);
      }
   }

   //---------------------------------------------------------------------------
}