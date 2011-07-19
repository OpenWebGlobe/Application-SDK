/*******************************************************************************
Project       : i3d
Purpose       : Calculate Hash Value from String
Creation Date : 12.02.2007
Author        : Martin Christen
Copyright     : This source code file is copyright (c) FHNW 2007
*******************************************************************************/

#include "StringHash.h"


uint64 StringHash::crc64(const std::string& s) 
{
   const char *p = s.c_str();
   size_t size = sizeof(char) * strlen(p);
   return Crypto::crc64(p, size);
}

unsigned int StringHash::crc32(const std::string& s) 
{
   const char *p = s.c_str();
   size_t size = sizeof(char) * strlen(p);
   return Crypto::crc32(p, size);
}

MD5Hash StringHash::md5(const std::string& s) 
{
   const char *p = s.c_str();
   size_t size = sizeof(char) * strlen(p);
   return Crypto::md5(p, size);
}
 