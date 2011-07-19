/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#include "og.h"

// System specific functions are in this class
// if OpenGlobe SDK is ported to another operating system you have to create
// a System.cpp which provides the functions listed here
// The System.cpp is always located in the operating system specific directory

class OPENGLOBE_API System
{
public:
   static int GetProcessId();      // Retrieve process id
   static int GetThreadId();      // Retrieve thread id


private:
   System(){}  // can't create an instance. System is all static.
   virtual ~System(){}
};

