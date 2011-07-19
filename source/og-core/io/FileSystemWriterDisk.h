
#ifndef _FILESYSTEMWRITERDISK_H_
#define _FILESYSTEMWRITERDISK_H_

#include "og.h"
#include <string>
#include <exception>
#include <boost/shared_ptr.hpp>

#include "IFileSystemWriter.h"

//! \brief File System for writing from hard disk-like file hierarchy.
//! Abstraction for writing files to a hard disk-like file hierarchy.
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup filesystem
class OPENGLOBE_API FileSystemWriterDisk : public IFileSystemWriter
{
public:
   FileSystemWriterDisk(const std::string& sRoot);
   virtual ~FileSystemWriterDisk();

   virtual bool WriteData(const std::string& sRelativePath, const boost::shared_array<unsigned char>& data, const size_t& data_size);

   /*!
   * Closes the output stream after writing.
   * \return virtual void
   * \throw								Exception if an error occurs.
   */
   virtual void CloseOutputStream();

protected:
   std::string   _sRoot;
};


#endif