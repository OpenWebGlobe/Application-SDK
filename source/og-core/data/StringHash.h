/*******************************************************************************
Project       : i3d
Purpose       : Calculate Hash Value from String
Creation Date : 12.02.2007
Author        : Martin Christen
Copyright     : This source code file is copyright (c) FHNW 2007
*******************************************************************************/

#ifndef _STRING_HASH_H
#define _STRING_HASH_H

#include "og.h"
#include <string>
#include "Crypto.h"


//! \ingroup datamanagement
class OPENGLOBE_API StringHash 
{
public:
   static uint64 crc64(const std::string& s); 
   static unsigned int crc32(const std::string& s); 
   static MD5Hash md5(const std::string& s);

}; 


#endif

