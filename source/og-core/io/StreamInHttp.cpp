/*******************************************************************************
Project       : i3D Studio  
Purpose       : HTTP Download
Creation Date : 20.8.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2006 by FHNW. All rights reserved.
*******************************************************************************/

#include "StreamInHttp.h"
#include "FileSystem.h"
#include "ogException.h"

StreamInHttp::StreamInHttp(Cache* pDiskCache) throw ()
{
   _pDiskCache = pDiskCache;
}

StreamInHttp::~StreamInHttp() 
{
#ifdef _DEBUG
   if (_oStream.is_open())
      assert(false);    // if it breaks here you forgot to close the file!
#endif
   Close();          // Anyways, make sure the file is closed.
}

void StreamInHttp::Open(const std::string& strURL) 
{
   _strPath = strURL;
 /*  std::wstring strFileName;

   if (FileSystem::DownloadURL(_strPath, 0))
   {
      if (!FileSystem::FindInternetCache(_strPath, strFileName))
      {
         throw ogException(1, std::string("Failed finding internet cache entry for: ") + strURL);
         return;
      }
   }
   else
   {
      throw ogException(1, std::string("Failed downloading URL: ") + strURL);
      return;
   }

   std::ios_base::openmode mode = std::ios::in | std::ios::binary;
#ifdef OS_WINDOWS
   _oStream.open(strFileName.c_str(), mode);
#else
   std::string s_ascii(strFileName.begin(), strFileName.end());
   _oStream.open(s_ascii.c_str(), mode);
#endif
 
   if (!_oStream.good())
      throw ogException(1, std::wstring(L"Failed opening file: ") + strFileName);
*/
}

int StreamInHttp::Read(void* pBuffer, size_t nSize) 
{
   if (!_oStream.is_open())
      throw ogException(2, std::string("File is not open"));

   if (_oStream.eof())
      return -1;

   _oStream.read((char*)pBuffer, (std::streamsize)nSize);

   //if (!_oStream.good())
   //   throw ogException(1, std::string("Failed reading from file ") + _strPath);

   return (int)_oStream.gcount();
}

void StreamInHttp::GetPos(int64 &nPos) 
{
   if (_oStream.is_open())
   {
      nPos = _oStream.tellg();
   }
   else
   {
      throw ogException(2, std::string("File is not open"));
   }
}

void StreamInHttp::SetPos(int64 nPos) 
{
   if (_oStream.is_open())
   {
      _oStream.seekg((long)nPos, std::ios::beg);
   }
   else
   {
      throw ogException(2, std::string("File is not open"));
   }
}

void StreamInHttp::Close() throw ()
{
   if (_oStream.is_open())
   {
      _oStream.close();
   }
}

std::string StreamInHttp::GetPath()
{
   return _strPath;
}



std::streamsize StreamInHttp::GetFileSize() 
{
   if(!_oStream.good())
   {   
      throw ogException(2, std::string("File is not open"));
   }

   std::streamsize pos=_oStream.tellg();
   _oStream.seekg(0,std::ios::end);
   std::streamsize len = _oStream.tellg();
   _oStream.seekg(pos);

   return len;
}