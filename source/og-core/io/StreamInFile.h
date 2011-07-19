/*******************************************************************************
Project       : i3D Studio  
Purpose       : File Handling
Creation Date : 29.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2006 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _STREAMINFILE_H_
#define _STREAMINFILE_H_

#include "IStreamIn.h"

#include "og.h"
#include <fstream>
#include <cassert>
#include <cstdio>
#include "ogException.h"

#define LOCKFREE_FILES

   //! \class StreamInFile
   //! \brief Input stream for binary files
	//! This class is for reading a binary file. Data must be stored in little endian.
   //! \ingroup iostreams file
   //! \author Martin Christen, martin.christen@fhnw.ch
	class OPENGLOBE_API StreamInFile : public IStreamIn
   { 
	public:

		/*! Standard constructor.
		*/
		StreamInFile();

		/*!	Destructor.
		*/
		virtual ~StreamInFile() ;

		/*!	Open the file stream.
		*		\param	strFileName	file to open
		*		\throw	Exception if an error occurs.
		*/
      void Open(const std::string& strFileName) ;

            /*!	Open the file stream.
		*		\param	strFileName	   file to open
      *		\param   hierarchyDepth open file from hierarchical directorystructure
		*/
      void Open(const std::string& strFileName, int hierarchyDepth);


		//! Reads bytes from the file stream.
		//! \param	pBuffer the buffer into which the data is read
		//! \param	nSize maximum number of bytes to read
		//! \return	number of bytes read, -1 at the end of the stream.
		//! \throw	Exception if an error occurs.
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


      std::streamsize GetFileSize() ;

	
		/*!	Gets the path of the stream.
		*		@return	path
		*/
      virtual std::string	GetPath();

	protected:
      std::string		_strPath;
#ifdef LOCKFREE_FILES
      FILE*             _oFile;
#else
      std::fstream      _oStream;
#endif
	};

#endif
