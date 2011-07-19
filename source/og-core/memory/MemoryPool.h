
#ifndef _MEMORYPOOL_H
#define _MEMORYPOOL_H

#include "og.h"
#include <stack>

template <class T>
class OPENGLOBE_API MemoryPool
{
public:
   //---------------------------------------------------------------------------
   //! \brief Create Memory Pool holding nElements.
   //! \param nElement number of Elements to initialize pool
   //! \param bGrow set true if memory pool may grow
   MemoryPool(size_t nElements, bool bGrow=true)
   {
      _bGrow = bGrow;
      for (size_t i=0;i<nElements;i++)
      {
         T* pNew = new T();
         if (pNew)
         {
            _StackFree.push(pNew);
            _nAllocCount++;
         }
      }
   }
   
   //---------------------------------------------------------------------------
   //! \brief Destructor. Free Memory!
   ~MemoryPool()
   {
      while (_StackFree.size()>0)
      {
         delete _StackFree.top();
         _StackFree.pop();
         _nAllocCount--;
      } 
      
      assert(_nAllocCount != 0); // memory leak!!
   }
   
   //---------------------------------------------------------------------------
   // \brief Get Memory from pool. Returns 0 if there is no memory available
   T* Alloc()
   {
      if (_StackFree.size()>0)
      {
         T* ret = _StackFree.top();
         _StackFree.pop();
         return ret;
      }
      else
      {
            if (_bGrow)
            {
               T* pNew = new T();
               if (pNew)
               {
                  _nAllocCount++;
               }
               return pNew;
               
            }
            else
            {
               return 0; // no memory!
            }
      }
      
   }
   
   //---------------------------------------------------------------------------
   //! \brief Release Memory. Puts the memory back to the pool.
   void Release(T* element)
   {
      _StackFree.push(element);
   }
   //---------------------------------------------------------------------------

protected:
   bool              _bGrow;
   int               _nAllocCount;
   std::stack<T*>    _StackFree;
};




#endif

