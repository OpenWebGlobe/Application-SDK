/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _IMAGETILE_H
#define _IMAGETILE_H

#include "og.h"
#include "image/ImageHandler.h"

//-----------------------------------------------------------------------------
// ImageTile (is actually an image object, but ImageTile is better semantic)


typedef ImageObject ImageTile;


//! \class ImageTile
//! \author Martin Christen, martin.christen@fhnw.ch
/*class OPENGLOBE_API ImageTile
{
public:
   unsigned int nTileSize;                                // usually 256 for 256x256 Textures...
   boost::shared_array<unsigned char> qTileImageData;     // Image data...
   unsigned int nBytesPerPixel;                           // 3 for RGB or 4 for RGBA

   void ReadPixel(int x, int y, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a);
   void ReadPixelBilinear(double x, double y, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a);
   void ReadPixelBicubic(double x, double y, unsigned char& r, unsigned char& g, unsigned char&b, unsigned char& a);
};*/

//-----------------------------------------------------------------------------

#endif