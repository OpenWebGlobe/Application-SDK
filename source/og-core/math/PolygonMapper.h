/*******************************************************************************
Project       : i3D Studio  
Purpose       : Map Triangle in 3D to a 2D-Sprite-Like Representation
Creation Date : 28.4.2008
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _POLYGONMAPPER_H
#define _POLYGONMAPPER_H

#include "og.h"
#include "math/vec3.h"
#include "math/vec2.h"
#include <vector>


//! \class PolygonMapper
//! \brief Convert (planar) 3D Polygon to 2D Polygon by projecting to the plane which is defined by the first 3 points of the polygon. 
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup math
class OPENGLOBE_API PolygonMapper
{
public:

   //! \brief Project planar 3D polygon to 2D.
   //! \param vecPosition vector of input positions (3D)
   //! \param vecOutputPosition vector if output positions (2D)
   //! \return true on success, false otherwise.
   static bool ProjectPolygon(const std::vector<vec3<double> >& vecPosition, std::vector<vec2<double> >& vecOutputPosition);

   //! \brief Triangulate (planar) 3D Polygon.
   static bool Triangulate(const std::vector<vec3<double> >& vecPosition, std::vector<int>& vecTriangleIndices);

   //! \brief Calculate Area of polygon. Result is positive if polygon is defined counterclockwise.
   //! more information can be found at: http://local.wasp.uwa.edu.au/~pbourke/geometry/clockwise/index.html
   static double CalcArea(const std::vector<vec3<double> >& vecPosition);
};



#endif
