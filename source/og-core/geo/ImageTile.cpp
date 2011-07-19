/*******************************************************************************
Project       : i3D OpenWebGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "ImageTile.h"
#include <math/mathutils.h>

#ifdef DEPRECATED_FUNCTION
//------------------------------------------------------------------------------
         
void ImageTile::ReadPixel(int x, int y, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a)
{
   unsigned long adr;

   // Validate Range
   if (x>(int)nTileSize-1) x = (int)nTileSize-1;
   if (y>(int)nTileSize-1) y = (int)nTileSize-1;
   if (x<0) x = 0;
   if (y<0) y = 0;

   if (nBytesPerPixel == 4)
   {
      adr = 4*y*nTileSize+4*x;
      b = qTileImageData.get()[adr];
      g = qTileImageData.get()[adr+1];
      r = qTileImageData.get()[adr+2];
      a = qTileImageData.get()[adr+3];
   }
   else if (nBytesPerPixel == 3)
   {
      adr = 3*y*nTileSize+3*x;
      b = qTileImageData.get()[adr];
      g = qTileImageData.get()[adr+1];
      r = qTileImageData.get()[adr+2];
      a = 255;
   }
}

//------------------------------------------------------------------------------

void ImageTile::ReadPixelBilinear(double x, double y, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a)
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

   if (u00<0) u00 = 0;
   if (v00<0) v00 = 0;
   if (u10<0) u10 = 0;
   if (v10<0) v10 = 0;
   if (u01<0) u01 = 0;
   if (v01<0) v01 = 0;
   if (u11<0) u11 = 0;
   if (v11<0) v11 = 0;

   if (u00>(int)nTileSize-1) u00 = (int)nTileSize-1;
   if (v00>(int)nTileSize-1) v00 = (int)nTileSize-1;
   if (u10>(int)nTileSize-1) u10 = (int)nTileSize-1;
   if (v10>(int)nTileSize-1) v10 = (int)nTileSize-1;
   if (u01>(int)nTileSize-1) u01 = (int)nTileSize-1;
   if (v01>(int)nTileSize-1) v01 = (int)nTileSize-1;
   if (u11>(int)nTileSize-1) u11 = (int)nTileSize-1;
   if (v11>(int)nTileSize-1) v11 = (int)nTileSize-1;

   unsigned char r00;
   unsigned char g00; 
   unsigned char b00;
   unsigned char a00;

   unsigned char r10; 
   unsigned char g10; 
   unsigned char b10;
   unsigned char a10;

   unsigned char r01; 
   unsigned char g01; 
   unsigned char b01;
   unsigned char a01;

   unsigned char r11; 
   unsigned char g11; 
   unsigned char b11;
   unsigned char a11;

   // 
   ReadPixel(u00, v00, r00, g00, b00, a00);
   ReadPixel(u10, v10, r10, g10, b10, a10);
   ReadPixel(u01, v01, r01, g01, b01, a01);
   ReadPixel(u11, v11, r11, g11, b11, a11);
   //

   r = (unsigned char) (r00*(1-uf)*(1-vf)+r10*uf*(1-vf)+r01*(1-uf)*vf+r11*uf*vf);
   g = (unsigned char) (g00*(1-uf)*(1-vf)+g10*uf*(1-vf)+g01*(1-uf)*vf+g11*uf*vf);
   b = (unsigned char) (b00*(1-uf)*(1-vf)+b10*uf*(1-vf)+b01*(1-uf)*vf+b11*uf*vf);
   a = (unsigned char) (a00*(1-uf)*(1-vf)+a10*uf*(1-vf)+a01*(1-uf)*vf+a11*uf*vf);
}

inline double _CalcCubic(double v0, double v1,double v2,double v3, double t)
{
   int p = (v3 - v2) - (v0 - v1);
   int q = (v0 - v1) - p;
   int r = v2 - v0;
   int s = v1;
   double tSqrd = t * t;

   return (p * (tSqrd * t)) + (q * tSqrd) + (r * t) + s;
}

//------------------------------------------------------------------------------

void ImageTile::ReadPixelBicubic(double x, double y, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a)
{
   int nPixelX = int(x);
   int nPixelY = int(y);

   double uf = math::Fract<double>(x);
   double vf = math::Fract<double>(y);

   int u00,v00,u01,v01,u02,v02,u03,v03;
   int u10,v10,u11,v11,u12,v12,u13,v13;
   int u20,v20,u21,v21,u22,v22,u23,v23;
   int u30,v30,u31,v31,u32,v32,u33,v33;

   u00 = nPixelX -1; v00 = nPixelY-1;
   u01 = nPixelX; v01 = nPixelY-1;
   u02 = nPixelX+1; v02 = nPixelY-1;
   u03 = nPixelX+2; v03 = nPixelY-1;

   u10 = nPixelX-1; v10 = nPixelY;
   u11 = nPixelX; v11 = nPixelY;
   u12 = nPixelX+1; v12 = nPixelY;
   u13 = nPixelX+2; v13 = nPixelY;

   u20 = nPixelX-1; v20 = nPixelY+1;
   u21 = nPixelX; v21 = nPixelY+1;
   u22 = nPixelX+1; v22 = nPixelY+1;
   u23 = nPixelX+2; v23 = nPixelY+1;

   u30 = nPixelX-1; v30 = nPixelY+2;
   u31 = nPixelX; v31 = nPixelY+2;
   u32 = nPixelX+1; v32 = nPixelY+2;
   u33 = nPixelX+2; v33 = nPixelY+2;


   if (u00<0) u00 = 0; if (v00<0) v00 = 0;
   if (u01<0) u01 = 0; if (v01<0) v01 = 0;
   if (u02<0) u02 = 0; if (v02<0) v02 = 0;
   if (u03<0) u03 = 0; if (v03<0) v03 = 0;

   if (u10<0) u10 = 0; if (v10<0) v10 = 0;
   if (u11<0) u11 = 0; if (v11<0) v11 = 0;
   if (u12<0) u12 = 0; if (v12<0) v12 = 0;
   if (u13<0) u13 = 0; if (v13<0) v13 = 0;

   if (u20<0) u20 = 0; if (v20<0) v20 = 0;
   if (u21<0) u21 = 0; if (v21<0) v21 = 0;
   if (u22<0) u22 = 0; if (v22<0) v22 = 0;
   if (u23<0) u23 = 0; if (v23<0) v23 = 0;

   if (u30<0) u30 = 0; if (v30<0) v30 = 0;
   if (u31<0) u31 = 0; if (v31<0) v31 = 0;
   if (u32<0) u32 = 0; if (v32<0) v32 = 0;
   if (u33<0) u33 = 0; if (v33<0) v33 = 0;


   if (u00>(int)nTileSize-1) u00 = (int)nTileSize-1;
   if (v00>(int)nTileSize-1) v00 = (int)nTileSize-1;
   if (u01>(int)nTileSize-1) u01 = (int)nTileSize-1;
   if (v01>(int)nTileSize-1) v01 = (int)nTileSize-1;
   if (u02>(int)nTileSize-1) u02 = (int)nTileSize-1;
   if (v02>(int)nTileSize-1) v02 = (int)nTileSize-1;
   if (u03>(int)nTileSize-1) u03 = (int)nTileSize-1;
   if (v03>(int)nTileSize-1) v03 = (int)nTileSize-1;

   if (u10>(int)nTileSize-1) u10 = (int)nTileSize-1;
   if (v10>(int)nTileSize-1) v10 = (int)nTileSize-1;
   if (u11>(int)nTileSize-1) u11 = (int)nTileSize-1;
   if (v11>(int)nTileSize-1) v11 = (int)nTileSize-1;
   if (u12>(int)nTileSize-1) u12 = (int)nTileSize-1;
   if (v12>(int)nTileSize-1) v12 = (int)nTileSize-1;
   if (u13>(int)nTileSize-1) u13 = (int)nTileSize-1;
   if (v13>(int)nTileSize-1) v13 = (int)nTileSize-1;

   if (u20>(int)nTileSize-1) u20 = (int)nTileSize-1;
   if (v20>(int)nTileSize-1) v20 = (int)nTileSize-1;
   if (u21>(int)nTileSize-1) u21 = (int)nTileSize-1;
   if (v21>(int)nTileSize-1) v21 = (int)nTileSize-1;
   if (u22>(int)nTileSize-1) u22 = (int)nTileSize-1;
   if (v22>(int)nTileSize-1) v22 = (int)nTileSize-1;
   if (u23>(int)nTileSize-1) u23 = (int)nTileSize-1;
   if (v23>(int)nTileSize-1) v23 = (int)nTileSize-1;

   if (u30>(int)nTileSize-1) u30 = (int)nTileSize-1;
   if (v30>(int)nTileSize-1) v30 = (int)nTileSize-1;
   if (u31>(int)nTileSize-1) u31 = (int)nTileSize-1;
   if (v31>(int)nTileSize-1) v31 = (int)nTileSize-1;
   if (u32>(int)nTileSize-1) u32 = (int)nTileSize-1;
   if (v32>(int)nTileSize-1) v32 = (int)nTileSize-1;
   if (u33>(int)nTileSize-1) u33 = (int)nTileSize-1;
   if (v33>(int)nTileSize-1) v33 = (int)nTileSize-1;


   unsigned char r00,r01,r02,r03;
   unsigned char r10,r11,r12,r13;
   unsigned char r20,r21,r22,r23;
   unsigned char r30,r31,r32,r33;

   unsigned char g00,g01,g02,g03;
   unsigned char g10,g11,g12,g13;
   unsigned char g20,g21,g22,g23;
   unsigned char g30,g31,g32,g33;

   unsigned char b00,b01,b02,b03;
   unsigned char b10,b11,b12,b13;
   unsigned char b20,b21,b22,b23;
   unsigned char b30,b31,b32,b33;

   unsigned char a00,a01,a02,a03;
   unsigned char a10,a11,a12,a13;
   unsigned char a20,a21,a22,a23;
   unsigned char a30,a31,a32,a33;


   ReadPixel(u00, v00, r00, g00, b00, a00);
   ReadPixel(u01, v01, r01, g01, b01, a01);
   ReadPixel(u02, v02, r02, g02, b02, a02);
   ReadPixel(u03, v03, r03, g03, b03, a03);

   ReadPixel(u10, v10, r10, g10, b10, a10);
   ReadPixel(u11, v11, r11, g11, b11, a11);
   ReadPixel(u12, v12, r12, g12, b12, a12);
   ReadPixel(u13, v13, r13, g13, b13, a13);

   ReadPixel(u20, v20, r20, g20, b20, a20);
   ReadPixel(u21, v21, r21, g21, b21, a21);
   ReadPixel(u22, v22, r22, g22, b22, a22);
   ReadPixel(u23, v23, r23, g23, b23, a23);

   ReadPixel(u30, v30, r30, g30, b30, a30);
   ReadPixel(u31, v31, r31, g31, b31, a31);
   ReadPixel(u32, v32, r32, g32, b32, a32);
   ReadPixel(u33, v33, r33, g33, b33, a33);

   double r0 = _CalcCubic(r00,r01,r02,r03, uf);
   double r1 = _CalcCubic(r10,r11,r12,r13, uf);
   double r2 = _CalcCubic(r20,r21,r22,r23, uf);
   double r3 = _CalcCubic(r30,r31,r32,r33, uf);

   double g0 = _CalcCubic(g00,g01,g02,g03, uf);
   double g1 = _CalcCubic(g10,g11,g12,g13, uf);
   double g2 = _CalcCubic(g20,g21,g22,g23, uf);
   double g3 = _CalcCubic(g30,g31,g32,g33, uf);

   double b0 = _CalcCubic(b00,b01,b02,b03, uf);
   double b1 = _CalcCubic(b10,b11,b12,b13, uf);
   double b2 = _CalcCubic(b20,b21,b22,b23, uf);
   double b3 = _CalcCubic(b30,b31,b32,b33, uf);

   double a0 = _CalcCubic(a00,a01,a02,a03, uf);
   double a1 = _CalcCubic(a10,a11,a12,a13, uf);
   double a2 = _CalcCubic(a20,a21,a22,a23, uf);
   double a3 = _CalcCubic(a30,a31,a32,a33, uf);

   double rd = _CalcCubic(r0,r1,r2,r3,vf);
   double gd = _CalcCubic(g0,g1,g2,g3,vf);
   double bd = _CalcCubic(b0,b1,b2,b3,vf);
   double ad = _CalcCubic(a0,a1,a2,a3,vf);

   if (rd>255) rd = 255.0;
   if (gd>255) gd = 255.0;
   if (bd>255) bd = 255.0;
   if (ad>255) ad = 255.0;
   if (rd<0) rd = 0;
   if (gd<0) gd = 0;
   if (bd<0) bd = 0;
   if (ad<0) ad = 0;

   r = unsigned char(rd);
   g = unsigned char(gd);
   b = unsigned char(bd);
   a = unsigned char(ad);
}

//------------------------------------------------------------------------------
#endif