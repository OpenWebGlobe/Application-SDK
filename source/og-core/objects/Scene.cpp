   
#include "Scene.h"
#include "Camera.h"


//---------------------------------------------------------------------------
using namespace OpenGlobe;
BeginPropertyMap(Scene);
   Inherits(Object);
   //XMLPropertyReadOnly(Scene, "width", _width);
EndPropertyMap(Scene);
//---------------------------------------------------------------------------

namespace OpenGlobe
{
   //---------------------------------------------------------------------------
   Scene::Scene()
      : _parentContext(0), _eType(Scene::INVALID), _pActiveCamera(0)
   {
      _name = "ogScene";
      _eObjectType = Object::SCENE;
   }
   //---------------------------------------------------------------------------
   Scene::~Scene()
   {
   }
   //---------------------------------------------------------------------------
   void Scene::CreateScene(Scene::Type eType)
   {
      _parentContext = dynamic_cast<Context*>(_pParent);
      _eType = eType;

      if (_parentContext)
      {
         _parentContext->SetScene(this);
      }

      if (!_pActiveCamera)
         CreateCamera();
   }
   //---------------------------------------------------------------------------
   void Scene::SetOptions(const OptionVector& options)
   {
      // scenes don't get options at this time.
   }
   //---------------------------------------------------------------------------
   void Scene::Render(IRenderEngine* pRenderEngine)
   {
      // render scene

      Camera* pCamera = GetActiveCamera();
      if (pCamera)
      {
         // update camera!
      }


   }
   //---------------------------------------------------------------------------
   void Scene::DestroyScene()
   {
      // called before scene is destroyed. Remove and delete all subnodes

      // remove scene from context
      if (_parentContext)
      {
         _parentContext->SetScene(0);
      }

   }
   //---------------------------------------------------------------------------
   Camera*  Scene::CreateCamera()
   {
      std::string opt;
      Object* pObject = OpenGlobe::CreateObject(OpenGlobe::Object::CAMERA, this, opt);
      Camera* pCamera = dynamic_cast<Camera*>(pObject);
      
      if (pCamera)
      {
         if (_pActiveCamera == 0) // no active camera yet ?
         {
            _pActiveCamera = pCamera;
         }

         _vecChildren.push_back(pObject); // push to children list
      }


      return pCamera;
   }
   //---------------------------------------------------------------------------
   void  Scene::SetActiveCamera(Camera* pCamera)
   {
      Object* pObject = (Object*)pCamera;

      for (size_t i=0;i<_vecChildren.size();i++)
      {
         if (_vecChildren[i] == pObject)
         {
            // found camera in list!
            // this means it is valid. Setting camera to active camera
            _pActiveCamera = pCamera;
            return;
         }
      }

   }
   //---------------------------------------------------------------------------
   unsigned int Scene::GetNumCameras()
   {
      size_t numCameras=0;

      for (size_t i=0;i<_vecChildren.size();i++)
      {
         if (_vecChildren[i]->GetObjectType() == Object::CAMERA)
         {
            numCameras++;
         }
      }

      return numCameras;
   }
   //---------------------------------------------------------------------------
   Camera* Scene::GetCameraAt(unsigned int idx)
   {
      size_t curidx=0;
      for (size_t i=0;i<_vecChildren.size();i++)
      {
         if (_vecChildren[i]->GetObjectType() == Object::CAMERA)
         {
            if (curidx == idx)
            {
               return (Camera*)_vecChildren[i];
            }
            curidx++;
         }
      }

      return 0;
   }
   //---------------------------------------------------------------------------


}