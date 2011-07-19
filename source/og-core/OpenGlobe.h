
#ifndef _OPENGLOBE_H
#define _OPENGLOBE_H

#ifndef OPENGLOBE_API
#error ("do not include OpenGlobe.h directly. Always include 'og.h'")
#endif

//------------------------------------------------------------------------------
// *** CALLBACKS ***
//------------------------------------------------------------------------------

typedef void (*CallBack_MouseDown)(int, int, int, int); // context, button, x, y
typedef void (*CallBack_MouseUp)(int, int, int, int);  // context, button, x, y
typedef void (*CallBack_MouseMove)(int, int, int);  // context, x, y
typedef void (*CallBack_MouseWheel)(int, float); // context, delta
typedef void (*CallBack_KeyDown)(int, unsigned int); // context, key
typedef void (*CallBack_KeyUp)(int, unsigned int); // context, key
typedef void (*CallBack_Render)(int);       // scene
typedef void (*CallBack_Resize)(int, int, int);   // context, w, h
typedef void (*CallBack_Timer)(int, float);       // context, delta time [ms]
typedef void (*CallBack_Init)(int);          // context
typedef void (*CallBack_Exit)(int);          // context
typedef void (*CallBack_Object)(int);         // object
typedef void (*CallBack_RenderGeometry)(int, int); // geometry, pass
typedef void (*CallBack_BeginRender)(int, int); // scene, pass
typedef void (*CallBack_EndRender)(int, int); // scene, pass


//------------------------------------------------------------------------------
// *** APPLICATION WIDE FUNCTIONS ***
//------------------------------------------------------------------------------

OPENGLOBE_API void ogSetApplicationName(const char* name);
OPENGLOBE_API const char* ogGetApplicationName();
OPENGLOBE_API const char* ogGetSettingsDirectory();

OPENGLOBE_API void ogSetDiskCacheDirectory(const char* directory);
OPENGLOBE_API void ogSetDiskCacheSize(float size);
OPENGLOBE_API void ogSetMemoryCacheSize(float size);
OPENGLOBE_API void ogSetTileCacheSize(float size);
OPENGLOBE_API void ogSetNumDownloadThreads(int numthrd);
OPENGLOBE_API void ogClearDiskCache();


//------------------------------------------------------------------------------
// *** GENERAL OBJECT FUNCTIONS ***
//------------------------------------------------------------------------------

// Object Types
#define OG_OBJECT_CONTEXT           0
#define OG_OBJECT_SCENE             1
#define OG_OBJECT_WORLD             2
#define OG_OBJECT_IMAGELAYER        3
#define OG_OBJECT_ELEVATIONLAYER    4
#define OG_OBJECT_WAYPOINTLAYER     5
#define OG_OBJECT_POILAYER          6
#define OG_OBJECT_GEOMETRYLAYER     7
#define OG_OBJECT_VOXELLAYER        8
#define OG_OBJECT_IMAGE             9
#define OG_OBJECT_TEXTURE           10
#define OG_OBJECT_PIXELBUFFER       11
#define OG_OBJECT_GEOMETRY          12
#define OG_OBJECT_MESH              13
#define OG_OBJECT_SURFACE           14
#define OG_OBJECT_CAMERA            15
#define OG_OBJECT_TEXT              16
#define OG_OBJECT_BINARYDATA        17
#define OG_OBJECT_LIGHT             18
#define OG_OBJECT_INVALID           65535

//--------------------------
// Constants for ogGetString
#define OG_VENDOR  0
#define OG_VERSION 1
//--------------------------

//--------------------------------
// Constants for ogGetObjectStatus
#define OG_OBJECT_READY    0  
#define OG_OBJECT_BUSY     1
#define OG_OBJECT_FAILED   2
//--------------------------------

//-----------------------------
// Constants for Property Type
#define OG_PROPERTY_STRING    0
#define OG_PROPERTY_INTEGER   1
#define OG_PROPERTY_DOUBLE    2
#define OG_PROPERTY_BOOL      3
#define OG_INVALID            65535
//----------------------------

OPENGLOBE_API unsigned int ogGetObjectType(int object);
OPENGLOBE_API unsigned int ogGetNumObjects();
OPENGLOBE_API int ogGetObjectAt(unsigned int index);
OPENGLOBE_API int ogGetParentObject(int object);
OPENGLOBE_API void ogSetObjectName(int object, const char* name);
OPENGLOBE_API int ogGetObjectByName(const char* name);
OPENGLOBE_API unsigned int ogGetObjectStatus(int object); // NOT YET IMPLEMENTED
OPENGLOBE_API void ogOnLoad(int object, CallBack_Object cbf);
OPENGLOBE_API void ogOnFailure(int object, CallBack_Object cbf);
OPENGLOBE_API void ogSetStringProperty(int object, const char* key, const char* value);
OPENGLOBE_API void ogSetIntegerProperty(int object, const char* key, int value);
OPENGLOBE_API void ogSetDoubleProperty(int object, const char* key, double value);
OPENGLOBE_API void ogSetBoolProperty(int object, const char* key, bool value);

OPENGLOBE_API const char* ogGetStringProperty(int object, const char* key);
OPENGLOBE_API int ogGetIntegerProperty(int object, const char* key);
OPENGLOBE_API double ogGetDoubleProperty(int object, const char* key);
OPENGLOBE_API bool ogGetBoolProperty(int object, const char* key);

OPENGLOBE_API unsigned int ogGetPropertyType(int object, const char* key);
OPENGLOBE_API bool ogHasProperty(int object, const char* key); 
OPENGLOBE_API bool ogPropertyIsReadonly(int object, const char* key); 
OPENGLOBE_API bool ogPropertyIsProtected(int object, const char* key);

OPENGLOBE_API unsigned int ogGetNumProperties(int object);
OPENGLOBE_API const char* ogGetPropertyKey(int object, unsigned int index);
OPENGLOBE_API void ogRemoveProperty(int object, const char* key);
OPENGLOBE_API const char* ogToXML(int object);  

//------------------------------------------------------------------------------
// *** CONTEXT ***
//------------------------------------------------------------------------------

OPENGLOBE_API int ogCreateContext(const char* options , CallBack_Init cbfInit, CallBack_Exit cbfExit, CallBack_Resize cbfResize);
OPENGLOBE_API int ogCreateRenderWindow(const char* title, unsigned int width, unsigned int height);
OPENGLOBE_API void ogDestroyContext(int context);
OPENGLOBE_API int ogGetWidth(int context);
OPENGLOBE_API int ogGetHeight(int context);
OPENGLOBE_API int ogGetScene(int context);
OPENGLOBE_API void ogSetBackgroundColor(int context, float r, float g, float b, float a);
OPENGLOBE_API void ogGetBackgroundColor(int context, float* r, float* g, float* b, float* a);
OPENGLOBE_API void ogSetDrawColor(int context, float r, float g, float b,  float a);
OPENGLOBE_API void ogDrawText(int context, const char* text, float x, float y);
OPENGLOBE_API void ogSetTextColor(int context, float r, float g, float b, float a);
OPENGLOBE_API void ogGetTextSize(int context, char* text, int* width, int* height);
OPENGLOBE_API void ogSetViewport(int context, int x, int y, int width, int height);
OPENGLOBE_API int ogExec();

//Primitives:
OPENGLOBE_API void ogDrawSolidTorus(double dInnerRadius, double dOuterRadius, int nSides, int nRings);
OPENGLOBE_API void ogDrawWireTorus(double dInnerRadius, double dOuterRadius, int nSides, int nRings);
OPENGLOBE_API void ogDrawWireCylinder(double radius, double height, int slices, int stacks);
OPENGLOBE_API void ogDrawSolidCylinder(double radius, double height, int slices, int stacks);
OPENGLOBE_API void ogDrawWireCone(double base, double height, int slices, int stacks);
OPENGLOBE_API void ogDrawSolidCone(double base, double height, int slices, int stacks);
OPENGLOBE_API void ogDrawWireSphere(double radius, int slices, int stacks);
OPENGLOBE_API void ogDrawSolidSphere(double radius, int slices, int stacks);
OPENGLOBE_API void ogDrawSolidCube(double dSize);
OPENGLOBE_API void ogDrawWireCube(double dSize);



//------------------------------------------------------------------------------
// *** SETTING CALLBACK FUNCTIONS ***
//------------------------------------------------------------------------------

OPENGLOBE_API void ogSetMouseDownFunction(int context, CallBack_MouseDown callback);
OPENGLOBE_API void ogSetMouseUpFunction(int context, CallBack_MouseUp callback);
OPENGLOBE_API void ogSetMouseMoveFunction(int context, CallBack_MouseMove callback);
OPENGLOBE_API void ogSetMouseWheelFunction(int context, CallBack_MouseWheel callback);
OPENGLOBE_API void ogSetKeyDownFunction(int context,  CallBack_KeyDown callback);
OPENGLOBE_API void ogSetKeyUpFunction(int context, CallBack_KeyUp callback);
OPENGLOBE_API void ogSetResizeFunction(int context,  CallBack_Resize callback);
OPENGLOBE_API void ogSetRenderFunction(int context,  CallBack_Render callback);
OPENGLOBE_API void ogSetTimerFunction(int context, CallBack_Timer callback);
OPENGLOBE_API void ogSetRenderGeometryFunction(int context, CallBack_RenderGeometry callback);
OPENGLOBE_API void ogSetBeginRenderFunction(int context, CallBack_BeginRender callback);
OPENGLOBE_API void ogSetEndRenderFunction(int context, CallBack_EndRender callback);
OPENGLOBE_API void ogSetNumRenderPasses(int context, int numPasses);

//------------------------------------------------------------------------------
// *** SCENE OBJECT ***
//------------------------------------------------------------------------------

//-----------------------------
// Constants for Scene Object
#define OG_SCENE_3D_ELLIPSOID_WGS84    0
#define OG_SCENE_3D_FLAT_CARTESIAN     1
#define OG_SCENE_2D_SCREEN             2
#define OG_SCENE_CUSTOM                3
//------------------------------

OPENGLOBE_API int ogCreateScene(int context, unsigned int sceneType);
//OPENGLOBE_API int ogLoadScene(int context, const char* url);
//OPENGLOBE_API int ogLoadSceneAsync(int context, const char* url);
OPENGLOBE_API void ogDestroyScene(int scene);
//OPENGLOBE_API void ogSaveScene(int scene, const char* filename);
OPENGLOBE_API int ogGetContext(int scene);
OPENGLOBE_API int ogGetWorld(int scene);
OPENGLOBE_API void ogSetActiveCamera(int scene, int camera);
OPENGLOBE_API int ogGetActiveCamera(int scene);
OPENGLOBE_API unsigned int ogGetNumCameras(int scene);
OPENGLOBE_API int ogGetCameraAt(int scene, unsigned int index);
OPENGLOBE_API int ogPickGeometry(int scene,float mx, float my, double* x, double* y, double* z);
OPENGLOBE_API void ogPickMesh(int scene,float mx, float my, double* x, double* y, double* z, int* meshobject, int* surfaceobject);

// WGS84 Globe
OPENGLOBE_API int ogCreateGlobe(int context);
OPENGLOBE_API int ogPickGlobe(int scene,float mx, float my, double* lng, double* lat, double* elv);
OPENGLOBE_API void ogToCartesian(int scene, double lng, double lat, double elv,double* x,double* y,double* z);
OPENGLOBE_API void ogToWGS84(int scene, double x, double y, double z, double* lng,double* lat,double* elv);
OPENGLOBE_API void ogSetPositionWGS84(int camera, double longitude, double latitude, double elv);
OPENGLOBE_API void ogSetOrientationWGS84(int camera, double yaw, double pitch, double roll);
OPENGLOBE_API void ogLookAtWGS84(int camera, double longitude, double latitude, double elv);


// Flat Earth
OPENGLOBE_API int ogPickTerrain(int scene,float mx, float my, double* x, double* y, double* z);

// --------------
// *** CAMERA ***
// --------------

OPENGLOBE_API int ogCreateCamera(int scene);
OPENGLOBE_API int ogLoadCamera(int scene, const char* url);
OPENGLOBE_API int ogLoadCameraAsync(int scene , const char* url);
OPENGLOBE_API void ogDestroyCamera(int camera);
OPENGLOBE_API int ogGetNavigationController(int camera);
OPENGLOBE_API void ogSaveCamera(int camera, const char* file);
OPENGLOBE_API void ogSetProjectionMatrixd(int camera, double* m);
OPENGLOBE_API void ogSetProjectionMatrixf(int camera, float* m);
OPENGLOBE_API void ogCreatePerspectiveProjection(int camera, float fovy, float zNear, float zFar);
OPENGLOBE_API void ogCreateOrtho2D(int camera, float left, float right, float bottom, float top); 
OPENGLOBE_API void ogSetViewMatrixd(int camera, double* m);
OPENGLOBE_API void ogSetViewMatrixf(int camera, float* m);
OPENGLOBE_API void ogLookAt(int camera, double eyeX, double eyeY, double eyeZ, double centerX, double centerY, double centerZ, double upX, double upY, double upZ); 

// -----------------------------
// *** NAVIGATION CONTROLLER ***
// -----------------------------

typedef void (*CallBack_NavigationInit)(int);	                     // (camera)
typedef void (*CallBack_NavigationExit)(int);	                     // (camera)
typedef void (*CallBack_NavigationMouseDown)(int, int, int, int);   	// (camera, button, mouseX, mouseY)
typedef void (*CallBack_NavigationMouseUp)(int, int, int, int);	   // (camera, button, mouseX, mouseY)
typedef void (*CallBack_NavigationMouseMove)(int, int, int);	      // (camera, mouseX, mouseY)
typedef void (*CallBack_NavigationMouseWheel)(int, float);	         // (camera, delta)
typedef void (*CallBack_NavigationKeyDown)(int, unsigned int);	      // (camera, key)
typedef void (*CallBack_NavigationKeyUp)(int, unsigned int);	      // (camera, key)
typedef void (*CallBack_NavigationPreRender)(int);       	         // (camera, scene)
typedef void (*CallBack_NavigationPostRender)(int);       	         // (camera, scene)
typedef void (*CallBack_NavigationResize)(int, int, int);   	      // (camera, width, height)
typedef void (*CallBack_NavigationTimer)(int, float);     	         // (camera, delta_time)
typedef void (*CallBack_NavigationCommand)(int, const char*);     	// (camera, command)

int ogCreateNavigationController(int camera, unsigned int type);
void SendNavigationCommand(int navigation, const char* command);

OPENGLOBE_API void ogSetNavigationEventInit(int nav, CallBack_NavigationInit cbf);
OPENGLOBE_API void ogSetNavigationEventExit(int nav, CallBack_NavigationExit cbf);
OPENGLOBE_API void ogSetNavigationEventMouseDown(int nav, CallBack_NavigationMouseDown cbf);
OPENGLOBE_API void ogSetNavigationEventMouseUp(int nav, CallBack_NavigationMouseUp cbf);
OPENGLOBE_API void ogSetNavigationEventMouseMove (int nav, CallBack_NavigationMouseMove cbf);
OPENGLOBE_API void ogSetNavigationEventMouseWheel (int nav, CallBack_NavigationMouseWheel cbf);
OPENGLOBE_API void ogSetNavigationEventKeyDown (int nav, CallBack_NavigationKeyDown cbf);
OPENGLOBE_API void ogSetNavigationEventKeyUp(int nav, CallBack_NavigationKeyUp cbf);
OPENGLOBE_API void ogSetNavigationEventPreRender (int nav, CallBack_NavigationPreRender cbf);
OPENGLOBE_API void ogSetNavigationEventPostRender(int nav, CallBack_NavigationPostRender cbf);
OPENGLOBE_API void ogSetNavigationEventResize(int nav, CallBack_NavigationResize cbf);
OPENGLOBE_API void ogSetNavigationEventTimer(int nav, CallBack_NavigationTimer cbf);
OPENGLOBE_API void ogSetNavigationEventCommand (int nav, CallBack_NavigationCommand cbf);

//------------------------------------------------------------------------------
// *** WORLD OBJECT ***
//------------------------------------------------------------------------------

OPENGLOBE_API int ogCreateWorld(int scene);
OPENGLOBE_API void ogDestroyWorld(int world);
OPENGLOBE_API void ogShowWorld(int world);
OPENGLOBE_API void ogHideWorld(int world);
OPENGLOBE_API void ogRenderWorld(int world);

// Image Layer:
OPENGLOBE_API int ogAddImageLayer(int world, const char* layeroptions);
OPENGLOBE_API void ogRemoveImageLayer(int layer);


//------------------------------------------------------------------------------
// *** IMAGE OBJECT ***
//------------------------------------------------------------------------------

//-----------------------------
// Constants for Image Object
//----------------------------

OPENGLOBE_API int ogCreateImage(int scene, int width, int height, unsigned int pixelformat);
OPENGLOBE_API int ogLoadImage(int scene, const char* url);
OPENGLOBE_API int ogLoadImageAsync(int scene, const char* url);
OPENGLOBE_API int ogDestroyImage(int image);
OPENGLOBE_API void ogSaveImage(int image, const char* filename, const char* formatoptions);
OPENGLOBE_API int ogGetImageWidth(int image);
OPENGLOBE_API int ogGetImageHeight(int image);
OPENGLOBE_API unsigned int ogGetImagePixelFormat(int image);
OPENGLOBE_API void ogAddAlphaChannel(int image);
OPENGLOBE_API int ogResizeImage(int image, int newwidth, int newheight);
OPENGLOBE_API unsigned char* ogLockImage(int image);
OPENGLOBE_API void ogUnlockImage(int image);

//------------------------------------------------------------------------------
// *** TEXTURE OBJECT ***
//------------------------------------------------------------------------------

#define OG_TEXTURE_2D (0)         
#define OG_TEXTURE_RECTANGLE (1)

#define OG_PIXELFORMAT_RED (0)
#define OG_PIXELFORMAT_GREEN (1)
#define OG_PIXELFORMAT_BLUE (2)
#define OG_PIXELFORMAT_ALPHA (3)
#define OG_PIXELFORMAT_RGB (4)
#define OG_PIXELFORMAT_BGR (5)
#define OG_PIXELFORMAT_RGBA (6)
#define OG_PIXELFORMAT_BGRA (7)
#define OG_PIXELFORMAT_LUMINANCE (8)
#define OG_PIXELFORMAT_LUMINANCE_ALPHA (9)

#define OG_TEXTUREDATATYPE_UNSIGNED_BYTE (0)
#define OG_TEXTUREDATATYPE_FLOAT (1)

OPENGLOBE_API int ogLoadTextureAsync(int scene, const char* url, const char* imageformat, unsigned int pixelformat);
OPENGLOBE_API void ogBlitTexture(int texture, int x, int y); // Blit Texture on screen (x,y screen coords)
OPENGLOBE_API void ogDestroyTexture(int texture);


//OPENGLOBE_API int ogCreateTextureFromImage(int scene, int image);
//OPENGLOBE_API int ogCreateTexture(int scene, unsigned int target, int width, int height, unsigned int pixelformat, unsigned int datatype); 

//OPENGLOBE_API void ogGetMaxTextureSize(int* w, int* h);
//OPENGLOBE_API void ogUpdateTexture(int texture, int xoffset, int yoffset, int width, int height, unsigned char* data);


//------------------------------------------------------------------------------
// *** MISC ***
//------------------------------------------------------------------------------

OPENGLOBE_API void ogMemoryDump(const char* file); // NOT YET IMPLEMENTED
OPENGLOBE_API const char* ogGetLastError(); // NOT YET IMPLEMENTED

OPENGLOBE_API const char* ogGetString(unsigned int type);
OPENGLOBE_API void ogInfo();

OPENGLOBE_API float ogRand01();  // Return a random number in range [0,1]





#endif