/*******************************************************************************
Project       : i3D Studio  
Purpose       : Map Triangle in 3D to a 2D-Sprite-Like Representation
Creation Date : 12.4.2008
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _TRIANGLEMAPPER_H
#define _TRIANGLEMAPPER_H

#include "og.h"
#include "math/vec3.h"


//! \class TriangleMapper
//! \brief Project triangle in 3D to 2D 
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup math
class OPENGLOBE_API TriangleMapper
{
public:
   //! \brief Project triangle given in 3D to 2D so that angles remain same and space is optimized. Using 64-bit floating point precision.
   static void ProjectTriangle(const vec3<double>& A, const vec3<double>& B, const vec3<double>& C, vec3<double>& Aout, vec3<double>& Bout, vec3<double>& Cout);
   
   //! \brief Project triangle given in 3D to 2D so that angles remain same and space is optimized. Using 32-bit floating point precision.
   static void ProjectTriangle_flt(const vec3<float>& A, const vec3<float>& B, const vec3<float>& C, vec3<float>& Aout, vec3<float>& Bout, vec3<float>& Cout);

};



#endif
