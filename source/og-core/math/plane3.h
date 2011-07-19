/*******************************************************************************
Project       : i3D Studio  
Purpose       : 3D Plane
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef A_PLANE3_H
#define A_PLANE3_H

#ifndef I3D_USE_PRECOMPILED_HEADERS
#include "vec3.h"
#endif

//namespace cwc
//{
//! Enumeration for intersection relations of 3d objects
//! \ingroup math linalg
   enum EIntersectionRelation3D 
   {
      ISREL3D_FRONT = 0,
      ISREL3D_BACK,
      ISREL3D_PLANAR,
      ISREL3D_SPANNING,
      ISREL3D_CLIPPED
   };

 //! \class plane3
 //! \brief Plane in 3D
 //! \author Martin Christen, martin.christen@fhnw.ch
 //! \ingroup math
   template <class T>
   class plane3
   {
   public:
      //! \brief Constructor
      plane3(): Normal(0,1,0) { recalculateD(vec3<T>(0,0,0)); };
      //! \brief Constructor: Create Plane using a point and a normal
      plane3(const vec3<T>& MPoint, const vec3<T>& Normal) : Normal(Normal) { recalculateD(MPoint); };
      //! \brief Constructor: Copy Contructor
      plane3(const plane3<T>& other) : Normal(other.Normal), D(other.D) {};
      //! \brief Constructor: Create Plane from 3 Points
      plane3(const vec3<T>& point1, const vec3<T>& point2, const vec3<T>& point3) { setPlane(point1, point2, point3); };

      //! \brief Destructor
      virtual ~plane3(){}

      // operators

      inline bool operator==(const plane3<T>& other) const { return (D==other.D && Normal==other.Normal);};
      inline bool operator!=(const plane3<T>& other) const { return !(D==other.D && Normal==other.Normal);};

      //! \brief Create plane from point and normal
      void setPlane(const vec3<T>& point, const vec3<T>& nvector)
      {
         Normal = nvector;
         Normal.Normalize();
         recalculateD(point);
      }

      //! \brief Create plane from point and d
      void setPlane(const vec3<T>& nvect, T d)
      {
         Normal = nvect;
         D = d;
      }

      //! \brief Create plane using 3 points
      void setPlane(const vec3<T>& point1, const vec3<T>& point2, const vec3<T>& point3)
      {
         Normal = (point2 - point1).crossProduct(point3 - point1);
         Normal.Normalize();

         recalculateD(point1);
      }

      //! Return intersection with a 3d line.
      //! \param lineVect: Vector of the line to intersect with.
      //! \param linePoint: Point of the line to intersect with.
      //! \param outIntersection: Place to store the intersection point, if there is one.
      //! \return Returns true if there was an intersection, false if there was not.
      bool getIntersectionWithLine(const vec3<T>& linePoint, const vec3<T>& lineVect, vec3<T>& outIntersection) const
      {
         T t2 = Normal.Dot(lineVect);

         if (t2 == 0)
            return false;

         T t =- (Normal.Dot(linePoint) + D) / t2;			
         outIntersection = linePoint + (lineVect * t);
         return true;
      }

      //! Returns where on a line between two points an intersection with this plane happened.
      //! Only useful if known that there is an intersection.
      //! \param linePoint1: Point1 of the line to intersect with.
      //! \param linePoint2: Point2 of the line to intersect with.
      //! \return Returns where on a line between two points an intersection with this plane happened.
      //! For example, 0.5 is returned if the intersection happened exectly in the middle of the two points.
      T getKnownIntersectionWithLine(const vec3<T>& linePoint1, const vec3<T>& linePoint2) const
      {
         vec3<T> vect = linePoint2 - linePoint1;
         T t2 = (T)Normal.Dot(vect);
         return (T)-((Normal.Dot(linePoint1) + D) / t2);
      }

      //! Returns an intersection with a 3d line, limited between two 3d points.
      //! \param linePoint1: Point 1 of the line.
      //! \param linePoint2: Point 2 of the line.
      //! \param outIntersection: Place to store the intersection point, if there is one.
      //! \return Returns true if there was an intersection, false if there was not.
      bool getIntersectionWithLimitedLine(const vec3<T>& linePoint1, 
         const vec3<T>& linePoint2, vec3<T>& outIntersection) const
      {
         return (	getIntersectionWithLine(linePoint1, linePoint2 - linePoint1, outIntersection) &&
            outIntersection.isBetweenPoints(linePoint1, linePoint2));
      }

      //! Classifies the relation of a point to this plane.
      //! \param point: Point to classify its relation.
      //! \return Returns ISREL3D_FRONT if the point is in front of the plane,
      //! ISREL3D_BACK if the point is behind of the plane, and
      //! ISREL3D_PLANAR if the point is within the plane.
      EIntersectionRelation3D classifyPointRelation(const vec3<T>& point) const
      {
         T d  = Normal.x*point.x + Normal.y*point.y + Normal.z*point.z + D;

         if (d < 0)
            return ISREL3D_BACK;

         if (d > 0)
            return ISREL3D_FRONT;

         return ISREL3D_PLANAR;
      }

      //! Recalculates the distance from origin by applying
      //! a new member point to the plane.
      void recalculateD(const vec3<T>& MPoint)
      {
         D = - MPoint.Dot(Normal);
      }

      //! Returns a member point of the plane.
      vec3<T> getMemberPoint() const
      {
         return Normal * -D;
      }

      //! Tests if there is a intersection between this plane and another
      //! \return Returns true if there is a intersection.
      bool existsInterSection(const plane3<T>& other) const
      {
         vec3<T> cross = other.Normal.Cross(Normal);
         return cross.Length() > (T)1e-08f;
      }

      //! Intersects this plane with another.
      //! \return Returns true if there is a intersection, false if not.
      bool getIntersectionWithPlane(const plane3<T>& other, vec3<T>& outLinePoint, vec3<T>& outLineVect) const
      {
         T fn00 = Normal.Length();
         T fn01 = Normal.Dot(other.Normal);
         T fn11 = other.Normal.Length();
         T det = fn00*fn11 - fn01*fn01;

         if (fabs(det) < (T)1e-08)
            return false;

         det = 1.0 / det;
         T fc0 = (fn11*-D + fn01*other.D) * det;
         T fc1 = (fn00*-other.D + fn01*D) * det;

         outLineVect = Normal.Cross(other.Normal);
         outLinePoint = Normal*(T)fc0 + other.Normal*(T)fc1;
         return true;
      }

      //! Returns the intersection point with two other planes if there is one.
      bool getIntersectionWithPlanes(const plane3<T>& o1, const plane3<T>& o2, vec3<T>& outPoint) const
      {
         vec3<T> linePoint, lineVect;
         if (getIntersectionWithPlane(o1, linePoint, lineVect))
            return o2.getIntersectionWithLine(linePoint, lineVect, outPoint);

         return false;
      }

      //! Returns if the plane is front of backfacing. Note that this only
      //! works if the normal is normalized.
      //! \param lookDirection: Look direction.
      //! \return Returns true if the plane is front facing, which mean it would be visible, and false if it is backfacing.
      bool isFrontFacing(const vec3<T>& lookDirection) const
      {
         return Normal.Dot(lookDirection) <= 0.0f;
      }

      //! Returns the distance to a point.  Note that this only works if the normal is normalized.
      T getDistanceTo(const vec3<T>& point) const
      {
         return point.Dot(Normal) + D;
      }

      //! Normalize Plane
      void Normalize(void)
      {
         T mag = sqrt(Normal.x*Normal.x+Normal.y*Normal.y+Normal.z*Normal.z);
         Normal.x /= mag;
         Normal.y /= mag;
         Normal.z /= mag;
         D /= mag;
      }

      // member variables

      vec3<T> Normal;		// normal vector
      T D;					// distance from origin
   };

//} // namespace

#endif