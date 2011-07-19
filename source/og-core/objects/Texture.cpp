   
#include "Texture.h"


//---------------------------------------------------------------------------
using namespace OpenGlobe;
BeginPropertyMap(TextureObject);
   Inherits(Object);
   //XMLPropertyReadOnly(Scene, "width", _width);
EndPropertyMap(TextureObject);
//---------------------------------------------------------------------------

namespace OpenGlobe
{
   //---------------------------------------------------------------------------
   TextureObject::TextureObject()
      : _parentScene(0), _pTexture(0)
   {
      _name = "ogTexture";
      _eObjectType = Object::TEXTURE;
      _eObjectStatus = Object::BUSY;

   }
   //---------------------------------------------------------------------------
   TextureObject::~TextureObject()
   {
      Destroy();
   }
   //---------------------------------------------------------------------------
   void TextureObject::SetOptions(const OptionVector& options)
   {
      _parentScene = dynamic_cast<Scene*>(_pParent);


   }
   //---------------------------------------------------------------------------

   void _tex_OnImageReady(ImageObject* pImage, void* userdata)
   {
      if (pImage)
      {
         TextureObject* texobj = (TextureObject*) userdata;
         texobj->CreateTextureFromImage(pImage);
         delete pImage;
      }
   }

   void _tex_OnImageFailed(void* userdata)
   {
      // called when image failed to load
   }


   void TextureObject::LoadTextureAsync(const std::string& url, Img::FileFormat fileformat, Texture::PixelFormat pixelformat)
   {
      Img::PixelFormat pf = Img::PixelFormat_RGBA;

      switch (pixelformat)
      {
         case  Texture::GFX_RED:
         assert(false); // currently unsupported
         break;
         case  Texture::GFX_GREEN:
         assert(false); // currently unsupported
         break;
         case  Texture::GFX_BLUE:
         assert(false); // currently unsupported
         break;
         case  Texture::GFX_ALPHA:
         assert(false); // currently unsupported
         break;
         case  Texture::GFX_RGB:
         pf = Img::PixelFormat_RGB;
         break;
         case  Texture::GFX_BGR:
         pf = Img::PixelFormat_BGR;
         break;
         case  Texture::GFX_RGBA:
         pf = Img::PixelFormat_RGBA;
         break;
         case  Texture::GFX_BGRA:
         pf = Img::PixelFormat_BGRA;
         break;
         case  Texture::GFX_LUMINANCE:
         assert(false); // currently unsupported
         break;
         case  Texture::GFX_LUMINANCE_ALPHA:
         assert(false); // currently unsupported
         break;
         default:
         assert(false); // unknown -> using RGBA
         break;
      }

      void* userdata = (void*) this;
      ImageLoader::LoadFromUrl(Img::Format_PNG, url, pf, &_tex_OnImageReady, &_tex_OnImageFailed, userdata);

   }

   //---------------------------------------------------------------------------

   void TextureObject::CreateTextureFromImage(ImageObject* pImage)
   {
       Img::PixelFormat pf = pImage->GetPixelFormat();
       Texture::PixelFormat tf = Texture::GFX_RGBA;

       switch(pf)
       {
          case Img::PixelFormat_RGB:
            tf = Texture::GFX_RGB;
          break;
          case Img::PixelFormat_RGBA:
            tf = Texture::GFX_RGBA;
          break;
          case Img::PixelFormat_BGR:
            tf = Texture::GFX_BGR;
          break;
          case Img::PixelFormat_BGRA:
            tf = Texture::GFX_BGRA;
          break;
          default:
            assert(false);
            _eObjectStatus = Object::FAILED;
            return;
       }


      Texture::Target target;

      if (math::IsPower2(pImage->GetWidth()) && math::IsPower2(pImage->GetHeight()))
      {
         target = Texture::GFX_TEXTURE_2D;
      }
      else
      {
         target = Texture::GFX_TEXTURE_RECTANGLE;
      }

      if (_parentScene)
      {
         OpenGlobe::Context* pContext = _parentScene->GetContext();
         if (pContext)
         {
            IRenderEngine* pEngine = pContext->GetRenderEngine();
            if (pEngine)
            {
               _pTexture = pEngine->CreateTexture();

               _pTexture->CreateTexture(target, 
                                        pImage->GetWidth(), 
                                        pImage->GetHeight(),
                                        tf,
                                        Texture::GFX_UNSIGNED_BYTE,
                                        pImage->GetRawData().get());

               _eObjectStatus = Object::READY;
               return;
            }
         }
      }

      _eObjectStatus = Object::FAILED;
   }

   //---------------------------------------------------------------------------

   void TextureObject::BlitTexture(int x, int y, double z, double angle, double scalex, double scaley, bool blend)
   {
      if (GetObjectStatus() == Object::READY && _pTexture)
      {
         if (_parentScene)
         {
            OpenGlobe::Context* pContext = _parentScene->GetContext();
            if (pContext)
            {
               IRenderEngine* pEngine = pContext->GetRenderEngine();
               if (pEngine)
               {
                  pEngine->BlitTexture(_pTexture, x,y,z,angle,scalex, scaley, blend);
               }
            }
         }
      }
   }

   //---------------------------------------------------------------------------

   void TextureObject::Destroy()
   {
      if (_pTexture)
      {
         OpenGlobe::Context* pContext = _parentScene->GetContext();
         if (pContext)
         {
            IRenderEngine* pEngine = pContext->GetRenderEngine();
            if (pEngine)
            {
                pEngine->Destroy(_pTexture);
                _pTexture = 0;
            }
         }
      }
   }

   //---------------------------------------------------------------------------
}