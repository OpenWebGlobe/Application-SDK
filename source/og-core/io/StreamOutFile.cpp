/*******************************************************************************
Project       : i3D Studio  
Purpose       : File Handling
Creation Date : 29.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2006 by FHNW. All rights reserved.
*******************************************************************************/


#include "StreamOutFile.h"
#include "FileSystem.h"
#include "string/FilenameUtils.h"
#include "ogException.h"

StreamOutFile::StreamOutFile()
{
}

StreamOutFile::StreamOutFile(const std::string& strFileName, bool append)
{
   Open(strFileName, append);
}

StreamOutFile::~StreamOutFile()
{
#ifdef _DEBUG
    if (_oStream.is_open())
      assert(false);    // if it breaks here you forgot to close the file!
#endif
   Close();          // Anyways, make sure the file is closed.
}


void StreamOutFile::Open(const std::string& strFileName, int hierarchyDepth, bool createDirs, bool append)
{
   std::string pHFileName = FilenameUtils::MakeHierarchicalFileName(strFileName, hierarchyDepth);
   if(createDirs) {
      for(size_t i = 0; i < pHFileName.length(); i++) 
      {
         if(pHFileName.substr(i, 1) == "/")
         {
            if(!FileSystem::DirExists(pHFileName.substr(0, i))) 
            {
               FileSystem::makedir(pHFileName.substr(0, i));
            }
         }
      }
   }
   Open(pHFileName, append);
}

void StreamOutFile::Open(const std::string& strFileName, bool append) 
{
   _strPath = strFileName;
   std::ios_base::openmode mode = std::ios::out | std::ios::binary;

   if (append)
      mode |= std::ios::app;

#ifdef OS_WINDOWS
   _oStream.open(strFileName.c_str(), mode);
#else
   std::string s_ascii(strFileName.begin(), strFileName.end());
   _oStream.open(s_ascii.c_str(), mode);
#endif

   if (!_oStream.good())
      throw ogException(1, std::string("Failed opening file"));

}

void StreamOutFile::Write(void* pBuffer, size_t nSize) 
{
   if (_oStream.is_open())
   {
      _oStream.write((const char*)pBuffer, nSize);
      if (!_oStream.good())
         throw ogException(1, std::string("Failed writing to file"));
   }
   else
   {   
      throw ogException(2, std::string("File is not open"));
   }
}

void StreamOutFile::Flush() 
{
   if (_oStream.is_open())
   {
      _oStream.flush();
   }
   else
   {
      throw ogException(2, std::string("File is not open"));
   }
}

void StreamOutFile::GetPos(int64 &nPos) 
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

void StreamOutFile::SetPos(int64 nPos) 
{
   if (_oStream.is_open())
   {
      _oStream.seekg(nPos, std::ios::beg);
   }
   else
   {
      throw ogException(2, std::string("File is not open"));
   }
}

void StreamOutFile::Close() throw ()
{
   if (_oStream.is_open())
   {
      _oStream.close();
   }
}

std::string StreamOutFile::GetPath()	
{
   return _strPath;
}

std::fstream&  StreamOutFile::GetStream()
{
   return _oStream;
}


