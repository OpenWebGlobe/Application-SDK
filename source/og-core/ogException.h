
#ifndef _OGEXCEPTION_H
#define _OGEXCEPTION_H

#include <string>
#include "og.h"


//! \brief Exception Handler for OpenWebGlobe
class OPENGLOBE_API ogException
{
public:

   /*!
   * Constructor
   * \param nCode                The error code.
   */
   ogException(int nCode){_sMessage = ""; _nCode = nCode;}

   /*!
   * Constructor
   * \param nCode                The error code.
   * \param pMessage             The error message.
   */
   ogException(int nCode, const std::string& sMessage){_sMessage = sMessage; _nCode = nCode;}

   virtual ~ogException(){}

   /*!
   * Get the error code.
   * \return int                 The error code.
   */
   int GetCode(){return _nCode;}

   /*!
   * Get the error message.
   * \return std::wstring&	     The error message.
   */
   std::string& GetExceptionMessage(){return _sMessage;}

protected:
   int               _nCode;
   std::string       _sMessage;
};

#endif