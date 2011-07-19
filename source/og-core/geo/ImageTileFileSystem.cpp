/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "geo/ImageTileFileSystem.h"
#include "MercatorQuadtree.h"
#include "math/mathutils.h"
#include "string/StringUtils.h"
#include "string/FilenameUtils.h"
#include "io/CommonPath.h"

#include <fstream>
#include <sstream>

#ifdef DEPRECATED_FUNCTION
//-----------------------------------------------------------------------------

ImageTileFileSystemReader::ImageTileFileSystemReader()
{
   _bGood = false;
   _bWeb = false;
   _pDatasetinfo = 0;
   _eTileFormat = Img::Format_PNG;
   _bWebService = false;
   _eTileService = TS_UNKNOWN;
   _dTransparency = 1.0;
   _nMaxLod = 18;
}

//-----------------------------------------------------------------------------

ImageTileFileSystemReader::~ImageTileFileSystemReader()
{
   
}

//-----------------------------------------------------------------------------

bool ImageTileFileSystemReader::Open(const std::wstring sPath, const std::wstring sName, double dTransparency, int nMaxLod)
{
   _dTransparency = dTransparency;
   _nMaxLod = nMaxLod;
   _sPath = FilenameUtils::UnifyPathDelimiter(sPath);
   _sPath = FilenameUtils::DelimitPath(_sPath);
   _sPath = CommonPath::ConvertWildcard(_sPath);
   _sName = sName;
   _eTileService = TS_UNKNOWN;

   _bWebService = false;

   if (_sName.length()>7 && StringUtils::Left(_sName, 7) == L"service")
   {
      if (_sName == L"service.openstreetmap" || _sName == L"service.osm")
      {
            _bWebService = true;
            _bGood = true;
            _pDatasetinfo = 0;

            // Spherical Quadtree for OSM
            _qQuadtreeSphere = boost::shared_ptr<MercatorQuadtree>(new MercatorQuadtree());

            _eTileService = TS_OPENSTREETMAP;
            return true;
      }
      else if (_sName == L"service.bing")
      {
         _bWebService = true;
         _bGood = true;
         _pDatasetinfo = 0;

         // Spherical Quadtree for OSM
         _qQuadtreeSphere = boost::shared_ptr<MercatorQuadtree>(new MercatorQuadtree());

         _eTileService = TS_BING;
         return true;
      }
      else
      {
         return false; // unknown service!
      }
   }
   else
   {
      if (FilenameUtils::IsFileOnWeb(_sPath))
      {
         _bWeb = true;
      }

      _qReader = boost::shared_ptr<IFileSystemReader>(new FileSystemReaderDisk(_sPath+_sName));
      
      if (_qReader)
      {
         _bGood = true;
         // Load dataset info
         LoadDatasetInfo();
      }
      return _bGood;
   }
}

//-----------------------------------------------------------------------------

void ImageTileFileSystemReader::Close()
{

}

//-----------------------------------------------------------------------------

namespace internal
{
   inline void ReadBufferRBGA(unsigned char* buffer, unsigned int nWidth, unsigned int nHeight, int x, int y, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a)
   {
      if (x<0) x = 0;
      if (y<0) y = 0;
      if (x>(int)nWidth-1) x = (int)nWidth-1;
      if (y>(int)nWidth-1) y = (int)nWidth-1;

      *r = buffer[nWidth*4*y+4*x];
      *g = buffer[nWidth*4*y+4*x+1];
      *b = buffer[nWidth*4*y+4*x+2];
      *a = buffer[nWidth*4*y+4*x+3];
   }

   inline void ReadBufferRBG(unsigned char* buffer, unsigned int nWidth, unsigned int nHeight, int x, int y, unsigned char* r, unsigned char* g, unsigned char* b)
   {
      if (x<0) x = 0;
      if (y<0) y = 0;
      if (x>(int)nWidth-1) x = (int)nWidth-1;
      if (y>(int)nWidth-1) y = (int)nWidth-1;

      *r = buffer[nWidth*3*y+3*x];
      *g = buffer[nWidth*3*y+3*x+1];
      *b = buffer[nWidth*3*y+3*x+2];
   }
 
}



void ImageTileFileSystemReader::_ReadImageValueBilinearRGBA(unsigned char* buffer, unsigned int nWidth, unsigned int nHeight, double x, double y, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a)
{
   double uf = math::Fract<double>(x);
   double vf = math::Fract<double>(y);
   int nPixelX = int(x);
   int nPixelY = int(y);

   int u00,v00,u10,v10,u01,v01,u11,v11;
   u00 = nPixelX;
   v00 = nPixelY;
   u10 = nPixelX+1;
   v10 = nPixelY;
   u01 = nPixelX;
   v01 = nPixelY+1;
   u11 = nPixelX+1;
   v11 = nPixelY+1;

   unsigned char r00,g00,b00,a00;
   unsigned char r10,g10,b10,a10;
   unsigned char r01,g01,b01,a01;
   unsigned char r11,g11,b11,a11;

   internal::ReadBufferRBGA(buffer, nWidth, nHeight,u00,v00,&r00,&g00,&b00,&a00);
   internal::ReadBufferRBGA(buffer, nWidth, nHeight,u10,v10,&r10,&g10,&b10,&a10);
   internal::ReadBufferRBGA(buffer, nWidth, nHeight,u01,v01,&r01,&g01,&b01,&a01);
   internal::ReadBufferRBGA(buffer, nWidth, nHeight,u11,v11,&r11,&g11,&b11,&a11);

   double rd, gd, bd, ad;

   rd = (double(r00)*(1-uf)*(1-vf)+double(r10)*uf*(1-vf)+double(r01)*(1-uf)*vf+double(r11)*uf*vf)+0.5;
   gd = (double(g00)*(1-uf)*(1-vf)+double(g10)*uf*(1-vf)+double(g01)*(1-uf)*vf+double(g11)*uf*vf)+0.5;
   bd = (double(b00)*(1-uf)*(1-vf)+double(b10)*uf*(1-vf)+double(b01)*(1-uf)*vf+double(b11)*uf*vf)+0.5;
   ad = (double(a00)*(1-uf)*(1-vf)+double(a10)*uf*(1-vf)+double(a01)*(1-uf)*vf+double(a11)*uf*vf)+0.5;

   rd = math::Clamp<double>(rd, 0.0, 255.0);
   gd = math::Clamp<double>(gd, 0.0, 255.0);
   bd = math::Clamp<double>(bd, 0.0, 255.0);
   ad = math::Clamp<double>(ad, 0.0, 255.0);

   *r = (unsigned char) rd;
   *g = (unsigned char) gd;
   *b = (unsigned char) bd;
   *a = (unsigned char) ad;
}

//-----------------------------------------------------------------------------

void ImageTileFileSystemReader::_ReadImageValueBilinearRGB(unsigned char* buffer, unsigned int nWidth, unsigned int nHeight, double x, double y, unsigned char* r, unsigned char* g, unsigned char* b)
{
   double uf = math::Fract<double>(x);
   double vf = math::Fract<double>(y);
   int nPixelX = int(x);
   int nPixelY = int(y);

   int u00,v00,u10,v10,u01,v01,u11,v11;
   u00 = nPixelX;
   v00 = nPixelY;
   u10 = nPixelX+1;
   v10 = nPixelY;
   u01 = nPixelX;
   v01 = nPixelY+1;
   u11 = nPixelX+1;
   v11 = nPixelY+1;

   unsigned char r00,g00,b00;
   unsigned char r10,g10,b10;
   unsigned char r01,g01,b01;
   unsigned char r11,g11,b11;

   internal::ReadBufferRBG(buffer, nWidth, nHeight,u00,v00,&r00,&g00,&b00);
   internal::ReadBufferRBG(buffer, nWidth, nHeight,u10,v10,&r10,&g10,&b10);
   internal::ReadBufferRBG(buffer, nWidth, nHeight,u01,v01,&r01,&g01,&b01);
   internal::ReadBufferRBG(buffer, nWidth, nHeight,u11,v11,&r11,&g11,&b11);

   double rd, gd, bd;

   rd = (double(r00)*(1-uf)*(1-vf)+double(r10)*uf*(1-vf)+double(r01)*(1-uf)*vf+double(r11)*uf*vf)+0.5;
   gd = (double(g00)*(1-uf)*(1-vf)+double(g10)*uf*(1-vf)+double(g01)*(1-uf)*vf+double(g11)*uf*vf)+0.5;
   bd = (double(b00)*(1-uf)*(1-vf)+double(b10)*uf*(1-vf)+double(b01)*(1-uf)*vf+double(b11)*uf*vf)+0.5;
 
   rd = math::Clamp<double>(rd, 0.0, 255.0);
   gd = math::Clamp<double>(gd, 0.0, 255.0);
   bd = math::Clamp<double>(bd, 0.0, 255.0);
 
   *r = (unsigned char) rd;
   *g = (unsigned char) gd;
   *b = (unsigned char) bd;
}

//-----------------------------------------------------------------------------

bool ImageTileFileSystemReader::_InterpolateFromHigher(std::string& sQuadcode, int nLod, boost::shared_array<unsigned char>& out_qImageDataRGBA, unsigned int& out_nTilesize, PixelFormat& out_PixelFormat)
{
   std::string sQuadcodeParent = StringUtils::Left(sQuadcode, nLod);

   double x0_r, y0_r, x1_r, y1_r; // requested coord
   double x0_a, y0_a, x1_a, y1_a; // available area
   MercatorQuadtree::QuadKeyToMercatorCoord(sQuadcode, x0_r, y0_r, x1_r, y1_r);
   MercatorQuadtree::QuadKeyToMercatorCoord(sQuadcodeParent, x0_a, y0_a, x1_a, y1_a);


   boost::shared_array<unsigned char> qParentImageDataRGBA;
   unsigned int nParentTilesize;
   PixelFormat nParentPixelFormat;

   if (ReadTile(sQuadcodeParent, qParentImageDataRGBA, nParentTilesize, nParentPixelFormat))
   {
      if (nParentPixelFormat == PF_RGBA)
      {
         boost::shared_array<unsigned char> qImageData;
         qImageData = boost::shared_array<unsigned char>(new unsigned char[nParentTilesize*nParentTilesize*4]);


         unsigned char r,g,b,a;
         for (size_t y=0;y<nParentTilesize;y++)
         {
            for (size_t x=0;x<nParentTilesize;x++)
            {
               double xx = x0_r + (x1_r-x0_r)/(double(nParentTilesize-1))*x;
               double yy = y0_r + (y1_r-y0_r)/(double(nParentTilesize-1))*(nParentTilesize-1-y);

               double xpos = (double(xx)-x0_a) * double(nParentTilesize)/(x1_a-x0_a);
               double ypos = double(nParentTilesize) - (double(yy)-y0_a) * double(nParentTilesize)/(y1_a-y0_a);

               _ReadImageValueBilinearRGBA(qParentImageDataRGBA.get(), nParentTilesize, nParentTilesize,
                  xpos, ypos, &r,&g,&b,&a);

               if (_dTransparency < 1.0)  
               {
                  a = unsigned char((double)a * _dTransparency);
               }

               size_t srcadr = 4*y*nParentTilesize+4*x;
               qImageData.get()[srcadr+0] = r;
               qImageData.get()[srcadr+1] = g;
               qImageData.get()[srcadr+2] = b;
               qImageData.get()[srcadr+3] = a;
            }
         }

         out_nTilesize = nParentTilesize;
         out_qImageDataRGBA = qImageData;
         out_PixelFormat = PF_RGBA;

         return true;
      }
      else if (nParentPixelFormat == PF_RGB)
      {
         boost::shared_array<unsigned char> qImageData;
         qImageData = boost::shared_array<unsigned char>(new unsigned char[nParentTilesize*nParentTilesize*3]);

         unsigned char r,g,b;
         for (size_t y=0;y<nParentTilesize;y++)
         {
            for (size_t x=0;x<nParentTilesize;x++)
            {
               double xx = x0_r + (x1_r-x0_r)/(double(nParentTilesize-1))*x;
               double yy = y0_r + (y1_r-y0_r)/(double(nParentTilesize-1))*(nParentTilesize-1-y);

               double xpos = (double(xx)-x0_a) * double(nParentTilesize)/(x1_a-x0_a);
               double ypos = double(nParentTilesize) - (double(yy)-y0_a) * double(nParentTilesize)/(y1_a-y0_a);

               _ReadImageValueBilinearRGB(qParentImageDataRGBA.get(), nParentTilesize, nParentTilesize,
                  xpos, ypos, &r,&g,&b);

               size_t srcadr = 3*y*nParentTilesize+3*x;
               qImageData.get()[srcadr+0] = r;
               qImageData.get()[srcadr+1] = g;
               qImageData.get()[srcadr+2] = b;
            }
         }


         unsigned int a = unsigned char(255.0 * _dTransparency);
         boost::shared_array<unsigned char> qFinImageData = boost::shared_array<unsigned char>(new unsigned char[nParentTilesize*nParentTilesize*4]);

         for (size_t y=0;y<nParentTilesize;y++)
         {
            for (size_t x=0;x<nParentTilesize;x++)
            {
               size_t dstadr = 4*y*nParentTilesize+4*x;
               size_t srcadr = 3*y*nParentTilesize+3*x;

               qFinImageData.get()[dstadr+0] = qImageData.get()[srcadr+0];
               qFinImageData.get()[dstadr+1] = qImageData.get()[srcadr+1];
               qFinImageData.get()[dstadr+2] = qImageData.get()[srcadr+2];
               qFinImageData.get()[dstadr+3] = a;

            }
         }

         out_nTilesize = nParentTilesize;
         out_qImageDataRGBA = qFinImageData;
         out_PixelFormat = PF_RGBA;

         return true;
      }

   }
   else
   {
      return false;
   }

   return false;
}



bool ImageTileFileSystemReader::ReadTile(std::string sQuadcode, boost::shared_array<unsigned char>& out_qImageDataRGBA, unsigned int& out_nTilesize, PixelFormat& out_PixelFormat)
{
   if (_bWebService)
   {
      switch (_eTileService)
      {
      case TS_UNKNOWN:
         assert(false);
         return false;
      case TS_OPENSTREETMAP:
         return _ReadTileServiceOpenStreetMap(sQuadcode, out_qImageDataRGBA, out_nTilesize, out_PixelFormat);
      case TS_BING:
         return _ReadTileServiceBingMaps(sQuadcode, out_qImageDataRGBA, out_nTilesize, out_PixelFormat);
      default:
         assert(false);
      }
   }

   if (!_bGood)
      return false;

   std::wstring sFileExtension;
   std::wstring sFilename(sQuadcode.begin(), sQuadcode.end());

   switch (_eTileFormat)
   {
   case Img::Format_PNG:
      sFileExtension = L".png";
      break;
   case Img::Format_JPG:
      sFileExtension = L".jpg";
      break;
   default:
      assert(false); // unsupported format!!
      return false;
   }

   sFilename = sFilename + sFileExtension;

   if (!_pDatasetinfo->IsFlat())
   {
      sFilename = FilenameUtils::MakeHierarchicalFileName(sFilename, 2);
   }


   unsigned int nRequestedLod = sQuadcode.length();
   unsigned int nLod = _pDatasetinfo->GetLevelOfDetail();

   // if requested lod is higher than available lod, then data must be
   // loaded from maximum resolution of this dataset:
   if (nRequestedLod>nLod)
   {
      return _InterpolateFromHigher(sQuadcode, nLod, out_qImageDataRGBA, out_nTilesize, out_PixelFormat);
   }
   else
   {
      boost::shared_array<unsigned char> qImageDataCompressed;
      size_t nImageSizeCompressed = 0;

      _qReader->ReadData(sFilename, qImageDataCompressed, nImageSizeCompressed);

      Img::ImageHandler IH;
      if (qImageDataCompressed && IH.LoadImageFromMemory(_eTileFormat, qImageDataCompressed, nImageSizeCompressed, true, false))
      {
         if (IH.GetWidth() == IH.GetHeight())
         {
            out_nTilesize = IH.GetWidth();
            out_qImageDataRGBA = IH.GetImageData();
            out_PixelFormat = IH.GetPixelFormat();
            //std::cout << "...[OK]\n";
            return true;
         }
      }

   }
   return false;
}

//-----------------------------------------------------------------------------

bool ImageTileFileSystemReader::IsOnWeb()
{
   return _bWeb;
}

//-----------------------------------------------------------------------------

std::string ImageTileFileSystemReader::GetDatasetInfoPath()
{
   std::string sFile = _sPath + _sName + "_meta.xml";
   return sFile;
}

//-----------------------------------------------------------------------------

void ImageTileFileSystemReader::LoadDatasetInfo()
{
/*
   std::string sOut;
   _pDatasetinfo = 0;

   std::wstring sURL = GetDatasetInfoPath(); 
   if (FilenameUtils::IsFileOnWeb(sURL))
   {
      if (FileSystem::DownloadURL(sURL, 0))
      {
         if (!FileSystem::FindInternetCache(sURL, sOut))
         {
            //Unable to find downloaded file in cache
            return;
         }
      }
      else
      {
         //Unable to download file
         return;
      }

      sOutAscii = std::string(sOut.begin(), sOut.end());
   }
   else
   {
      sOutAscii = std::string(sURL.begin(), sURL.end());
   }

   std::ifstream fXML;
#ifdef OS_WINDOWS
   fXML.open(sURL.c_str());
#else   
   fXML.open(sOutAscii.c_str());
#endif
   _qDatasetInfo = boost::shared_ptr<Object>(ObjectFactory::Deserialize(fXML));
   fXML.close();

   _pDatasetinfo = dynamic_cast<DatasetInfo*>(_qDatasetInfo.get());

   if (_pDatasetinfo)
   {
      if (_pDatasetinfo->GetTileFormat() == "image/png")
      {
         _eTileFormat = IF_PNG;
      }
      else if (_pDatasetinfo->GetTileFormat() == "image/jp2")
      {
         _eTileFormat = IF_JP2;
      }
      else
      {
         assert(false);
      }
   }
   */
}


bool ImageTileFileSystemReader::_ReadTileServiceBingMaps(std::string sQuadcode, boost::shared_array<unsigned char>& out_qImageDataRGBA, unsigned int& out_nTilesize, PixelFormat& out_PixelFormat)
{
   if (!_qQuadtreeSphere)
      return false;

   int outLod;
   int64 outX;
   int64 outY;

   ImageHandler IH;
   boost::shared_array<unsigned char> qImageRGBA;

   _qQuadtreeSphere->QuadKeyToTileCoord(sQuadcode, outX, outY, outLod);

   static unsigned int OSMLod = 20;

   if (outLod>(int)OSMLod) // Bing doesn't support LOD>20
   {
      return _InterpolateFromHigher(sQuadcode, OSMLod, out_qImageDataRGBA, out_nTilesize, out_PixelFormat);
   } 

   std::wstring sOut;

   _pDatasetinfo = 0;

   std::wostringstream woss;


   std::wstring wQuad(sQuadcode.begin(), sQuadcode.end());
   woss << _sPath;
   woss << "a" <<  wQuad << ".jpeg?g=487&mkt=en-us&n=z";
  
   std::wstring sURL = woss.str();
   std::string sURLAscii = StringUtils::wstring_To_Utf8(sURL);

   //std::cout << "[download] " << sURLAscii << "\n";



   if (FilenameUtils::IsFileOnWeb(sURL))
   {
      if (FileSystem::DownloadURL(sURL, 0))
      {
         if (!FileSystem::FindInternetCache(sURL, sOut))
         {
            //Unable to find downloaded file in cache
            return false;
         }
      }
      else
      {
         //Unable to download file
         return false;
      }
   }
   else
   {
      // local file:
      sOut = sURL;
      if (!FileSystem::FileExists(sOut))
      {
         return false;
      }
   }

   boost::shared_array<unsigned char> qImageDataCompressed;
   size_t nImageSizeCompressed = 0;

   if (FileSystem::FileToMemory(sOut, qImageDataCompressed, nImageSizeCompressed))
   {

      if (qImageDataCompressed && IH.LoadImageFromMemory(IF_JPG, qImageDataCompressed, nImageSizeCompressed, true, false))
      {
         if (IH.GetWidth() == IH.GetHeight())
         {

            // Create Transparent Layer:
            if (IH.GetPixelFormat() == PF_RGB)
            {
               qImageRGBA = boost::shared_array<unsigned char>(new unsigned char[4*IH.GetWidth()*IH.GetHeight()]);
               unsigned char* pImage = qImageRGBA.get();
               unsigned char* pImgSrc = IH.GetImageData().get();

               for (size_t y=0;y<IH.GetHeight();y++)
               {
                  for (size_t x=0;x<IH.GetWidth();x++)
                  {
                     size_t adr4 = 4*y*IH.GetWidth()+4*x;
                     size_t adr3 = 3*y*IH.GetWidth()+3*x;

                     unsigned char r,g,b,a;
                     r = pImgSrc[adr3+0];
                     g = pImgSrc[adr3+1];
                     b = pImgSrc[adr3+2];
                     a = 150;

                     /*if (r== 232 && g== 238 && b== 241)
                     {
                     a = 0;
                     }*/
                     pImage[adr4+0] = r;
                     pImage[adr4+1] = g;
                     pImage[adr4+2] = b;
                     pImage[adr4+3] = a;

                  }
               }

            }
            else if (IH.GetPixelFormat() == PF_RGBA)
            {
               qImageRGBA = boost::shared_array<unsigned char>(new unsigned char[4*IH.GetWidth()*IH.GetHeight()]);
               unsigned char* pImage = qImageRGBA.get();
               unsigned char* pImgSrc = IH.GetImageData().get();

               for (size_t y=0;y<IH.GetHeight();y++)
               {
                  for (size_t x=0;x<IH.GetWidth();x++)
                  {
                     size_t adr4 = 4*y*IH.GetWidth()+4*x;

                     unsigned char r,g,b,a;
                     r = pImgSrc[adr4+0];
                     g = pImgSrc[adr4+1];
                     b = pImgSrc[adr4+2];
                     a = 150;

                     /*if (r== 232 && g== 238 && b== 241)
                     {
                     a = 0;
                     }*/
                     pImage[adr4+0] = r;
                     pImage[adr4+1] = g;
                     pImage[adr4+2] = b;
                     pImage[adr4+3] = a;

                  }
               }
            }

            out_nTilesize = IH.GetWidth();
            out_qImageDataRGBA = qImageRGBA;
            out_PixelFormat = PF_RGBA;


            //

            //out_nTilesize = IH.GetWidth();
            //out_qImageDataRGBA = IH.GetImageData();
            //out_PixelFormat = IH.GetPixelFormat();
            return true;
         }
      }
   }
   else
   {
      return false;
   }

   return false;
}


bool ImageTileFileSystemReader::_ReadTileServiceOpenStreetMap(std::string sQuadcode, boost::shared_array<unsigned char>& out_qImageDataRGBA, unsigned int& out_nTilesize, PixelFormat& out_PixelFormat)
{
   if (!_qQuadtreeSphere)
      return false;

   int outLod;
   int64 outX;
   int64 outY;

   ImageHandler IH;
   boost::shared_array<unsigned char> qImageRGBA;

   _qQuadtreeSphere->QuadKeyToTileCoord(sQuadcode, outX, outY, outLod);

   unsigned int OSMLod = _nMaxLod;

   if (outLod>(int)OSMLod) // OSM doesn't support LOD>18
   {
      return _InterpolateFromHigher(sQuadcode, OSMLod, out_qImageDataRGBA, out_nTilesize, out_PixelFormat);
   } 

   std::wstring sOut;

   _pDatasetinfo = 0;

   std::wostringstream woss;

   //woss << L"http://tile.openstreetmap.org/";
   woss << _sPath;
   woss << outLod << L"/";
   woss << outX << L"/";
   woss << outY << L".png";
   std::wstring sURL = woss.str();

   std::string sURLAscii = StringUtils::wstring_To_Utf8(sURL);

   //std::cout << "[download] " << sURLAscii << "\n";
   
   

   if (FilenameUtils::IsFileOnWeb(sURL))
   {
      if (FileSystem::DownloadURL(sURL, 0))
      {
         if (!FileSystem::FindInternetCache(sURL, sOut))
         {
            //Unable to find downloaded file in cache
            return false;
         }
      }
      else
      {
         //Unable to download file
         return false;
      }
   }
   else
   {
      // local file:
      sOut = sURL;
      if (!FileSystem::FileExists(sOut))
      {
         return false;
      }
   }

   boost::shared_array<unsigned char> qImageDataCompressed;
   size_t nImageSizeCompressed = 0;

   if (FileSystem::FileToMemory(sOut, qImageDataCompressed, nImageSizeCompressed))
   {
      /*if (nImageSizeCompressed < 300)
         return false; */
 
      if (qImageDataCompressed && IH.LoadImageFromMemory(IF_PNG, qImageDataCompressed, nImageSizeCompressed, true, false))
      {
         if (IH.GetWidth() == IH.GetHeight() && IH.GetWidth() > 0 && IH.GetHeight()>0)
         {

            // Create Transparent Layer:
            if (IH.GetPixelFormat() == PF_RGB)
            {
               qImageRGBA = boost::shared_array<unsigned char>(new unsigned char[4*IH.GetWidth()*IH.GetHeight()]);
               unsigned char* pImage = qImageRGBA.get();
               unsigned char* pImgSrc = IH.GetImageData().get();

               for (size_t y=0;y<IH.GetHeight();y++)
               {
                  for (size_t x=0;x<IH.GetWidth();x++)
                  {
                     size_t adr4 = 4*y*IH.GetWidth()+4*x;
                     size_t adr3 = 3*y*IH.GetWidth()+3*x;
                    
                     unsigned char r,g,b,a;
                     r = pImgSrc[adr3+0];
                     g = pImgSrc[adr3+1];
                     b = pImgSrc[adr3+2];
                     a = 255;
                     
                     if (_dTransparency < 1.0)  
                     {
                        a = unsigned char((double)a * _dTransparency);
                     }

        
                     pImage[adr4+0] = r;
                     pImage[adr4+1] = g;
                     pImage[adr4+2] = b;
                     pImage[adr4+3] = a;  

                  }
               }
            
            }
            else if (IH.GetPixelFormat() == PF_RGBA)
            {
               qImageRGBA = boost::shared_array<unsigned char>(new unsigned char[4*IH.GetWidth()*IH.GetHeight()]);
               unsigned char* pImage = qImageRGBA.get();
               unsigned char* pImgSrc = IH.GetImageData().get();

               for (size_t y=0;y<IH.GetHeight();y++)
               {
                  for (size_t x=0;x<IH.GetWidth();x++)
                  {
                     size_t adr4 = 4*y*IH.GetWidth()+4*x;

                     unsigned char r,g,b,a;
                     r = pImgSrc[adr4+0];
                     g = pImgSrc[adr4+1];
                     b = pImgSrc[adr4+2];
                     a = pImgSrc[adr4+3];

                     if (_dTransparency < 1.0)  
                     {
                        a = unsigned char((double)a * _dTransparency);
                     }

                     pImage[adr4+0] = r;
                     pImage[adr4+1] = g;
                     pImage[adr4+2] = b;
                     pImage[adr4+3] = a;

                  }
               }
            }

            out_nTilesize = IH.GetWidth();
            out_qImageDataRGBA = qImageRGBA;
            out_PixelFormat = PF_RGBA;


            //

            //out_nTilesize = IH.GetWidth();
            //out_qImageDataRGBA = IH.GetImageData();
            //out_PixelFormat = IH.GetPixelFormat();
            return true;
         }
      }
   }
   else
   {
      return false;
   }

   return false;
}


#endif

