

#include "StreamInMem.h"
#include "ogException.h"

StreamInMem::StreamInMem() throw ()
{
   _pos = 0;
   _mem_size = 0;
}


StreamInMem::~StreamInMem() 
{

}


void StreamInMem::Attach(boost::shared_array<unsigned char>& pMem, size_t size) 
{
   _mem = pMem;
   _mem_size = size;
   _pos = 0;
}



int	StreamInMem::Read(void* pBuffer, size_t nSize) 
{
   if (_pos>=_mem_size || _pos<0)
      return -1;

   size_t real_size = nSize;
   if (_pos+nSize>=_mem_size)
   {
      real_size = _mem_size-(size_t)_pos;
   }

   memcpy(pBuffer, (_mem.get())+_pos, real_size);
   _pos+=real_size;

   return real_size;

   //-----------------------------------------------
   // Beispiel:
   //   Buffer-Grösse = 10 (_mem_size)
   //   pos = 5;  [0,1,2,3,4,*5*,6,7,8,9]
   //
   //  Read(buf, 15)
   //
   //   pos+15 = 20, ist grösser als 10
   //   real_size = 10-5 = 5
   //   Es werden also nur 5 Bytes gelesen!
   //-----------------------------------------------
}


void	StreamInMem::GetPos(int64 &nPos) 
{
   nPos = _pos;
}


void	StreamInMem::SetPos(int64 nPos) 
{
   _pos = nPos;
}


void	StreamInMem::Close() throw ()
{
   _pos = 0;
   _mem.reset();
   _mem_size = 0;
}



std::string StreamInMem::GetPath()
{
   return std::string("@MEMORY_STREAM@");
}