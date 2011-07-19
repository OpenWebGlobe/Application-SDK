#ifndef _QUATERNION_H
#define _QUATERNION_H

#include "og.h"
#include <cmath>
#include <iostream>
#include <cassert>
#include "vec3.h"
#include "mat4.h"
#include "vec2.h"
#include "yawpitchroll.h"
#include "angleaxis.h"

#define SIN_EPS (1e-03)
 
   //-------------------------------------------------------------------------
   //! \ingroup math linalg
   //! \bug this class needs to be revised. Do not use if possible.
   template<typename TType>
   class quaternion
   {
   public:
      TType	w;
      TType	x;
      TType	y;
      TType	z;

      // Default Constructor
      quaternion() : x(0),y(0),z(0),w(0){};
      
      // Construct from vec3
      quaternion(vec3<TType> &v3)							         {x = v3.x; y = v3.y; z = v3.z; w = 1;};		// zero angle rotation
      
      // Construct from values
      quaternion(TType ww, TType xx, TType yy, TType zz)	   {w = ww; x = xx; y = yy; z = zz;};
      
      // Construct using array [x,y,z,w]
      quaternion(TType vv[4])										   {w = vv[3]; x = vv[0]; y = vv[1]; z = vv[2];};
      
      // Construct from Angle Axis
      quaternion(TType fAngle, const vec3<TType>& axis)			{FromAngleAxis (fAngle,axis);};
      
      // Construct using Angle Axis class
      quaternion(angleaxis<TType> &aa)										{FromAngleAxis (aa.fAngle, aa.vAxis);};

      const TType& operator[](int i) const				{assert(0 <= i && i < 4); return ((TType *)this)[i];};
      TType& operator[](int i)								{assert(0 <= i && i < 4); return ((TType *)this)[i];};

      void Set(TType ww, TType xx, TType yy, TType zz)	   {w = ww; x = xx; y = yy; z = zz;};

      quaternion<TType> GetConjugate(){quaternion<TType> res; res.x = -x; res.y = -y, res.z = -z; res.w = w; return res;}


      quaternion& operator  = (const quaternion& v);
      quaternion& operator += (const quaternion& v);
      quaternion& operator -= (const quaternion& v);
      quaternion& operator *= (const quaternion& v);
      quaternion& operator *= (TType s);
      quaternion& operator /= (TType s);

      // Addition and subtraction
      friend quaternion operator + (const quaternion& v1, const quaternion& v2)
      {
         return quaternion<TType>(v1.w + v2.w, v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
      }
      
      friend quaternion operator - (const quaternion& v1, const quaternion& v2)
      {
         return quaternion<TType>(v1.w - v2.w, v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
      }
      
      // Scalar multiplication and division
      friend quaternion operator * (TType s, const quaternion& v)
      {
         return quaternion<TType>(v.w*s, v.x*s , v.y*s, v.z*s);
      }
      
      friend quaternion operator * (const quaternion& v, TType s)
      {
         return quaternion<TType>(v.w*s, v.x*s , v.y*s, v.z*s);
      }
      
      friend quaternion operator / (const quaternion& v, TType s)
      {
         TType fInv = 1.0/s;
         return quaternion<TType>(v.w*fInv, v.x*fInv , v.y*fInv, v.z*fInv);
      }
      
      // Memberwise multiplication and division
      friend quaternion operator * (const quaternion& v1, const quaternion& v2)
      {
         return quaternion<TType>(
            v1.w*v2.w -v1.x*v2.x -v1.y*v2.y -v1.z*v2.z,	// NOTE: multiplication is
            v1.w*v2.x +v1.x*v2.w +v1.y*v2.z -v1.z*v2.y,	//			not commutative
            v1.w*v2.y +v1.y*v2.w +v1.z*v2.x -v1.x*v2.z,	//			( v1*v2 != v2*v1 )
            v1.w*v2.z +v1.z*v2.w +v1.x*v2.y -v1.y*v2.x);
      }
      
      // Bitwise equality
      friend int operator == (const quaternion& v1, const quaternion& v2)
      {
         return (v1.w == v2.w && v1.x == v2.x && v1.y == v2.y && v1.z == v2.z);
      }

      friend int operator != (const quaternion& v1, const quaternion& v2)
      {
         return (v1.w != v2.w || v1.x != v2.x || v1.y != v2.y || v1.z != v2.z);
      }

      // Length-related functions
      static  TType Length  (const quaternion& v)
      {
         return sqrt(Length2(v));
      }
      
      static TType Length2 (const quaternion& v)
      {
         return v.w*v.w + v.x*v.x + v.y*v.y + v.z*v.z;
      }

      // Returns vector with same direction and unit length
      static quaternion Normalize	(const quaternion& v)
      {
         return v/Length(v);
      }

      // Dot product
      static  TType DotProduct		(const quaternion& v1, const quaternion& v2)
      {
         return v1.w*v2.w + v1.x*v2.x + v1.y * v2.y + v1.z*v2.z;
      }

      static quaternion Inverse			(const quaternion& v)	// apply to non-zero quaternion
      {
         TType fNorm = Length(v);
         if (0.0 < fNorm)					// quaternion length > 0 ?
         {
            fNorm = 1.0/fNorm;
            return quaternion<TType>(v.w*fNorm,-v.x*fNorm,-v.y*fNorm,-v.z*fNorm);
         }
         else return quaternion<TType>(0,0,0,0);		// return zero length quat as error
      }
      
      static quaternion UnitInverse		(const quaternion& v)	// apply to unit-length quaternion
      {
         return quaternion<TType>(v.w,-v.x,-v.y,-v.z);	// assert unit length
      }
      
      static quaternion Exp				(const quaternion& q)
      {
         // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
         // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
         // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

         TType fAngle = Length(q);
         TType fSin = sin(fAngle);

         if ( SIN_EPS <= fabs(fSin) )
         {
            TType coeff = fSin/fAngle;
            return quaternion<TType>(cos(fAngle), coeff*q.x, coeff*q.y, coeff*q.z);
         }
         else
            return quaternion<TType>(cos(fAngle), q.x, q.y, q.z);
      }
      
      static quaternion Log				(const quaternion& q)
      {
         // If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
         // log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
         // sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

         if (1.0 > fabs(q.w))
         {
            TType fAngle	= acos(q.w);
            TType fSin		= sin(fAngle);
            if ( SIN_EPS <= fabs(fSin) )
            {
               TType coeff = fAngle/fSin;
               return quaternion<TType>(0.0, coeff*q.x, coeff*q.y, coeff*q.z);
            }
         }
         return quaternion<TType>(0.0, q.x, q.y, q.z);
      }

      // spherical linear interpolation
      static quaternion Slerp (TType t, const quaternion& p, const quaternion& q)
      {
         // assert:  DotProduct(p,q) >= 0
         TType fCos = DotProduct(p,q);

         // numerical round-off error could create problems in call to acos
         if (-1 > fCos)		
            fCos = -1;
         else 
            if (1 < fCos)
               fCos =  1;

         TType fAngle	= acos(fCos);
         TType fSin		= sin(fAngle);  // fSin >= 0 since fCos >= 0

         if ( fSin < SIN_EPS )
            return p;
         else
         {
            TType fInvSin = 1/fSin;
            TType fCoeff0 = sin((1-t)*fAngle /*- fPhase*/)*fInvSin;
            TType fCoeff1 = sin(   t   *fAngle /*+ fPhase*/)*fInvSin;

            quaternion<TType> q0 = fCoeff0*p;
            quaternion<TType> q1 = fCoeff1*q;
            quaternion<TType> res;
            res.w = q0.w + q1.w;
            res.x = q0.x + q1.x;
            res.y = q0.y + q1.y;
            res.z = q0.z + q1.z;

            return res;
         }
      }
      static quaternion SlerpExtraSpins (TType t, const quaternion& p,
         const quaternion& q, int iExtraSpins)
      {
         // assert:  DotProduct(p,q) >= 0
         TType fCos = DotProduct(p,q);

         // numerical round-off error could create problems in call to acos
         if (-1.0 >fCos)		fCos = -1.0;
         else if (1.0 < fCos)	fCos =  1.0;

         TType fAngle	= acos(fCos);
         TType fSin		= sin(fAngle);  // fSin >= 0 since fCos >= 0

         if ( fSin < SIN_EPS )
            return p;
         else
         {
            TType fPhase = AGEPI*iExtraSpins*t;
            TType fInvSin = 1.0/fSin;
            TType fCoeff0 = sin((1.0-t)*fAngle - fPhase)*fInvSin;
            TType fCoeff1 = sin(   t   *fAngle + fPhase)*fInvSin;
            
            quaternion<TType> q0 = fCoeff0*p;
            quaternion<TType> q1 = fCoeff1*q;
            quaternion<TType> res;
            res.w = q0.w + q1.w;
            res.x = q0.x + q1.x;
            res.y = q0.y + q1.y;
            res.z = q0.z + q1.z;

            return res;
         }
      }

      // spherical quadratic interpolation
      quaternion Squad(TType t, const quaternion& a, const quaternion& b,
         const quaternion& ta, const quaternion& tb)
      {
         return Slerp(2*t*(1-t),Slerp(t,a,b),Slerp(t,ta,tb));
      }

      // spherical quadratic interpolation
      static quaternion Squad2(TType t, const quaternion &qA1,
         const quaternion &pQ1,
         const quaternion &pQ2,
         const quaternion &qA2)
      {
         return Slerp(2*t*(1 - t), Slerp(t, pQ1, pQ2), Slerp(t, qA1, qA2));
      }

      // conversion between quaternions, matrices, and angle-axes
      void FromRotationMatrix (const mat4<TType>& m);
      void ToRotationMatrix (mat4<TType>& m) const;
      // added by GJ. Fix possible row/column inversion...
      //		void ToRotationMatrix2 (mat4& m) const;
      void ToRotationMatrix2 (mat4<TType>& m) {ToRotationMatrix(m);}
      void FromAngleAxis (TType  fAngle, TType  ax, TType  ay, TType  az);
      void ToAngleAxis   (TType& fAngle, TType& ax, TType& ay, TType& az) const;
      void FromAngleAxis (TType  fAngle, const vec3<TType>& axis);
      void ToAngleAxis   (TType& fAngle,			vec3<TType>& axis) const;

      void FromAngleAxis (const	angleaxis<TType>& aa);
      void ToAngleAxis   (			angleaxis<TType>& aa) const;
      angleaxis<TType> ToAngleAxis();

      void FromYawPitchRoll(TType yaw, TType fPitch, TType fRoll, EYPROrder eYPROrder = YPR_ORDER_ZXY);
      void FromYawPitchRoll2(const yawpitchroll<TType>& ypr);
      void ToYawPitchRoll2(yawpitchroll<TType>& ypr);
   };


   template<typename TType>
   inline quaternion<TType>& quaternion<TType>::operator  = (const quaternion<TType>& v)
   {
      w = v.w; x = v.x; y = v.y; z = v.z;
      return *this;
   }

   template<typename TType>
   inline quaternion<TType>& quaternion<TType>::operator += (const quaternion<TType>& v)
   {
      w += v.w; x += v.x; y += v.y; z += v.z;
      return *this;
   }

   template<typename TType>
   inline quaternion<TType>& quaternion<TType>::operator -= (const quaternion<TType>& v)
   {
      w -= v.w; x -= v.x; y -= v.y; z -= v.z;
      return *this;
   }

   template<typename TType>
   inline quaternion<TType>& quaternion<TType>::operator *= (const quaternion<TType>& v)
   {
      TType W = w*v.w -x*v.x -y*v.y -z*v.z;
      TType X = w*v.x +x*v.w +y*v.z -z*v.y;
      TType Y = w*v.y +y*v.w +z*v.x -x*v.z;
      TType Z = w*v.z +z*v.w +x*v.y -y*v.x;
      w = W; x = X; y = Y; z = Z;
      return *this;
   }

   template<typename TType>
   inline quaternion<TType>& quaternion<TType>::operator *= (TType s)
   {
      x *= s; y *= s; z *= s; w *= s;
      return *this;
   }

   template<typename TType>
   inline quaternion<TType>& quaternion<TType>::operator /= (TType s)
   {
      TType fInv = 1.0/s;
      x *= fInv; y *= fInv; z *= fInv; w *= fInv;
      return *this;
   }

   /////////////////////////////*****************************************

   template<typename TType>
   void quaternion<TType>::FromRotationMatrix (const mat4<TType>& m)
   {
      // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
      // article "Quaternion Calculus and Fast Animation".

      TType fTrace = m.GetVal(0,0)+m.GetVal(1,1)+m.GetVal(2,2);
      TType fRoot;

      if ( fTrace > 0.0 )
      {
         // |w| > 1/2, may as well choose w > 1/2
         fRoot = sqrt(fTrace+1.0);  // 2w
         w = 0.5*fRoot;
         fRoot = 0.5/fRoot;  // 1/(4w)
         x = (m.GetVal(2,1)-m.GetVal(1,2))*fRoot;
         y = (m.GetVal(0,2)-m.GetVal(2,0))*fRoot;
         z = (m.GetVal(1,0)-m.GetVal(0,1))*fRoot;
      }
      else
      {
         // |w| <= 1/2
         int next[3] = { 1, 2, 0 };
         int i = 0;
         if (m.GetVal(1,1) > m.GetVal(0,0))  i = 1;
         if (m.GetVal(2,2) > m.GetVal(i,i))  i = 2;
         int j = next[i];
         int k = next[j];

         fRoot = sqrt(m.GetVal(i, i) - m.GetVal(j, j) - m.GetVal(k, k) + 1.0);
         (*this)[i + 1]	=  0.5 * fRoot;							// x for i=0
         fRoot		= 0.5 / fRoot;
         w			= (m.GetVal(k, j) - m.GetVal(j, k)) * fRoot;
         (*this)[j + 1]	= (m.GetVal(j, i) + m.GetVal(i, j)) * fRoot;
         (*this)[k + 1]	= (m.GetVal(k, i) + m.GetVal(i, k)) * fRoot;
      }
   }

   template<typename TType>
   void quaternion<TType>::ToRotationMatrix (mat4<TType>& m) const
   {
      TType tx  = 2.0*x;
      TType ty  = 2.0*y;
      TType tz  = 2.0*z;
      TType twx = tx*w;
      TType twy = ty*w;
      TType twz = tz*w;
      TType txx = tx*x;
      TType txy = ty*x;
      TType txz = tz*x;
      TType tyy = ty*y;
      TType tyz = tz*y;
      TType tzz = tz*z;

      m.SetVal(0 ,0, 1-(tyy+tzz));
      m.SetVal(0 ,1, txy-twz);
      m.SetVal(0 ,2, txz+twy);
      m.SetVal(0 ,3, 0);
      m.SetVal(1 ,0, txy+twz);
      m.SetVal(1 ,1, 1-(txx+tzz));
      m.SetVal(1 ,2, tyz-twx);
      m.SetVal(1 ,3, 0);
      m.SetVal(2 ,0, txz-twy);
      m.SetVal(2 ,1, tyz+twx);
      m.SetVal(2 ,2, 1-(txx+tyy));
      m.SetVal(2 ,3, 0);
      m.SetVal(3 ,0, 0);
      m.SetVal(3 ,1, 0);
      m.SetVal(3 ,2, 0);
      m.SetVal(3 ,3, 1);
   }

   template<typename TType>
   void quaternion<TType>::FromAngleAxis (TType fAngle, TType ax, TType ay, TType az)
   {
      // axis is unit length
      //
      // The quaternion representing the rotation is
      //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

      TType fHalfAngle = 0.5*fAngle;
      TType fSin = sin(fHalfAngle);
      w = cos(fHalfAngle);
      x = fSin*ax;
      y = fSin*ay;
      z = fSin*az;
   }

   template<typename TType>
   void quaternion<TType>::FromAngleAxis (TType fAngle, const vec3<TType>& axis)
   {
      FromAngleAxis(fAngle, axis.x, axis.y, axis.z);
   }

   template<typename TType>
   void quaternion<TType>::FromAngleAxis (const angleaxis<TType>& aa)
   {
      FromAngleAxis(aa.fAngle, aa.vAxis.x, aa.vAxis.y, aa.vAxis.z);
   }

   template<typename TType>
   void quaternion<TType>::ToAngleAxis (angleaxis<TType>& aa) const
   {
      ToAngleAxis(aa.fAngle, aa.vAxis.x, aa.vAxis.y, aa.vAxis.z);
   }

   template<typename TType>
   void quaternion<TType>::ToAngleAxis (TType& fAngle, vec3<TType>& axis) const
   {
      ToAngleAxis(fAngle, axis.x, axis.y, axis.z);
   }

   template<typename TType>
   angleaxis<TType> quaternion<TType>::ToAngleAxis()
   {
      angleaxis<TType> aa;
      ToAngleAxis(aa.fAngle, aa.vAxis.x, aa.vAxis.y, aa.vAxis.z);
      return aa;
   }

   template<typename TType>
   void quaternion<TType>::ToAngleAxis (TType& fAngle, TType& ax, TType& ay, TType& az) const
   {
      // The quaternion representing the rotation is
      //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)
      /*angle = 2 * acos(qw)
      x = qx / sqrt(1-qw*qw)
      y = qy / sqrt(1-qw*qw)
      z = qz / sqrt(1-qw*qw)*/
      
      if (w > 1.0f) 
         Normalize(*this);
      TType s = sqrt(1.0 - w * w);//x*x+y*y+z*z;
      fAngle = 2.0*acos(w);
      if ( s > 0.0 )
      {

         //TType fInvlen = 1.0/sqrt(fLength2);	// normalize
         TType fInvlen = 1.0/s;
         ax = x*fInvlen;
         ay = y*fInvlen;
         az = z*fInvlen;
      }
      else	// angle is 0 (mod 2*pi), so any axis will do
      {
         fAngle = 0;
         ax = 1.0;
         ay = 0.0;
         az = 0.0;
      }
   }

   template<typename TType>
   void quaternion<TType>::FromYawPitchRoll(TType yaw, TType fPitch, TType fRoll, EYPROrder eYPROrder)
   {
      TType ti,tj,tk;

      // Calculate half angles
      ti = fPitch	   * (TType)0.5;
      tj = yaw		* (TType)0.5;
      tk = fRoll		* (TType)0.5;

      quaternion<TType> qx((TType)cos(ti),	(TType)sin(ti),		0,						0);
      quaternion<TType> qy((TType)cos(tj),	0,							(TType)sin(tj),	0);
      quaternion<TType> qz((TType)cos(tk),	0,							0,						(TType)sin(tk));

      switch (eYPROrder)
      {
      case YPR_ORDER_ZXY:
         *this = Normalize(qz * qx * qy);
         break;
      case YPR_ORDER_ZYX:
         *this = Normalize(qz * qy * qx);
         break;
      case YPR_ORDER_XYZ:
         *this = Normalize(qx * qy * qz);
         break;
      case YPR_ORDER_XZY:
         *this = Normalize(qx * qz * qy);
         break;
      case YPR_ORDER_YXZ:
         *this = Normalize(qy * qx * qz);
         break;
      case YPR_ORDER_YZX:
         *this = Normalize(qy * qz * qx);
         break;
      }
   }

   
   template<typename TType>
   void quaternion<TType>::ToYawPitchRoll2(yawpitchroll<TType>& ypr)
   {
      ypr.fYaw = (TType)atan2(2*(x*y + z*w),  1-2*(y*y+z*z)); 
      ypr.fPitch = (TType)asin(2*(x*z-w*y));
      ypr.fRoll = (TType)atan2(2*(x*w + y*z),  1-2*(z*z+w*w)); 
   }
   
   
   template<typename TType>
   void quaternion<TType>::FromYawPitchRoll2(const yawpitchroll<TType>& ypr)
   {      
      TType yaw,pitch,roll;    // half yaw,pitch and roll.
      pitch = ypr.fPitch	* (TType)0.5;
      yaw = ypr.fYaw * (TType)0.5;
      roll = ypr.fRoll * (TType)0.5;
      
      x = cos(yaw)*cos(pitch)*cos(roll)+sin(yaw)*sin(pitch)*sin(roll);
      y = sin(yaw)*cos(pitch)*cos(roll)-cos(yaw)*sin(pitch)*sin(roll);
      z = cos(yaw)*sin(pitch)*cos(roll)+sin(yaw)*cos(pitch)*sin(roll);
      w = cos(yaw)*cos(pitch)*sin(roll)-sin(yaw)*sin(pitch)*cos(roll);
   }
 

#endif