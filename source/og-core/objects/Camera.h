
#ifndef _CAMERA_H
#define _CAMERA_H

#include "Object.h"
#include "Scene.h"
#include "NavigationController.h"
#include <list>

namespace OpenGlobe
{
   class OPENGLOBE_API Context;
 
   class OPENGLOBE_API Camera : public OpenGlobe::Object
   {
   public:
      Camera();
      virtual ~Camera();

      virtual void SetOptions(const OptionVector& options);

      NavigationController* GetNavigationController() { return _pController;}

      // PROJECTION MATRIX

      // create a 2D orthographic projection matrix.
      void CreateOrtho2D(double left, double right, double bottom, double top);

      // create perspective projection matrix
      void CreatePerspectiveProjection(double fovY, double zNear, double zFar);


      // VIEW MATRIX

      // create a lookat view matrix
      void CreateLookAt(double eyeX,    double eyeY,    double eyeZ, 
                        double centerX, double centerY, double centerZ,
                        double upX,     double upY,     double upZ);

      // Set projection matrix manually.
      void SetProjectionMatrix(const mat4<double>& matProjection);
      
      // Set view matrix manually.
      void SetViewMatrix(const mat4<double>& matView);

      // Set position on WGS84 ellipsoid (WGS84 scene only)
      void SetPositionWGS84(double longitude, double latitude, double elv);

      // Set orientation on WGS84 ellipsoid (WGS84 scene only)
      void SetOrientationWGS84(double yaw, double pitch, double roll);

   protected:
      Scene*                  _parentScene;
      Context*                _pContext;
      NavigationController*   _pController;
   };

}

#endif
