


#include "FileSystemWriterDisk.h"
#include "StreamOutFile.h"
#include "string/FilenameUtils.h"
#include "ogException.h"
#include <boost/shared_array.hpp>

/*
   EXAMPLE
   =======

   boost::shared_array<unsigned char> data;
   size_t data_size;

   FileSystemReaderDisk fsrd(L"c:/base");
   fsrd.ReadData(L"info.txt", data, data_size);

   FileSystemWriterDisk fswd(L"c:/base");
   fswd.WriteData(L"copy of info.txt", data, data_size);

*/

FileSystemWriterDisk::FileSystemWriterDisk(const std::string& sRoot)
{
   _sRoot = FilenameUtils::DelimitPath(sRoot);
}

FileSystemWriterDisk::~FileSystemWriterDisk()
{

}

bool FileSystemWriterDisk::WriteData(const std::string& sRelativePath, const boost::shared_array<unsigned char>& data, const size_t& data_size)
{
   if (!data.get() || data_size == 0)
      return false;

   std::string sPath = _sRoot + sRelativePath;

   boost::shared_ptr<IStreamOut> qStreamOut = boost::shared_ptr<IStreamOut>(new StreamOutFile());

   StreamOutFile* pStreamout = (StreamOutFile*)qStreamOut.get();
   if (pStreamout)
   {
      try
      {
         pStreamout->Open(sPath);
         pStreamout->Write(data.get(), data_size);
         pStreamout->Close();
         return true;
      }
      catch (ogException e)
      {
         return false;
      }
   }

   return false;
}

void FileSystemWriterDisk::CloseOutputStream()
{
   // not required when writing files to disk
}