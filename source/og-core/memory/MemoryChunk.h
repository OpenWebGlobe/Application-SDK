
#ifndef _MEMORYCHUNK_H
#define _MEMORYCHUNK_H

#include "og.h"

struct OPENGLOBE_API SMemoryChunk
{
   size_t   size;
   boost::shared_array<unsigned char> mem;
};



#endif