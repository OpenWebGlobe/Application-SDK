/*******************************************************************************
Project       : i3D Studio  
Purpose       : File Handling
Creation Date : 29.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2006 by FHNW. All rights reserved.
*******************************************************************************/
 

#ifndef _IFILESYSTEMREADER_H_
#define _IFILESYSTEMREADER_H_

#include "og.h"
#include <string>
#include <exception>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include "IStreamIn.h"

//namespace i3d
//{

//! \interface IFileSystemReader
//! \brief Abstraction of File System for reading
//! Interface for reading files from a file system.
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup filesystem
class OPENGLOBE_API IFileSystemReader
	{
	public:
      /*!
		 * \brief Copy file to memory
		 * \param sRelativePath relative path to file
		 * \param data the actual data (returned as shared array)
		 * \param data_size size in bytes
		 * \return 
		 */
		virtual bool ReadData(const std::wstring& sRelativePath, boost::shared_array<unsigned char>& data, size_t& data_size) = 0;
   };
//}
#endif

