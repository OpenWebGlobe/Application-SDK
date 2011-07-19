
#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "Object.h"
#include "Scene.h"
#include "renderer/ITexture.h"
#include "image/ImageLoader.h"
#include <list>


namespace OpenGlobe
{
   class OPENGLOBE_API TextureObject : public OpenGlobe::Object
   {
   public:
      TextureObject();
      virtual ~TextureObject();

      // load texture from url
      void LoadTextureAsync(const std::string& url, Img::FileFormat fileformat=Img::Format_PNG, Texture::PixelFormat pixelformat=Texture::GFX_RGBA);

      // Blit Texture to screen
      void BlitTexture(int x, int y, double z=0, double angle=0, double scalex=1.0, double scaley=1.0, bool blend=false);

      // Destroy Texture (free GPU memory)
      void Destroy();

      //
      virtual void SetOptions(const OptionVector& options);

      void CreateTextureFromImage(ImageObject* pImage);
      ITexture*               GetTexture() {return _pTexture;}

   protected:
      Scene*                  _parentScene;
      ITexture*               _pTexture;
   };

}

#endif
