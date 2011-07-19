/*******************************************************************************
Project       : i3D Studio  
Purpose       : File Handling
Creation Date : 29.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2006 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _STREAMOUTFILE_H_
#define _STREAMOUTFILE_H_


#include "og.h"
#include <fstream>
#include <cassert>
#include "ogException.h"
#include "IStreamOut.h"

//namespace i3d
//{
   //! \class StreamOutFile
   //! \brief Output stream for binary files
   //! This class is for writing a binary file. Data is stored in little endian (even on big endian machines)
   //! \ingroup iostreams file
   //! \author Martin Christen, martin.christen@fhnw.ch
	class OPENGLOBE_API StreamOutFile : public IStreamOut
	{
	public:
		StreamOutFile();
      StreamOutFile(const std::string& strFileName, bool append = false);

		virtual ~StreamOutFile();

		/*!	Open the file stream.
		*		\param	strFileName	   file to open
      *		\param   append         if true, opens file in append mode
		*		\throw	Exception      if an error occurs.
		*/
      void Open(const std::string& strFileName, bool append = false) ;

      /*!	Open the file stream.
		*		\param	strFileName	   file to open
      *		\param   hierarchyDepth open file from hierarchical directorystructure
      *		\param   createDirs define if the hierarchical directorystructure shall be created
      *		\param   append         if true, opens file in append mode
		*/
      void Open(const std::string& strFileName, int hierarchyDepth, bool createDirs = false, bool append = false);



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

      /*!	Returns the C++ file stream
      *		\return	stream
      */
      std::fstream&  GetStream();

	protected:
      std::string       _strPath;
      std::fstream      _oStream;
	};


   
//}
#endif
