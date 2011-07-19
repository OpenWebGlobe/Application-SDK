/*******************************************************************************
Project       : File Utilities
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "FileSystem.h"
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>

//------------------------------------------------------------------------------

time_t FileSystem::modificationtime(const std::string& sFile)
{
   return boost::filesystem::last_write_time(sFile);
}

//------------------------------------------------------------------------------

bool FileSystem::makedir(const std::string& sPath)
{
   return boost::filesystem::create_directory(sPath);
};
//------------------------------------------------------------------------------

bool FileSystem::makeallsubdirs(const std::string& sFilename)
{
   bool result = true;
   for (size_t i = 0; i < sFilename.length(); i++) 
   {
      std::string s=sFilename.substr(i, 1);
      if(s == "/" || s == "\\")
      {
         if(!FileSystem::DirExists(sFilename.substr(0, i))) 
         {
            result = result && FileSystem::makedir(sFilename.substr(0, i));
         }
      }
   }

   return result;
}

//------------------------------------------------------------------------------

bool FileSystem::rm(const std::string& sFile)
{
   boost::filesystem::remove(sFile);
   return !boost::filesystem::exists(sFile);
}

//------------------------------------------------------------------------------

bool FileSystem::rm_all(const std::string& sDirectory)
{
   boost::filesystem::remove_all(sDirectory);

   return !boost::filesystem::exists(sDirectory);

}

//------------------------------------------------------------------------------

bool FileSystem::rename(const std::string& sFile1, const std::string& sFile2)
{
   //return ::wxRenameFile(sFile1.c_str(), sFile2.c_str());

   boost::filesystem::rename(sFile1, sFile2);
   return (!boost::filesystem::exists(sFile1) && boost::filesystem::exists(sFile2));
}

//-----------------------------------------------------------------------------

bool FileSystem::FileExists(const std::string& sFile)
{
   return boost::filesystem::exists(sFile);
}

//-----------------------------------------------------------------------------

bool FileSystem::DirExists(const std::string& sDir)
{
   return boost::filesystem::exists(sDir) && boost::filesystem::is_directory(sDir);
}

//-----------------------------------------------------------------------------

void FileSystem::GetDiskSpace(const std::string& sPath, uintmax_t& capacity, uintmax_t& free, uintmax_t& available)
{
   boost::filesystem::space_info sp = boost::filesystem::space(sPath);

   capacity = sp.capacity;
   free = sp.free;
   available = sp.available;
}

//-----------------------------------------------------------------------------

std::vector<std::string> FileSystem::GetFilesInDirectory(const std::string& sDir, const std::string& sExtension)
{
   std::vector<std::string> vecFiles;

   std::string sPathAscii(sDir.begin(), sDir.end());
   boost::filesystem::path dir_path(sDir);

   if ( !boost::filesystem::exists(dir_path))
   {
      return vecFiles; // empty vector
   }

   boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end

   size_t extension_size = sExtension.size();

   for (boost::filesystem::directory_iterator itr( dir_path ); itr != end_itr; ++itr)
   {
      std::string sPath;
      sPath = itr->path().file_string();

      if (extension_size>0)
      {
         if (sPath.size()>extension_size)
         {
            bool bMatch = true;
            for (size_t i=0;i<extension_size;i++)
               bMatch &= sPath[sPath.size()-extension_size+i] == sExtension[i];

            if (bMatch)
               vecFiles.push_back(sPath);
         }

      }
      else
         vecFiles.push_back(sPath);
   }

   return vecFiles;
}


//-----------------------------------------------------------------------------

std::vector<std::string> FileSystem::GetFileNamesInDirectory(const std::string& sDir, const std::string& sExtension)
{
   std::vector<std::string> vecFileNames;

   boost::filesystem::path dir_path(sDir);

   if ( !boost::filesystem::exists(dir_path))
   {
      return vecFileNames; // empty vector
   }

   boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end

   size_t extension_size = sExtension.size();

   for (boost::filesystem::directory_iterator itr( dir_path ); itr != end_itr; ++itr)
   {
      std::string sFileName;
      sFileName = itr->path().filename();

      if (extension_size>0)
      {
         if (sFileName.size()>extension_size)
         {
            bool bMatch = true;
            for (size_t i=0;i<extension_size;i++)
               bMatch &= sFileName[sFileName.size()-extension_size+i] == sExtension[i];

            if (bMatch)
               vecFileNames.push_back(sFileName);
         }

      }
      else
         vecFileNames.push_back(sFileName);
   }

   return vecFileNames;
}

//-----------------------------------------------------------------------------

std::vector<std::string> FileSystem::GetSubdirsInDirectory(const std::string& sDir)
{
   std::vector<std::string> vecDirNames;

   std::string sPathAscii(sDir.begin(), sDir.end());
   boost::filesystem::path dir_path(sDir);

   if ( !boost::filesystem::exists(dir_path))
   {
      return vecDirNames; // empty vector
   }

   boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end


   for (boost::filesystem::directory_iterator itr( dir_path ); itr != end_itr; ++itr)
   {
      std::string sDirName;
      if(boost::filesystem::is_directory(itr->path()))
      {
         sDirName = itr->path().filename();
         vecDirNames.push_back(sDirName);
      }
   }
   return vecDirNames;
}

//-----------------------------------------------------------------------------

bool FileSystem::FileToMemory(const std::string& sPath, std::vector<unsigned char>& vResult)
{
   size_t nSize;
   bool bStatus = false;

   std::ifstream myfile(sPath.c_str(), std::ios::in|std::ios::binary);

   if (myfile.good())
   {
      std::streamoff begin, end;
      begin = myfile.tellg();
      myfile.seekg (0, std::ios::end);
      end = myfile.tellg();
      nSize = size_t(end-begin);

      if (nSize>0)
      {
         vResult.resize(nSize); // resize vector (if necessary)
         if (vResult.size() == nSize)
         {
            myfile.seekg(0, std::ios::beg);
            myfile.read((char*)&vResult[0], nSize);
            bStatus = true;
         }
      }
      myfile.close();
   }

   return bStatus;
}

//------------------------------------------------------------------------------

bool FileSystem::FileToAllocedMemory(const std::string& sPath, unsigned char*& pResult, size_t& nSize)
{
   pResult = 0;

   std::ifstream myfile(sPath.c_str(), std::ios::in|std::ios::binary);

   if (myfile.good())
   {
      std::streamoff begin, end;
      begin = myfile.tellg();
      myfile.seekg (0, std::ios::end);
      end = myfile.tellg();
      nSize = size_t(end-begin);

      if (nSize>0)
      {
         pResult = new unsigned char[nSize];
        
         if (pResult != 0)
         {
            myfile.seekg(0, std::ios::beg);
            myfile.read((char*)pResult, nSize);
         }
      }
      myfile.close();
   }

   return (pResult != 0);
}

//------------------------------------------------------------------------------

void FileSystem::FreeAllocedMemory(unsigned char* memory)
{
   if (memory)
   {
      delete[] memory;
   }
}

//------------------------------------------------------------------------------

std::string FileSystem::ReadLine(const std::string& sPath)
{
   bool bStatus = false;
   std::string sOut;

   std::ifstream myfile(sPath.c_str(), std::ios::in);

   if (myfile.good())
   {
      char line[4096];

      myfile.getline(line, 4095);

      sOut = line;

      myfile.close();
   }

   return sOut;
}

//------------------------------------------------------------------------------

std::vector<std::string> FileSystem::LinesToVector(const std::string& sPath)
{
   bool bStatus = false;
   std::vector<std::string> vOut;

   std::ifstream myfile(sPath.c_str(), std::ios::in);

   char line[4096];
   while (!myfile.eof())
   {
      myfile.getline(line, 4095);
      vOut.push_back(std::string(line));
   }

   myfile.close();
   return vOut;
}
