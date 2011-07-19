/*******************************************************************************
Project       : High Precision Timer abstraction
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _TIMER_H
#define _TIMER_H

#include "og.h"

class OPENGLOBE_API Timer
{
public:
	//! initializes the real timer
	static void initTimer();

	//! returns the current real time in milliseconds
	static unsigned int getRealTime();
	
	//! returns the current real time in milliseconds with high precision
	//! not available on all platforms!
	static double getRealTimeHighPrecision();
};

#endif

