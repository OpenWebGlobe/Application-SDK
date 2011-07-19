
#ifndef _SCENE_H
#define _SCENE_H

#include "Object.h"
#include "Context.h"
#include <vector>


namespace OpenGlobe
{
   class Camera;
   class OPENGLOBE_API Scene : public OpenGlobe::Object
   {
   public:
      Scene();
      virtual ~Scene();

      enum Type
      {
         ELLIPSOID_WGS84,     // virtual globe with wgs84 ellipsoid
         FLAT_CARTESIAN,      // flat earth rendering with cartesian coordinates
         SCREEN,              // 2d-screen coordinates (maps-type applications)
         CUSTOM,              // custom scene type (for advanced users)

         INVALID,
      };

      void CreateScene(Scene::Type eType);
      void DestroyScene();

      Context* GetContext() { return _parentContext;}
      Scene::Type GetSceneType() {return _eType;}
      //World* GetWorld();

      //void SetDiskCacheDirectory(const std::string& dir);
      //void SetMemoryCacheSize(float size);
      //void SetTileCacheSize(float size);
      //void SetNumDownloadThreads(float size);
      //void ClearDiskCache();

      void Render(IRenderEngine* pRenderEngine);

      // Create and Add Child objects
      
      Camera* CreateCamera();  // Create and add camera!
      void SetActiveCamera(Camera* pCamera);
      unsigned int GetNumCameras();
      Camera* GetCameraAt(unsigned int idx);
      Camera* GetActiveCamera(){return _pActiveCamera;}

      /*void CreateWorld();
      void CreateImage();
      void CreatePixelbuffer();
      void CreateTexture();
      void CreateGeometry();
      void CreateMaterial();
      void CreatePoi();
      void CreateWaypoint();
      void CreateLight();
      void CreateBinaryData();
      void CreateText();
      */

      virtual void SetOptions(const OptionVector& options);

      const std::vector<OpenGlobe::Object*>& GetChildren() {return _vecChildren;}

   protected:
      Context*          _parentContext;
      std::vector<OpenGlobe::Object*> _vecChildren;  // Child Objects
      Scene::Type       _eType;
      Camera*           _pActiveCamera;
   };

}

#endif
