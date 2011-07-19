/*******************************************************************************
Project       : i3D Studio  
Purpose       : File Handling
Creation Date : 29.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2006 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _ISTREAMIN_H_
#define _ISTREAMIN_H_

#include "og.h"
#include <string>
#include <exception>
#include "math/vec3.h"
#include "math/vec2.h"
#include "ogException.h"

class IStreamable;

   //! \interface IStreamOut
   //! \brief Interface to input stream.
   //! An interface to an input stream for reading data. Input streams can be files, zip-archives, network, ...
   //! \ingroup iostreams
   //! \author Martin Christen, martin.christen@fhnw.ch
class OPENGLOBE_API IStreamIn
	{
	public:
      //! Empty Constructor
      IStreamIn(){}

      //! Empty Destructor
      virtual ~IStreamIn(){}

		//! \brief Reads bytes from the file stream.
      //! Read nSize Bytes of data and store in buffer pBuffer
		//! \param	pBuffer  the buffer into which the data is read
		//! \param	nSize    maximum number of bytes to read
		//! \return	number of bytes read, -1 at the end of the stream.
		//! \throw	Exception if an error occurs.
		virtual int	   Read(void* pBuffer, size_t nSize)  = 0;

      //! Read a byte (character) from the stream
      virtual int   ReadByte(char* pByte) ;
      //! Read a unsigned char from the stream
      virtual int   ReadUByte(unsigned char* pUByte);
      //! Read a short from the stream
      virtual int   ReadShort(short* pUByte);
      //! Read an unsigned short from the stream
      virtual int   ReadUShort(unsigned short* pUByte);
      //! Read an int from the stream
      virtual int   ReadInt(int* pInt);
      //! Read a unsigned int from the stream
      virtual int   ReadUInt(unsigned int* pUInt);
      //! Read a 64bit-integer from the stream
      virtual int   ReadInt64(int64* pInt64);
      //! Read an unsigned 64bit-integer from the stream
      virtual int   ReadUInt64(uint64* pUInt64);
      //! Read a single-precision-float from the stream
      virtual int   ReadFloat(float* pFloat);
      //! Read a double-precision-float from the stream
      virtual int   ReadDouble(double* pDouble);
      //! Read a strimg from the stream
      virtual int   ReadString(std::string* pString);
      //! Read a class that inherits IStreamable interface.
      virtual int   ReadClass(IStreamable* pStreamable);
      //! Read a vec3 from the stream
      virtual int   ReadVec3(vec3<double>* pVec) ;
      //! Read a vec2 from the stream
      virtual int   ReadVec2(vec2<double>* pVec) ;


		/*!	Returns the current value of the stream's position indicator.
		*		\param	nPos Position-indicator storage.
		*		\throw	Exception if an error occurs.
		*/
		virtual void	GetPos(int64 &nPos)  = 0;

		/*!	Sets the stream's position indicator to the value nPos.
		*		\param	nPos     Position-indicator storage.
		*		\throw	Exception if an error occurs.
		*/
		virtual void	SetPos(int64 nPos)  = 0;

		/*!	Closes the stream.
		*/
		virtual void	Close() = 0;

		/*!	Gets the path of the stream.
		*		\return	path
		*/
      virtual std::string	GetPath() = 0;
	};
//}
#endif
