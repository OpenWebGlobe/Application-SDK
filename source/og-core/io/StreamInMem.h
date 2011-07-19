/*******************************************************************************
Project       : i3d
Purpose       : Memory File
Creation Date : 23.03.2007
Author        : Martin Christen
Copyright     : This source code file is copyright (c) FHNW 2006
*******************************************************************************/


#ifndef _STREAMINMEM_H_
#define _STREAMINMEM_H_

#include "IStreamIn.h"

#include "og.h"
#include <boost/shared_array.hpp>
#include <cassert>
#include "ogException.h"

//namespace i3d
//{

//! \class StreamInMem
//! \brief Read binary data from memory
//! Memory based binary data reader.
//! \ingroup iostreams
//! \author Martin Christen, martin.christen@fhnw.ch
class OPENGLOBE_API StreamInMem : public IStreamIn
{
public:

   /*! Standard constructor.
   */
   StreamInMem() throw ();

   /*!	Destructor.
   */
   virtual ~StreamInMem() ;

   /*!	Attach memory. This memory is *not* under control of this class.
   *		\param	pMem	memory to attach
   *     \param   size  size of memory in bytes
   *		\throw	Exception if an error occurs.
   */
   void Attach(boost::shared_array<unsigned char>& pMem, size_t size) ;

   /*!	Reads bytes from the file stream.
   *		\param	pBuffer the buffer into which the data is read
   *		\param	nSize maximum number of bytes to read
   *		\return	number of bytes read, -1 at the end of the stream.
   *		\throw	Exception if an error occurs.
   */
   virtual int	Read(void* pBuffer, size_t nSize) ;

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
   *		@return	path
   */
   virtual std::string	GetPath();

protected:
   int64    _pos;
   boost::shared_array<unsigned char> _mem;
   size_t   _mem_size;
};

//}
#endif
