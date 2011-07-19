/*******************************************************************************
Project       : i3D Studio  
Purpose       : Billboard
Creation Date : 25.7.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _BILLBOARD_H
#define _BILLBOARD_H

#ifndef I3D_USE_PRECOMPILED_HEADERS
#include "vec3.h"
#include "mat4.h"
#endif

//! \class Billboard
//! \brief Helper class to calculate different types of Billboard matrices.
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup math gfxtoolbox
class OPENGLOBE_API Billboard
{
public:
   //! Constructor.
   Billboard();

   //! Destructor
   virtual ~Billboard();

   //! Create point based billboard matrix.
   //! \param pos Position of the billboard point.
   //! \param camPos Position of the camera.
   //! \param camUp Up-vector of the camera.
   void	CreateBillboardPoint(const vec3<double> &pos, const vec3<double> &camPos, const vec3<double> &camUp);
   
   //! Create axis based billboard matrix
   //! \param pos Position of the axis
   //! \param axis Billboard-Axis
   //! \param camPos Position of the camera.
   void	CreateBillboardAxis(const vec3<double> &pos, const vec3<double> &axis, const vec3<double> &camPos);
   
   //! Create custom billboard matrix
   //! \param right Right vector of the camera
   //! \param up Up vector of the camera
   //! \param look Look vector of the camera
   //! \param pos position of the billboard
   void  CreateBillboard(const vec3<double> &right, const vec3<double> &up, const vec3<double> &look, const vec3<double> &pos);

   //! Retrieve Billboard matrix previously specified with a Create-Billboard function.
   //! \return billboard matrix
   mat4<double>& GetBillboardMatrix(){return _matBillboard;}

protected:
   void _createBillboardMatrix(const vec3<double> &right, const vec3<double> &up, const vec3<double> &look, const vec3<double> &pos);
   mat4<double>   _matBillboard;

};



#endif