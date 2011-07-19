/*******************************************************************************
Project       : i3D Studio  
Purpose       : File Handling
Creation Date : 29.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2006 by FHNW. All rights reserved.
*******************************************************************************/


#include "IStreamIn.h"
#include "IStreamable.h"
#include "ogException.h"

//#include "base.h"

int   IStreamIn::ReadByte(char* pByte) 
{
   return Read((void*)pByte, sizeof(char));
};

int   IStreamIn::ReadUByte(unsigned char* pUByte) 
{
   return Read((void*)pUByte, sizeof(unsigned char));
};

int   IStreamIn::ReadShort(short* pShort) 
{
#ifdef I3D_BIG_ENDIAN
   short oShort = Byteswap::byteswap(*pShort);
   return Read((void*)&oShort, sizeof(short));
#else
   return Read((void*)pShort, sizeof(short));
#endif
};

int   IStreamIn::ReadUShort(unsigned short* pUShort) 
{
#ifdef I3D_BIG_ENDIAN
   short oShort = Byteswap::byteswap(*pUShort);
   return Read((void*)&oShort, sizeof(short));
#else
   return Read((void*)pUShort, sizeof(unsigned short));
#endif
};

int   IStreamIn::ReadInt(int* pInt) 
{
#ifdef I3D_BIG_ENDIAN
   int oInt = Byteswap::byteswap(*pInt);
   return Read((void*)&oInt, sizeof(int));
#else
   return Read((void*)pInt, sizeof(int));
#endif
};

int   IStreamIn::ReadUInt(unsigned int* pUInt) 
{
#ifdef I3D_BIG_ENDIAN
   int oInt = Byteswap::byteswap(*pUInt);
   return Read((void*)&oInt, sizeof(unsigned int));
#else
   return Read((void*)pUInt, sizeof(unsigned int));
#endif
};

int   IStreamIn::ReadInt64(int64* pInt64) 
{
#ifdef I3D_BIG_ENDIAN
   int64 oInt = Byteswap::byteswap(*pInt64);
   return Read((void*)&oInt, sizeof(int64));
#else
   return Read((void*)pInt64, sizeof(int64));
#endif
};

int   IStreamIn::ReadUInt64(uint64* pUInt64) 
{
#ifdef I3D_BIG_ENDIAN
   int64 oInt = Byteswap::byteswap(*pUInt64);
   return Read((void*)&oInt, sizeof(uint64));
#else
   return Read((void*)pUInt64, sizeof(uint64));
#endif
};

int   IStreamIn::ReadFloat(float* pFloat) 
{
#ifdef I3D_BIG_ENDIAN
   int64 oFloat = Byteswap::byteswap(*pFloat);
   return Read((void*)&oFloat, sizeof(float));
#else
   return Read((void*)pFloat, sizeof(float));
#endif
};

int   IStreamIn::ReadDouble(double* pDouble) 
{
#ifdef I3D_BIG_ENDIAN
   int64 oDouble = Byteswap::byteswap(*pDouble);
   return Read((void*)&oDouble, sizeof(double));
#else
   return Read((void*)pDouble, sizeof(double));
#endif
};


int   IStreamIn::ReadString(std::string* pString)
{
   pString->clear();
   int cnt = 0;
   unsigned int slen;
   cnt = ReadUInt(&slen);
   if (cnt == 0)
      return 0;
   for (unsigned int i=0;i<slen;i++)
   {
      char c;
      cnt += ReadByte(&c);
      *pString += c;
   }
   return cnt;
}

int   IStreamIn::ReadClass(IStreamable* pStreamable)
{
   return pStreamable->Read(this);
}



int   IStreamIn::ReadVec3(vec3<double>* pVec) 
{
   int r = 0;
   r += ReadDouble(&pVec->x);
   r += ReadDouble(&pVec->y);
   r += ReadDouble(&pVec->z);

   return r;
};

int   IStreamIn::ReadVec2(vec2<double>* pVec) 
{
   int r = 0;
   r += ReadDouble(&pVec->x);
   r += ReadDouble(&pVec->y);

   return r;
};