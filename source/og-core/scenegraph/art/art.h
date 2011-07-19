// Embedded Artwork

#ifndef _EMBEDDED_ART_H
#define _EMBEDDED_ART_H

#include "og.h"

// Compass Background: (RGBA)
   OPENGLOBE_API void* getImage_cmp128bg();
   OPENGLOBE_API int getWidth_cmp128bg();
   OPENGLOBE_API int getHeight_cmp128bg();

// Compass Rose: (RGBA)
   OPENGLOBE_API void* getImage_cmp128r();
   OPENGLOBE_API int getWidth_cmp128r();
   OPENGLOBE_API int getHeight_cmp128r();

// i3d Logo: (RGBA)
   OPENGLOBE_API void* getImage_i3d();
   OPENGLOBE_API int getWidth_i3d();
   OPENGLOBE_API int getHeight_i3d();

#endif
