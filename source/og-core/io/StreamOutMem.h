/*******************************************************************************
Project       : i3D Studio  
Purpose       : File Handling
Creation Date : 5.4.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2007 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _STREAMOUTMEM_H_
#define _STREAMOUTMEM_H_


#include "og.h"
#include <fstream>
#include <cassert>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include "ogException.h"
#include "IStreamOut.h"
#include "memory/MemoryChunk.h"

//namespace i3d
//{
//! \class StreamOutMem
//! \brief Write binary data to memory
//! Memory based binary data writer.
//! \ingroup iostreams
//! \author Martin Christen, martin.christen@fhnw.ch
	class OPENGLOBE_API StreamOutMem : public IStreamOut
	{
	public:
		StreamOutMem();
		virtual ~StreamOutMem();

      // Attach EXISTING Memory (deprecated)
      //void Attach(boost::shared_array<unsigned char>& oData, size_t data_size) ;

      // Clear All Data (actually calls "Close" but this often makes more sense in a semantical way)
      void  Reset();

      // Retrieve all data written (in one buffer)
      void GetBuffer(boost::shared_array<unsigned char>& buf, size_t& nSize);

		/*!	Writes the given byte buffer to the file stream.
		*		\param pBuffer	bytes to write.
		*     \param nSize size of pBuffer.
		*		\throw Exception if an error occurs.
		*/
		void Write(void* pBuffer, size_t nSize) ;

		/*!   Reflect changes in the file.
		 */
		void Flush() ;

		/*!	Returns the current value of the stream's position indicator.
		*		\param	nPos Position-indicator storage.
		*		\throw	Exception if an error occurs.
		*/
		void	GetPos(int64 &nPos) ;

		/*!	Sets the stream's position indicator to the value nPos.
		*		\param	nPos Position-indicator storage.
		*		\throw	Exception if an error occurs.
		*/
		void	SetPos(int64 nPos) ;

		/*!	Closes the stream.
		*/
		void	Close() throw ();

		/*!	Gets the path of the stream.
		*		\return	path
		*/
		virtual std::string	GetPath();

   protected:
      void     MergeMemory();   // Merge All Memory Chunks to one large chunk.
      size_t   _pos;
      size_t   _max_size;
      boost::shared_array<unsigned char> _oData;

      // Memory Chunks to store data
      std::list<SMemoryChunk> _lstMemory;

	};


   
//}
#endif
