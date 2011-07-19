/*******************************************************************************
Project       : i3D Studio  
Purpose       : Index buffer
Creation Date : 26.1.2007
Author        : Martin Christen
Copyright     : Copyright (c) 2004-2007 by Martin Christen. All rights reserved.
*******************************************************************************/

#ifndef A_IINDEXBUFFER_H
#define A_IINDEXBUFFER_H

#include "og.h"


class OPENGLOBE_API IIndexBuffer
{
public:
   virtual ~IIndexBuffer(){};

   //! Initialize Index Buffer. Specify its size.
   virtual void Initialize(unsigned int size) = 0;

   //! Retrieve pointer to index data (System Memory)
   virtual unsigned int* GetIndicesPtr(void) = 0; 

   //! Retrieve pointer to user defined face ids
   virtual int* GetFaceIdPtr(void) = 0;
   
   //! Set value at specified index buffer location (System Memory)
   virtual void SetValue(unsigned int index, unsigned int value) = 0;

   //! Set value at specified index buffer location (System Memory) and also set a (user defined) face id
   virtual void SetValue(unsigned int index, unsigned int value, int faceId) = 0;

   //! Return size of index buffer (number of index entries)
   virtual unsigned int GetSize(void) = 0;

   virtual void FreeResources() = 0;       //!< Free all resources occupied by this index buffer (system memory + video memory)

   virtual unsigned int CreateVRAMObject(void)  = 0; //!< allocate data on GPU
   virtual unsigned int CopyToVRAM(bool bDynamic) = 0;   //!< copies all data from system to VRAM.  
   virtual unsigned int StreamToVRAM() = 0; 

};



#endif