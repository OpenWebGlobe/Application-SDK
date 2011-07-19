#ifndef _TEXTURETRANSFORMATION_H
#define _TEXTURETRANSFORMATION_H

#include "og.h"
#include "math/vec3.h"
#include "math/mat4.h"
#include "math/quaternion.h"

//! \brief Utility class to create texture matrix.
//! \ingroup scenegraph
template <class T>
class TextureTransformation
{
public:
   vec3<T>	      _vTranslation;
   quaternion<T>	_qRotation;
   vec3<T>	      _vScale;
   int			   _iTexCoordsCount;
   bool		      _bEnable;

   TextureTransformation()
   {
      SetIdentity();
      _iTexCoordsCount = 2;
      _bEnable	= true;
   };

   TextureTransformation(int iTexCoordsCount, bool bEnable, const vec3<T>& vTranslation, const quaternion<T>& qRotation, const vec3<T>& vScale) 
      : _vTranslation(vTranslation), _vScale(vScale), _qRotation(qRotation), _bEnable(bEnable)
   {
      _iTexCoordsCount = iTexCoordsCount;
   };

   void SetIdentity()
   {
      _vTranslation.x = _vTranslation.y = _vTranslation.z = 0;
      _vScale.x = _vScale.y = _vScale.z = 1;
      _qRotation.w = 1;
      _qRotation.x = _qRotation.y = _qRotation.z = 0;
   };

   void ToMatrix4x4(mat4<T>& mat)
   {
      mat4<T> matTemp;
      mat.SetScale(_vScale);
      _qRotation.ToRotationMatrix(matTemp);
      mat = matTemp * mat;
      matTemp.SetTranslation(_vTranslation);
      mat = matTemp * mat;			
   };

   /*void ToMatrix3x3(mat4<T>& mat)
   {
      mat4<T> matTemp;
      mat.SetScale(_vScale);
      _qRotation.ToRotationMatrix(matTemp);
      mat = matTemp * mat;
      matTemp.SetTranslation(_vTranslation);
      mat = matTemp * mat;			

      // make 3x3 matrix
      mat._31 = mat._41;
      mat._32 = mat._42;
      mat._33 = mat._44;
      mat._13 = mat._14;
      mat._12 = mat._24;
   };*/

};

#endif