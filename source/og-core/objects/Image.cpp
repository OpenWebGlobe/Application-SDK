   
#include "Image.h"



//---------------------------------------------------------------------------
using namespace OpenGlobe;
BeginPropertyMap(Image);
   Inherits(Object);
   //XMLPropertyReadOnly(Scene, "width", _width);
EndPropertyMap(Image);
//---------------------------------------------------------------------------

namespace OpenGlobe
{
   //---------------------------------------------------------------------------
   Image::Image()
      : _parentScene(0), _bLock(false)
   {
      _name = "ogImage";
      _eObjectType = Object::IMAGE;

   }
   //---------------------------------------------------------------------------
   Image::~Image()
   {

   }
   //---------------------------------------------------------------------------
   void Image::SetOptions(const OptionVector& options)
   {
      _parentScene = dynamic_cast<Scene*>(_pParent);
   }
   //---------------------------------------------------------------------------
   int Image::GetWidth()
   {
      return (int)_Image.GetWidth();
   }
   //---------------------------------------------------------------------------
   int Image::GetHeight()
   {
      return (int)_Image.GetHeight();
   }
   //---------------------------------------------------------------------------
   Img::PixelFormat Image::GetPixelFormat()
   {
      return _Image.GetPixelFormat();
   }
   //---------------------------------------------------------------------------
   int Image::GetNumChannels()
   {
      switch (_Image.GetPixelFormat())
      {
         case Img::PixelFormat_RGB:
         case Img::PixelFormat_BGR:
            return 3;
         case Img::PixelFormat_RGBA:
         case Img::PixelFormat_BGRA:
            return 4;
      }

      return 0; // error...
   }
   //---------------------------------------------------------------------------
   unsigned char* Image::LockImage()
   {
      if (_bLock)
      {
         assert(false);
         return 0; // already locked!!
      }

      _bLock = true;
      return _Image.GetRawData().get();
   }
   //---------------------------------------------------------------------------
   void Image::UnlockImage()
   {
      if (!_bLock)
      {
         assert(false); // image is not locked but you try to unlock!
      }

      _bLock = false;
   }
   //---------------------------------------------------------------------------
   bool SaveImage(const std::string& filename, Img::FileFormat format)
   {
      return false;
   }

}