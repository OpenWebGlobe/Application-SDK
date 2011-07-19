/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _IMAGETILEFILESYSTEM_H
#define _IMAGETILEFILESYSTEM_H

#ifdef DEPRECATED_FUNCTION

#include "og.h"
#include <boost/shared_ptr.hpp>
#include <string>
#include "image/ImageHandler.h"
#include "image/ImageLoader.h"
#include "DatasetInfo.h"
#include "MercatorQuadtree.h"

//-----------------------------------------------------------------------------

enum TileService
{
   TS_UNKNOWN,
   TS_OPENSTREETMAP,
   TS_BING,
};

//-----------------------------------------------------------------------------
//! \class ImageTileFileSystemReader
//! \author Martin Christen, martin.christen@fhnw.ch
class /*OPENGLOBE_API*/ ImageTileFileSystemReader
{
public:
   ImageTileFileSystemReader();
   virtual ~ImageTileFileSystemReader();

   //! \brief Opens an Image Tile System. Image tiles are stored locally or on network.
   //! \param sPath Path to data
   //! \param sName Name of dataset.
   bool Open(const std::string& sPath, const std::string& sName, double dTransparency, int nMaxLod);

   void Close();

   //! \brief Read a tile 
   bool ReadTile(std::string sQuadcode, boost::shared_array<unsigned char>& out_qImageDataRGBA, unsigned int& out_nTilesize, Img::PixelFormat& out_PixelFormat);

   //! \brief Returns true if data is streamed from http
   bool IsOnWeb();

   //! \brief Retrieve Path where archive is located
   std::string         GetPath(){return _sPath;}

   //! \brief Retrieve Name of Archive
   std::string         GetName(){return _sName;}

   //! \brief get path of dataset info file...
   std::string         GetDatasetInfoPath();

   //! \breief get dataset info.
   DatasetInfo*        GetDatasetInfo(){return _pDatasetinfo;}

protected:
   void                 LoadDatasetInfo();

   void _ReadImageValueBilinearRGBA(unsigned char* buffer, unsigned int nWidth, unsigned int nHeight, double x, double y, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);
   void _ReadImageValueBilinearRGB(unsigned char* buffer, unsigned int nWidth, unsigned int nHeight, double x, double y, unsigned char* r, unsigned char* g, unsigned char* b);

   bool _ReadTileServiceOpenStreetMap(std::string sQuadcode, boost::shared_array<unsigned char>& out_qImageDataRGBA, unsigned int& out_nTilesize, Img::PixelFormat& out_PixelFormat);
   bool _ReadTileServiceBingMaps(std::string sQuadcode, boost::shared_array<unsigned char>& out_qImageDataRGBA, unsigned int& out_nTilesize, Img::PixelFormat& out_PixelFormat);

   bool _InterpolateFromHigher(std::string& sQuadcode, int nLod, boost::shared_array<unsigned char>& out_qImageDataRGBA, unsigned int& out_nTilesize, Img::PixelFormat& out_PixelFormat);


   double                                 _dTransparency;
   int                                    _nMaxLod;
   std::string                           _sPath;
   std::string                           _sName;
   bool                                   _bGood;
   bool                                   _bWeb;
   DatasetInfo*                           _pDatasetinfo;
   Img::FileFormat                       _eTileFormat;
   bool                                   _bWebService;
   TileService                            _eTileService;
   boost::shared_ptr<MercatorQuadtree>    _qQuadtreeSphere;



};

#endif

#endif