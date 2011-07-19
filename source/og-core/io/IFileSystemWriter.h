/*******************************************************************************
Project       : i3D Studio  
Purpose       : File Handling
Creation Date : 29.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2006 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _IFILESYSTEMWRITER_H_
#define _IFILESYSTEMWRITER_H_

#include "og.h"
#include <string>
#include <exception>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include "IStreamOut.h"


//namespace i3d
//{
	//! \interface IFileSystemWriter
   //! \brief Abstraction of File System for writing
	//! Interface for writing files to a file system.
   //! \author Martin Christen, martin.christen@fhnw.ch
   //! \ingroup filesystem
class OPENGLOBE_API IFileSystemWriter 
	{
	public:

      //! \brief Write data from memory to file
      //! Write data from memory to a file based representation
      //! \param sRelativePath Relative Path. Example: 'data/bla/file.txt'.
      //! \param data memory to write (as shared_array)
      //! \param data_size size of memory in bytes
      virtual bool WriteData(const std::string& sRelativePath, const boost::shared_array<unsigned char>& data, const size_t& data_size) = 0;

		
		//! Closes the output stream after writing.
      virtual void CloseOutputStream() = 0;
	};
//}
#endif
