/*******************************************************************************
Project       : i3D Studio  
Purpose       : Interface for custom streamable classes
Creation Date : 10.9.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2006 by FHNW. All rights reserved.
*******************************************************************************/


#ifndef _ISTREAMABLE_H_
#define _ISTREAMABLE_H_

#include "og.h"
#include <string>
#include <exception>

#include "io/IStreamOut.h"
#include "io/IStreamIn.h"

//! \interface IStreamable
//! \brief interface to a streamable class.
//! This class should be inherited by all classes that require an explicit binary serialization in their own format.
//! Please note that this has nothing to do with XML Serialization, this should be used to export a class to a custom format.
//! \ingroup iostreams
//! \author Martin Christen, martin.christen@fhnw.ch
class OPENGLOBE_API IStreamable
{
public:
   //! \brief Empty Constructor
   IStreamable(){};

   //! \brief Empty Destructor
   virtual ~IStreamable(){}

   //! \brief Write Stream
   //! \param pStreamout pointer to an output stream for writing data.
   virtual void Write(IStreamOut* pStreamout) = 0;

   //! \brief Read Stream. 
   //! \param pStreamin Pointer to an input stream for reading data.
   //! \return Number of bytes read.
   virtual int Read(IStreamIn* pStreamin) = 0;
};




#endif