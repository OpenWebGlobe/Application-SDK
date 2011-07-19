
#ifndef A_VIEWFRUSTUM_H
#define A_VIEWFRUSTUM_H

#include "og.h"
#include <cmath>
#include <iostream>
#include "math/vec3.h"
#include "math/mat4.h"
#include "math/plane3.h"
#include "math/mathutils.h"
#include "math/aabb3.h"

//-----------------------------------------------------------------------------

template <class T>
T SideOfPlane(const plane3<T> & plane, const vec3<T> & pt)
{
   return (dot<T>( pt, plane.Normal ) + plane.D);
}

//-----------------------------------------------------------------------------

//! \brief View frustum with intersection tests.
//! \ingroup math graphics
template <class T>
class ViewFrustum
{
public:
   ViewFrustum(){}
   ViewFrustum(const mat4<T>& oMatView, const mat4<T>& oMatProjection){Setup(oMatView, oMatProjection);}
   virtual     ~ViewFrustum(){}

   void        Setup(const mat4<T>& oMatView, const mat4<T>& oMatProjection)
   {
      _MatMVP = oMatProjection * oMatView;
      _MatView = oMatView;
      _MatProjection = oMatProjection;
      _bHasProjInverse = false;

      UpdateFrustumPlanes();
   }

   //--------------------------------------------------------------------------

   bool     TestBox(aabb3<T>& oAABB) const
   {
      return TestBox(oAABB.MinEdge, oAABB.MaxEdge);
   }

   //--------------------------------------------------------------------------

   bool      TestBox(vec3<T>& vMin, vec3<T>& vMax) const
   {

      vec3<T> vCorner[8];
      int iTotalIn = 0;
      vCorner[0].x = vMin.x; vCorner[0].y = vMin.y; vCorner[0].z = vMin.z;
      vCorner[1].x = vMax.x; vCorner[1].y = vMin.y; vCorner[1].z = vMin.z;
      vCorner[2].x = vMax.x; vCorner[2].y = vMax.y; vCorner[2].z = vMin.z;
      vCorner[3].x = vMin.x; vCorner[3].y = vMax.y; vCorner[3].z = vMin.z;
      vCorner[4].x = vMin.x; vCorner[4].y = vMin.y; vCorner[4].z = vMax.z;
      vCorner[5].x = vMin.x; vCorner[5].y = vMax.y; vCorner[5].z = vMax.z;
      vCorner[6].x = vMax.x; vCorner[6].y = vMax.y; vCorner[6].z = vMax.z;
      vCorner[7].x = vMax.x; vCorner[7].y = vMin.y; vCorner[7].z = vMax.z;

      for (int p=0;p<6;++p)
      {
         int iInCount = 8;
         int iPtIn = 1;

         for (int i=0;i<8;++i)
         {
            if (SideOfPlane(_oFrustumPlanes[p], vCorner[i]) < 0) 
             //if (_oFrustumPlanes[p].classifyPointRelation(vCorner[i]) == ISREL3D_BACK)
            {
               iPtIn = 0;
               --iInCount;
            }
         }
            
         if (iInCount == 0)
         {
               return false;
         }

         iTotalIn += iPtIn;
         
      }

      if (iTotalIn==6) 
         return true;  // COMPLETELY IN

      return true;      // PARTIALLY IN or COMPLETELY IN
   }

   //--------------------------------------------------------------------

   bool      TestSphere(vec3<T>& vCenter, T fRadius)
   {
      // various distances
      T fDistance;
      // calculate our distances to each
      for(int i = 0; i < 6; ++i) 
      {
         // find the distance to this plane
         fDistance = dot<T>(_oFrustumPlanes[i].Normal, vCenter)+_oFrustumPlanes[i].D;
         // if this distance is < -sphere.
         if(fDistance < - fRadius)
            return(false);    // OUTSIDE
         // else if the distance is between
         if((T)fabs(fDistance) < fRadius)
            return(true);        // INTERSECT 
      }
      // otherwise we are fully in view
      return   (true);           // IN         

      return false;        
   }

   //
   // Returns Frustum Coordinates:
   //   7------------6    
   //   |\          /|    
   //   | 3--------2 |    
   //   | |        | |    
   //   4-|--------|-5    
   //    \|        |/     
   //     0--------1  

   std::vector<vec3<double> > GetFrustum()
   {
      std::vector<vec3<double> > vCoordList;

      if (!_bHasProjInverse)
      {
         _MatProjectionInverse = _MatProjection.Inverse();
         //_MatViewProjectionInverse = _MatView * _MatProjectionInverse;
         _MatViewProjectionInverse = _MatMVP.Inverse();

         _bHasProjInverse = true;
      }
    
      // NEAR
      vCoordList.push_back(_MatViewProjectionInverse.vec3mul(vec3<double>(-1,-1,-1)));
      vCoordList.push_back(_MatViewProjectionInverse.vec3mul(vec3<double>(1,-1,-1)));
      vCoordList.push_back(_MatViewProjectionInverse.vec3mul(vec3<double>(1,1,-1)));
      vCoordList.push_back(_MatViewProjectionInverse.vec3mul(vec3<double>(-1,1,-1)));

      // FAR
      vCoordList.push_back(_MatViewProjectionInverse.vec3mul(vec3<double>(-1,-1,1)));
      vCoordList.push_back(_MatViewProjectionInverse.vec3mul(vec3<double>(1,-1,1)));
      vCoordList.push_back(_MatViewProjectionInverse.vec3mul(vec3<double>(1,1,1)));
      vCoordList.push_back(_MatViewProjectionInverse.vec3mul(vec3<double>(-1,1,1)));
      return vCoordList;

   }
   
   /*
   
   bool OBBToFrustumIntersect(const OBB &obb, const FrustumVolume &frustum)
   {
	   // Do a SAT test along each frustum axis.
	   // A frustum consists of six planes. Note that as near and far planes are parallel, we could optimize 
	   // one iteration away.
	   for(int i = 0; i < 6; i++) 
	   {
		   // Find the negative and positive far points of the OBB to the current frustum plane.
		   float x = Dot(obb.axes[0], frustum.planes[i].normal) >= 0.f ? obb.halfDistances[0] : -obb.halfDistances[0];
		   float y = Dot(obb.axes[1], frustum.planes[i].normal) >= 0.f ? obb.halfDistances[1] : -obb.halfDistances[1];
		   float z = Dot(obb.axes[2], frustum.planes[i].normal) >= 0.f ? obb.halfDistances[2] : -obb.halfDistances[2];

		   // There are eight half-diagonal vectors on an OBB. (A half-diagonal is a vector from OBB center to one of its
		   // corner vertices). Compute the half-diagonal vector that points most in the same direction than the plane normal.
		   const float3 diag = x*obb.axes[0] + y*obb.axes[1] + z*obb.axes[2];
		   // nPoint is now the corner point of the OBB that lies "the most" inside the frustum, 
		   // its projection on the plane normal extends the most to the negative direction of that normal.
		   const float3 nPoint = obb.center - diag;
   //		const float3 pPoint = obb.center + diag;

		   if (Dot(nPoint, frustum.planes[i].normal) + frustum.planes[i].d >= 0.f)
			   return false; // OBB outside frustum.

		   // If we would like to check whether the OBB is fully inside the frustum, need to compute
		   // Dot(pPoint, frustum.planes[i].normal) + frustum.planes[i].d. If it's < 0 for all planes, OBB is totally
		   // inside the frustum and doesn't intersect any of the frustum planes.
	   }

	   return true; // OBB inside the frustum or part of the OBB intersects the frustum.
   }
*/
   double GetLeft(){return _oFrustumPlanes[0].D; }
   double GetRight(){return _oFrustumPlanes[1].D; }
   double GetTop(){return _oFrustumPlanes[2].D; }
   double GetBottom(){return _oFrustumPlanes[3].D; }
   double GetNear(){return _oFrustumPlanes[4].D; }
   double GetFar(){return _oFrustumPlanes[5].D; }

protected:
   void UpdateFrustumPlanes(void)
   {
      for (int s=0;s<6;s++)
      {
         float fSign = 1.0f;
         int r;
         if (s==0){ fSign=1.0f;     r=0;} // LEFT
         if (s==1){ fSign=-1.0f;    r=0;} // RIGHT
         if (s==2){ fSign=-1.0f;    r=1;} // TOP
         if (s==3){ fSign=1.0f;     r=1;} // BOTTOM
         if (s==4){ fSign=1.0f;     r=2;} // NEAR
         if (s==5){ fSign=-1.0f;    r=2;} // FAR

         _oFrustumPlanes[s].Normal.x = _MatMVP.GetVal(3,0) + fSign*_MatMVP.GetVal(r,0);
         _oFrustumPlanes[s].Normal.y = _MatMVP.GetVal(3,1) + fSign*_MatMVP.GetVal(r,1);
         _oFrustumPlanes[s].Normal.z = _MatMVP.GetVal(3,2) + fSign*_MatMVP.GetVal(r,2);
         _oFrustumPlanes[s].D =        _MatMVP.GetVal(3,3) + fSign*_MatMVP.GetVal(r,3);
         _oFrustumPlanes[s].Normalize();
      }

   }

   mat4<T>        _MatMVP;
   mat4<T>        _MatView;
   mat4<T>        _MatProjection;
   mat4<T>        _MatProjectionInverse;
   mat4<T>        _MatViewProjectionInverse;
   bool           _bHasProjInverse;
      
   // Plane Numbering Scheme:
   // 0 = LEFT
   // 1 = RIGHT
   // 2 = TOP
   // 3 = BOTTOM
   // 4 = NEAR
   // 5 = FAR 
   plane3<T>      _oFrustumPlanes[6];





private:
   ViewFrustum(const ViewFrustum&){}
};


#endif

