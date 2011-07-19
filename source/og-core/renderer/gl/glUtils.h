#ifndef _I3D_GLUTILS
#define _I3D_GLUTILS

#include "og.h"
#include <GL/glew.h>
#include <iostream>


//! \class glUtils
//! \brief Various OpenGL related utilities.
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup opengl
class OPENGLOBE_API glUtils
{
public:
   //! \brief Check opengl error and report to std::cout if an error is found.
   //! \param file Contains filename of sourcecode. This should be set to the macro __FILE__
   //! \param line Contains Linenumber of current instruction. Set to macro __LINE__
   //! \warning intended for internal use only.
   static int CheckOpenGLError(char *file, int line)
   {
      GLenum glErr;
      int    retCode = 0;

      glErr = glGetError();
      while (glErr != GL_NO_ERROR) 
      {
         const GLubyte* sError = gluErrorString(glErr);

         if (sError)
            std::cout << "GL Error #" << glErr << "(" << gluErrorString(glErr) << ") " << " in File " << file << " at line: " << line << std::endl;
         else
            std::cout << "GL Error #" << glErr << " (no message available)" << " in File " << file << " at line: " << line << std::endl;

         retCode = 1;
         glErr = glGetError();
      }
      return retCode;
   }

};

#define CHECK_GL_ERROR() glUtils::CheckOpenGLError(__FILE__, __LINE__)

#ifdef _DEBUG
#define ASSERT_ON_GLERROR if (0!=CHECK_GL_ERROR()){ assert(false); }
#else
#define ASSERT_ON_GLERROR
#endif

#endif