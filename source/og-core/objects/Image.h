 
#ifndef _IMAGE_H
#define _IMAGE_H

#include "Object.h"
#include "Scene.h"
#include "image/ImageHandler.h"
#include "image/ImageLoader.h"
#include <list>


namespace OpenGlobe
{
   class OPENGLOBE_API Image : public OpenGlobe::Object
   {
   public:
      Image();
      virtual ~Image();

      virtual void SetOptions(const OptionVector& options);

      bool LoadImage(const std::string& url);
      void CreateImage(int width, int height, Img::PixelFormat format);

      // get width of image:
      int GetWidth();

      // get height of image:
      int GetHeight();

      // Returns pixelformat
      Img::PixelFormat GetPixelFormat();

      // Get Number of channels (3 for RGB or 4 for RGBA)
      int GetNumChannels();

      // Exclusive lock. Get Raw Data of image. You can access pixel data. Make sure returned data is not 0.
      unsigned char* LockImage();

      // Call unlock when finished accessing pixel data
      void           UnlockImage();

      // Add an alpha channel
      //void           AddAlphaChannel();

      

      // save image:
      bool SaveImage(const std::string& filename, Img::FileFormat format);


   protected:
      ImageObject             _Image;
      Scene*                  _parentScene;
      bool                    _bLock;
   };

}

#endif
