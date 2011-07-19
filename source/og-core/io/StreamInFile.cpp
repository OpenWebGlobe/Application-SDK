/*******************************************************************************
Project       : i3D Studio  
Purpose       : File Handling
Creation Date : 29.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2006 by FHNW. All rights reserved.
*******************************************************************************/


#include "StreamInFile.h"
#include "string/FilenameUtils.h"
#include "string/StringUtils.h"
#include "ogException.h"

StreamInFile::StreamInFile() throw ()
{
#ifdef LOCKFREE_FILES
   _oFile = 0;
#endif
}

StreamInFile::~StreamInFile() 
{
#ifdef LOCKFREE_FILES
   if (_oFile)
      Close();
#else
#ifdef _DEBUG
   if (_oStream.is_open())
      assert(false);    // if it breaks here you forgot to close the file!
#endif
   Close();          // Anyways, make sure the file is closed.
#endif
}


void StreamInFile::Open(const std::string& strFileName, int hierarchyDepth)
{
   std::string pHFileName = FilenameUtils::MakeHierarchicalFileName(strFileName, hierarchyDepth);
   Open(pHFileName);
}


void StreamInFile::Open(const std::string& strFileName) 
{
#ifdef LOCKFREE_FILES
   _strPath = strFileName;
   std::string s_ascii(strFileName.begin(), strFileName.end());
#ifdef OS_WINDOWS   
   std::wstring sFileW = StringUtils::Utf8_To_wstring(strFileName);
   _oFile = _wfopen(sFileW.c_str(), L"rb");
#else
   _oFile = fopen(s_ascii.c_str(), "rb");
#endif
   if (_oFile == 0)
   {
      throw ogException(1, std::string("Failed opening file: ") + strFileName);
   }
#else
   _strPath = strFileName;
   std::ios_base::openmode mode = std::ios::in | std::ios::binary;
#ifdef OS_WINDOWS
   _oStream.open(strFileName.c_str(), mode);
#else
   std::string s_ascii(strFileName.begin(), strFileName.end());
   _oStream.open(s_ascii.c_str(), mode);
#endif

   if (!_oStream.good())
      throw ogException(1, std::string("Failed opening file: ") + strFileName);
#endif
}

int StreamInFile::Read(void* pBuffer, size_t nSize) 
{
#ifdef LOCKFREE_FILES
   if (_oFile == 0)
   {
      throw ogException(2, std::string("File is not open"));
   }
   return (int)_fread_nolock(pBuffer, 1, nSize, _oFile);
#else
   if (!_oStream.is_open())
      throw ogException(2, std::string("File is not open"));

   if (_oStream.eof())
      return -1;

   _oStream.read((char*)pBuffer, (std::streamsize)nSize);

   //if (!_oStream.good())
   //   throw ogException(1, std::string("Failed reading from file ") + _strPath);

   return _oStream.gcount();
#endif
}

void StreamInFile::GetPos(int64 &nPos) 
{
#ifdef LOCKFREE_FILES
   if (_oFile == 0)
   {
      throw ogException(2, std::string("File is not open"));
   }
   else
   {   
      nPos = _ftell_nolock(_oFile);
   }
#else
   if (_oStream.is_open())
   {
      nPos = _oStream.tellg(); 
   }
   else
   {
      throw ogException(2, std::string("File is not open"));
   }
#endif
}

void StreamInFile::SetPos(int64 nPos) 
{
#ifdef LOCKFREE_FILES
   if (_oFile == 0)
   {
      throw ogException(2, std::string("File is not open"));
   }
   else
   {
      _fseek_nolock(_oFile, nPos, 0);
   }
#else
   if (_oStream.is_open())
   {
      _oStream.seekg((long)nPos, std::ios::beg);
   }
   else
   {
      throw ogException(2, std::string("File is not open"));
   }
#endif
}

void StreamInFile::Close() throw ()
{
#ifdef LOCKFREE_FILES
   if (_oFile)
   {
      _fclose_nolock(_oFile);
      _oFile = 0;
   }
#else
   if (_oStream.is_open())
   {
      _oStream.close();
   }
#endif
}

std::string StreamInFile::GetPath()
{
   return _strPath;
}



std::streamsize StreamInFile::GetFileSize() 
{
#ifdef LOCKFREE_FILES
   if (_oFile == 0)
   {
      throw ogException(2, std::string("File is not open"));
   }
   else
   {
      std::streamsize pos = _ftell_nolock(_oFile);
      _fseek_nolock(_oFile, 0 , SEEK_END);
      std::streamsize len = _ftell_nolock(_oFile);
      _fseek_nolock(_oFile, pos, SEEK_SET);
      return len;
   }
#else
   if(!_oStream.good())
   {   
      throw ogException(2, std::string("File is not open"));
   }

   std::streamsize pos=_oStream.tellg();
   _oStream.seekg(0,std::ios::end);
   std::streamsize len = _oStream.tellg();
   _oStream.seekg(pos);

   return len;
#endif
}