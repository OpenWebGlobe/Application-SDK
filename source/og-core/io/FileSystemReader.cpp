


#include "FileSystemReader.h"
#include "StreamInFile.h"
#include "StreamInHttp.h"
#include "string/FilenameUtils.h"

#include <list>
#include <io/FileSystem.h>
#include "ogException.h"
#include "memory/MemoryChunk.h"

/*
   Example Code:
   =============

   FileSystemReader fsrd(L"c:/base");

   boost::shared_array<unsigned char> data;
   size_t data_size;

   fsrd.ReadData(L"info.txt", data, data_size);

*/

FileSystemReader::FileSystemReader(Cache* pDiskCache, std::string sRoot)
{
   _pDiskCache = pDiskCache;
   _sRoot = FilenameUtils::DelimitPath(sRoot);
}

FileSystemReader::~FileSystemReader()
{

}

bool FileSystemReader::ReadData(const std::string& sRelativePath, boost::shared_array<unsigned char>& data, size_t& data_size)
{
   std::string sTotalString = _sRoot + sRelativePath;
   boost::shared_ptr<IStreamIn> ppStreamIn;
   
   if (FilenameUtils::IsFileOnWeb(sTotalString))
   {
      ppStreamIn = boost::shared_ptr<IStreamIn>(new StreamInHttp(_pDiskCache));
      if (ppStreamIn)
      {
         try
         {
            ((StreamInHttp*)ppStreamIn.get())->Open(sTotalString);
         }
         catch (ogException e)
         {
            return false;
         }
      }

   }
   else
   {
      if (!FileSystem::FileExists(sTotalString))
         return false;

      ppStreamIn = boost::shared_ptr<IStreamIn>(new StreamInFile());
      if (ppStreamIn)
      {
         try
         {
            ((StreamInFile*)ppStreamIn.get())->Open(sTotalString);
         }
         catch (ogException e)
         {
            return false;
         }
      }
   }

   //StreamInFile* pStreamin = (StreamInFile*)ppStreamIn.get();
   if (ppStreamIn)
   {
      size_t total_size = 0;
      data_size = 0;
      int read_buffer_size = 16384;
      std::list< SMemoryChunk > lstMemoryChunks;
      int nRead;

      boost::shared_array<unsigned char> read_buffer(new unsigned char[read_buffer_size]);
      while ((nRead = ppStreamIn->Read(read_buffer.get(), read_buffer_size)) > 0)
      {

         boost::shared_array<unsigned char> temp_buffer(new unsigned char[nRead]);
         SMemoryChunk chunk;

         memcpy(temp_buffer.get(), read_buffer.get(), nRead);

         chunk.size = nRead;
         chunk.mem  = temp_buffer;
         lstMemoryChunks.push_back(chunk);

         total_size += nRead;
      }

      ppStreamIn->Close();

      //now create one large chunk of memory

      if (total_size>0)
      {
         boost::shared_array<unsigned char> final_buffer(new unsigned char[total_size]);


         unsigned int offset = 0;

         std::list< SMemoryChunk >::iterator i = lstMemoryChunks.begin();

         while (i!=lstMemoryChunks.end())
         {
            memcpy(final_buffer.get() + offset, (*i).mem.get(), (*i).size);
            offset += (*i).size;
            ++i;
         }

         data = final_buffer;
         data_size = total_size;

         return true;
      }
      else
      {
         return false;
      }
   }

   return false; 
}