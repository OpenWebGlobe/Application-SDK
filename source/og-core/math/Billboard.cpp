/*******************************************************************************
Project       : i3D Studio  
Purpose       : Billboard
Creation Date : 25.7.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#include "Billboard.h"


Billboard::Billboard()
{

}

Billboard::~Billboard()
{

}

void Billboard::_createBillboardMatrix(const vec3<double> &right, const vec3<double> &up, const vec3<double> &look, const vec3<double> &pos)
{
   double bbmat[16];
   bbmat[0] = right.x;
   bbmat[1] = right.y;
   bbmat[2] = right.z;
   bbmat[3] = 0;
   bbmat[4] = up.x;
   bbmat[5] = up.y;
   bbmat[6] = up.z;
   bbmat[7] = 0;
   bbmat[8] = look.x;
   bbmat[9] = look.y;
   bbmat[10] = look.z;
   bbmat[11] = 0;
   bbmat[12] = pos.x;
   bbmat[13] = pos.y;
   bbmat[14] = pos.z;
   bbmat[15] = 1;

   _matBillboard.SetGLMatrix(bbmat);
}


void Billboard::CreateBillboardPoint(const vec3<double> &pos, const vec3<double> &camPos, const vec3<double> &camUp)
{	
   vec3<double>	look	= camPos - pos;
   look.Normalize();

   vec3<double>	right	= cross_dbl(camUp,look);
   vec3<double>	up		= cross_dbl(look,right);

   right.Normalize();
   up.Normalize();

   _createBillboardMatrix(right, up, look, pos);
};


void Billboard::CreateBillboardAxis(const vec3<double> &pos, const vec3<double> &axis, const vec3<double> &camPos)
{	
   vec3<double>	look	= camPos - pos;
   look.Normalize();

   // billboard about the direction vector
   vec3<double>	up		= axis;
   vec3<double>	right	= cross_dbl(up,look);

   right.Normalize();
   up.Normalize();
   look = cross_dbl(right,up);
   look.Normalize();

   _createBillboardMatrix( right, up, look, pos);
};


void  Billboard::CreateBillboard(const vec3<double> &right, const vec3<double> &up, const vec3<double> &look, const vec3<double> &pos)
{
   _createBillboardMatrix( right, up, look, pos);
}