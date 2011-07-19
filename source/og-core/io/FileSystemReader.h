
#ifndef _FILESYSTEMREADERDISK_H_
#define _FILESYSTEMREADERDISK_H_

#include "og.h"
#include "data/DiskCache.h"

#include <string>
#include <exception>
#include <boost/shared_ptr.hpp>


#include "IFileSystemReader.h"

//! \class FileSystemReader
//! \brief Cached File System Reader
//! Abstraction for reading files from a hard disk-like file hierarchy. This also includes internet downloads.
//! (Local and remote file systems and http:// downloads)
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup filesystem
class OPENGLOBE_API FileSystemReader : public IFileSystemReader
{
public:
   //! Constructor
   //! \param sRoot directory from where data should be read. This must be an absolute directory or an http:// path
   FileSystemReader(Cache* pDiskCache, std::string sRoot);
   virtual ~FileSystemReader();

   /*!
   * Copy file (or internet download) to memory
   * \param sRelativePath relative path to file
   * \param data the actual data
   * \param data_size size in bytes
   * \return 
   */
   virtual bool ReadData(const std::string& sRelativePath, boost::shared_array<unsigned char>& data, size_t& data_size);


protected:
   std::string _sRoot;
   Cache* _pDiskCache;

private:
   FileSystemReader(){}
};



#endif