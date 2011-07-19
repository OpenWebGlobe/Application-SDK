/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef A_IVERTEXATTRIBUTEBUFFER_H
#define A_IVERTEXATTRIBUTEBUFFER_H

#include "og.h"

//! \brief Vertex Attribute Data Types.
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup vb
enum EVertexDataType
{
   float1 = 0,              //!< One-component float expanded to (float, 0, 0, 1)
   float2 = 2,              //!< Two-component float expanded to (float, float, 0, 1)
   float3 = 3,              //!< Three-component float expanded to (float, float, float, 1)
   float4 = 4,              //!< Four-component float expanded to (float, float, float, float)
   pcolor = 5,              //!< Four-component, packed, unsigned bytes mapped to 0 to 1 range (expanded as r,g,b,a)
   ubyte4 = 6,              //!< Four-component, unsigned byte. Non-normalized!
   ubyte4_normalized = 7,   //!< Four-component, unsigned byte, normalized [0,1] by dividing 255.0f.
   
   vertextype_unused        //!< invalid component, do not use.
};

enum EVertexAttribute
{
   Vertex = 0,       //!< \brief [0] gl_Vertex: Vertex  x,y,z,[w]
   Normal,           //!< \brief [2] gl_Normal: Normal        
   PrimaryColor,     //!< \brief [3] gl_Color: Color          
   SecondaryColor,   //!< \brief [4] gl_SecondaryColor: SecondaryColor 

   TexCoord0,        //!< \brief [8] gl_MultiTexCoord0
   TexCoord1,        //!< \brief [9] gl_MultiTexCoord1
   TexCoord2,        //!< \brief [10] gl_MultiTexCoord2
   TexCoord3,        //!< \brief [11] gl_MultiTexCoord3
   TexCoord4,        //!< \brief [12] gl_MultiTexCoord4
   TexCoord5,        //!< \brief [13] gl_MultiTexCoord5
   TexCoord6,        //!< \brief [14] gl_MultiTexCoord6
   TexCoord7,        //!< \brief [15] gl_MultiTexCoord7

   MaxAttribute      //!< Used To count number of attributes   
};

#define MAX_VERTEX_ATTRIB ((int)MaxAttribute)

class OPENGLOBE_API IVertexAttributeBuffer
{
public:
   //! Destructor
   virtual ~IVertexAttributeBuffer(){}

   //! \brief Create Vertex Attribute Buffer.
   //! \param Size number of vertices
   //! \param eVertexAttribute The vertex attribute description.
   //! \param eVertexDataType The vertex attribute data type.
   //! \return 0 if everything is ok. 1 if there is not enough memory.
   virtual int   Create(unsigned int Size, EVertexAttribute eVertexAttribute, EVertexDataType eVertexDataType) = 0;
   //! \brief Release buffer - free RAM and VRAM.
   virtual void  Release(void) = 0;

   //! \brief returns native RAM representation of this vertex buffer.
   //! \warning This is meant for experts only, as it dangerous to use.
   //! \return pointer to data
   virtual void* GetDataPtr(void) = 0;  
   //! \brief Set attribute value.
   //! \param index Specify vertex number. If you have n vertices this is between 0 and n-1.
   //! \param a First value. This may be the x-component or r-component.
   //! \param b Second value. This may be the y-component or g-component. If unused, set to 0.
   //! \param c Third value. This may be the z-component or b-component. If unused, set to 0.
   //! \param d Fourth value. This may be the w-component or a-component. If unused, set to 0.
   virtual void  SetValue(unsigned int index, float a=0, float b=0, float c=0, float d=0) = 0;
   
   //! \brief Set vertex attribute as packed DWORD value. This is usually done for colors, e.g. 0xFFAAFFCC
   //! \param index Vertex number.
   //! \param a Packed DWORD value.
   virtual void  SetPackedValue(unsigned int index, unsigned int a) = 0;
   //! \brief retrieve packed DWORD value. This only works if you previously specified this with "SetPackedValue").
   //! \param index Vertex number.
   virtual unsigned int GetPackedValue(unsigned int index) = 0;

   //! \brief Return number of bytes used per vertex.
   //! \return bytes used per vertex.
   virtual unsigned int GetChannelSizeInBytes()   = 0;
   
   //! \brief Return number of components used per vertex. For example a RGBA Color would return 4 or A xyz vertex would return 3.
   //! \return number of components per vertex.
   virtual unsigned int GetNumComponents()        = 0;  // Returns number of components a channel has (note: packed values count as total 1)

   //! \brief Return Attribute data type. (system memory type).
   //! \return 1 means FLOAT and 2 means DWORD.
   virtual int GetAttributeDataType() = 0;   

   //! \brief Retrieve attribute type. This is normal, vertex, primary color, etc.
   //! \return Vertex attribute type.
   virtual EVertexAttribute GetAttributeType(void) = 0;

   //! \brief Return number of vertices (attribute descriptions).
   //! \return number of vertex attributes.
   virtual unsigned int GetSize(void)           = 0;

   //! \brief Create GPU representation.
   //! \return Returns 0 if everything is fine.
   virtual unsigned int CreateVRAMObject(void)  = 0;

   //! \brief Copy GPU represention to VRAM. This is called after "CreateVRAMObject".
   //! \param bDynamic Set to true if you want to update this buffer using "StreamToVRAM".
   //! \return 0 if everything is fine.
   virtual unsigned int CopyToVRAM(bool bDynamic)        = 0;

   //! \brief Updated RAM representation must be streamed to VRAM.
   //! \return 0 if everything is fine.
   virtual unsigned int StreamToVRAM() = 0;
};

#endif