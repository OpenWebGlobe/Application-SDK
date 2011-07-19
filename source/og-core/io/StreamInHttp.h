/*******************************************************************************
Project       : i3D Studio  
Purpose       : HTTP Download
Creation Date : 20.8.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2006 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _StreamInHttp_H_
#define _StreamInHttp_H_

#include "IStreamIn.h"

#include "og.h"
#include <fstream>
#include <cassert>
#include "data/DiskCache.h"
#include "ogException.h"

//namespace i3d
//{
   //! \brief Download and read data from the web
	//! Read data from a http:// source (webpage or file).
   //! \ingroup iostreams
   //! \author Martin Christen, martin.christen@fhnw.ch
	class OPENGLOBE_API StreamInHttp : public IStreamIn
	{
	public:

		/*! Standard constructor.
		*/
		StreamInHttp(Cache* pDiskCache) throw ();

		/*!	Destructor.
		*/
		virtual ~StreamInHttp();

		/*!	Open the http stream.
		*		\param	strURL	file to open
		*		\throw	Exception if an error occurs.
		*/
      void Open(const std::string& strURL);

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


      std::streamsize GetFileSize();

	
		/*!	Gets the path of the stream.
		*		@return	path
		*/
      virtual std::string	GetPath();

	protected:
      StreamInHttp(){}
      std::string		   _strPath;
      std::fstream      _oStream;
      Cache*            _pDiskCache;
	};

//}
#endif
