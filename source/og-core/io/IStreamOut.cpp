/*******************************************************************************
Project       : i3D Studio  
Purpose       : File Handling
Creation Date : 29.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2006 by FHNW. All rights reserved.
*******************************************************************************/


#include "IStreamOut.h"
#include "io/IStreamable.h"
#include "ogException.h"

void   IStreamOut::WriteByte(char* pByte) 
{
   Write((void*)pByte, sizeof(char));
};

void   IStreamOut::WriteUByte(unsigned char* pUByte) 
{
   Write((void*)pUByte, sizeof(unsigned char));
};

void   IStreamOut::WriteShort(short* pShort) 
{
#ifdef I3D_BIG_ENDIAN
   short oShort = Byteswap::byteswap(*pShort);
   Write((void*)&oShort, sizeof(short));
#else
   Write((void*)pShort, sizeof(short));
#endif
};

void   IStreamOut::WriteUShort(unsigned short* pUShort) 
{
#ifdef I3D_BIG_ENDIAN
   short oShort = Byteswap::byteswap(*pUShort);
   Write((void*)&oShort, sizeof(short));
#else
   Write((void*)pUShort, sizeof(unsigned short));
#endif
};

void   IStreamOut::WriteInt(int* pInt) 
{
#ifdef I3D_BIG_ENDIAN
   int oInt = Byteswap::byteswap(*pInt);
   Write((void*)&oInt, sizeof(int));
#else
   Write((void*)pInt, sizeof(int));
#endif
};

void   IStreamOut::WriteUInt(unsigned int* pUInt) 
{
#ifdef I3D_BIG_ENDIAN
   int oInt = Byteswap::byteswap(*pUInt);
   Write((void*)&oInt, sizeof(unsigned int));
#else
   Write((void*)pUInt, sizeof(unsigned int));
#endif
};

void   IStreamOut::WriteInt64(int64* pInt64) 
{
#ifdef I3D_BIG_ENDIAN
   int64 oInt = Byteswap::byteswap(*pInt64);
   Write((void*)&oInt, sizeof(int64));
#else
   Write((void*)pInt64, sizeof(int64));
#endif
};

void   IStreamOut::WriteUInt64(uint64* pUInt64) 
{
#ifdef I3D_BIG_ENDIAN
   int64 oInt = Byteswap::byteswap(*pUInt64);
   Write((void*)&oInt, sizeof(uint64));
#else
   Write((void*)pUInt64, sizeof(uint64));
#endif
};

void   IStreamOut::WriteFloat(float* pFloat) 
{
#ifdef I3D_BIG_ENDIAN
   int64 oFloat = Byteswap::byteswap(*pFloat);
   Write((void*)&oFloat, sizeof(float));
#else
   Write((void*)pFloat, sizeof(float));
#endif
};

void   IStreamOut::WriteDouble(double* pDouble) 
{
#ifdef I3D_BIG_ENDIAN
   int64 oDouble = Byteswap::byteswap(*pDouble);
   Write((void*)&oDouble, sizeof(double));
#else
   Write((void*)pDouble, sizeof(double));
#endif
};


void IStreamOut::WriteString(std::string* pString)
{
   unsigned int slen = (unsigned int) pString->length();
   WriteUInt(slen);
  
   std::string::iterator strit = pString->begin();
   while(strit != pString->end())
   {
      char c = *strit;
      WriteByte(&c);
      ++strit;
   }
}

void IStreamOut::WriteClass(IStreamable* pStreamable)
{
   pStreamable->Write(this);
}

//------------------------------------------------


void   IStreamOut::WriteByte(char pByte) 
{
   WriteByte(&pByte);
}

void   IStreamOut::WriteUByte(unsigned char pUByte)
{
   WriteUByte(&pUByte);
}

void   IStreamOut::WriteShort(short pUByte)
{
   WriteShort(&pUByte);
}

void   IStreamOut::WriteUShort(unsigned short pUByte)
{
   WriteUShort(&pUByte);
}

void   IStreamOut::WriteInt(int pInt)
{
   WriteInt(&pInt);
}

void   IStreamOut::WriteUInt(unsigned int pUInt)
{
   WriteUInt(&pUInt);
}

void   IStreamOut::WriteInt64(int64 pInt64)
{
   WriteInt64(&pInt64);
}

void   IStreamOut::WriteUInt64(uint64 pUInt64)
{
   WriteUInt64(&pUInt64);
}

void   IStreamOut::WriteFloat(float pFloat)
{
   WriteFloat(&pFloat);
}

void   IStreamOut::WriteDouble(double pDouble)
{
   WriteDouble(&pDouble);
}


void IStreamOut::WriteString(std::string& pString)
{
   WriteString(&pString);
}

void   IStreamOut::WriteVec3(vec3<double>& pVec)
{
   WriteDouble(pVec.x);
   WriteDouble(pVec.y);
   WriteDouble(pVec.z);
}

void   IStreamOut::WriteVec2(vec2<double>& pVec)
{
   WriteDouble(pVec.x);
   WriteDouble(pVec.y);
}