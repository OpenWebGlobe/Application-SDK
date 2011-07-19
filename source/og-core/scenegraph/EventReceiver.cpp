/*******************************************************************************
Project       : i3D Studio  
Purpose       : Event Manager
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2006 by FHNW. All rights reserved.
*******************************************************************************/

#include "EventReceiver.h"



EventReceiver::EventReceiver()
{
}

EventReceiver::~EventReceiver()
{
   // disconnect all signals!
   std::list<boost::signals::connection>::iterator i = _lstConnections.begin();

   while (i!=_lstConnections.end())
   {
      (*i).disconnect();
      ++i;
   }
}