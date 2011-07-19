/*******************************************************************************
Project       : i3D Studio  
Purpose       : 3D Line
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef A_LINE3_H
#define A_LINE3_H

#ifndef I3D_USE_PRECOMPILED_HEADERS
#include "vec3.h"
#endif


//! \class line3
//! \brief 3D line between two points with intersection methods.
template <class T>
class line3
{
public:

  // Constructors

  line3() : start(0,0,0), end(1,1,1) {};
  line3(T xa, T ya, T za, T xb, T yb, T zb) : start(xa, ya, za), end(xb, yb, zb) {};
  line3(const vec3<T>& start, const vec3<T>& end) : start(start), end(end) {};
  line3(const line3<T>& other) : start(other.start), end(other.end) {};

  // operators

  line3<T> operator+(const vec3<T>& point) const { return line3<T>(start + point, end + point); };
  line3<T>& operator+=(const vec3<T>& point) { start += point; end += point; return *this; };

  line3<T> operator-(const vec3<T>& point) const { return line3<T>(start - point, end - point); };
  line3<T>& operator-=(const vec3<T>& point) { start -= point; end -= point; return *this; };

  bool operator==(const line3<T>& other) const { return (start==other.start && end==other.end) || (end==other.start && start==other.end);};
  bool operator!=(const line3<T>& other) const { return !(start==other.start && end==other.end) || (end==other.start && start==other.end);};

  // functions

  void setLine(const T& xa, const T& ya, const T& za, const T& xb, const T& yb, const T& zb){start.set(xa, ya, za); end.set(xb, yb, zb);}
  void setLine(const vec3<T>& nstart, const vec3<T>& nend){start.set(nstart); end.set(nend);}
  void setLine(const line3<T>& line){start.set(line.start); end.set(line.end);}

  T getLength() const { return start.getDistanceFrom(end); };

  T getLengthSQ() const { return start.getDistanceFromSQ(end); };

  vec3<T> getMiddle() const
  {
     return (start + end) * (T)0.5;
  }

  vec3<T> getVector() const
  {
     return end - start;
  }
  bool isPointBetweenStartAndEnd(const vec3<T>& point) const
  {
     return point.isBetweenPoints(start, end);
  }

  vec3<T> getClosestPoint(const vec3<T>& point) const
  {
     vec3<T> c = point - start;
     vec3<T> v = end - start;
     T d = (T)v.getLength();
     v /= d;
     T t = v.Dot(c);

     if (t < (T)0.0) return start;
     if (t > d) return end;

     v *= t;
     return start + v;
  }

  bool getIntersectionWithSphere(vec3<T> sorigin, T sradius, T& outdistance) const
  {
     vec3<T> q = sorigin - start;
     T c = q.Length();
     T v = q.Dot(getVector().Normalize());
     T d = sradius * sradius - (c*c - v*v);

     if (d < 0.0) 
        return false;

     outdistance = v - sqrt(d);
     return true;
  }

  // member variables

  vec3<T> start;
  vec3<T> end;
};



#endif