   
#include "Camera.h"
#include "World.h"
#include "ImageLayer.h"
//#include "ElevationLayer.h"
#include "scenegraph/i3dNodeTerrain.h"

//---------------------------------------------------------------------------
using namespace OpenGlobe;
BeginPropertyMap(World);
   Inherits(Object);
   //XMLPropertyReadOnly(Scene, "width", _width);
EndPropertyMap(World);
//---------------------------------------------------------------------------

namespace OpenGlobe
{
   //---------------------------------------------------------------------------
   World::World()
      : _parentScene(0), _bShow(true), _pRenderer(0)
   {
      _name = "ogWorld";
      _eObjectType = Object::WORLD;

   }
   //---------------------------------------------------------------------------
   World::~World()
   {
      // Delete image layer:
      std::list<ImageLayer*>::iterator it=_lstImgLayers.begin();

      /*while (it != _lstImgLayers.end())
      {
         delete *it; // delete it!
         ++it;
      }*/
      /*
       at this time scene graph is already destroyed, this shouldn't happen

        // remove virtual globe renderer from scenegraph:
      if (_parentScene)
      {
         OpenGlobe::Context* pContext = _parentScene->GetContext();
         if (pContext)
         {
            SceneGraph* pScene = pContext->GetSceneGraph();
            if (pScene)
            {
               i3dNodeTerrain* pTerrain = pScene->GetTerrainNode();
               
               if (pTerrain)
               {
                  pTerrain->SetTerrainRenderer(0);
               }
            }
         }
      }

      */
      // and delete virtual globe renderer:
      if (_pRenderer)
      {
         delete _pRenderer;
         _pRenderer = 0;
      }
   }
   //---------------------------------------------------------------------------
   void World::SetOptions(const OptionVector& options)
   {
      _parentScene = dynamic_cast<Scene*>(_pParent);
   }

   //---------------------------------------------------------------------------
   void World::Show()
   {
      _bShow = true;
   }
   //---------------------------------------------------------------------------
   void World::Hide()
   {
      _bShow = false;
   }

   //---------------------------------------------------------------------------

   void World::AddImageLayer(ImageLayer* pImgLayer)
   {
      if (pImgLayer)
      {
         _lstImgLayers.push_back(pImgLayer);
      }

       _RecreateWorld();
   }

   //---------------------------------------------------------------------------
   
   void World::RemoveImageLayer(ImageLayer* pImgLayer)
   {
      std::list<ImageLayer*>::iterator it=_lstImgLayers.begin();

      while (it != _lstImgLayers.end())
      {
         if (*it == pImgLayer)
         {
            it = _lstImgLayers.erase(it);
         }
         else
         {
            ++it;
         }
      }

      _RecreateWorld();
   }
   
   //---------------------------------------------------------------------------
  
   /*bool World::TestLayers()
   {
      std::list<ImageLayer*>::iterator it=_lstImgLayers.begin();

      size_t cnt = 0;

      while (it != _lstImgLayers.end())
      {
         if ((*it)->Failed())
         {
            //this layer failed, remove it!!
            ImageLayer* pLayer = *it;
            it = _lstImgLayers.erase(it);
            delete pLayer;
         }
         else
         {
            if ((*it)->Ready())
            {
               ++cnt;
            }
            ++it;
         }
      }

      return (cnt == _lstImgLayers.size());

   }
   */

   //---------------------------------------------------------------------------
   
   void World::_RecreateWorld()
   {
      if (_pRenderer)
      {
         delete _pRenderer;
      }

      _pRenderer = new VirtualGlobeRenderer();
      
      // Convert Image Layer
      std::list<ImageLayer*>::iterator it=_lstImgLayers.begin();
      std::vector< boost::shared_ptr<IImageLayer> > vecImgLayer;

      while (it != _lstImgLayers.end())
      {
         vecImgLayer.push_back((*it)->GetImageLayer());
         ++it;
      }

      _pRenderer->SetLayers(vecImgLayer/*, _lstElvLayers*/);

      //

      if (_parentScene)
      {
         OpenGlobe::Context* pContext = _parentScene->GetContext();
         if (pContext)
         {
            SceneGraph* pScene = pContext->GetSceneGraph();
            if (pScene)
            {
               i3dNodeTerrain* pTerrain = pScene->GetTerrainNode();
               
               if (pTerrain)
               {
                  pTerrain->SetTerrainRenderer(_pRenderer);
               }
            }
         }
      }
   }

   //---------------------------------------------------------------------------

}