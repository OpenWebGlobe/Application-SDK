/*******************************************************************************
Project       : i3D Studio  
Purpose       : File Handling
Creation Date : 29.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2006 by FHNW. All rights reserved.
*******************************************************************************/

#ifndef _ISTREAMOUT_H_
#define _ISTREAMOUT_H_

#include "og.h"
#include <string>
#include <exception>
#include <math/vec2.h>
#include <math/vec3.h>
#include "ogException.h"

class IStreamable;

//namespace i3d
//{
   //! \interface IStreamOut
   //! \brief Interface for output streams
   //! An interface to an output stream for writing data. Output streams can be files, zip-archives, memory files and more.
   //! \ingroup iostreams
   //! \author Martin Christen, martin.christen@fhnw.ch
   class OPENGLOBE_API IStreamOut
	{
	public:
      //! Empty constructor
      IStreamOut(){}

      //! Empty destructor
      virtual ~IStreamOut(){}

		//! \brief Write custom data to file stream.
      //! Writes the given byte buffer to the file stream.
		//! \param pBuffer	bytes to write.
		//! \param nSize size of pBuffer.
		//! \throw Exception if an error occurs.
		virtual void   Write(void* pBuffer, size_t nSize)  = 0;
      //! Write a byte (char) to the stream
      //! \param pByte Pointer to char
      //! \throw	Exception if an error occurs.
      virtual void   WriteByte(char* pByte) ;
      //! Write an unsigned byte (unsigned char) to the stream
      //! \param pUByte Pointer to unsigned char
      //! \throw	Exception if an error occurs.
      virtual void   WriteUByte(unsigned char* pUByte);
      //! Write a short to the stream
      //! \param pUByte Pointer to short
      //! \throw	Exception if an error occurs.
      virtual void   WriteShort(short* pUByte);
      //! Write an unsigned short to the stream
      //! \param pUByte Pointer to unsigned short
      //! \throw	Exception if an error occurs.
      virtual void   WriteUShort(unsigned short* pUByte);
      //! Write an integer to the stream
      //! \param pInt Pointer to integer
      //! \throw	Exception if an error occurs.
      virtual void   WriteInt(int* pInt);
      //! Write an unsigned integer to the stream
      //! \param pUInt Pointer to unsigned integer
      //! \throw	Exception if an error occurs.
      virtual void   WriteUInt(unsigned int* pUInt);
      //! Write a 64-bit integer to the stream
      //! \param pInt64 Pointer to 64-bit integer
      //! \throw	Exception if an error occurs.
      virtual void   WriteInt64(int64* pInt64);
      //! Write an unsigned 64-bit integer to the stream
      //! \param pUInt64 Pointer to unsigned 64-bit integer
      //! \throw	Exception if an error occurs.
      virtual void   WriteUInt64(uint64* pUInt64);
      //! Write a single precision floating point to the stream
      //! \param pFloat Pointer to float value
      //! \throw	Exception if an error occurs.
      virtual void   WriteFloat(float* pFloat);
      //! Write a double precision floating point to the stream
      //! \param pDouble Pointer to double value
      //! \throw	Exception if an error occurs.
      virtual void   WriteDouble(double* pDouble);
      //! Write a string to the stream
      //! \param pString Pointer to string
      //! \throw	Exception if an error occurs.
      virtual void   WriteString(std::string* pString);
      //! Write a class inherited from IStreamable to the stream
      //! \param pStreamable Pointer to IStreamable, which must be inherited.
      //! \throw	Exception if an error occurs.
      virtual void   WriteClass(IStreamable* pStreamable);

      //== Pointerless writes ==
      //! Write a byte (char) to the stream
      //! \param pByte the char to write
      //! \throw	Exception if an error occurs.
      virtual void   WriteByte(char pByte) ;
      //! Write an unsigned byte (unsigned char) to the stream
      //! \param pUByte the unsigned char to write
      //! \throw	Exception if an error occurs.
      virtual void   WriteUByte(unsigned char pUByte);
      //! Write a short to the stream
      //! \param pUByte the short to write
      //! \throw	Exception if an error occurs.
      virtual void   WriteShort(short pUByte);
      //! Write an unsigned short to the stream
      //! \param pUByte the unsigned short to write
      //! \throw	Exception if an error occurs.
      virtual void   WriteUShort(unsigned short pUByte);
      //! Write an integer to the stream
      //! \param pInt the integer to write
      //! \throw	Exception if an error occurs.
      virtual void   WriteInt(int pInt);
      //! Write an unsigned integer to the stream
      //! \param pUInt the unsigned int to write
      //! \throw	Exception if an error occurs.
      virtual void   WriteUInt(unsigned int pUInt);
      //! Write a 64-bit integer to the stream
      //! \param pInt64 the 64-bit integer to write
      //! \throw	Exception if an error occurs.
      virtual void   WriteInt64(int64 pInt64);
      //! Write an unsigned 64-bit integer to the stream
      //! \param pUInt64 the unsigned 64-bit integer to write
      //! \throw	Exception if an error occurs.
      virtual void   WriteUInt64(uint64 pUInt64);
      //! Write a single precision floating point to the stream
      //! \param pFloat the single precision floating point to write
      //! \throw	Exception if an error occurs.
      virtual void   WriteFloat(float pFloat);
      //! Write a double precision floating point to the stream
      //! \param pDouble the double precision floating point to write
      //! \throw	Exception if an error occurs.
      virtual void   WriteDouble(double pDouble);
      //! Write a string to the stream
      //! \param pString the string to write
      //! \throw	Exception if an error occurs.
      virtual void   WriteString(std::string& pString);
      //! Write a vec3 to the stream
      //! \param pVec the vec3 to write
      //! \throw	Exception if an error occurs.
      virtual void   WriteVec3(vec3<double>& pVec);
      //! Write a ve2 to the stream
      //! \param pVec the vec2 to write
      //! \throw	Exception if an error occurs.
      virtual void   WriteVec2(vec2<double>& pVec);


		/*!	\brief Return stream position.
      *     Returns the current value of the stream's position indicator.
		*		\param	nPos Position-indicator storage.
		*		\throw	Exception if an error occurs.
		*/
		virtual void	GetPos(int64 &nPos)  = 0;

		/*!	\brief Set stream position.
      *     Sets the stream's position indicator to the value nPos.
		*		\param	nPos Position-indicator storage.
		*		\throw	Exception if an error occurs.
		*/
		virtual void	SetPos(int64 nPos)  = 0;

		/*!	\brief close stream
      *     Closes the stream.
      *     \throw	Exception if an error occurs.
		*/
		virtual void	Close() throw () = 0;

		/*!	\brief get path of stream.
      *     Gets the path of the stream. Not standardized - for example for files this is the filename. 
		*		\return	path
      *     \throw	Exception if an error occurs.
		*/
      virtual std::string	GetPath() = 0;
	};
//}
#endif
