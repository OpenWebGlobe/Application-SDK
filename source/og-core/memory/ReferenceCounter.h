#ifndef _REFERENCECOUNTER_H
#define _REFERENCECOUNTER_H

#include "og.h"
#include <cassert>

// This is a manually controlled reference counter. The programmer is responsible
// To increase or decrease references and object deletion.
// This reference counter can be used for example for cyclic data structures.

class OPENGLOBE_API ReferenceCounter
{
   public:
      ReferenceCounter(){}
      virtual ~ReferenceCounter(){}
	   
      virtual void IncRef() = 0;
      virtual bool DecRef() = 0;
      virtual int  GetRefCount() = 0;
	   
};



#endif