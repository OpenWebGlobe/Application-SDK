#include "og.h"


//-------------
#define DEMO_13
//-------------


#ifdef DEMO_0

#include "data/DiskCache.h"

//------------------------------------------------------------------------------
// Callback Funktion, wird aufgerufen
void OnLoad(const std::string& url, MemoryEntry* pMem, void* userdata)
{
   if (pMem->IsFailed())
   {
      std::cout << "FAILED DOWNLOADING " << url << "\n";
      
      return;
   }

   std::cout << url << ": " << pMem->DataSize() << " bytes.\n";
}
//------------------------------------------------------------------------------
int main(void)
{
   
   Cache cache("C:/test/", 100.0, 50.0, 2);

   // Asnychrones herunterladen von Daten:
   cache.RequestDataAsync("http://a.tile.openstreetmap.org/18/136601/91590.png", &OnLoad); 
   cache.RequestDataAsync("http://a.tile.openstreetmap.org/18/136601/91591.png", &OnLoad); 
   cache.RequestDataAsync("http://a.tile.openstreetmap.org/18/136601/91592.png", &OnLoad); 
   cache.RequestDataAsync("http://a.tile.openstreetmap.org/18/136601/91593.png", &OnLoad); 
   cache.RequestDataAsync("http://www.openwebglobe.org/data/bla.txt", &OnLoad); 

   // Synchrones herunterladen: (callback Funktion is fakultativ und oft unnötig)
   MemoryEntry* pMem = cache.RequestData("http://a.tile.openstreetmap.org/18/136601/91590.png", &OnLoad); 

   cache.wait();
   cache.dump();
   
   return 0;
}

//------------------------------------------------------------------------------
#endif

#ifdef DEMO_1

// How to Create a RenderWindowGL (Windows) directly:
// this should be avoided

#include "win32/RenderWindowGL.h"
#include "ObjectFactory.h"

using namespace OpenGlobe;

int main(void)
{
   RenderWindowGL win;
   win.SetOptions(OptionString("height=640 width=320")); // Set Options by using OptionString
   win.CreateRenderWindow();
   return win.exec();
}

#endif

#ifdef DEMO_2

#include "RenderFactory.h"

// this is the official way to create a render window:

using namespace OpenGlobe;

int main(void)
{
   shared_ptr<IRenderWindow> qRenderWindow;
   
   qRenderWindow = RenderFactory::RenderWindow(OptionString("width=640 height=480"));
   qRenderWindow->Create();

   return OpenGlobe::Exec();
}

#endif



#ifdef DEMO_3

#include "ObjectFactory.h"
#include "objects/Context.h"
#include <iostream>

// this is the official way to create a render window:

using namespace OpenGlobe;

//------------------------------------------------------------------------------

void OnInit(int context)
{
   std::cout << "OnInit()\n";
}

//------------------------------------------------------------------------------

void OnMouseMove(int context, int x, int y)
{
   std::cout << "OnMouseMove(" << x << ", " << y << ")\n";
}

//------------------------------------------------------------------------------

int main(void)
{
   Object* pObject;
   Context* pContext;
   
   pObject = CreateObject(Object::CONTEXT, shared_ptr<Object>(), "title='OpenWebGlobe Demo' width=640 height=480");
   pContext = dynamic_cast<Context*>(pObject);

   if (pContext)
   {
      pContext->SetCallBack_Init(OnInit);
      pContext->SetCallBack_MouseMove(OnMouseMove);
  
      pContext->CreateContext();
      int ret = OpenGlobe::Exec();

      delete pContext;
      return ret;
   }

   return -1;
}

//------------------------------------------------------------------------------

#endif



#ifdef DEMO_4

#include <iostream>

float r,g,b,a;
float addtime;

//------------------------------------------------------------------------------

void OnInit(int context)
{
   std::cout << "OnInit()\n";
}

//------------------------------------------------------------------------------

void OnExit(int context)
{
   std::cout << "OnExit()\n";
}

//------------------------------------------------------------------------------

void OnResize(int context, int w, int h)
{
   std::cout << "OnResize(" << w << ", " << h << ")\n";
}

//------------------------------------------------------------------------------

void OnKeyDown(int context, unsigned int key)
{
   std::cout << "key = " << key << "\n";
   if (key == 27) // ESCAPE
   {
      ogDestroyContext(context);
   }
   if (key == ' ')
   {
      std::cout << "w=" << ogGetWidth(context) << ", h=" << ogGetHeight(context) << "\n";
   }
}

//------------------------------------------------------------------------------
// Render Callback

void OnRender(int scene)
{
  ogSetBackgroundColor(context, r,g,b,a);  
}

//------------------------------------------------------------------------------

void OnTimer(int context, float dt)
{
   addtime += dt;
   while (addtime>500)
   {
      addtime -= 500;
      r = ogRand01(); g = ogRand01(); b = ogRand01();
   }
}

//------------------------------------------------------------------------------

int main(void)
{
   ogInfo();
   addtime = 0;
   r = g = b = a = 0;
   //int context = ogCreateRenderWindow("Hello World", 640, 480);
   int context = ogCreateContext("title='Hello World' width=640 height=480" , &OnInit, &OnExit, &OnResize);
   ogSetKeyDownFunction(context, &OnKeyDown);
   ogSetRenderFunction(context, &OnRender);
   ogSetTimerFunction(context, &OnTimer);

   //std::cout << "Num Objects: " << ogGetNumObjects() << "\n";
   ogSetStringProperty(context, "myString", "This is the main context!");
   ogSetIntegerProperty(context, "myInt", 5);
   ogSetDoubleProperty(context, "myFloat", 3.141);
   ogSetBoolProperty(context,"myBool", true);
   
   std::string sXML = ogToXML(context); 
   
   std::cout << sXML << "\n";
   
   //ogSetBackgroundColor(context, 0,1,1,0); 
   int ret = ogExec();

   //std::cout << "Num Objects: " << ogGetNumObjects() << "\n";
   

   return ret;
}


#endif

#ifdef DEMO_5

void OnRender(int scene)
{

}

int main(void)
{
   int context = ogCreateRenderWindow("Hello World", 640, 480);
   ogSetRenderFunction(context, &OnRender);
   ogSetBackgroundColor(context, 0,0.5,0.5,0.0);
   
   // Create custom scene:
   // the scene already has a default camera
   int scene = ogCreateScene(context, OG_SCENE_CUSTOM);

   // ogGetScene also retrieves the scene attached to the context:
   assert(ogGetScene(context) == scene);

   // add 4 additional cameras
   int camera1 = ogCreateCamera(scene);
   int camera2 = ogCreateCamera(scene);
   int camera3 = ogCreateCamera(scene);
   int camera4 = ogCreateCamera(scene);
   
   std::cout << "There are " << ogGetNumCameras(scene) << " cameras in the scene\n";

   // retrieve current camera of the scene
   int camera = ogGetActiveCamera(scene);
   std::cout << "The active camera has id: " << camera << "\n";
  
   int navigationcontroller = ogGetNavigationController(camera);
   std::cout << "The (active) navigation controller has id: " << navigationcontroller << "\n";
   

   ogExec();
   

}


#endif



#ifdef DEMO_6

#include <iostream>

//-----------------------------------------------------------------------------
void OnRender(int scene)
{
   int context = ogGetParentObject(scene);
   //ogSetDrawColor(context, ogRand01(),ogRand01(),ogRand01(),0);
   //ogDrawWireSphere(1.0,64,64);
   //ogDrawWireTorus(0.4,0.5,64,32);
   ogDrawWireCube(1.0);
}
//------------------------------------------------------------------------------
void OnResize(int context, int w, int h)
{
   int camera = ogGetActiveCamera(ogGetScene(context));
   //ogCreatePerspectiveProjection(camera, 45, 0.1, 100.0);
   //ogCreateOrtho2D(camera, -2, 2, -2, 2);
   //ogLookAt(camera,0,0,5, 0,0,0, 0,1,0);
}
//------------------------------------------------------------------------------
void OnKeyDown(int context, unsigned int key)
{
   //std::cout << "Key: " << key << "\n";
   if (key == 80) // 'P' for Position!
   {
      double lng = 7.583613395690918;
      double lat = 47.55837035257613;
      double elv = 300;

      double x,y,z;
      int scene = ogGetScene(context);
      ogToCartesian(scene, lng, lat, elv, &x, &y, &z);
      std::cout << "cartesian pos: (" << x << ", " << y << ", " << z << ")\n";

      int camera = ogGetActiveCamera(ogGetScene(context));
      ogSetPositionWGS84(camera, lng, lat, elv);
      ogSetOrientationWGS84(camera,0,-3.141/2.0,0);
   }
}
//------------------------------------------------------------------------------
int main(void)
{
   int context = ogCreateRenderWindow("Hello World", 640, 480);
   
   int scene = ogCreateScene(context, OG_SCENE_CUSTOM);

   ogSetResizeFunction(context, &OnResize);
   ogSetRenderFunction(context, &OnRender);
   ogSetKeyDownFunction(context, &OnKeyDown);
   
   ogSetBackgroundColor(context, 1,0,0,0.0);
   
   ogExec();
}
//------------------------------------------------------------------------------

#endif


#ifdef DEMO_7

#include "Object.h"
#include "image/ImageLoader.h"

bool bDone = false;

void OnImageReady(ImageObject* pImage, void* userdata)
{
   if (pImage)
   {
      pImage->SavePPM("c:\\out2.ppm");
      delete pImage;
   }

   //bDone = true;
}

void OnImageFailed(void* userdata)
{
   std::cout << "FAILED LOADING!!\n";
}

int main(void)
{
   // Synchronous Loading:
   //ImageObject outputimage;
   //if (ImageLoader::LoadFromDisk(Img::Format_JPG, "C:\\image.jpg", Img::PixelFormat_RGB, outputimage))
   //{
   //   outputimage.SavePPM("c:\\out.ppm");
   //}

   // Asynchronous Loading:w 
   ImageLoader::LoadFromUrl_ThreadedUnpack(Img::Format_PNG, "http://tuxpaint.org/stamps/stamps/animals/birds/cartoon/tux.png", Img::PixelFormat_RGBA, &OnImageReady, &OnImageFailed, 0);
   //ImageLoader::LoadFromUrl(Img::Format_PNG, "http://tuxpaint.org/stamps/stamps/animals/birds/cartoon/tux23.png", Img::PixelFormat_RGBA, &OnImageReady, &OnImageFailed, 0);
  
   // wait for downloads
   OpenGlobe::_ogCache().wait();
   return 0;
}

#endif



#ifdef DEMO_8

#include "io/CommonPath.h"
#include <string>
#include <iostream>

int main(void)
{
   ogSetApplicationName("Demo 8"); // set application name so we have our exclusive cache for this app

   // get settings directory (this is where the cache is located)
   const char* sDir = ogGetSettingsDirectory();
   std::cout << "Settings Dir= " << sDir << "\n";

   return 0;
}

#endif

#ifdef DEMO_9

#include "io/CommonPath.h"
#include <string>
#include <iostream>

int main(void)
{
   ogSetApplicationName("Demo 9"); // set application name so we have our exclusive cache for this app

   // get settings directory (this is where the cache is located)
   const char* sDir = ogGetSettingsDirectory();
   std::cout << "Settings Dir= " << sDir << "\n";

   return 0;
}

#endif


#ifdef DEMO_10


#include <iostream>

int tex0 = -1;
int tex1 = -1;
int tex2 = -1;
int tex3 = -1;
int tex4 = -1;
int tex5 = -1;
int tex6 = -1;
int tex7 = -1;

int tux = -1;

//-----------------------------------------------------------------------------
void OnRender(int scene)
{
   ogBlitTexture(tex0,0,0);
   ogBlitTexture(tex1,256,0);
   ogBlitTexture(tex2,512,0);
   ogBlitTexture(tex3,768,0);
   ogBlitTexture(tex4,0,256);
   ogBlitTexture(tex5,256,256);
   ogBlitTexture(tex6,512,256);
   ogBlitTexture(tex7,768,256);

   ogBlitTexture(tux, 100,100);
   
}
//------------------------------------------------------------------------------
void OnResize(int context, int w, int h)
{

}

void OnKeyDown(int context, unsigned int key)
{
   int scene = ogGetScene(context);

   if (key == 65)
   {
      tex0 = ogLoadTextureAsync(scene, "http://a.tile.openstreetmap.org/18/136601/91589.png", "png", OG_PIXELFORMAT_RGBA);
      tex1 = ogLoadTextureAsync(scene, "http://a.tile.openstreetmap.org/18/136602/91589.png", "png", OG_PIXELFORMAT_RGBA);
      tex2 = ogLoadTextureAsync(scene, "http://a.tile.openstreetmap.org/18/136603/91589.png", "png", OG_PIXELFORMAT_RGBA);
      tex3 = ogLoadTextureAsync(scene, "http://a.tile.openstreetmap.org/18/136604/91589.png", "png", OG_PIXELFORMAT_RGBA);

      tex4 = ogLoadTextureAsync(scene, "http://a.tile.openstreetmap.org/18/136601/91590.png", "png", OG_PIXELFORMAT_RGBA);
      tex5 = ogLoadTextureAsync(scene, "http://a.tile.openstreetmap.org/18/136602/91590.png", "png", OG_PIXELFORMAT_RGBA);
      tex6 = ogLoadTextureAsync(scene, "http://a.tile.openstreetmap.org/18/136603/91590.png", "png", OG_PIXELFORMAT_RGBA);
      tex7 = ogLoadTextureAsync(scene, "http://a.tile.openstreetmap.org/18/136604/91590.png", "png", OG_PIXELFORMAT_RGBA);
   
      tux = ogLoadTextureAsync(scene, "http://tuxpaint.org/stamps/stamps/animals/birds/cartoon/tux.png", "png", OG_PIXELFORMAT_RGBA);

   }
}

//------------------------------------------------------------------------------
int main(void)
{
   int context = ogCreateRenderWindow("Hello World", 1024, 512);
   int scene = ogCreateScene(context, OG_SCENE_CUSTOM);

   ogSetResizeFunction(context, &OnResize);
   ogSetRenderFunction(context, &OnRender);
   ogSetKeyDownFunction(context, &OnKeyDown);
   
   ogSetBackgroundColor(context, 0,0,0,0);

 
   ogExec();
}


#endif


#ifdef DEMO_11

//---------------------------------------------------------------------------------------------
// This LOW LEVEL example shows how to create a custom tile reader and start downloading tiles
//---------------------------------------------------------------------------------------------

#include "Object.h"
#include "geo/layer/LayerFactory.h"
#include <iostream>

//------------------------------------------------------------------------------
void OnTileReady(const std::string& sQuadcode, ImageTile* pTile)
{
    std::cout << "Finished downloading tile " << sQuadcode << "\n";
    std::cout << "width=" << pTile->GetWidth() << ", height=" << pTile->GetHeight() << "\n";
    // Store in temp dir:
    std::string sFile = "c:/openwebglobe/" + sQuadcode + ".ppm";
    pTile->SavePPM(sFile);
    delete pTile; // don't need anymore.. oh what a waste!!
}
//------------------------------------------------------------------------------
void OnTileFailed(const std::string& sQuadcode)
{
   std::cout << "Can't read tile " << sQuadcode << "\n";
}
//------------------------------------------------------------------------------
void OnNotify(IImageLayer* pLayer)
{
   if (pLayer->Ready())
   {
      std::cout << "Layer is now ready!\n";
   }
   else
   {
      std::cout << "Layer failed!!\n";
   }
}
//------------------------------------------------------------------------------
int main(void)
{
   boost::shared_ptr<IImageLayer> imglayer;
   //imglayer = LayerFactory::CreateImageLayeri3D("G:\\i3d\\Tiles", "World500", OnNotify);
   imglayer = LayerFactory::CreateImageLayeri3D("http://www.openwebglobe.org/data/img", "World500", OnNotify);

   
   if (imglayer)
   {
      // wait for image layer:
      while (!imglayer->Ready())
      {
         if (imglayer->Failed())
         {
            std::cout << "Failed opening image layer!!\n";
            return -1;
         }

         OpenGlobe::_ogCache().process();
      }

      // download some tiles:
      imglayer->RequestTile("0", OnTileReady, OnTileFailed);
      imglayer->RequestTile("1", OnTileReady, OnTileFailed);
      imglayer->RequestTile("2", OnTileReady, OnTileFailed);
      imglayer->RequestTile("3", OnTileReady, OnTileFailed);
      //imglayer->RequestTile("02010101", OnTileReady, OnTileFailed);
      //imglayer->RequestTile("02010101111", OnTileReady, OnTileFailed);

   }
   else
   {
      std::cout << "Failed creating i3d image layer!\n";
   }

   OpenGlobe::_ogCache().wait();
}
//------------------------------------------------------------------------------
#endif

#ifdef DEMO_12

// This demo shows how to use the Virtual Globe Cache

#include "Object.h"
#include "geo/VirtualGlobeCache.h"
#include "geo/layer/LayerFactory.h"

bool bGood = false;

void OnNotify(IImageLayer* pLayer)
{
  bGood = true;
}

int main(void)
{
   std::vector< boost::shared_ptr<IImageLayer> > lstImg;
   std::vector< boost::shared_ptr<IElevationLayer> > lstElv;
   boost::shared_ptr<MercatorQuadtree> quadtree =  boost::shared_ptr<MercatorQuadtree>(new MercatorQuadtree);

   boost::shared_ptr<IImageLayer> layerWorld500 = LayerFactory::CreateImageLayeri3D("http://www.openwebglobe.org/data/img", "World500", 0);
   lstImg.push_back(layerWorld500);

    while (!layerWorld500->Ready())
      {
         if (layerWorld500->Failed())
         {
            std::cout << "Failed opening image layer!!\n";
            return -1;
         }

         OpenGlobe::_ogCache().process();
      }

    // Layer is now ready!
   VirtualGlobeCache globecache(lstImg, lstElv, quadtree);

   boost::shared_ptr<VirtualGlobeTerrainBlock> block0 = globecache.RequestBlock("0");
   boost::shared_ptr<VirtualGlobeTerrainBlock> block1 = globecache.RequestBlock("1");
   boost::shared_ptr<VirtualGlobeTerrainBlock> block2 = globecache.RequestBlock("2");
   boost::shared_ptr<VirtualGlobeTerrainBlock> block3 = globecache.RequestBlock("3");

  


   OpenGlobe::_ogCache().wait();
   return 0;
}

#endif

#ifdef DEMO_13

#include "og.h"
#include "Object.h"
#include "geo/VirtualGlobeCache.h"
#include "geo/layer/LayerFactory.h"




//------------------------------------------------------------------------------

boost::shared_ptr<VirtualGlobeTerrainBlock> block0;
boost::shared_ptr<VirtualGlobeTerrainBlock> block1;
boost::shared_ptr<VirtualGlobeTerrainBlock> block2;
boost::shared_ptr<VirtualGlobeTerrainBlock> block3;

void OnRender(int scene)
{
   int context = ogGetParentObject(scene);
   ogSetBackgroundColor(context, 1,0,0,0);
   //ogDrawWireCube(1.0);  
}

//------------------------------------------------------------------------------

int main(void)
{
   int context = ogCreateRenderWindow("Hello World", 640, 480);
   ogSetRenderFunction(context, &OnRender);

   // Create custom scene:
   // the scene already has a default camera
   int scene = ogCreateScene(context, OG_SCENE_3D_ELLIPSOID_WGS84);
   int world = ogCreateWorld(scene); // Create Virtual Globe

   //int imagelayerBL = ogAddImageLayer(world, "url='G:/i3d/Tiles' layer='BL'");
   int imagelayer = ogAddImageLayer(world, "url='G:/i3d/Tiles' layer='World500'");
   
   //int imagelayer = ogAddImageLayer(world, "url='http://www.openwebglobe.org/data/img' layer='World500'");
   //int failedlayer = ogAddImageLayer(world, "url='http://www.openwebglobe.org/data/img' layer='error'");

   ogExec();
}
#endif
//------------------------------------------------------------------------------

#ifdef DEMO_14

#include "math/GeoCoord.h"
#include "geo/MercatorTransformation.h"

int main(void)
{
   GeoCoord coord(47.56123456789, 7.6123456789, 250);

   double x,y,z;
   coord.ToCartesian(&x,&y,&z);

   std::cout.precision(17);
   std::cout << "lng=" << coord.GetLongitude() << ", lat=" << coord.GetLatitude() << ", elv=" << coord.GetEllipsoidHeight() << "\n";
   std::cout << "cartesian: (" << x << ", " << y << ", " << z << ")\n"; 

   std::cout << "Backwards:\n";
   coord.FromCartesian(x,y,z);
   std::cout << "lng=" << coord.GetLongitude() << ", lat=" << coord.GetLatitude() << ", elv=" << coord.GetEllipsoidHeight() << "\n";

   std::cout << "\nMERCATOR:\n";


   double lng,lat;
   //GeoRef::Mercator::ReverseCustom(-1,1,lng, lat, 0.081819190842961775161887117288255);
   //GeoRef::Mercator::Reverse(-1,1,lng, lat);
   GeoRef::Mercator::ReverseCustom(-1,1,lng, lat, 0);

   std::cout << lng << ", " << lat << "\n";
   return 0;
}

#endif

