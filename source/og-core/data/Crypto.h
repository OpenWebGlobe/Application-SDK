/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _CRYPTO_H
#define _CRYPTO_H

#include "og.h"
#include <cassert>
#include "shared_array.h"

//! Class holding a MD5 Hash value
//! \ingroup datamanagement
class OPENGLOBE_API MD5Hash 
{
public:

   MD5Hash() 
   {
      for (int i = 0; i < 16; ++i) 
      {
         value[i] = 0;
      }
   }

   unsigned char& operator[](int i) 
   {
      assert(i>=0 && i<16);
      return value[i];
   }

   const unsigned char& operator[](int i) const 
   {
      assert(i>=0 && i<16);
      return value[i];
   }

   bool operator==(const MD5Hash& other) const 
   {
      bool match = true;
      for (int i = 0; i < 16; ++i) 
      {
         match = match && (other.value[i] == value[i]);
      }
      return match;
   }

   inline bool operator!=(const MD5Hash& other) const 
   {
      return !(*this == other);
   }

private:
   unsigned char value[16];
};

//-----------------------------------------------------------------------------

//! \ingroup datamanagement
class OPENGLOBE_API Crypto 
{
public:

   /*!
   Computes the CRC32 value of a byte array.  CRC32 is designed to be a hash
   function that produces different values for similar strings.

   This implementation is compatible with PKZIP and GZIP.

   Based on http://www.gamedev.net/reference/programming/features/crc32/
   */
   static unsigned int crc32(const void* bytes, size_t numBytes);

   // Compute the CRC64 value of a byte array
   static uint64 crc64(const void* bytes, size_t numBytes);

   /*!
   Computes the MD5 hash (message digest) of a byte stream, as defined by
   http://www.ietf.org/rfc/rfc1321.txt.

   \cite Based on implementation by L. Peter Deutsch, ghost@aladdin.com
   */
   static MD5Hash md5(const void* bytes, size_t numBytes);

   /*!
   Returns the nth prime less than 10000 in constant time.  The first prime has index
   0 and is the number 2.
   */
   static int smallPrime(int n);

   /*! Returns 1 + the largest value that can be passed to smallPrime. */
   static int numSmallPrimes();

   //! Convert Binary Data to ASCII-String (Hex)
   //! \param pData [in] pointer to binary data
   //! \param pDataSize [in] size of binary data in bytes
   //! \param sOut [out] output string. 
   static void Bin2Hex(const unsigned char* pData, size_t pDataSize, std::string& sOut);

   //! Convert Hex-Encoded String to binary Data
   //! \param sDataIn [in] input data: Hex-Encoded ASCII string (example: "FFEEA002")
   //! \param qOut [out] binary data
   //! \param nSizeOut [out] size of binary data in bytes
   static void Hex2Bin(std::string sDataIn, shared_array<unsigned char>& qOut, size_t& nSizeOut);

   
   
private:
   static const int prime_table[]; 
};


#endif

