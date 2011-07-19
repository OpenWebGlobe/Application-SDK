

#ifndef _STREAMIN_H 
#define _STREAMIN_H

#include <boost/shared_ptr.hpp>
#include "IStreamIn.h"

// Create/Open StreamIn from URL
// this can a a common filename (c:/data/txt.txt or /home/user/data.txt or a protocol like http:// or file://
boost::shared_ptr<IStreamIn> CreateStreamInFromURL(const std::string& sURL);


#endif