/*******************************************************************************
Project       : i3D Studio  
Purpose       : File Handling
Creation Date : 5.4.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/


#include "StreamOutMem.h"
#include "ogException.h"

StreamOutMem::StreamOutMem()
{
   _max_size = 0;
   _pos = 0;
}

StreamOutMem::~StreamOutMem()
{
   Close();
}

/*void StreamOutMem::Attach(boost::shared_array<unsigned char>& oData, size_t data_size) 
{
   _oData = oData;
   _max_size = data_size;
   _pos = 0;
 
}*/

void StreamOutMem::Reset()
{
   Close();
}

void StreamOutMem::Write(void* pBuffer, size_t nSize) 
{
  
   
   if (_pos>=_max_size) // EOF -> Reserver more memory
   {
      SMemoryChunk tmpMemChunk;
      tmpMemChunk.size = nSize;
      tmpMemChunk.mem = boost::shared_array<unsigned char>(new unsigned char[nSize]);
      memcpy(tmpMemChunk.mem.get(), pBuffer, nSize);
      _lstMemory.push_back(tmpMemChunk);
      _max_size += nSize;
      _pos += nSize;
      return;
   }
   else if (_pos+nSize>_max_size)
   {
      SMemoryChunk tmpMemChunk;
      
      size_t newsize = _pos+nSize-_max_size;
      tmpMemChunk.mem = boost::shared_array<unsigned char>(new unsigned char[newsize]);
      tmpMemChunk.size = newsize;
      _lstMemory.push_back(tmpMemChunk);
      _max_size += newsize;
      MergeMemory();
      memcpy(_oData.get()+_pos, pBuffer, nSize);
      _pos += nSize;
   }
   else
   {
      MergeMemory();
      memcpy(_oData.get()+_pos, pBuffer, nSize);
      _pos += nSize;
   }
}

void StreamOutMem::Flush() 
{
   // Does Nothing
}

void StreamOutMem::GetPos(int64 &nPos) 
{
   nPos = (int64) _pos;
}

void StreamOutMem::SetPos(int64 nPos) 
{
   _pos = (size_t) nPos;
}

void StreamOutMem::Close() throw ()
{
   //Free all Memory
   _lstMemory.clear();
   _pos = 0;
   _oData.reset();
   _max_size = 0;
}

std::string StreamOutMem::GetPath()	
{
   return "data@memory";
}

void StreamOutMem::MergeMemory()
{
   if (_lstMemory.size()<1)
      return;

   std::list<SMemoryChunk>::iterator i = _lstMemory.begin();

   size_t size = 0;

   while (i!=_lstMemory.end())
   {
      size += (*i).size;
      i++;
   }

   if (size>0)
   {
      size_t pos = 0;
      _oData = boost::shared_array<unsigned char>(new unsigned char[size]);
      i = _lstMemory.begin();
      while (i!=_lstMemory.end())
      {
         memcpy(_oData.get()+pos, (*i).mem.get(), (*i).size);
         pos += (*i).size;
         i++;
      }
      _lstMemory.clear();
      SMemoryChunk tmpMemChunk;
      tmpMemChunk.size = size;
      tmpMemChunk.mem = _oData;
      _lstMemory.push_back(tmpMemChunk);
      _max_size = size;
   }

}


void StreamOutMem::GetBuffer(boost::shared_array<unsigned char>& buf, size_t& nSize)
{
   MergeMemory();
   nSize = _max_size;
   buf = _oData;
}




